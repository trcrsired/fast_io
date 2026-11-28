# fast_io × safe C++ — vector experiment

This branch is a **diagnostic experiment**, not a finished safe fast_io. It uses
`fast_io::containers::vector` to find out what is wrong with our safe C++ rules
and what it would take to mark an existing container safe.

## What changed

`include/fast_io_dsal/impl/vector.h`:

- the container kernel (raw storage, pointers, internal helpers) is wrapped in
  a header-local `[[unsafe]] { ... }` fence. A fence is a **syntax relaxation**
  for its own source range; it is not a call restriction for the resulting API.
- iterator-based and unchecked operations carry `FAST_IO_DSAL_UNSAFE`
  (`[[unsafe]]` under `__SAFECXX__`, empty otherwise):
  `begin/end/cbegin/cend/rbegin/rend/crbegin/crend`, `data`,
  `insert/emplace/erase(const_iterator, ...)`, `*_unchecked`,
  `grow_*`/`move_backward_*` helpers.
- index-based and bounds-checked operations stay **safe** and are callable
  directly: `operator[]`, `front`, `back`, `size/capacity/max_size`,
  `push_back/emplace_back/pop_back`, `reserve/resize/clear`,
  `insert_index/emplace_index/erase_index/insert_range_index`, `swap`,
  `operator==`/`<=>`, `erase_if`.

`include/fast_io_dsal/vector.h`: deduction guides are fenced because they must
repeat a constructor's parameter types verbatim (a guide declares no runtime
entity, so it should arguably be exempt from the pointer/reference ban — see
below).

All new spellings are guarded by `__SAFECXX__`, so an ordinary compiler sees
plain C++.

## Running the experiment

The support headers are imported as legacy through a main-file fence, then the
vector itself is parsed under safe-source rules:

```cpp
[[unsafe]] {
#include <fast_io_dsal/impl/misc/push_macros.h>
#include <fast_io_dsal/impl/misc/push_warnings.h>
#include <fast_io_core.h>
#include <fast_io_dsal/impl/freestanding.h>
#include <fast_io_dsal/impl/common.h>
}
#include <fast_io_dsal/vector.h>
```

Build with the safe compiler (verbatim flag spellings):

```sh
clang++ --config=$HOME/cfgs/x86_64-linux-gnu-libcxx.cfg \
  -fsafecxx -fherbceptions -lherbceptions -O3 -s -flto=thin \
  safecxx_examples/vector_index.cc -o /tmp/vector_index
```

`safecxx_examples/vector_index.cc` compiles, links and runs successfully.

Calling an iterator API written in source from safe code is rejected:

```
error: call to 'begin' is unsafe: it is declared [[unsafe]] and may only be
  called from inside a [[unsafe]] block
error: call to 'data' is unsafe: ...
```

But a range-for does **not** need a fence — the loop itself stays safe:

```cpp
for (in auto x : v)   // OK: begin()/end() are compiler-generated
    sum += x;
v.begin();            // error: source-written call is restricted
[[unsafe]] { v.begin(); }   // OK
```

The range object is **frozen** for the loop's duration: any use of `v` inside
the body is an error in safe source. Put those uses in a fence:

```cpp
for (in auto x : v) {
    // v.push_back(4);   // error: v is frozen by the range-for
    [[unsafe]] { v.push_back(4); }   // OK
    sum += x;
}
```

This is a purely lexical rule (no provenance/dataflow/contracts) that rejects
the unprovable case of mutating a container while it is being iterated.

Derived from the same rule, the container's **move constructor, move
assignment, member `swap` and free `swap` are `[[unsafe]]`**: safe code cannot
move or swap a vector. `destructive_move` remains the safe consuming path
(where the type is trivially relocatable). This is much simpler than proving
that a move or swap did not invalidate any outstanding view.

### span and owner lifetime

`span.h` follows the same split: the kernel (raw pointer + length) is fenced;
iterator accessors, `*_unchecked` operations and byte views are `[[unsafe]]`;
checked `operator[]`, `front`/`back` and size queries stay callable.

The `from_range` constructor is declared in safe source as

```cpp
span(from_range_t, forward R range [[lifetime("this")]])
```

`forward` is the d0708 "pass along" parameter kind (D0708 R0 draft 32): it
preserves the range's cv-qualification and value category without writing
`&&`, and lowers to `R&`. `[[lifetime("this")]]` is the P1179 public provenance
mark; the compiler translates it to its internal lifetimebound attribute under
the lifetime-protocol toggle, so the Clang attribute never appears in source.
`vector` is marked `[[owner]]`: its pointer fields are owned by the object.
Note that a fenced `[[unsafe]] { ... }` region does **not** suppress the
lifetime analysis.

`span_dangling.cc` is the passing smoke test. `span_dangling_reject.cc`
documents the now-rejected cases:

- a local owner dying at `return` →
  `-Werror,-Wlifetime-safety-return-stack-addr`;
- a temporary owner → no viable constructor (`forward R` is `R&`, so rvalues
  do not bind — `&`/`&&` never changes the borrow semantics);
- iterator construction (`v.begin()`, `span(begin, end)`) → `[[unsafe]]` calls.

The `subspan` family also carries `[[clang::lifetimebound]]` on its implicit
object parameter, so a subspan inherits the borrow of the span it was cut from.
Clang cannot express "borrows what `*this` borrows" — only "borrows `*this`" —
so a subspan is tied to the span object as well as to its owner. Source rule:
do not return a subspan of a local span or by-value span parameter; return the
original span and let the caller cut it. GSL `[[gsl::Pointer]]` would avoid the
false positive, but GSL attributes are deliberately not part of this dialect.

### string_view, cstring_view and index_span

These three headers use the same split as `span.h`: a header-local `[[unsafe]]`
fence for the kernel, with `[[lifetime("this")]]` on the borrowing constructors
(`from_range`, array, cstring-view and manipulator conversions) and on the
`subview`/`subspan` families. The fence macros now live in
`impl/safecxx_marks.h`, shared with `span.h`; each definition is individually
guarded so a kernel that undefines them at its end cannot leave a later header
without them.

`view_families.cc` is the passing smoke test. `view_families_reject.cc`
documents the rejected cases: a view of a local owner returned from a function,
including through `subview`. Because these headers now fence themselves, they
can be included directly after the usual support-header fence.

All spellings are guarded by `__SAFECXX__`, so an ordinary compiler sees plain
C++ and no lifetime attributes.

### `print` without `[[unsafe]]`

`fast_io.h` self-fences its implementation under `__SAFECXX__`, so a normal
include with `-I` is enough — no `-isystem` and no fence in user code:

```sh
clang++ --config=$HOME/cfgs/x86_64-linux-gnu-libcxx.cfg \
  -std=c++26 -fsafecxx -fherbceptions -lherbceptions -O2 -s -Iinclude \
  safecxx_examples/print_hello.cc -o /tmp/print_hello
```

The fence is opened in the library header: it relaxes syntax for the imported
implementation but does not mark the public API unsafe-to-call, so `print` is
callable from safe source. `print_hello.cc` prints `hello world`. Lifetime
diagnostics are unaffected: the `[[lifetime("this")]]` marks in the DSAL
headers still classify views, so dangling views and owner-freeze violations are
still rejected in user code.

`view_invalidation_reject.cc` documents the owner-freeze rule: a non-const
member call on an owner is rejected while a live view borrows from it. The same
applies to named functions — a frozen owner may only be passed as `in T` or
copied by value; `out`, `inout`, `move`, `forward` and `destructive_move` are
rejected. The freeze follows liveness rather than a hardcoded list of container
mutators, and `[[unsafe]]` fences permit the call because it is a call
restriction.

## What this exposed about safe C++

Fixed in the compiler (`safecxx` branch of the LLVM fork):

1. Nine safe-mode rules (RTTI, legacy EH, virtual marks, rule-of-five, raw
   pointer/reference) used the parse-time fence depth, so they diagnosed
   system headers (libc++/herbceptions) and fenced headers whose templates were
   instantiated later.
2. A fence in a library header must not implicitly call-restrict its whole API;
   only a fence written in the main file around an `#include` is a legacy
   import. Otherwise a safe facade over an unsafe kernel is impossible.
3. A fence is a precise source range, not a whole file; code after the closing
   brace is checked again.
4. Macro-expanded fence braces had mismatched FileIDs, so a header-local fence
   failed to exempt its own kernel.
5. Range-for builds `begin()`/`end()` through `BuildCallExpr`, bypassing
   `ActOnCallExpr`; the `[[unsafe]]` restriction was therefore invisible to
   the `BuildForRangeBeginEndCall` check. Resolved by **deliberately exempting**
   compiler-generated `begin()`/`end()` calls: the iterator never escapes into
   safe source and the loop variable must be an `in`/`inout` borrow. A
   source-written `v.begin()` is still restricted.

Still open / design notes:

- Deduction guides cannot be written in safe source because they must mirror a
  constructor's `R &&` parameter. Arguably the raw-pointer/reference ban
  should skip deduction guides entirely (they declare no runtime entity).
- `in auto &x` is rejected while `in auto x` is accepted. The diagnostic is
  contradictory: the raw-reference rule says "use in/inout", the range-for
  rule says "use 'in auto'". Either accept the explicit `&` or give one clear
  message ("write `in auto` without `&`").
- The driver config includes fast_io with `-I`. Every public header now
  declares its own trust with a library-header fence, so the whole library is
  usable directly from safe source (see `print_hello.cc`, `include_layers.cc`).
  A fence in a library header is a **syntax relaxation for the implementation**,
  not a call restriction for the API.

## Whole-library classification

All public headers (`fast_io_concept.h`, `fast_io_core.h`,
`fast_io_freestanding.h`, `fast_io_hosted.h`, `fast_io_crypto.h`,
`fast_io_i18n.h`, `fast_io_legacy.h`, `fast_io_hosted_crypto.h`, `fast_io.h`,
and the `fast_io_dsal/*.h` family) open `FAST_IO_SAFECXX_IMPL_BEGIN` from
`fast_io_core_impl/safecxx_fence.h` under `__SAFECXX__`. That single fence
covers every transitive implementation header, including all uses of the C++
standard library (whose iterator/pointer APIs are inherently unsafe) — no
`-isystem`, no facade header, no user-written fence.

Public APIs are then classified:

- **Iterator / pointer+length APIs are `[[unsafe]]`** (`FAST_IO_UNSAFE_API` from
  `fast_io_core_impl/safecxx_attr.h`). A function taking an iterator pair or a
  pointer plus a length carries no owner a borrow can be tied to. In the
  algorithm family this covers the `*_iters`, `*_pointer` and `*_ptr` kernels
  (322 marks); in the containers it is the existing iterator/unchecked split
  in `vector.h`, `span.h`, `string_view.h`, `cstring_view.h` and
  `index_span.h`. A source-written call is rejected unless it is inside a
  fence; see `include_layers_reject.cc`.
- **Range-based APIs, index-based and checked access stay safe** — `count(v, x)`
  works from safe source, `count_iters(first, last, x)` does not.
- **C++ concepts and `consteval` functions are automatically safe** — they are
  compile-time / unevaluated constructs, so they are never marked.

`include_layers.cc` includes every layer and uses a range-based algorithm from
safe source; `include_layers_reject.cc` documents a rejected iterator call.

Algorithms follow the same rule: iterator-taking kernels (`*_iters`,
`*_pointer`, `*_ptr`) and range overloads that **return an iterator** are
`[[unsafe]]` (36 additional range overloads); index-based overloads and pure
range algorithms that return a value stay safe. Index access requires a
random-access range, so node-based containers (`list`, `forward_list`) have no
safe index API and remain iterator-only — their `begin()` is call-restricted.
See `algorithms_range.cc` and `algorithms_range_reject.cc`.

All container families are classified the same way as `vector`/`span`:
`array`, `bitvec`, `deque`, `forward_list`, `list`, `priority_queue`, `queue`,
`sized_string`, `sized_vector`, `stack`, the btree/swiss map and set families,
and their shared helpers carry `FAST_IO_UNSAFE_API` on iterator accessors
(`begin`/`end`/`cbegin`/.../`data`), `*_unchecked` operations, iterator- and
pointer+length-taking operations, move construction/assignment and `swap`.
Index/checked access, `push_back`/`pop_back`, size queries, checked insert/erase
and range-for stay safe (compiler-generated `begin`/`end` calls are exempt).
`containers_classify.cc` and `containers_classify_reject.cc` demonstrate both
sides.

### `scan` and expression-only manipulators

`scan` and `scan_some` take `FAST_IO_SCAN_INOUT ...` (`inout Args...` under
`__SAFECXX__`, `Args&&...` in plain C++): a scan target is a read/write borrow,
not a read-only one. `out` was rejected by the definite-assignment rule because
the forwarding implementation reads the argument through the alias machinery
before writing it. The `io_scan_may_throw` noexcept probe likewise names an
lvalue in safe mode, matching the lowered `inout` parameter. With this, the
10-million-integer `benchmark/0002.file_io_tsc/time/iobuf_file.cc` compiles,
links and runs in safe source, producing byte-identical output to plain C++.

C-string manipulator factories that take raw pointers (`os_c_str(p)`,
`os_c_str_with_known_size(p, n)`, `strvw(first, last)`, `code_cvt_os_c_str(p)`,
…) are `[[unsafe]]`: a C-style string or iterator pair carries no owner. The
array overload `os_c_str_carr("literal")` stays callable from safe source.
Their result types carry `FAST_IO_LIFETIME_EXPRESSION`
(`[[lifetime_expression]]`): they may only exist as temporaries inside an
expression. Storing one in a variable or field is rejected. This applies to
**every** manipulator in fast_io — the C-string family plus all types
satisfying the library's `manipulator` concept (`scalar_manip_t`, `chvw_t`,
`code_cvt_t`, `parameter`, the width/percentage/IP/date/hash/serialization
manipulators, …), 34 types in total. Driver types are excluded. See
`manipulator_expression.cc` and `manipulator_expression_reject.cc`. This is a
lifetime property of the type, not an `[[unsafe]]` fence or call restriction.

### How much of a real program is safe

`construct_fstream_from_syscall.cc` is the safe-C++ version of the upstream
`examples/0007.legacy` program. Compiled verbatim it produces exactly **two**
diagnostics, both on one statement:

```cpp
*fout.rdbuf() = std::move(*fbf.fb);
```

Moving a `std::filebuf` across the fast_io/legacy boundary requires
dereferencing two opaque `streambuf*` pointers, so only that statement is
wrapped in `[[unsafe]] { ... }`. Everything else is safe source: syscall file
construction (`posix_file`, `c_file`, `filebuf_file`) and their move chains,
`std::ofstream`, `filebuf_io_observer`, `print`/`println` with timestamps,
UTC/local time, manipulators, handle views and observer casts. The fenced
version builds, links and runs, printing the same report as plain C++. The
`_l10n` variant has the same two diagnostics on its equivalent line.
