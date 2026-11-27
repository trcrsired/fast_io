// Safe C++ experiment: the DSAL kernels store opaque pointers and name raw
// reference types. They live in an [[unsafe]] fence so the implementation is
// trusted, while the public borrow relationships are stated explicitly with
// the P1179 mark [[lifetime("this")]].
//
// `forward` is the d0708 (draft 32) parameter kind for an argument the callee
// passes along. It keeps a range's constness without a const_cast. In plain
// C++ it expands to an ordinary forwarding reference; the lifetime mark
// expands to nothing.
//
// No `#pragma once` here: some kernels (vector.h) undef these macros when they
// finish. Each definition is individually guarded so re-including this header
// restores whatever an earlier header removed.
#if defined(__SAFECXX__)
#ifndef FAST_IO_DSAL_UNSAFE
#define FAST_IO_DSAL_UNSAFE [[unsafe]]
#endif
#ifndef FAST_IO_DSAL_KERNEL_BEGIN
#define FAST_IO_DSAL_KERNEL_BEGIN [[unsafe]] {
#endif
#ifndef FAST_IO_DSAL_KERNEL_END
#define FAST_IO_DSAL_KERNEL_END }
#endif
#ifndef FAST_IO_DSAL_FORWARD
#define FAST_IO_DSAL_FORWARD forward
#endif
#ifndef FAST_IO_DSAL_FWD_TYPE
#define FAST_IO_DSAL_FWD_TYPE(R) R
#endif
#ifndef FAST_IO_DSAL_LIFETIME
#define FAST_IO_DSAL_LIFETIME [[lifetime("this")]]
#endif
#else
#ifndef FAST_IO_DSAL_UNSAFE
#define FAST_IO_DSAL_UNSAFE
#endif
#ifndef FAST_IO_DSAL_KERNEL_BEGIN
#define FAST_IO_DSAL_KERNEL_BEGIN
#endif
#ifndef FAST_IO_DSAL_KERNEL_END
#define FAST_IO_DSAL_KERNEL_END
#endif
#ifndef FAST_IO_DSAL_FORWARD
#define FAST_IO_DSAL_FORWARD
#endif
#ifndef FAST_IO_DSAL_FWD_TYPE
#define FAST_IO_DSAL_FWD_TYPE(R) R &&
#endif
#ifndef FAST_IO_DSAL_LIFETIME
#define FAST_IO_DSAL_LIFETIME
#endif
#endif
