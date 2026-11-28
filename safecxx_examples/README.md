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

The core I/O API (`fast_io.h`) can be used from safe source with **no fence in
user code**. The implementation is trusted-unsafe; only the user's calls are
checked. Two equivalent trust declarations:

- **`#include <fast_io_safecxx_safe.h>`** (with plain `-I`). This facade opens a
  `[[unsafe]]` fence in a *library header* around `<fast_io.h>`. A fence opened
  in a header (not the main file) relaxes syntax for the implementation but
  does **not** mark its declarations unsafe-to-call, so `print` is callable.
- **`-isystem /path/to/fast_io/include`** with a plain `#include <fast_io.h>`.
  The dialect already exempts system headers from safe-source checks and does
  not implicitly call-restrict functions declared there.

`print_hello.cc` demonstrates the facade form; it compiles, links and runs with
`-Iinclude` and prints `hello world`. Lifetime diagnostics are unaffected: the
`[[lifetime("this")]]` marks in the DSAL headers still classify views, so
dangling views and owner-freeze violations are still rejected in user code.

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
- Making fast_io safe for real still requires marking its *other* headers:
  `freestanding/herbceptions.h`, `freestanding/addressof.h`,
  `concepts/type.h`, etc. The isolated run produced 204 diagnostics, all in
  the vector implementation once those support headers were fenced.
- The driver config includes fast_io with `-I`, so fast_io is not a system
  header. Fencing it is the user's trust declaration; system headers stay
  directly usable. `fast_io_safecxx_safe.h` provides the header-local fence for
  core I/O, so user code needs no fence at all (see `print_hello.cc`).
