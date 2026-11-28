// Safe C++ facade for fast_io's core I/O (`print`, `println`, `scan`, ...).
//
// Under -fsafecxx, this header imports fast_io.h's implementation as
// trusted-unsafe with a library-header [[unsafe]] fence. Because the fence is
// opened in a library header rather than the main file, the declarations are
// exempt from safe-source checks but are NOT marked unsafe-to-call. User code
// can therefore write plain safe C++ with no [[unsafe]] of its own:
//
//   #include <fast_io_safecxx_safe.h>
//   int main() { fast_io::print("hello world\n"); }
//
// The view/lifetime diagnostics are unaffected: [[lifetime("this")]] marks in
// the DSAL headers still classify span/string_view as views, so dangling views
// and owner-freeze violations are still rejected in user code.
//
// In plain C++ the fence disappears and this is an ordinary fast_io.h include.
#pragma once

#ifdef __SAFECXX__
[[unsafe]] {
#endif

#include <fast_io.h>

#ifdef __SAFECXX__
}
#endif
