/* Fuzz target for the fast_io.json module.
 *
 * Three entry points are exercised on the same bytes, because they have to
 * agree with each other: the DOM builder, the streaming callback parser, and
 * the DOM built again with json_policy::none - the FPU-free arm that kernel
 * code uses, where a real number is rejected instead of converted.
 *
 * The oracle is structural, not just "it did not crash". The flat node array
 * promises that every child index is in range and that sibling chains always
 * terminate; a parser that leaked a cycle or an out-of-range index into the
 * chains would still return normally, so this walk is what actually catches
 * it. Agreement between the two policies is checked as well: without
 * json_policy::floating_point a document may only fail *in addition* to the
 * default one, never instead of it.
 */
#include <cstddef>
#include <cstdint>

/* catch throws(::std::error) needs the error class visible, and the module
 * deliberately does not re-export the whole heritage. */
#include <herbceptions/error>

import fast_io.json;

using ::fast_io::json::json_policy;

namespace
{

/* Kept global so the compiler cannot drop the byte hashing below. */
inline ::std::uint_least64_t sink{};

/* Touch every byte of a view so ASan catches a view that reaches past its
 * buffer or into a freed chunk. */
inline void hash(::fast_io::u8string_view s) noexcept
{
	::std::uint_least64_t h{};
	for (::std::size_t i{}; i != s.size(); ++i)
	{
		h = h * 31u + s.data()[i];
	}
	sink ^= h;
}

/* A walk that never returns means a cycle; the node count is the only bound
 * the format guarantees, so it is the cap. */
[[nodiscard]] inline bool walk_value_chain(::fast_io::json::document const &doc,
										   ::std::uint_least32_t first) noexcept
{
	::std::size_t const cap{doc.values.size()};
	::std::size_t steps{};
	::std::uint_least32_t index{first};
	while (index != ::fast_io::json::no_index)
	{
		if (doc.values.size() <= index || cap <= steps)
		{
			return false;
		}
		++steps;
		index = doc.values.index_unchecked(index).next_sibling;
	}
	return true;
}

[[nodiscard]] inline bool check_document(::fast_io::json::document const &doc) noexcept
{
	for (::std::uint_least32_t i{}; i != doc.values.size(); ++i)
	{
		::fast_io::json::value const &v{doc.values.index_unchecked(i)};
		switch (v.kind)
		{
		case ::fast_io::json::value_kind::array:
			if (!walk_value_chain(doc, v.first_child))
			{
				return false;
			}
			break;
		case ::fast_io::json::value_kind::object:
		{
			::std::size_t const cap{doc.members.size()};
			::std::size_t steps{};
			::std::uint_least32_t index{v.first_child};
			while (index != ::fast_io::json::no_index)
			{
				if (doc.members.size() <= index || cap <= steps)
				{
					return false;
				}
				++steps;
				::fast_io::json::member const &m{doc.members.index_unchecked(index)};
				if (doc.values.size() <= m.value_index)
				{
					return false;
				}
				index = m.next_sibling;
			}
			break;
		}
		case ::fast_io::json::value_kind::string:
			/* Unescaped strings are views into the fuzzer's buffer and escaped
			 * ones point into the document arena; both must be dereferenceable
			 * for their whole length, which the copy below makes the sanitizer
			 * check. */
			hash(v.as_string());
			break;
		default:
			break;
		}
	}
	return true;
}

struct counting_handler
{
	::std::size_t events{};
	::std::size_t strings{};

	void on_null() throws { ++events; }
	void on_bool(bool) throws { ++events; }
	void on_uint64(::std::uint_least64_t) throws { ++events; }
	void on_int64(::std::int_least64_t) throws { ++events; }
	void on_float64(::fast_io::json::float64_type) throws { ++events; }
	void on_string(::fast_io::u8string_view s) throws
	{
		++events;
		++strings;
		hash(s);
	}
	void on_array_begin() throws { ++events; }
	void on_array_end(::std::size_t) throws { ++events; }
	void on_object_begin() throws { ++events; }
	void on_object_end(::std::size_t) throws { ++events; }
	void on_key(::fast_io::u8string_view s) throws
	{
		++events;
		hash(s);
	}
};

} // namespace

extern "C" int LLVMFuzzerTestOneInput(::std::uint8_t const *ptr, ::std::size_t n) noexcept
{
	::fast_io::u8string_view const input{reinterpret_cast<char8_t const *>(ptr), n};
	bool dom_ok{};
	bool stream_ok{};
	bool no_fpu_ok{};

	try
	{
		auto const doc{::fast_io::json::parse(input)};
		dom_ok = check_document(doc);
		if (!dom_ok)
		{
			__builtin_trap();
		}
	}
	catch throws(::std::error)
	{
	}

	try
	{
		counting_handler h{};
		auto const consumed{::fast_io::json::parse_into(input, h)};
		stream_ok = consumed == input.size();
		if (!stream_ok)
		{
			/* The DOM accepted these bytes, so a full parse that stopped
			 * early is a bug in the streaming path. */
			__builtin_trap();
		}
	}
	catch throws(::std::error)
	{
	}

	try
	{
		auto const doc{::fast_io::json::parse<json_policy::none>(input)};
		no_fpu_ok = check_document(doc);
		if (!no_fpu_ok)
		{
			__builtin_trap();
		}
	}
	catch throws(::std::error)
	{
	}

	/* Dropping the floating_point flag can only add rejections (documents
	 * holding a fraction or an exponent), never remove one. */
	if (!dom_ok && no_fpu_ok)
	{
		__builtin_trap();
	}
	if (dom_ok && !stream_ok)
	{
		__builtin_trap();
	}
	return 0;
}
