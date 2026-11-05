/*
 * How to use fast_io.json — one section per entry point, each one a check, so
 * running this doubles as a test.
 *
 *   1. dom()          parse() + walk the tree (the easy path)
 *   2. stream()       a handler + parse_into() (no tree, no allocation)
 *   3. records()      a handler that keeps state and fills your own structs
 *   4. from_file()    parse_file() over an mmap, nothing copied
 *   5. errors()       parser + parse_document(), so error_offset is readable
 *   6. callback()     parse_callback(), the noexcept error-handle contract
 *   7. coroutine()    co_await parse_async() / parse_document_async()
 *   8. constant()     parse_into() with a fixed arena, in a static_assert
 *   9. kernel()       json_policy::none, no FPU, reals rejected
 *
 * Every string is a ::fast_io::u8string_view built with an explicit length
 * (see view() below): the parser never scans for a terminator, so nothing here
 * hands it a bare pointer.
 */
#include <coroutine>
#include <cstdio>
#include <cstdlib>
#include <cstring>

/* catch throws(::std::error) needs the error class visible; the module does not
 * re-export the herbception heritage. */
#include <herbceptions/error>

import fast_io.json;

namespace
{

using ::fast_io::u8string_view;
using ::fast_io::json::float64_type;
using ::fast_io::json::json_policy;
using ::fast_io::json::no_index;
using ::fast_io::json::value;
using ::fast_io::json::value_kind;

/* The only way strings are built here: an array literal, sized at compile time.
 * A pointer plus a length is all the parser ever gets. */
template <::std::size_t n>
[[nodiscard]] inline constexpr u8string_view view(char8_t const (&text)[n]) noexcept
{
	return u8string_view{text, n - 1};
}

/* Plain byte compare, so the example needs no helper from fast_io itself. */
[[nodiscard]] inline bool equals(u8string_view a, u8string_view b) noexcept
{
	if (a.size() != b.size())
	{
		return false;
	}
	for (::std::size_t i{}; i != a.size(); ++i)
	{
		if (a.data()[i] != b.data()[i])
		{
			return false;
		}
	}
	return true;
}

inline ::std::size_t failures{};

inline void check(bool ok, u8string_view what) noexcept
{
	std::printf("%s %.*s\n", ok ? "ok  " : "FAIL", static_cast<int>(what.size()),
				reinterpret_cast<char const *>(what.data()));
	if (!ok)
	{
		++failures;
	}
}

inline void show(char const *label, u8string_view s) noexcept
{
	std::printf("     %s: %.*s\n", label, static_cast<int>(s.size()),
				reinterpret_cast<char const *>(s.data()));
}

/* ---------------------------------------------------------------------------
 * The documents used below. Trailing NUL is not counted by view().
 * ------------------------------------------------------------------------- */
inline constexpr char8_t catalogue[]{
	u8R"({"feed":"apps","apps":[)"
	u8R"({"name":"PWA Player","size":312670,"score":4.5,"tags":["player","pwa"]},)"
	u8R"({"name":"PWA Store","size":211840,"score":4,"tags":["store"]}],)"
	u8R"("count":2})"};

/* A file path is the one argument the OS insists on receiving NUL terminated,
 * so it goes to parse_file() as the array it is, not as a bare pointer. */
inline constexpr char example_path[]{"/tmp/fast_io_json_example.json"};

/* ---------------------------------------------------------------------------
 * 1. parse() then walk it. `document` is a flat array of `value` plus a flat
 *    array of `member`; children are 32 bit indices, so a node is 32 bytes and
 *    no container allocates its own vector.
 * ------------------------------------------------------------------------- */
void dom() throws
{
	auto doc{::fast_io::json::parse(view(catalogue))};
	value const &root{doc.root()};
	check(root.kind == value_kind::object, view(u8"dom: root is an object"));

	/* object lookup is a linear scan of the sibling chain — fine for a handful
	 * of keys, and the reason the DOM stores keys as views, not as nodes */
	value const *apps{doc.object_at(root, view(u8"apps"))};
	check(apps != nullptr && apps->kind == value_kind::array,
		  view(u8"dom: apps is an array"));

	value const *count{doc.object_at(root, view(u8"count"))};
	check(count != nullptr && count->kind == value_kind::integer && count->as_uint64() == 2u,
		  view(u8"dom: count == 2 as an integer"));

	check(doc[apps->first_child].kind == value_kind::object &&
			  doc[apps->first_child].kind == doc.index_unchecked(apps->first_child).kind,
		  view(u8"dom: operator[] and index_unchecked agree on the first app"));

	::std::size_t seen{};
	::std::size_t reals{};
	::std::size_t integers{};
	for (::std::uint_least32_t index{apps->first_child}; index != no_index;)
	{
		/* index_unchecked() is for a loop that trusts its own bookkeeping; `[]`
		 * is the checking form, and checks exactly as the vector does */
		value const &app{doc.index_unchecked(index)};
		value const *name{doc.object_at(app, view(u8"name"))};
		if (name != nullptr)
		{
			show("name", name->as_string());
		}
		/* 4.5 is a real, 4 is an integer: the payload union says which arm is
		 * live and as_xxx() does not check, so read `kind` first */
		value const *score{doc.object_at(app, view(u8"score"))};
		if (score != nullptr && score->kind == value_kind::real)
		{
			++reals;
		}
		else if (score != nullptr && score->kind == value_kind::integer)
		{
			++integers;
		}
		value const *tags{doc.object_at(app, view(u8"tags"))};
		value const *first_tag{tags == nullptr ? nullptr : doc.array_at(*tags, 0)};
		check(first_tag != nullptr && first_tag->kind == value_kind::string,
			  view(u8"dom: first tag is a string"));
		index = doc.next_sibling(app);
		++seen;
	}
	check(seen == 2, view(u8"dom: two apps"));
	check(reals == 1 && integers == 1, view(u8"dom: 4.5 is a real, 4 stays an integer"));
	std::printf("     nodes: %zu, members: %zu\n", doc.values.size(), doc.members.size());
}

/* ---------------------------------------------------------------------------
 * 2. The streaming form. A handler is matched by duck typing — there is no
 *    base class, no virtual function and no error slot: every callback is
 *    `void f(...) throws`, so a malformed document simply leaves through the
 *    herbception channel from wherever it was found.
 * ------------------------------------------------------------------------- */
struct counter
{
	::std::size_t objects{}, arrays{}, keys{}, strings{}, integers{}, reals{}, booleans{}, nulls{};

	void on_null() throws { ++nulls; }
	void on_bool(bool) throws { ++booleans; }
	void on_uint64(::std::uint_least64_t) throws { ++integers; }
	void on_int64(::std::int_least64_t i) throws
	{
		/* negative integers arrive here, non-negative ones at on_uint64 */
		(void)i;
		++integers;
	}
	void on_float64(float64_type d) throws
	{
		(void)d;
		++reals;
	}
	void on_string(u8string_view) throws { ++strings; }
	void on_array_begin() throws { ++arrays; }
	void on_array_end(::std::size_t) throws {}
	void on_object_begin() throws { ++objects; }
	void on_object_end(::std::size_t) throws {}
	void on_key(u8string_view) throws { ++keys; }
};

void stream() throws
{
	counter h{};
	/* the return value is the number of bytes consumed: the whole buffer unless
	 * a handler stopped early */
	::std::size_t const consumed{::fast_io::json::parse_into(view(catalogue), h)};
	check(consumed == view(catalogue).size(), view(u8"stream: every byte consumed"));
	check(h.objects == 3 && h.arrays == 3, view(u8"stream: 3 objects, 3 arrays"));
	check(h.keys == 11 && h.strings == 6, view(u8"stream: 11 keys, 6 strings"));
	check(h.integers == 4 && h.reals == 1, view(u8"stream: 4 integers, 1 real"));
	std::printf("     objects %zu arrays %zu keys %zu strings %zu\n", h.objects, h.arrays,
				h.keys, h.strings);
}

/* ---------------------------------------------------------------------------
 * 3. Keeping state in a handler is the caller's job, and that is the trade:
 *    the DOM is easier, the handler is smaller and never builds a tree. This
 *    one collects the apps into structs the program actually wants.
 *
 *    Note the copy: `parse_into` owns a local arena and destroys it on return,
 *    so a view handed to `on_string` is only valid during the call. Anything
 *    kept afterwards is copied out here.
 * ------------------------------------------------------------------------- */
struct record
{
	char8_t name[32]{};
	::std::size_t name_len{};
	::std::uint_least64_t size{};
	float64_type score{};
	::std::size_t tags{};
};

struct record_handler
{
	::fast_io::vector<record> &out;
	::std::size_t containers{};
	bool in_app{};
	bool in_tags{};
	record current{};
	u8string_view key{};

	explicit record_handler(::fast_io::vector<record> &o) noexcept : out{o} {}

	void on_key(u8string_view k) throws { key = k; }

	void on_string(u8string_view s) throws
	{
		if (in_tags)
		{
			++current.tags;
			return;
		}
		if (!in_app || !equals(key, view(u8"name")))
		{
			return;
		}
		current.name_len = s.size() < sizeof(current.name) ? s.size() : sizeof(current.name);
		for (::std::size_t i{}; i != current.name_len; ++i)
		{
			current.name[i] = s.data()[i];
		}
	}

	void on_uint64(::std::uint_least64_t u) throws
	{
		if (in_app && equals(key, view(u8"size")))
		{
			current.size = u;
		}
	}

	void on_int64(::std::int_least64_t i) throws
	{
		if (in_app && equals(key, view(u8"size")))
		{
			current.size = static_cast<::std::uint_least64_t>(i);
		}
	}

	void on_float64(float64_type d) throws
	{
		if (in_app && equals(key, view(u8"score")))
		{
			current.score = d;
		}
	}

	void on_array_begin() throws
	{
		++containers;
		in_tags = in_app && equals(key, view(u8"tags"));
	}

	void on_array_end(::std::size_t) throws
	{
		in_tags = false;
		--containers;
	}

	/* the app object is the third container: root, apps array, app */
	void on_object_begin() throws
	{
		++containers;
		if (containers == 3)
		{
			in_app = true;
			current = record{};
		}
	}

	void on_object_end(::std::size_t) throws
	{
		if (containers == 3 && in_app)
		{
			out.push_back(current);
			in_app = false;
		}
		--containers;
	}

	void on_bool(bool) throws {}
	void on_null() throws {}
};

void records() throws
{
	::fast_io::vector<record> collected;
	record_handler h{collected};
	(void)::fast_io::json::parse_into(view(catalogue), h);
	check(collected.size() == 2, view(u8"records: two structs collected"));
	if (collected.size() == 2)
	{
		check(collected[0].size == 312670u && collected[1].tags == 1,
			  view(u8"records: size and tag count landed in the right fields"));
		show("record 0",
			 u8string_view{collected[0].name, collected[0].name_len});
	}
}

/* ---------------------------------------------------------------------------
 * 4. A file: native_file_loader maps it, so the parse reads straight out of the
 *    mapping and not one byte is copied. The loader has to outlive the string
 *    views, which is why parse_file hands back both together.
 * ------------------------------------------------------------------------- */
void from_file() throws
{
	{
		std::FILE *f{std::fopen(example_path, "wb")};
		if (f == nullptr)
		{
			check(false, view(u8"file: could not write the temporary document"));
			return;
		}
		u8string_view const text{view(catalogue)};
		std::fwrite(text.data(), 1, text.size(), f);
		std::fclose(f);
	}

	auto loaded{::fast_io::json::parse_file(example_path)};
	check(!loaded->is_empty(), view(u8"file: mapped and parsed"));
	value const *count{loaded->object_at(loaded->root(), view(u8"count"))};
	check(count != nullptr && count->as_uint64() == 2u, view(u8"file: count == 2"));
	std::remove(example_path);
}

/* ---------------------------------------------------------------------------
 * 5. A thrown value carries no position, so when the offset matters drive the
 *    parser yourself: it is public, and `error_offset` is where it stopped.
 * ------------------------------------------------------------------------- */
void errors() throws
{
	char8_t const truncated[]{u8R"({"apps":[{"name":"cut)"};
	::fast_io::json::details::string_arena arena{};
	::fast_io::json::parser<> parser{view(truncated), __builtin_addressof(arena)};
	counter h{};
	::std::size_t offset{};
	bool caught{};
	try
	{
		::fast_io::json::parse_document(parser, h);
	}
	catch throws(::std::error)
	{
		caught = true;
		offset = parser.error_offset;
	}
	check(caught, view(u8"errors: truncated document rejected"));
	check(offset != 0 && offset <= view(truncated).size(),
		  view(u8"errors: error_offset points inside the buffer"));
	std::printf("     error_offset: %zu of %zu bytes\n", offset, view(truncated).size());
}

/* ---------------------------------------------------------------------------
 * 6. The callback form exists for code that cannot throw. The parse runs inside
 *    a `catch throws(::std::error)` and the callback is invoked exactly once,
 *    with an error handle whose `domain` is nullptr on success. Ownership of the
 *    handle transfers to the callback, which is how the function stays noexcept.
 * ------------------------------------------------------------------------- */
struct error_sink
{
	::std::size_t succeeded{};
	::std::size_t failed{};

	void operator()(::std::cxx_std_error e) noexcept
	{
		if (e.domain == nullptr)
		{
			++succeeded;
			return;
		}
		++failed;
		/* the sink owns the handle now, so it has to be released */
		::fast_io::json::dispose_error(e);
	}
};

void callback() throws
{
	char8_t const broken[]{u8"[1,]"};
	error_sink sink{};
	counter h{};
	::fast_io::json::parse_callback(view(catalogue), h, sink);
	::fast_io::json::parse_callback(view(broken), h, sink);
	check(sink.succeeded == 1 && sink.failed == 1,
		  view(u8"callback: called once per parse, success then failure"));
}

/* ---------------------------------------------------------------------------
 * 7. The coroutine form, so JSON sits in the same `co_await ... throws` shape as
 *    the async I/O operations. Parsing is CPU work, so the awaiter runs it in
 *    await_suspend() and returns false: awaiting it never really suspends, and a
 *    failure is rethrown from await_resume().
 * ------------------------------------------------------------------------- */
struct task
{
	struct promise_type
	{
		[[nodiscard]] task get_return_object() noexcept
		{
			return task{::std::coroutine_handle<promise_type>::from_promise(*this)};
		}
		[[nodiscard]] ::std::suspend_never initial_suspend() const noexcept { return {}; }
		/* suspend_always: destroying a frame that already ran to the end and was
		 * destroyed by the compiler would be a double destroy */
		[[nodiscard]] ::std::suspend_always final_suspend() const noexcept { return {}; }
		void return_void() const noexcept {}
		void unhandled_exception() const noexcept { std::terminate(); }
	};

	::std::coroutine_handle<promise_type> handle{};

	task() = default;
	explicit task(::std::coroutine_handle<promise_type> h) noexcept : handle{h} {}
	task(task const &) = delete;
	task &operator=(task const &) = delete;
	~task()
	{
		if (handle)
		{
			handle.destroy();
		}
	}
};

task coroutine_form(::std::size_t &nodes, ::std::size_t &objects, ::std::size_t &threw)
{
	try
	{
		auto doc{co_await ::fast_io::json::parse_document_async(view(catalogue))};
		nodes = doc.values.size();
	}
	catch throws(::std::error)
	{
		++threw;
	}
	counter h{};
	try
	{
		co_await ::fast_io::json::parse_async(view(catalogue), h);
		objects = h.objects;
	}
	catch throws(::std::error)
	{
		++threw;
	}
}

void coroutine() throws
{
	::std::size_t nodes{};
	::std::size_t objects{};
	::std::size_t threw{};
	task t{coroutine_form(nodes, objects, threw)};
	(void)t;
	check(nodes != 0 && objects == 3 && threw == 0,
		  view(u8"coroutine: co_await parse_document_async and parse_async"));
	std::printf("     nodes: %zu, objects: %zu\n", nodes, objects);
}

/* ---------------------------------------------------------------------------
 * 8. Constant evaluation. The arena is a template parameter, so the caller can
 *    bring a fixed one: no heap, no standard container, nothing that could run
 *    at start-up.
 * ------------------------------------------------------------------------- */
struct constexpr_handler
{
	::fast_io::array<char8_t, 64> text{};
	::std::size_t text_len{};
	::std::size_t objects{};
	::std::size_t arrays{};

	constexpr void on_null() throws {}
	constexpr void on_bool(bool) throws {}
	constexpr void on_uint64(::std::uint_least64_t) throws {}
	constexpr void on_int64(::std::int_least64_t) throws {}
	constexpr void on_float64(float64_type) throws {}
	constexpr void on_string(u8string_view s) throws
	{
		for (::std::size_t i{}; i != s.size() && i != text.size(); ++i)
		{
			text[i] = s.data()[i];
		}
		text_len = s.size();
	}
	constexpr void on_array_begin() throws { ++arrays; }
	constexpr void on_array_end(::std::size_t) throws {}
	constexpr void on_object_begin() throws { ++objects; }
	constexpr void on_object_end(::std::size_t) throws {}
	constexpr void on_key(u8string_view) throws {}
};

constexpr ::std::size_t constant_parse() throws
{
	::fast_io::json::details::fixed_string_arena<128> arena{};
	constexpr_handler h{};
	(void)::fast_io::json::parse_into(view(u8R"({"k":[1,2],"s":"a\u00e9b"})"), h, arena);
	/* "a\u00e9b" decodes to four bytes: 'a', C3 A9, 'b' */
	return h.text_len * 1000u + h.objects * 10u + h.arrays;
}

static_assert(constant_parse() == 4011u,
			  "constexpr parse: 4 decoded bytes, 1 object, 1 array");

void constant() throws
{
	check(constant_parse() == 4011u, view(u8"constant: the parse ran in a static_assert"));
}

/* ---------------------------------------------------------------------------
 * 9. json_policy::none: the FPU-free instantiation. The real-number arm and the
 *    decimal conversion table are `if constexpr`-ed away rather than branched
 *    around, so a kernel build carries no floating point for JSON at all. A
 *    number with a fraction or an exponent is then rejected as invalid, while
 *    integers parse exactly as before.
 * ------------------------------------------------------------------------- */
void kernel() throws
{
	bool rejected{};
	try
	{
		(void)::fast_io::json::parse<json_policy::none>(view(catalogue));
	}
	catch throws(::std::error)
	{
		rejected = true;
	}
	check(rejected, view(u8"kernel: a real number is rejected without json_policy::floating_point"));

	/* no reals in this one, so it has to be accepted whole */
	char8_t const integers[]{u8R"({"a":1,"b":-2,"c":18446744073709551615})"};
	bool accepted{};
	::std::uint_least64_t biggest{};
	try
	{
		auto doc{::fast_io::json::parse<json_policy::none>(view(integers))};
		value const *c{doc.object_at(doc.root(), view(u8"c"))};
		biggest = c == nullptr ? 0u : c->as_uint64();
		accepted = true;
	}
	catch throws(::std::error)
	{
	}
	check(accepted && biggest == 18446744073709551615ull,
		  view(u8"kernel: integers parse identically, the full uint64 range included"));
}

/* ---------------------------------------------------------------------------
 * 10. `document` is two containers, so its allocator is a template parameter and
 *     `[]` checks the way the containers do: fail fast by default, throw when the
 *     allocator carries allocator_adapter_flags::throws_on_violations. The flag is
 *     not reinterpreted — it is the vector's own contract_violation_report.
 * ------------------------------------------------------------------------- */
using throwing_allocator =
	::fast_io::generic_allocator_adapter<::fast_io::native_global_allocator,
										 ::fast_io::allocator_adapter_flags::throws_on_violations>;
using throwing_document = ::fast_io::json::basic_document<throwing_allocator>;

static_assert(!::fast_io::json::document::throwing_violations,
			  "the default document fails fast on a bad index, like ::fast_io::vector");
static_assert(throwing_document::throwing_violations,
			  "a document over a throwing allocator reports the violation as a herbception");

void bounds() throws
{
	auto doc{::fast_io::json::parse<::fast_io::json::default_json_policy, throwing_document>(
		view(catalogue))};
	/* in range: no check is skipped, there is simply nothing to report */
	check(doc[0].kind == value_kind::object, view(u8"bounds: [] reads the root"));
	bool caught{};
	try
	{
		(void)doc[doc.values.size()];
	}
	catch throws(::std::error)
	{
		caught = true;
	}
	check(caught, view(u8"bounds: [] reports an out-of-range index as the allocator asked"));
	/* and the unchecked form is exactly that: no report, caller's responsibility */
	check(doc.index_unchecked(0).kind == value_kind::object,
		  view(u8"bounds: index_unchecked skips the check"));
}

} // namespace

int main()
{
	/* every section either succeeds or throws; one catch is enough, because the
	 * herbception channel carries the reason (parse_errc) and nothing else */
	try
	{
		dom();
		stream();
		records();
		from_file();
		errors();
		callback();
		coroutine();
		constant();
		kernel();
		bounds();
	}
	catch throws(::std::error)
	{
		std::printf("FAIL a section threw unexpectedly\n");
		++failures;
	}

	if (failures != 0)
	{
		std::printf("%zu FAILURES\n", failures);
		return 1;
	}
	std::printf("JSON OK\n");
	return 0;
}
