#pragma once
// Self-fence for fast_io's public umbrella headers under the safe dialect.
//
// Each umbrella header opens this fence around the implementation it includes.
// The fence relaxes safe-source syntax (raw pointers/references, dereference,
// new/delete) for the imported code. Because the fence lives in a library
// header, it does NOT call-restrict the public API the way a fence in the main
// file would: safe users can call print/scan/... without writing [[unsafe]].
//
// In plain C++ both macros are empty, so the headers are unchanged.
#if defined(__SAFECXX__)
#define FAST_IO_SAFECXX_IMPL_BEGIN [[unsafe]] {
#define FAST_IO_SAFECXX_IMPL_END }
#else
#define FAST_IO_SAFECXX_IMPL_BEGIN
#define FAST_IO_SAFECXX_IMPL_END
#endif
