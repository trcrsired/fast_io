# fast_io.json

`parse.cc` is both the usage guide and its own test: ten sections, one entry
point each, every one of them a check. Build and run with `./build.sh`; it ends
in `JSON OK` or lists what failed.

The module is the only artifact — there is no header to include. Precompile
`share/fast_io/fast_io_json.cppm` once (build.sh does it) and import it.

```cpp
#include <herbceptions/error>   // catch throws(::std::error) needs it
import fast_io.json;
```

## Strings are always sized

Every string in and out is a `::fast_io::u8string_view`. The parser never scans
for a terminator, so nothing is ever handed a bare pointer:

```cpp
template <::std::size_t n>
[[nodiscard]] inline constexpr ::fast_io::u8string_view view(char8_t const (&text)[n]) noexcept
{
	return ::fast_io::u8string_view{text, n - 1};
}

inline constexpr char8_t text[]{u8R"({"count":2})"};
auto doc{::fast_io::json::parse(view(text))};
```

A string without a backslash is a view into your buffer — no allocation at all.
Only an escaped string is decoded, into an arena the document owns.

## 1. The DOM

```cpp
auto doc{::fast_io::json::parse(source)};       // throws on a bad document
auto const &root{doc.root()};                   // value_kind::object/array/...

auto const *apps{doc.object_at(root, view(u8"apps"))};   // linear scan, pointer or null
auto const *tags{doc.object_at(*apps, ...)};
auto const *first_tag{doc.array_at(*apps, 0)};           // nth child, pointer or null

for (::std::uint_least32_t i{apps->first_child}; i != ::fast_io::json::no_index;)
{
	auto const &app{doc.index_unchecked(i)};    // [] is the checking form
	i = doc.next_sibling(app);
}
```

`value` carries a `kind` next to a union payload. Read the kind, then the arm:

```cpp
switch (v.kind)
{
case ::fast_io::json::value_kind::integer: v.as_uint64(); v.as_int64(); break;
case ::fast_io::json::value_kind::real:    v.as_float64(); break;
case ::fast_io::json::value_kind::string:  v.as_string(); break;
case ::fast_io::json::value_kind::boolean: v.as_bool(); break;
default: break;                                  // null, array, object
}
```

`4.5` is a real, `4` is an integer; negatives are stored two's complement in
the unsigned slot, so `as_int64()` reads them back and the whole `uint64` range
survives.

Indexing follows the containers. `doc[i]` checks, `doc.index_unchecked(i)` does
not, and *whether a violation throws or terminates is the allocator's flag*,
not a policy invented here:

```cpp
// the default: fail fast, exactly like ::fast_io::vector
::fast_io::json::document doc{::fast_io::json::parse(source)};

// ask for a herbception instead
using throwing_allocator = ::fast_io::generic_allocator_adapter<
	::fast_io::native_global_allocator,
	::fast_io::allocator_adapter_flags::throws_on_violations>;
using throwing_document = ::fast_io::json::basic_document<throwing_allocator>;
static_assert(throwing_document::throwing_violations);

auto checked{::fast_io::json::parse<::fast_io::json::default_json_policy, throwing_document>(source)};
checked[checked.values.size()];        // throws ::std::errc::invalid_argument
```

## 2. Streaming, without a tree

A handler is anything with these twelve members, every one `void f(...) throws`:

```cpp
struct counter
{
	void on_null() throws;
	void on_bool(bool) throws;
	void on_uint64(::std::uint_least64_t) throws;
	void on_int64(::std::int_least64_t) throws;
	void on_float64(::fast_io::json::float64_type) throws;
	void on_string(::fast_io::u8string_view) throws;
	void on_array_begin() throws;
	void on_array_end(::std::size_t) throws;
	void on_object_begin() throws;
	void on_object_end(::std::size_t) throws;
	void on_key(::fast_io::u8string_view) throws;
};

counter h{};
::std::size_t consumed{::fast_io::json::parse_into(source, h)};   // == source.size()
```

Matched by duck typing: no base class, no virtual call, and no `bool
on_x(..., error_code&)` slot — a malformed document leaves through the
herbception channel from wherever it was found, so the callbacks stay `void`.

Keeping state (`in_object`, "which key am I under") is the caller's job in this
form. That is the trade: the DOM is easier, the handler allocates no tree.

**A view handed to `on_string` is only valid for the duration of the call.**
`parse_into` owns a local arena and destroys it on return, so copy anything you
keep. The DOM does not have that problem — it owns its arena.

## 3. A file

```cpp
auto loaded{::fast_io::json::parse_file(path)};   // mmap, nothing copied
value const *v{loaded->object_at(loaded->root(), view(u8"count"))};
```

`loaded` holds the loader *and* the document, loader first, so the mapping
outlives the views into it. The source buffer always has to outlive the
document; `parse_file` is how that is made safe.

## 4. Errors

```cpp
try { auto doc{::fast_io::json::parse(source)}; }
catch throws(::std::error) { /* parse_errc::invalid / end_of_file / overflow */ }
```

Three errors, all `::fast_io::freestanding::parse_errc`: `invalid` (malformed),
`end_of_file` (truncated), `overflow` (deeper than `max_depth`, more than
`max_nodes` nodes, or an integer out of range). A thrown value carries no
position, so when the offset matters drive the parser yourself — `parser` and
`parse_document` are public:

```cpp
::fast_io::json::details::string_arena arena{};
::fast_io::json::parser<> p{source, __builtin_addressof(arena)};
try { ::fast_io::json::parse_document(p, handler); }
catch throws(::std::error) { offset = p.error_offset; }
```

## 5. Callback, for code that cannot throw

```cpp
::fast_io::json::parse_callback(source, handler, [](::std::cxx_std_error e) noexcept {
	if (e.domain == nullptr) { /* success */ }
	else { ::fast_io::json::dispose_error(e); }   // the callback owns the handle
});
```

One call, exactly once either way, `domain == nullptr` on success. Ownership of
the error handle transfers to the callback, which is how the parse itself stays
`noexcept` and still loses nothing.

## 6. Coroutine

Same contract as fast_io's async I/O, so JSON sits in the same `co_await ...
throws` shape:

```cpp
auto doc{co_await ::fast_io::json::parse_document_async(source)};
co_await ::fast_io::json::parse_async(source, handler);
```

Parsing is CPU work with no I/O, so the awaiter runs it inline in
`await_suspend()` and returns `false` — awaiting it never actually suspends. A
failure is rethrown from `await_resume()`, and the awaiter's destructor releases
an error payload nobody consumed.

## 7. Constant evaluation

The arena is a template parameter, so a `constexpr` caller brings a fixed one
and nothing touches the heap:

```cpp
constexpr ::std::size_t value_count() throws
{
	::fast_io::json::details::fixed_string_arena<128> arena{};
	my_handler h{};
	(void)::fast_io::json::parse_into(view(u8R"({"k":[1,2]})"), h, arena);
	return h.objects;
}
static_assert(value_count() == 1u);
```

## 8. Kernel / no FPU

`json_policy` is a flag enum, and `floating_point` is one bit:

```cpp
auto doc{::fast_io::json::parse<::fast_io::json::json_policy::none>(source)};
```

Without it the real-number arm and the decimal conversion table are
`if constexpr`-ed away — no FPU instruction and no converter linked. A number
with a fraction or an exponent is then rejected as `invalid`, while integers
parse identically, the full `uint64` range included.

## What is not here

There is no writer and no number formatting: `fast_io` already prints, and a
second printer would only be a second thing to keep in step.

## Limits worth knowing

| | |
|---|---|
| `max_depth` | 512 |
| `max_nodes` | `0xFFFFFFF0` |
| node size | 32 bytes, 32-bit child/sibling indices |
| reals | need `json_policy::floating_point`; conversion is in-module, no `fast_io.floating` dependency |
| GCC | does not build this module (ICE at `cp/module.cc`), use clang with `-fherbceptions` |
