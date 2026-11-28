#pragma once
// Public `[[unsafe]]` mark for APIs that cannot be safe source: a function
// taking an iterator or a pointer+length pair carries no owner its borrow can
// be tied to, so a safe caller must fence the call.
//
// C++ concepts and consteval functions are automatically safe (they are
// compile-time only / unevaluated) and are never marked with this macro.
// A library-header [[unsafe]] fence relaxes syntax for the implementation but
// does not call-restrict the API; this attribute is what restricts calls.
#if defined(__SAFECXX__)
#define FAST_IO_UNSAFE_API [[unsafe]]
// Expression-only manipulator result: legal as a temporary inside a call,
// illegal as a stored variable or field in safe source.
#define FAST_IO_LIFETIME_EXPRESSION [[lifetime_expression]]
#else
#define FAST_IO_UNSAFE_API
#define FAST_IO_LIFETIME_EXPRESSION
#endif
