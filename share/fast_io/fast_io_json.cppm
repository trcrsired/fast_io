module;

/*
fast_io.json — the JSON (RFC 8259) reader.

Two entry points over one parser:

  - a streaming handler interface (named requirements; every member is
    `void f(...) throws`). Handlers are matched by duck typing, so there is
    no virtual function, no vtable and no type erasure in the parse path.
    Boost.JSON spells these `bool on_xxx(..., error_code&)`, which forces a
    branch per callback and a mutable error slot carried through every frame.
    Here a failure leaves through the herbception channel instead, so the
    callbacks stay `void` and the fast path has no failure plumbing at all.

  - a DOM (`document`) built by `dom_handler`, for callers that want to walk
    a parsed tree instead of reacting to it.

Errors are herbceptions only — `::fast_io::herbceptions::throws_parse_errc`
with `parse_errc::invalid` (malformed), `parse_errc::end_of_file` (truncated)
or `parse_errc::overflow` (nesting too deep, node count exhausted, integer
out of range) — the same channel `fast_io_driver/m3u8.h` uses, so a caller
catching `throws(::std::error)` cannot tell the two parsers apart. A thrown
value carries no position, so `parser` records the byte offset it failed at
in `error_offset`; parse_callback() reports a failure that already happened
inside a `noexcept` continuation the way the async operations do.

Strings are sized ::fast_io::u8string_view throughout, never NUL scanned, and
a string without escapes is a view straight into the caller's buffer, so a
catalogue of plain text costs no allocation to parse. Only a string that
actually contains a backslash is decoded, into a chunked arena whose chunks
are reserved up front, so every view handed out stays valid for the
document's lifetime. The source buffer must outlive the document.

Scanning uses ::fast_io::intrinsics::simd_vector — the compiler's vector
extension — rather than intrinsics, so one source vectorises to SSE2, AVX2,
AVX-512, wasm SIMD128 or NEON by target, and the scalar fallback is the same
code with a single lane. Control characters are detected as (ch & 0xE0) == 0
rather than by an unsigned compare, which x86 SSE2 has no cheap form of.

Recursion is bounded by max_depth and the node count by max_nodes, so a
hostile document is rejected instead of exhausting the stack or the 32 bit
index space. Integers accumulate by hand with exact overflow checks and are
stored two's complement in one unsigned slot (use value::as_int64() to read a
negative back); a number with a fraction or an exponent becomes float64_type
through a Clinger fast path plus a long double tail, converted in this module
rather than through fast_io.floating, so no #embed and no extra module is
needed. json_policy::floating_point is the bit that carries all of it: without
it neither the arm nor the table exists.

The tree is two flat containers (values and members) whose allocator is
basic_document's template parameter, so indexing and allocation report
violations through the same allocator_adapter_flags the containers use:
fail fast by default, herbceptions when the allocator asks for them.

Usage, one section per entry point, lives in tests/0047.json.
*/

#include <coroutine>
/* std::float64_t is the C++23 fixed-width type, but wasm/libc++ sysroots do
 * not all ship <stdfloat>; when it is missing the alias below falls back to
 * double, which is binary64 everywhere we target. */
#if __has_include(<stdfloat>)
#include <stdfloat>
#endif

#include <herbceptions/error>

#include <fast_io.h>
#include <fast_io_dsal/array.h>
#include <fast_io_dsal/string_view.h>
#include <fast_io_dsal/vector.h>

export module fast_io.json;

export namespace fast_io
{

/* The API is spelled in these types, so an importer must be able to name them
 * without textually including the headers they come from. */
using ::fast_io::allocator_adapter_flags;
using ::fast_io::array;
using ::fast_io::generic_allocator_adapter;
using ::fast_io::native_global_allocator;
using ::fast_io::u8string_view;
using ::fast_io::vector;

namespace json
{

/* __STDCPP_FLOAT64_T__ is set only when the implementation really provides
 * the type; the header alone is not enough (libstdc++ ships <stdfloat> on
 * targets without _Float64). */
#if defined(__STDCPP_FLOAT64_T__)
using float64_type = ::std::float64_t;
#else
using float64_type = double;
#endif

/* Parser policy flags. Kernel code must not touch the FPU and no kernel path
 * wants a decimal converter linked in, so the policy is a template parameter
 * instead of a runtime branch: without json_policy::floating_point a number
 * carrying a fraction or an exponent is rejected as invalid, and no
 * floating-point instruction or conversion table is emitted at all - the
 * real-number arm is `if constexpr`-ed away, not merely left unused. */
enum class json_policy : ::std::uint_least16_t
{
	none = 0,
	floating_point = 1u << 0
};

[[nodiscard]] inline constexpr json_policy operator|(json_policy a, json_policy b) noexcept
{
	return static_cast<json_policy>(static_cast<::std::uint_least16_t>(a) |
								   static_cast<::std::uint_least16_t>(b));
}

[[nodiscard]] inline constexpr json_policy operator&(json_policy a, json_policy b) noexcept
{
	return static_cast<json_policy>(static_cast<::std::uint_least16_t>(a) &
								   static_cast<::std::uint_least16_t>(b));
}

[[nodiscard]] inline constexpr json_policy operator^(json_policy a, json_policy b) noexcept
{
	return static_cast<json_policy>(static_cast<::std::uint_least16_t>(a) ^
								   static_cast<::std::uint_least16_t>(b));
}

[[nodiscard]] inline constexpr json_policy operator~(json_policy a) noexcept
{
	return static_cast<json_policy>(~static_cast<::std::uint_least16_t>(a));
}

inline constexpr json_policy default_json_policy{json_policy::floating_point};

/* Nesting limit: deeper documents are rejected rather than allowed to
 * exhaust the stack. */
inline constexpr ::std::size_t max_depth{512};

/* Node/member indices are 32 bit, so a document tops out below what a 32 bit
 * address space could hold anyway. */
inline constexpr ::std::uint_least32_t max_nodes{static_cast<::std::uint_least32_t>(0xFFFFFFF0u)};
inline constexpr ::std::uint_least32_t no_index{static_cast<::std::uint_least32_t>(-1)};

enum class value_kind : ::std::uint_least8_t
{
	null,
	boolean,
	integer,
	real,
	string,
	array,
	object
};

/* Children of an array are values, children of an object are members; both
 * are chained through 32 bit indices instead of a vector per container, which
 * keeps a node 32 bytes and allocates nothing per container. The scalar
 * payload is one union: `kind` says which arm is live. */
struct value
{
	value_kind kind{value_kind::null};
	::std::uint_least32_t first_child{no_index};
	::std::uint_least32_t next_sibling{no_index};

	union payload_type
	{
		bool boolean;
		::std::uint_least64_t integer;
		float64_type real;
		::fast_io::u8string_view string;
		/* A view carries initialisers, so its default constructor is not
		 * trivial and the union has to name a member to start with; every
		 * later change of the active member is a placement new. */
		inline constexpr payload_type() noexcept : integer{}
		{
		}
	} payload;

	[[nodiscard]] inline constexpr bool as_bool() const noexcept
	{
		return payload.boolean;
	}

	[[nodiscard]] inline constexpr ::std::uint_least64_t as_uint64() const noexcept
	{
		return payload.integer;
	}

	/* Integers are stored two's complement in an unsigned slot so that the
	 * whole 64 bit unsigned range survives. */
	[[nodiscard]] inline constexpr ::std::int_least64_t as_int64() const noexcept
	{
		return static_cast<::std::int_least64_t>(payload.integer);
	}

	[[nodiscard]] inline constexpr float64_type as_float64() const noexcept
	{
		return payload.real;
	}

	[[nodiscard]] inline constexpr ::fast_io::u8string_view as_string() const noexcept
	{
		return payload.string;
	}
};

struct member
{
	::fast_io::u8string_view key{};
	::std::uint_least32_t value_index{no_index};
	::std::uint_least32_t next_sibling{no_index};
};

[[noreturn]] inline constexpr void throws_invalid() throws
{
	::fast_io::herbceptions::throws_parse_errc(::fast_io::freestanding::parse_errc::invalid);
}

[[noreturn]] inline constexpr void throws_end_of_file() throws
{
	::fast_io::herbceptions::throws_parse_errc(::fast_io::freestanding::parse_errc::end_of_file);
}

[[noreturn]] inline constexpr void throws_overflow() throws
{
	::fast_io::herbceptions::throws_parse_errc(::fast_io::freestanding::parse_errc::overflow);
}

/* Switch the payload to a string: placement new, not construct_at, which
 * does not work under herbceptions. */
inline void set_string(value &v, ::fast_io::u8string_view s) noexcept
{
	::new (static_cast<void *>(__builtin_addressof(v.payload.string))) ::fast_io::u8string_view(s);
}

[[nodiscard]] inline value make_string(::fast_io::u8string_view s) noexcept
{
	value v{};
	v.kind = value_kind::string;
	set_string(v, s);
	return v;
}

namespace details
{

inline constexpr ::std::size_t simd_width{16};
using simd_type = ::fast_io::intrinsics::simd_vector<char8_t, simd_width>;

/* The splat vectors are constant-initialised lane by lane. Building them with
 * a helper that stores into a buffer would need a dynamic initialiser, and a
 * dynamic initialiser at namespace scope drags in __cxa_guard_acquire /
 * __cxa_guard_release - symbols a freestanding or wasm target does not have.
 * Written out, they cost nothing and link anywhere. */
inline constexpr simd_type quote_vec{static_cast<char8_t>(0x22), static_cast<char8_t>(0x22), static_cast<char8_t>(0x22), static_cast<char8_t>(0x22), static_cast<char8_t>(0x22), static_cast<char8_t>(0x22), static_cast<char8_t>(0x22), static_cast<char8_t>(0x22), static_cast<char8_t>(0x22), static_cast<char8_t>(0x22), static_cast<char8_t>(0x22), static_cast<char8_t>(0x22), static_cast<char8_t>(0x22), static_cast<char8_t>(0x22), static_cast<char8_t>(0x22), static_cast<char8_t>(0x22)};
inline constexpr simd_type backslash_vec{static_cast<char8_t>(0x5C), static_cast<char8_t>(0x5C), static_cast<char8_t>(0x5C), static_cast<char8_t>(0x5C), static_cast<char8_t>(0x5C), static_cast<char8_t>(0x5C), static_cast<char8_t>(0x5C), static_cast<char8_t>(0x5C), static_cast<char8_t>(0x5C), static_cast<char8_t>(0x5C), static_cast<char8_t>(0x5C), static_cast<char8_t>(0x5C), static_cast<char8_t>(0x5C), static_cast<char8_t>(0x5C), static_cast<char8_t>(0x5C), static_cast<char8_t>(0x5C)};
inline constexpr simd_type space_vec{static_cast<char8_t>(0x20), static_cast<char8_t>(0x20), static_cast<char8_t>(0x20), static_cast<char8_t>(0x20), static_cast<char8_t>(0x20), static_cast<char8_t>(0x20), static_cast<char8_t>(0x20), static_cast<char8_t>(0x20), static_cast<char8_t>(0x20), static_cast<char8_t>(0x20), static_cast<char8_t>(0x20), static_cast<char8_t>(0x20), static_cast<char8_t>(0x20), static_cast<char8_t>(0x20), static_cast<char8_t>(0x20), static_cast<char8_t>(0x20)};
inline constexpr simd_type tab_vec{static_cast<char8_t>(0x09), static_cast<char8_t>(0x09), static_cast<char8_t>(0x09), static_cast<char8_t>(0x09), static_cast<char8_t>(0x09), static_cast<char8_t>(0x09), static_cast<char8_t>(0x09), static_cast<char8_t>(0x09), static_cast<char8_t>(0x09), static_cast<char8_t>(0x09), static_cast<char8_t>(0x09), static_cast<char8_t>(0x09), static_cast<char8_t>(0x09), static_cast<char8_t>(0x09), static_cast<char8_t>(0x09), static_cast<char8_t>(0x09)};
inline constexpr simd_type newline_vec{static_cast<char8_t>(0x0A), static_cast<char8_t>(0x0A), static_cast<char8_t>(0x0A), static_cast<char8_t>(0x0A), static_cast<char8_t>(0x0A), static_cast<char8_t>(0x0A), static_cast<char8_t>(0x0A), static_cast<char8_t>(0x0A), static_cast<char8_t>(0x0A), static_cast<char8_t>(0x0A), static_cast<char8_t>(0x0A), static_cast<char8_t>(0x0A), static_cast<char8_t>(0x0A), static_cast<char8_t>(0x0A), static_cast<char8_t>(0x0A), static_cast<char8_t>(0x0A)};
inline constexpr simd_type carriage_return_vec{static_cast<char8_t>(0x0D), static_cast<char8_t>(0x0D), static_cast<char8_t>(0x0D), static_cast<char8_t>(0x0D), static_cast<char8_t>(0x0D), static_cast<char8_t>(0x0D), static_cast<char8_t>(0x0D), static_cast<char8_t>(0x0D), static_cast<char8_t>(0x0D), static_cast<char8_t>(0x0D), static_cast<char8_t>(0x0D), static_cast<char8_t>(0x0D), static_cast<char8_t>(0x0D), static_cast<char8_t>(0x0D), static_cast<char8_t>(0x0D), static_cast<char8_t>(0x0D)};
/* A UTF-8 lead or continuation byte always has one of the top three bits
 * set, so (ch & 0xE0) == 0 selects exactly 0x00..0x1F. */
inline constexpr simd_type control_mask{static_cast<char8_t>(0xE0), static_cast<char8_t>(0xE0), static_cast<char8_t>(0xE0), static_cast<char8_t>(0xE0), static_cast<char8_t>(0xE0), static_cast<char8_t>(0xE0), static_cast<char8_t>(0xE0), static_cast<char8_t>(0xE0), static_cast<char8_t>(0xE0), static_cast<char8_t>(0xE0), static_cast<char8_t>(0xE0), static_cast<char8_t>(0xE0), static_cast<char8_t>(0xE0), static_cast<char8_t>(0xE0), static_cast<char8_t>(0xE0), static_cast<char8_t>(0xE0)};
inline constexpr simd_type zero_vec{static_cast<char8_t>(0x00), static_cast<char8_t>(0x00), static_cast<char8_t>(0x00), static_cast<char8_t>(0x00), static_cast<char8_t>(0x00), static_cast<char8_t>(0x00), static_cast<char8_t>(0x00), static_cast<char8_t>(0x00), static_cast<char8_t>(0x00), static_cast<char8_t>(0x00), static_cast<char8_t>(0x00), static_cast<char8_t>(0x00), static_cast<char8_t>(0x00), static_cast<char8_t>(0x00), static_cast<char8_t>(0x00), static_cast<char8_t>(0x00)};

[[nodiscard]] inline constexpr bool is_control(char8_t ch) noexcept
{
	return static_cast<char8_t>(ch & 0xE0) == 0;
}

/* First byte of a string body that needs attention: the closing quote, a
 * backslash, or a control character JSON forbids unescaped. Returns `last`
 * when the whole range is plain. */
[[nodiscard]] inline constexpr char8_t const *scan_string_body(char8_t const *first, char8_t const *last) noexcept
{
	/* Vector registers do not exist in constant evaluation, so the SIMD loop
	 * is runtime only and the scalar tail below covers the whole range when
	 * it is skipped. */
	while (!__builtin_is_constant_evaluated() && simd_width <= static_cast<::std::size_t>(last - first))
	{
		simd_type chunk;
		chunk.load(first);
		simd_type const stop{(chunk == quote_vec) | (chunk == backslash_vec) |
							 ((chunk & control_mask) == zero_vec)};
		if (!::fast_io::intrinsics::is_all_zeros(stop))
		{
			for (::std::size_t i{}; i != simd_width; ++i)
			{
				char8_t const ch{first[i]};
				if (ch == u8'"' || ch == u8'\\' || is_control(ch))
				{
					return first + i;
				}
			}
		}
		first += simd_width;
	}
	for (; first != last; ++first)
	{
		char8_t const ch{*first};
		if (ch == u8'"' || ch == u8'\\' || is_control(ch))
		{
			break;
		}
	}
	return first;
}

/* First byte that is not JSON whitespace. */
[[nodiscard]] inline constexpr char8_t const *scan_whitespace(char8_t const *first, char8_t const *last) noexcept
{
	while (!__builtin_is_constant_evaluated() && simd_width <= static_cast<::std::size_t>(last - first))
	{
		simd_type chunk;
		chunk.load(first);
		simd_type const stop{((chunk == space_vec) | (chunk == tab_vec) |
							  (chunk == newline_vec) | (chunk == carriage_return_vec)) == zero_vec};
		if (!::fast_io::intrinsics::is_all_zeros(stop))
		{
			for (::std::size_t i{}; i != simd_width; ++i)
			{
				char8_t const ch{first[i]};
				if (ch != u8' ' && ch != u8'\t' && ch != u8'\n' && ch != u8'\r')
				{
					return first + i;
				}
			}
		}
		first += simd_width;
	}
	while (first != last)
	{
		char8_t const ch{*first};
		if (ch != u8' ' && ch != u8'\t' && ch != u8'\n' && ch != u8'\r')
		{
			break;
		}
		++first;
	}
	return first;
}

/* Reject overlong forms, surrogates and anything above U+10FFFF. Only run
 * for text with a non-ASCII byte, so an ASCII document never pays for it. */
[[nodiscard]] inline constexpr bool valid_utf8(::fast_io::u8string_view s) noexcept
{
	char8_t const *p{s.data()};
	char8_t const *const last{p + s.size()};
	while (p != last)
	{
		char8_t const ch{*p};
		if (ch < 0x80)
		{
			++p;
			continue;
		}
		::std::size_t continuations{};
		::std::uint_least32_t cp{};
		::std::uint_least32_t lowest{};
		if ((ch & 0xE0) == 0xC0)
		{
			continuations = 1;
			cp = static_cast<::std::uint_least32_t>(ch & 0x1F);
			lowest = 0x80;
		}
		else if ((ch & 0xF0) == 0xE0)
		{
			continuations = 2;
			cp = static_cast<::std::uint_least32_t>(ch & 0x0F);
			lowest = 0x800;
		}
		else if ((ch & 0xF8) == 0xF0)
		{
			continuations = 3;
			cp = static_cast<::std::uint_least32_t>(ch & 0x07);
			lowest = 0x10000;
		}
		else
		{
			return false;
		}
		if (static_cast<::std::size_t>(last - p) <= continuations)
		{
			return false;
		}
		for (::std::size_t i{1}; i <= continuations; ++i)
		{
			char8_t const cont{p[i]};
			if ((cont & 0xC0) != 0x80)
			{
				return false;
			}
			cp = static_cast<::std::uint_least32_t>((cp << 6) | (cont & 0x3F));
		}
		if (cp < lowest || (0xD800 <= cp && cp <= 0xDFFF))
		{
			return false;
		}
		p += continuations + 1;
	}
	return true;
}

[[nodiscard]] inline constexpr bool has_non_ascii(::fast_io::u8string_view s) noexcept
{
	char8_t const *p{s.data()};
	char8_t const *const last{p + s.size()};
	for (; p != last; ++p)
	{
		if (0x80 <= *p)
		{
			return true;
		}
	}
	return false;
}

/* Decimal -> binary64 without the fast_io.floating module, which needs #embed
 * and so is not buildable on every toolchain. The Clinger fast path is
 * exact: a mantissa of at most 2^53 scaled by a power of ten within 10^22 is
 * represented exactly by one double operation. Anything past that goes through
 * long double, which is why the header comment says "correctly rounded when
 * the significand fits"; a JSON catalogue never leaves the fast path. */
namespace floating_conversion
{

inline constexpr float64_type pow10_table[]{
	1e0, 1e1, 1e2, 1e3, 1e4, 1e5, 1e6, 1e7, 1e8, 1e9, 1e10, 1e11,
	1e12, 1e13, 1e14, 1e15, 1e16, 1e17, 1e18, 1e19, 1e20, 1e21, 1e22};

inline constexpr ::std::uint_least64_t exact_mantissa_limit{(1ULL << 53)};

inline constexpr float64_type from_parts(bool negative, ::std::uint_least64_t mantissa,
							   ::std::int_least32_t e10) noexcept
{
	float64_type value{};
	if (mantissa <= exact_mantissa_limit && -22 <= e10 && e10 <= 22)
	{
		value = static_cast<float64_type>(mantissa);
		if (e10 < 0)
		{
			value /= pow10_table[-e10];
		}
		else if (e10 > 0)
		{
			value *= pow10_table[e10];
		}
	}
	else
	{
		long double wide{static_cast<long double>(mantissa)};
		if (e10 > 0)
		{
			while (e10 > 22)
			{
				wide *= 1e22L;
				e10 -= 22;
			}
			wide *= static_cast<long double>(pow10_table[e10]);
		}
		else
		{
			while (e10 < -22)
			{
				wide /= 1e22L;
				e10 += 22;
			}
			wide /= static_cast<long double>(pow10_table[-e10]);
		}
		value = static_cast<float64_type>(wide);
	}
	return negative ? -value : value;
}

} // namespace floating_conversion

/* Decoded-text storage. Every chunk reserves its full capacity on creation,
 * so appending never reallocates and every view already handed out stays
 * valid; one string larger than a chunk gets a chunk of its own. */
struct string_arena
{
	inline static constexpr ::std::size_t chunk_capacity{1u << 16};

	::fast_io::vector<::fast_io::vector<char8_t>> chunks{};
	::std::size_t pending_offset{};

	[[nodiscard]] char8_t *reserve(::std::size_t n) throws
	{
		if (chunks.is_empty() || chunk_capacity - chunks.back().size() < n)
		{
			chunks.push_back(::fast_io::vector<char8_t>{});
			chunks.back().reserve(n > chunk_capacity ? n : chunk_capacity);
			pending_offset = 0;
		}
		else
		{
			pending_offset = chunks.back().size();
		}
		chunks.back().resize(pending_offset + n, ::fast_io::for_overwrite_t{});
		return chunks.back().data() + pending_offset;
	}

	/* Keep only the first `written` bytes of the last reservation. */
	void commit(::std::size_t written) noexcept
	{
		chunks.back().resize(pending_offset + written);
	}

	[[nodiscard]] ::fast_io::u8string_view allocate(::std::size_t n) throws
	{
		char8_t *const p{reserve(n)};
		commit(n);
		return ::fast_io::u8string_view(p, n);
	}
};

/* Arena for constant-evaluated and heap-free parses: one fixed buffer the
 * caller owns, so the views handed out stay valid exactly as long as the
 * caller keeps it. It is ::fast_io::array - fast_io's own container - because
 * the module deliberately uses no standard container at all. */
template <::std::size_t capacity>
struct fixed_string_arena
{
	::fast_io::array<char8_t, capacity> buffer{};
	::std::size_t used{};

	[[nodiscard]] inline constexpr char8_t *reserve(::std::size_t n) throws
	{
		if (capacity - used < n)
		{
			throws_overflow();
		}
		return buffer.data() + used;
	}

	inline constexpr void commit(::std::size_t written) noexcept
	{
		used += written;
	}

	[[nodiscard]] inline constexpr ::fast_io::u8string_view allocate(::std::size_t n) throws
	{
		char8_t *const p{reserve(n)};
		commit(n);
		return ::fast_io::u8string_view(p, n);
	}
};

} // namespace details

/*
 * Handler requirements — every member is `void ... throws`, matched by duck
 * typing:
 *
 *   void on_null() throws;
 *   void on_bool(bool) throws;
 *   void on_uint64(::std::uint_least64_t) throws;
 *   void on_int64(::std::int_least64_t) throws;
 *   void on_float64(float64_type) throws;
 *   void on_string(::fast_io::u8string_view) throws;   // decoded
 *   void on_array_begin() throws;
 *   void on_array_end(::std::size_t n) throws;
 *   void on_object_begin() throws;
 *   void on_object_end(::std::size_t n) throws;
 *   void on_key(::fast_io::u8string_view) throws;      // decoded
 */

template <typename handler_type>
concept handler = requires(handler_type &h, ::fast_io::u8string_view s, ::std::size_t n,
						   ::std::uint_least64_t u, ::std::int_least64_t i, float64_type d,
						   bool b) {
	h.on_null();
	h.on_bool(b);
	h.on_uint64(u);
	h.on_int64(i);
	h.on_float64(d);
	h.on_string(s);
	h.on_array_begin();
	h.on_array_end(n);
	h.on_object_begin();
	h.on_object_end(n);
	h.on_key(s);
};

/* Single pass recursive descent over one buffer. Public because a caller may
 * want to keep the parser around to read error_offset after a failure. */
template <json_policy policy = default_json_policy, typename arena_type = details::string_arena>
struct parser
{
	::fast_io::u8string_view src{};
	::std::size_t pos{};
	::std::size_t depth{};
	::std::size_t error_offset{};
	/* Decoded text lands here. The arena is not owned: a dynamic call keeps a
	 * details::string_arena on the stack, a constant-evaluated call brings a
	 * details::fixed_string_arena, and neither ever moves its buffer, so the
	 * views the handler receives stay valid. */
	arena_type *arena{};

	inline constexpr parser(::fast_io::u8string_view s, arena_type *a) noexcept : src{s}, arena{a}
	{
	}

	[[nodiscard]] inline constexpr bool eof() const noexcept
	{
		return src.size() <= pos;
	}

	[[nodiscard]] inline constexpr char8_t peek() const noexcept
	{
		return src.data()[pos];
	}

	inline constexpr void skip_whitespace() noexcept
	{
		char8_t const *const base{src.data()};
		pos = static_cast<::std::size_t>(details::scan_whitespace(base + pos, base + src.size()) - base);
	}

	constexpr void enter() throws
	{
		if (max_depth <= depth)
		{
			error_offset = pos;
			throws_overflow();
		}
		++depth;
	}

	inline constexpr void leave() noexcept
	{
		--depth;
	}

	constexpr void expect_literal(char8_t const *text, ::std::size_t n) throws
	{
		char8_t const *const base{src.data()};
		if (src.size() - pos < n)
		{
			error_offset = pos;
			throws_end_of_file();
		}
		for (::std::size_t i{}; i != n; ++i)
		{
			if (base[pos + i] != text[i])
			{
				error_offset = pos + i;
				throws_invalid();
			}
		}
		pos += n;
	}

	[[nodiscard]] constexpr ::fast_io::u8string_view parse_string() throws;

	[[nodiscard]] constexpr ::fast_io::u8string_view parse_string_slow(char8_t const *body) throws;
};


/* Encode one code point as UTF-8 into `out`, which must hold four bytes.
 * Returns the number written. No container, so it works in constant
 * evaluation and on the stack. */
[[nodiscard]] inline constexpr ::std::size_t encode_utf8(::std::uint_least32_t cp,
														  char8_t *out) noexcept
{
	if (cp < 0x80)
	{
		out[0] = static_cast<char8_t>(cp);
		return 1;
	}
	if (cp < 0x800)
	{
		out[0] = static_cast<char8_t>(0xC0 | (cp >> 6));
		out[1] = static_cast<char8_t>(0x80 | (cp & 0x3F));
		return 2;
	}
	if (cp < 0x10000)
	{
		out[0] = static_cast<char8_t>(0xE0 | (cp >> 12));
		out[1] = static_cast<char8_t>(0x80 | ((cp >> 6) & 0x3F));
		out[2] = static_cast<char8_t>(0x80 | (cp & 0x3F));
		return 3;
	}
	out[0] = static_cast<char8_t>(0xF0 | (cp >> 18));
	out[1] = static_cast<char8_t>(0x80 | ((cp >> 12) & 0x3F));
	out[2] = static_cast<char8_t>(0x80 | ((cp >> 6) & 0x3F));
	out[3] = static_cast<char8_t>(0x80 | (cp & 0x3F));
	return 4;
}

/* Read four hex digits of a \u escape at `p`; `p` is advanced by the caller. */
[[nodiscard]] inline constexpr ::std::uint_least32_t read_hex4(char8_t const *p, char8_t const *base,
													 ::std::size_t &error_offset) throws
{
	::std::uint_least32_t cp{};
	for (::std::size_t i{}; i != 4; ++i)
	{
		char8_t const ch{p[i]};
		if (!::fast_io::char_category::is_c_xdigit(ch))
		{
			error_offset = static_cast<::std::size_t>(p - base) + i;
			throws_invalid();
		}
		cp = static_cast<::std::uint_least32_t>(cp << 4);
		if (ch <= u8'9')
		{
			cp = static_cast<::std::uint_least32_t>(cp | (ch - u8'0'));
		}
		else
		{
			/* 'a'..'f' and 'A'..'F' differ only in bit 0x20 */
			cp = static_cast<::std::uint_least32_t>(cp | ((ch & 0xDF) - u8'A' + 10));
		}
	}
	return cp;
}

template <json_policy policy, typename arena_type>
inline constexpr ::fast_io::u8string_view parser<policy, arena_type>::parse_string() throws
{
	char8_t const *const base{src.data()};
	char8_t const *const body{base + pos + 1};
	char8_t const *const last{base + src.size()};
	char8_t const *const stop{details::scan_string_body(body, last)};
	if (stop == last)
	{
		error_offset = pos;
		throws_end_of_file();
	}
	if (*stop == u8'"')
	{
		::fast_io::u8string_view const text{body, static_cast<::std::size_t>(stop - body)};
		if (details::has_non_ascii(text) && !details::valid_utf8(text))
		{
			error_offset = pos;
			throws_invalid();
		}
		pos = static_cast<::std::size_t>(stop - base) + 1;
		return text;
	}
	if (*stop != u8'\\')
	{
		error_offset = static_cast<::std::size_t>(stop - base);
		throws_invalid();
	}
	return parse_string_slow(body);
}

/* Escape decoding for one string, in two passes over the same bytes: the
 * first sizes the arena reservation, the second writes into it. Sizing first
 * means the arena never over-allocates, the text is never copied twice, and
 * nothing is allocated on the side - which is what keeps this path usable in
 * constant evaluation and on targets without a heap. `dest == nullptr` is the
 * sizing pass; `self.pos` is left just past the closing quote either way. */
template <json_policy policy, typename arena_type>
[[nodiscard]] inline constexpr ::std::size_t decode_string(parser<policy, arena_type> &self,
												 char8_t const *body, char8_t *dest) throws
{
	char8_t const *const base{self.src.data()};
	char8_t const *const last{base + self.src.size()};
	char8_t const *p{body};
	::std::size_t written{};
	for (;;)
	{
		char8_t const *const stop{details::scan_string_body(p, last)};
		if (stop == last)
		{
			self.error_offset = self.pos;
			throws_end_of_file();
		}
		::std::size_t const run{static_cast<::std::size_t>(stop - p)};
		if (dest != nullptr)
		{
			for (::std::size_t i{}; i != run; ++i)
			{
				dest[written + i] = p[i];
			}
		}
		written += run;
		if (*stop == u8'"')
		{
			p = stop + 1;
			break;
		}
		if (*stop != u8'\\')
		{
			self.error_offset = static_cast<::std::size_t>(stop - base);
			throws_invalid();
		}
		if (static_cast<::std::size_t>(last - stop) < 2)
		{
			self.error_offset = static_cast<::std::size_t>(stop - base);
			throws_end_of_file();
		}
		char8_t const esc{stop[1]};
		p = stop + 2;
		if (esc != u8'u')
		{
			char8_t decoded{};
			switch (esc)
			{
			case u8'"':
				decoded = u8'"';
				break;
			case u8'\\':
				decoded = u8'\\';
				break;
			case u8'/':
				decoded = u8'/';
				break;
			case u8'b':
				decoded = static_cast<char8_t>(0x08);
				break;
			case u8'f':
				decoded = static_cast<char8_t>(0x0C);
				break;
			case u8'n':
				decoded = static_cast<char8_t>(0x0A);
				break;
			case u8'r':
				decoded = static_cast<char8_t>(0x0D);
				break;
			case u8't':
				decoded = static_cast<char8_t>(0x09);
				break;
			default:
				self.error_offset = static_cast<::std::size_t>(stop - base) + 1;
				throws_invalid();
			}
			if (dest != nullptr)
			{
				dest[written] = decoded;
			}
			++written;
			continue;
		}
		if (static_cast<::std::size_t>(last - p) < 4)
		{
			self.error_offset = static_cast<::std::size_t>(p - base);
			throws_end_of_file();
		}
		::std::uint_least32_t cp{read_hex4(p, base, self.error_offset)};
		p += 4;
		if (0xD800 <= cp && cp <= 0xDBFF)
		{
			/* high surrogate: the matching low surrogate has to follow */
			if (static_cast<::std::size_t>(last - p) < 6 || p[0] != u8'\\' || p[1] != u8'u')
			{
				self.error_offset = static_cast<::std::size_t>(p - base);
				throws_invalid();
			}
			::std::uint_least32_t const low{read_hex4(p + 2, base, self.error_offset)};
			if (low < 0xDC00 || 0xDFFF < low)
			{
				self.error_offset = static_cast<::std::size_t>(p - base) + 2;
				throws_invalid();
			}
			p += 6;
			cp = static_cast<::std::uint_least32_t>(0x10000 + ((cp - 0xD800) << 10) + (low - 0xDC00));
		}
		else if (0xDC00 <= cp && cp <= 0xDFFF)
		{
			self.error_offset = static_cast<::std::size_t>(stop - base) + 2;
			throws_invalid();
		}
		char8_t bytes[4];
		::std::size_t const n{encode_utf8(cp, bytes)};
		if (dest != nullptr)
		{
			for (::std::size_t i{}; i != n; ++i)
			{
				dest[written + i] = bytes[i];
			}
		}
		written += n;
	}
	self.pos = static_cast<::std::size_t>(p - base);
	return written;
}

template <json_policy policy, typename arena_type>
inline constexpr ::fast_io::u8string_view parser<policy, arena_type>::parse_string_slow(char8_t const *body) throws
{
	::std::size_t const length{decode_string(*this, body, nullptr)};
	char8_t *const dest{arena->reserve(length)};
	(void)decode_string(*this, body, dest);
	arena->commit(length);
	::fast_io::u8string_view const text(dest, length);
	if (details::has_non_ascii(text) && !details::valid_utf8(text))
	{
		error_offset = pos;
		throws_invalid();
	}
	return text;
}

/* Forward declaration: parse_object and parse_array recurse through
 * parse_value, and every step is a free function template. */
template <json_policy policy, typename arena_type, typename handler_type>
constexpr void parse_value(parser<policy, arena_type> &self, handler_type &h) throws;
template <json_policy policy, typename arena_type, typename handler_type>
constexpr void parse_number(parser<policy, arena_type> &self, handler_type &h) throws
{
	char8_t const *const base{self.src.data()};
	char8_t const *p{base + self.pos};
	char8_t const *const last{base + self.src.size()};
	char8_t const *const start{p};
	bool is_real{};
	bool negative{};
	if (*p == u8'-')
	{
		negative = true;
		++p;
		if (p == last)
		{
			self.error_offset = self.pos;
			throws_end_of_file();
		}
	}
	if (!::fast_io::char_category::is_c_digit(*p))
	{
		self.error_offset = static_cast<::std::size_t>(p - base);
		throws_invalid();
	}
	if (*p == u8'0')
	{
		++p;
	}
	else
	{
		while (p != last && ::fast_io::char_category::is_c_digit(*p))
		{
			++p;
		}
	}
	if (p != last && *p == u8'.')
	{
		is_real = true;
		++p;
		if (p == last)
		{
			self.error_offset = static_cast<::std::size_t>(p - base);
			throws_end_of_file();
		}
		if (!::fast_io::char_category::is_c_digit(*p))
		{
			self.error_offset = static_cast<::std::size_t>(p - base);
			throws_invalid();
		}
		while (p != last && ::fast_io::char_category::is_c_digit(*p))
		{
			++p;
		}
	}
	if (p != last && (*p == u8'e' || *p == u8'E'))
	{
		is_real = true;
		++p;
		if (p != last && (*p == u8'+' || *p == u8'-'))
		{
			++p;
		}
		if (p == last)
		{
			self.error_offset = static_cast<::std::size_t>(p - base);
			throws_end_of_file();
		}
		if (!::fast_io::char_category::is_c_digit(*p))
		{
			self.error_offset = static_cast<::std::size_t>(p - base);
			throws_invalid();
		}
		while (p != last && ::fast_io::char_category::is_c_digit(*p))
		{
			++p;
		}
	}
	self.pos = static_cast<::std::size_t>(p - base);
	if (!is_real)
	{
		/* integers never touch the FPU, so they are parsed the same way
		 * whatever the floating_point bit says */
		::std::uint_least64_t magnitude{};
		for (char8_t const *q{negative ? start + 1 : start}; q != p; ++q)
		{
			/* (max - digit) / 10 is the largest value that can still be
			 * multiplied by ten and take the digit, so this cannot wrap */
			::std::uint_least64_t const digit{static_cast<::std::uint_least64_t>(*q - u8'0')};
			if ((0xFFFFFFFFFFFFFFFFu - digit) / 10u < magnitude)
			{
				self.error_offset = static_cast<::std::size_t>(q - base);
				throws_overflow();
			}
			magnitude = static_cast<::std::uint_least64_t>(magnitude * 10u + digit);
		}
		if (negative)
		{
			if (static_cast<::std::uint_least64_t>(0x8000000000000000u) < magnitude)
			{
				self.error_offset = static_cast<::std::size_t>(start - base);
				throws_overflow();
			}
			h.on_int64(static_cast<::std::int_least64_t>(0u - magnitude));
		}
		else
		{
			h.on_uint64(magnitude);
		}
	}
	else if constexpr ((policy & json_policy::floating_point) != json_policy::none)
	{
		/* digits, optional point, optional exponent; the span was already
		 * validated above, so this only accumulates */
		::std::uint_least64_t mantissa{};
		::std::int_least32_t e10{};
		bool seen_point{};
		bool digit_dropped{};
		for (char8_t const *q{start}; q != p; ++q)
		{
			char8_t const ch{*q};
			if (::fast_io::char_category::is_c_digit(ch))
			{
				::std::uint_least64_t const digit{static_cast<::std::uint_least64_t>(ch - u8'0')};
				if (mantissa <= (::std::uint_least64_t{0xFFFFFFFFFFFFFFFFu} - digit) / 10u)
				{
					mantissa = static_cast<::std::uint_least64_t>(mantissa * 10u + digit);
					if (seen_point)
					{
						--e10;
					}
				}
				else
				{
					digit_dropped = true;
					if (!seen_point)
					{
						++e10;
					}
				}
			}
			else if (ch == u8'.')
			{
				seen_point = true;
			}
			else if (ch == u8'e' || ch == u8'E')
			{
				bool negative_exponent{};
				++q;
				if (*q == u8'+')
				{
					++q;
				}
				else if (*q == u8'-')
				{
					negative_exponent = true;
					++q;
				}
				::std::int_least32_t exponent{};
				for (; q != p; ++q)
				{
					if (exponent < 100000)
					{
						exponent = static_cast<::std::int_least32_t>(exponent * 10 +
							static_cast<::std::int_least32_t>(*q - u8'0'));
					}
				}
				e10 = static_cast<::std::int_least32_t>(e10 + (negative_exponent ? -exponent : exponent));
				break;
			}
		}
		if (digit_dropped)
		{
			/* keep the trailing significance cheap and monotone */
			mantissa = static_cast<::std::uint_least64_t>(mantissa | 1u);
		}
		h.on_float64(details::floating_conversion::from_parts(negative, mantissa, e10));
	}
	else
	{
		/* floating_point is off for this instantiation. A real number is not
		 * something the target can honour, and truncating it to an integer
		 * would be a silently wrong answer, so it is rejected. */
		self.error_offset = static_cast<::std::size_t>(start - base);
		throws_invalid();
	}
}

template <json_policy policy, typename arena_type, typename handler_type>
constexpr void parse_object(parser<policy, arena_type> &self, handler_type &h) throws
{
	self.enter();
	++self.pos;
	h.on_object_begin();
	::std::size_t count{};
	self.skip_whitespace();
	if (!self.eof() && self.peek() == u8'}')
	{
		++self.pos;
	}
	else
	{
		for (;;)
		{
			self.skip_whitespace();
			if (self.eof())
			{
				self.error_offset = self.pos;
				throws_end_of_file();
			}
			if (self.peek() != u8'"')
			{
				self.error_offset = self.pos;
				throws_invalid();
			}
			h.on_key(self.parse_string());
			self.skip_whitespace();
			if (self.eof())
			{
				self.error_offset = self.pos;
				throws_end_of_file();
			}
			if (self.peek() != u8':')
			{
				self.error_offset = self.pos;
				throws_invalid();
			}
			++self.pos;
			parse_value(self, h);
			++count;
			self.skip_whitespace();
			if (self.eof())
			{
				self.error_offset = self.pos;
				throws_end_of_file();
			}
			if (self.peek() == u8',')
			{
				++self.pos;
				continue;
			}
			if (self.peek() == u8'}')
			{
				++self.pos;
				break;
			}
			self.error_offset = self.pos;
			throws_invalid();
		}
	}
	h.on_object_end(count);
	self.leave();
}

template <json_policy policy, typename arena_type, typename handler_type>
constexpr void parse_array(parser<policy, arena_type> &self, handler_type &h) throws
{
	self.enter();
	++self.pos;
	h.on_array_begin();
	::std::size_t count{};
	self.skip_whitespace();
	if (!self.eof() && self.peek() == u8']')
	{
		++self.pos;
	}
	else
	{
		for (;;)
		{
			parse_value(self, h);
			++count;
			self.skip_whitespace();
			if (self.eof())
			{
				self.error_offset = self.pos;
				throws_end_of_file();
			}
			if (self.peek() == u8',')
			{
				++self.pos;
				continue;
			}
			if (self.peek() == u8']')
			{
				++self.pos;
				break;
			}
			self.error_offset = self.pos;
			throws_invalid();
		}
	}
	h.on_array_end(count);
	self.leave();
}

template <json_policy policy, typename arena_type, typename handler_type>
constexpr void parse_value(parser<policy, arena_type> &self, handler_type &h) throws
{
	self.skip_whitespace();
	if (self.eof())
	{
		self.error_offset = self.pos;
		throws_end_of_file();
	}
	switch (self.peek())
	{
	case u8'{':
		parse_object(self, h);
		return;
	case u8'[':
		parse_array(self, h);
		return;
	case u8'"':
		h.on_string(self.parse_string());
		return;
	case u8't':
		self.expect_literal(u8"true", 4);
		h.on_bool(true);
		return;
	case u8'f':
		self.expect_literal(u8"false", 5);
		h.on_bool(false);
		return;
	case u8'n':
		self.expect_literal(u8"null", 4);
		h.on_null();
		return;
	default:
		parse_number(self, h);
		return;
	}
}

template <json_policy policy, typename arena_type, typename handler_type>
constexpr void parse_document(parser<policy, arena_type> &self, handler_type &h) throws
{
	parse_value(self, h);
	self.skip_whitespace();
	if (!self.eof())
	{
		self.error_offset = self.pos;
		throws_invalid();
	}
}

/* A document owns the node array, the member array and the decoded text; the
 * source buffer handed to parse() must outlive it. */
template <typename allocator_type = ::fast_io::native_global_allocator>
struct basic_document
{
	/* The tree is two containers, so the allocator is a template parameter here
	 * exactly as it is for the containers themselves: out-of-range indexing
	 * throws only when the allocator carries allocator_adapter_flags::
	 * throws_on_violations, and otherwise fails fast, which is what `[]` on a
	 * vector already does. Nothing about the flag is reinterpreted here. */
	using value_container = ::fast_io::containers::vector<value, allocator_type>;
	using member_container = ::fast_io::containers::vector<member, allocator_type>;
	static inline constexpr bool throwing_violations{
		::fast_io::containers::details::allocator_throws_on_violations<allocator_type>};

	::fast_io::u8string_view source{};
	value_container values{};
	member_container members{};
	details::string_arena arena{};

	[[nodiscard]] inline constexpr bool is_empty() const noexcept
	{
		return values.is_empty();
	}

	[[nodiscard]] inline constexpr value const &root() const noexcept
	{
		return values.front();
	}

	/* Index the flat node array the way the containers do: `[]` checks, so a
	 * walk that runs off the end of a sibling chain is caught instead of
	 * silently reading a neighbour, and `index_unchecked` is the form for a
	 * loop that has already proved it is in range. The check is the vector's
	 * own, so whether it throws ::std::errc::invalid_argument or terminates is
	 * the allocator's throws_on_violations flag, not a decision made here. */
	[[nodiscard]] inline constexpr value const &operator[](::std::size_t index) const
		throws(throwing_violations)
	{
		return values[index];
	}

	[[nodiscard]] inline constexpr value const &index_unchecked(::std::size_t index) const noexcept
	{
		return values.index_unchecked(index);
	}

	/* first child of an array, or first member of an object */
	[[nodiscard]] inline ::std::uint_least32_t first_child(value const &v) const noexcept
	{
		return v.first_child;
	}

	[[nodiscard]] inline ::std::uint_least32_t next_sibling(value const &v) const noexcept
	{
		return v.next_sibling;
	}

	[[nodiscard]] inline ::std::uint_least32_t next_member(member const &m) const noexcept
	{
		return m.next_sibling;
	}

	/* nth child of an array */
	[[nodiscard]] inline value const *array_at(value const &arr, ::std::size_t n) const noexcept
	{
		if (arr.kind != value_kind::array)
		{
			return nullptr;
		}
		::std::uint_least32_t index{arr.first_child};
		for (::std::size_t i{}; i != n; ++i)
		{
			if (index == no_index)
			{
				return nullptr;
			}
			index = values.index_unchecked(index).next_sibling;
		}
		return index == no_index ? nullptr : __builtin_addressof(values.index_unchecked(index));
	}

	[[nodiscard]] inline member const *object_find(value const &obj,
												   ::fast_io::u8string_view key) const noexcept
	{
		if (obj.kind != value_kind::object)
		{
			return nullptr;
		}
		for (::std::uint_least32_t index{obj.first_child}; index != no_index;)
		{
			member const &m{members.index_unchecked(index)};
			if (m.key.size() == key.size() &&
				::fast_io::freestanding::my_memcmp(m.key.data(), key.data(), key.size()) == 0)
			{
				return __builtin_addressof(m);
			}
			index = m.next_sibling;
		}
		return nullptr;
	}

	[[nodiscard]] inline value const *object_at(value const &obj,
												::fast_io::u8string_view key) const noexcept
	{
		member const *const m{object_find(obj, key)};
		return m == nullptr ? nullptr : __builtin_addressof(values.index_unchecked(m->value_index));
	}
};

/* The default: a global allocator, fail fast on a bad index. Pick another
 * allocator only when the caller wants violations to come back as herbceptions. */
using document = basic_document<>;

/* Builds a document. Children are chained as they are appended, so the tree
 * is complete in a single pass: no value stack and no finalisation step. */
template <typename document_type>
class dom_handler
{
private:
	document_type &docs;
	::fast_io::vector<::std::uint_least32_t> open{};
	::fast_io::vector<::std::uint_least32_t> open_last{};
	::std::uint_least32_t pending_member{no_index};

	::std::uint_least32_t append(value v) throws
	{
		if (docs.values.size() >= max_nodes)
		{
			throws_overflow();
		}
		::std::uint_least32_t const index{static_cast<::std::uint_least32_t>(docs.values.size())};
		docs.values.push_back(v);
		if (!open.is_empty())
		{
			value &parent{docs.values.index_unchecked(open.back())};
			if (parent.kind == value_kind::object)
			{
				docs.members.index_unchecked(pending_member).value_index = index;
				pending_member = no_index;
			}
			else
			{
				::std::uint_least32_t const last{open_last.back()};
				if (last == no_index)
				{
					parent.first_child = index;
				}
				else
				{
					docs.values.index_unchecked(last).next_sibling = index;
				}
				open_last.back() = index;
			}
		}
		return index;
	}

	void open_container(value_kind kind) throws
	{
		value v{};
		v.kind = kind;
		open.push_back(append(v));
		open_last.push_back(no_index);
	}

	void close_container() noexcept
	{
		open.pop_back();
		open_last.pop_back();
	}

public:
	explicit dom_handler(document_type &d) noexcept : docs(d)
	{
	}

	void on_null() throws
	{
		append(value{});
	}

	void on_bool(bool b) throws
	{
		value v{};
		v.kind = value_kind::boolean;
		v.payload.boolean = b;
		append(v);
	}

	void on_uint64(::std::uint_least64_t u) throws
	{
		value v{};
		v.kind = value_kind::integer;
		v.payload.integer = u;
		append(v);
	}

	void on_int64(::std::int_least64_t i) throws
	{
		value v{};
		v.kind = value_kind::integer;
		v.payload.integer = static_cast<::std::uint_least64_t>(i);
		append(v);
	}

	void on_float64(float64_type d) throws
	{
		value v{};
		v.kind = value_kind::real;
		v.payload.real = d;
		append(v);
	}

	void on_string(::fast_io::u8string_view s) throws
	{
		append(make_string(s));
	}

	void on_array_begin() throws
	{
		open_container(value_kind::array);
	}

	void on_array_end(::std::size_t) throws
	{
		close_container();
	}

	void on_object_begin() throws
	{
		open_container(value_kind::object);
	}

	void on_object_end(::std::size_t) throws
	{
		close_container();
	}

	void on_key(::fast_io::u8string_view key) throws
	{
		if (docs.members.size() >= max_nodes)
		{
			throws_overflow();
		}
		::std::uint_least32_t const index{static_cast<::std::uint_least32_t>(docs.members.size())};
		docs.members.push_back(member{key, no_index, no_index});
		value &parent{docs.values.index_unchecked(open.back())};
		::std::uint_least32_t const last{open_last.back()};
		if (last == no_index)
		{
			parent.first_child = index;
		}
		else
		{
			docs.members.index_unchecked(last).next_sibling = index;
		}
		open_last.back() = index;
		pending_member = index;
	}
};

/* Parse into a document. `document_type` defaults to `document`; naming another
 * one only changes the allocator, and with it the bounds behaviour. */
template <json_policy policy = default_json_policy, typename document_type = document>
[[nodiscard]] inline document_type parse(::fast_io::u8string_view source) throws
{
	document_type docs;
	dom_handler<document_type> h{docs};
	details::string_arena arena{};
	parser<policy> p{source, __builtin_addressof(arena)};
	parse_document(p, h);
	docs.source = source;
	/* the chunks never move, only the bookkeeping does, so the views in
	 * `docs` survive this handover */
	docs.arena = ::std::move(arena);
	return docs;
}

/* Stream a document into a caller's handler; returns the bytes consumed
 * (which is the whole buffer unless the handler stopped early). */
template <json_policy policy = default_json_policy, typename handler_type>
	requires handler<handler_type>
inline ::std::size_t parse_into(::fast_io::u8string_view source, handler_type &h) throws
{
	details::string_arena arena{};
	parser<policy> p{source, __builtin_addressof(arena)};
	parse_document(p, h);
	return p.pos;
}

/* Constant-evaluation and no-heap flavour: the caller owns the buffer, the
 * decoded strings are views into it, and nothing is allocated. Capacity is
 * deduced from the arena, so a fixed-size caller writes
 *
 *   details::fixed_string_arena<4096> arena{};
 *   json::parse_into(source, handler, arena);
 *
 * and the whole call is usable in a `constexpr` initialiser. */
template <json_policy policy = default_json_policy, ::std::size_t capacity, typename handler_type>
	requires handler<handler_type>
[[nodiscard]] inline constexpr ::std::size_t parse_into(::fast_io::u8string_view source,
														handler_type &h,
														details::fixed_string_arena<capacity> &arena) throws
{
	parser<policy, details::fixed_string_arena<capacity>> p{
		source, __builtin_addressof(arena)};
	parse_document(p, h);
	return p.pos;
}

/* File flavour: ::fast_io::native_file_loader maps the file, so nothing is
 * copied and the parse reads straight out of the mapping. The loader has to
 * outlive the document (its strings are views into it), which is why the two
 * travel together and the loader is declared first, so it is destroyed last. */
struct loaded_document
{
	::fast_io::native_file_loader loader;
	document docs;

	[[nodiscard]] inline constexpr document const &operator*() const noexcept
	{
		return docs;
	}

	[[nodiscard]] inline constexpr document const *operator->() const noexcept
	{
		return __builtin_addressof(docs);
	}
};

template <json_policy policy = default_json_policy, typename... Args>
[[nodiscard]] inline loaded_document parse_file(Args &&...args) throws
{
	::fast_io::native_file_loader loader(::std::forward<Args>(args)...);
	::fast_io::u8string_view const src{
		reinterpret_cast<char8_t const *>(loader.data()),
		static_cast<::std::size_t>(loader.size())};
	return loaded_document{::std::move(loader), parse<policy>(src)};
}

/* Callback flavour, mirroring the async operations' contract: the parse runs
 * inside a `catch throws(::std::error)` and `cb` is invoked exactly once with
 * a ::std::cxx_std_error, where `domain == nullptr` means success. Ownership
 * of the error handle transfers to `cb`, which is why this function can be
 * noexcept and still lose nothing. The handler and the source must outlive the
 * call, not the callback. */
template <json_policy policy = default_json_policy, typename handler_type,
		  typename callback_type>
	requires handler<handler_type>
inline void parse_callback(::fast_io::u8string_view source, handler_type &h,
						   callback_type &&cb) noexcept
{
	try
	{
		details::string_arena arena{};
		parser<policy> p{source, __builtin_addressof(arena)};
		parse_document(p, h);
		::std::forward<callback_type>(cb)(::std::cxx_std_error{});
	}
	catch throws(::std::error e)
	{
		::std::forward<callback_type>(cb)(e.release());
	}
}

/* Release an error handle the callback/awaiter path is holding. */
inline void dispose_error(::std::cxx_std_error e) noexcept
{
	if (e.domain != nullptr) [[unlikely]]
	{
		auto const *const domain{static_cast<::std::error_domain_singleton const *>(e.domain)};
		if (domain->do_cleanup != nullptr)
		{
			domain->do_cleanup(e.code);
		}
	}
}

/* Coroutine flavour. Parsing is CPU work with no I/O, so the awaiter runs the
 * parse inside await_suspend() and resumes inline (it returns false): awaiting
 * it never actually suspends, it only gives a coroutine the same
 * `co_await ... throws` shape as the async operations. A failure is rethrown
 * from await_resume() through the herbception channel, so
 * `co_await json::parse_async(source, h);` fails exactly like the call, and the
 * awaiter's destructor releases an error payload nobody consumed (a coroutine
 * frame destroyed after the failure). */
template <typename handler_type, json_policy policy = default_json_policy>
class parse_awaiter
{
private:
	::fast_io::u8string_view source;
	handler_type *handler_ptr;
	::std::cxx_std_error err{};

public:
	inline constexpr parse_awaiter(::fast_io::u8string_view s, handler_type &h) noexcept
		: source{s}, handler_ptr{__builtin_addressof(h)}
	{
	}

	parse_awaiter(parse_awaiter const &) = delete;
	parse_awaiter &operator=(parse_awaiter const &) = delete;

	inline ~parse_awaiter() noexcept
	{
		dispose_error(err);
	}

	[[nodiscard]] inline constexpr bool await_ready() const noexcept
	{
		return false;
	}

	inline bool await_suspend(::std::coroutine_handle<>) noexcept
	{
		details::string_arena arena{};
		try
		{
			parser<policy> p{source, __builtin_addressof(arena)};
			parse_document(p, *handler_ptr);
		}
		catch throws(::std::error e)
		{
			err = e.release();
		}
		return false;
	}

	inline void await_resume() throws
	{
		if (err.domain != nullptr) [[unlikely]]
		{
			auto e{err};
			err = {};
			throw throws e;
		}
	}
};

template <json_policy policy = default_json_policy, typename handler_type>
	requires handler<handler_type>
[[nodiscard]] inline parse_awaiter<handler_type, policy> parse_async(::fast_io::u8string_view source,
															 handler_type &h) noexcept
{
	return parse_awaiter<handler_type, policy>{source, h};
}

/* Same, for the DOM: `auto doc = co_await json::parse_document_async(source);` */
template <json_policy policy = default_json_policy>
class document_awaiter
{
private:
	::fast_io::u8string_view source;
	document docs{};
	::std::cxx_std_error err{};

public:
	inline explicit constexpr document_awaiter(::fast_io::u8string_view s) noexcept : source{s}
	{
	}

	document_awaiter(document_awaiter const &) = delete;
	document_awaiter &operator=(document_awaiter const &) = delete;

	inline ~document_awaiter() noexcept
	{
		dispose_error(err);
	}

	[[nodiscard]] inline constexpr bool await_ready() const noexcept
	{
		return false;
	}

	inline bool await_suspend(::std::coroutine_handle<>) noexcept
	{
		try
		{
			dom_handler<document> h{docs};
			details::string_arena arena{};
			parser<policy> p{source, __builtin_addressof(arena)};
			parse_document(p, h);
			docs.source = source;
			docs.arena = ::std::move(arena);
		}
		catch throws(::std::error e)
		{
			err = e.release();
		}
		return false;
	}

	[[nodiscard]] inline document await_resume() throws
	{
		if (err.domain != nullptr) [[unlikely]]
		{
			auto e{err};
			err = {};
			throw throws e;
		}
		return ::std::move(docs);
	}
};

template <json_policy policy = default_json_policy>
[[nodiscard]] inline document_awaiter<policy> parse_document_async(::fast_io::u8string_view source) noexcept
{
	return document_awaiter<policy>{source};
}

} // namespace json
} // namespace fast_io
