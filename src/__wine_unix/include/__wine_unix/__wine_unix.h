#pragma once
/*
The shared contract between the PE side (include/fast_io_hosted/platforms/wine/wine.h)
and the host unixlib (src/__wine_unix/unix/unixhost.cc).

Nothing is called directly across the PE <-> unix boundary. Every host libc call is
wrapped as a unixcall:

	__wine_unix_status_t (*)(void *args)

where args points at a "params" struct. Inputs go in, outputs are written back through
the struct, and the return value is the status (0 = success, otherwise an errno value
from __wine_unix_errno.h).

The params struct is the real ABI contract between the two compilers (mingw/MSVC PE vs
host clang/gcc), so its layout is frozen here. For wow64 (a 32-bit PE on a 64-bit host)
we also define *_params32 variants, explicitly packed so every toolchain agrees on the
layout; the 64-bit unixlib's __wine_unix_call_wow64_funcs[] wrappers marshal between
them.

On the PE side the call goes through ntdll's __wine_unix_call_dispatcher, which does the
whole context save (registers, stack, TLS) before entering the unixlib. See the wine
source: include/wine/unixlib.h and dlls/ntdll/unix/signal_x86_64.c
(__wine_unix_call_dispatcher). The unixlib is loaded by name with
NtQueryVirtualMemory(GetCurrentProcess(), &name, 1002, ...) which does a plain dlopen of
the .so and dlsym's "__wine_unix_call_funcs" (see dlls/ntdll/unix/virtual.c).
*/

#include <stdint.h>
#include <limits.h>
#include <stddef.h>

#if defined(_WIN32) && !defined(__WINE__) || defined(__CYGWIN__)
#define __WINE_UNIX_DEFAULTCALL __stdcall
#define __WINE_UNIX_DLLEXPORT __declspec(dllexport)
#elif defined(__GNUC__) || defined(__clang__)
#define __WINE_UNIX_DEFAULTCALL
#define __WINE_UNIX_DLLEXPORT __attribute__((visibility("default")))
#else
#define __WINE_UNIX_DEFAULTCALL
#define __WINE_UNIX_DLLEXPORT
#endif

#ifdef __cplusplus
#if __cplusplus <= 201107
#define __WINE_UNIX_NOEXCEPT throw()
#else
#define __WINE_UNIX_NOEXCEPT noexcept
#endif
extern "C"
{
#else
#define __WINE_UNIX_NOEXCEPT
#endif

#if INTPTR_MAX < INT_LEAST32_MAX
	typedef uint_least32_t __wine_host_fd_t;
	typedef uint_least32_t __wine_errno_t;
#else
	typedef size_t __wine_host_fd_t;
	typedef size_t __wine_errno_t;
#endif
#if INTPTR_MAX < INT_LEAST64_MAX
	typedef int_least64_t __wine_off_t;
#else
	typedef ptrdiff_t __wine_off_t;
#endif

	typedef __wine_host_fd_t __wine_host_flags_t;
	typedef __wine_host_fd_t __wine_host_mode_t;

	/* 32-bit pointer value, used to carry wow64 pointers across the boundary. */
	typedef uint32_t __wine_unix_ptr32_t;

	/* matches struct iovec layout on every unix host. */
	typedef struct
	{
		void const *iov_base;
		size_t iov_len;
	} __wine_unix_iovec_t;

	/* uniform status/return type: 0 = success, otherwise an errno value. */
	typedef int_least32_t __wine_unix_status_t;

	/* opaque handle to the unixlib call table. always 64-bit so it can cross wow64. */
	typedef uint_least64_t __wine_unixlib_handle_t;

	/* every unixcall has this signature; args always points at a params struct. */
	typedef __wine_unix_status_t (*__wine_unixlib_entry_t)(void *args);

	/*
	Call codes. The order MUST match the __wine_unix_call_funcs and
	__wine_unix_call_wow64_funcs tables in unixhost.cc.
	*/
	enum __wine_unix_funcs
	{
		__wine_unix_host_fd_to_unix_fd,
		__wine_unix_unix_fd_to_host_fd,
		__wine_unix_host_fd_to_nt_handle,
		__wine_unix_nt_handle_to_host_fd,
		__wine_unix_openat,
		__wine_unix_close,
		__wine_unix_writev,
		__wine_unix_readv,
		__wine_unix_pwritev,
		__wine_unix_preadv,
		__wine_unix_funcs_count,
	};

	/* host fds are stored with a +1 offset: 0 means "no fd". */

	typedef struct
	{
		__wine_host_fd_t host_fd;
		int unix_fd; /* output */
	} __wine_unix_host_fd_to_unix_fd_params;

	typedef struct
	{
		int unix_fd;
		__wine_host_fd_t host_fd; /* output */
	} __wine_unix_unix_fd_to_host_fd_params;

	typedef struct
	{
		__wine_host_fd_t host_fd;
		ptrdiff_t handle; /* output */
	} __wine_unix_host_fd_to_nt_handle_params;

	typedef struct
	{
		ptrdiff_t handle;
		__wine_host_fd_t host_fd; /* output */
	} __wine_unix_nt_handle_to_host_fd_params;

	typedef struct
	{
		__wine_host_fd_t host_dirfd; /* 0 means AT_FDCWD */
		char const *filename;
		size_t filenamelen;
		__wine_host_flags_t flags;
		__wine_host_mode_t mode;
		__wine_host_fd_t host_fd; /* output */
	} __wine_unix_openat_params;

	typedef struct
	{
		__wine_host_fd_t host_fd;
	} __wine_unix_close_params;

	typedef struct
	{
		__wine_host_fd_t host_fd;
		__wine_unix_iovec_t const *iovs;
		size_t iovsize;
		size_t total;	  /* output: bytes transferred */
		size_t baseindex; /* output: iovec index the transfer stopped at */
		size_t index;	  /* output: bytes consumed within that iovec */
	} __wine_unix_readwritev_params;

	typedef struct
	{
		__wine_host_fd_t host_fd;
		__wine_unix_iovec_t const *iovs;
		size_t iovsize;
		__wine_off_t offset;
		size_t total;	  /* output */
		size_t baseindex; /* output */
		size_t index;	  /* output */
	} __wine_unix_preadwritev_params;

	/*
	wow64 (32-bit PE on a 64-bit host) variants. Explicitly packed so the layout is
	identical under MSVC x86, mingw x86 and the 64-bit unixlib reader.
	*/
#pragma pack(push, 1)
	typedef struct
	{
		__wine_unix_ptr32_t host_fd;
		int32_t unix_fd;
	} __wine_unix_host_fd_to_unix_fd_params32;

	typedef struct
	{
		int32_t unix_fd;
		__wine_unix_ptr32_t host_fd;
	} __wine_unix_unix_fd_to_host_fd_params32;

	typedef struct
	{
		__wine_unix_ptr32_t host_fd;
		int32_t handle;
	} __wine_unix_host_fd_to_nt_handle_params32;

	typedef struct
	{
		int32_t handle;
		__wine_unix_ptr32_t host_fd;
	} __wine_unix_nt_handle_to_host_fd_params32;

	typedef struct
	{
		__wine_unix_ptr32_t host_dirfd;
		__wine_unix_ptr32_t filename;
		__wine_unix_ptr32_t filenamelen;
		__wine_unix_ptr32_t flags;
		__wine_unix_ptr32_t mode;
		__wine_unix_ptr32_t host_fd;
	} __wine_unix_openat_params32;

	typedef struct
	{
		__wine_unix_ptr32_t host_fd;
	} __wine_unix_close_params32;

	typedef struct
	{
		__wine_unix_ptr32_t host_fd;
		__wine_unix_ptr32_t iovs;
		uint32_t iovsize;
		uint32_t total;
		uint32_t baseindex;
		uint32_t index;
	} __wine_unix_readwritev_params32;

	typedef struct
	{
		__wine_unix_ptr32_t host_fd;
		__wine_unix_ptr32_t iovs;
		uint32_t iovsize;
		__wine_off_t offset;
		uint32_t total;
		uint32_t baseindex;
		uint32_t index;
	} __wine_unix_preadwritev_params32;
#pragma pack(pop)

	/* arch-selected params types: what a given side actually builds/passes. */
#if INTPTR_MAX < INT64_MAX
	typedef __wine_unix_host_fd_to_unix_fd_params32 __wine_unix_host_fd_to_unix_fd_params_t;
	typedef __wine_unix_unix_fd_to_host_fd_params32 __wine_unix_unix_fd_to_host_fd_params_t;
	typedef __wine_unix_host_fd_to_nt_handle_params32 __wine_unix_host_fd_to_nt_handle_params_t;
	typedef __wine_unix_nt_handle_to_host_fd_params32 __wine_unix_nt_handle_to_host_fd_params_t;
	typedef __wine_unix_openat_params32 __wine_unix_openat_params_t;
	typedef __wine_unix_close_params32 __wine_unix_close_params_t;
	typedef __wine_unix_readwritev_params32 __wine_unix_readwritev_params_t;
	typedef __wine_unix_preadwritev_params32 __wine_unix_preadwritev_params_t;
#else
	typedef __wine_unix_host_fd_to_unix_fd_params __wine_unix_host_fd_to_unix_fd_params_t;
	typedef __wine_unix_unix_fd_to_host_fd_params __wine_unix_unix_fd_to_host_fd_params_t;
	typedef __wine_unix_host_fd_to_nt_handle_params __wine_unix_host_fd_to_nt_handle_params_t;
	typedef __wine_unix_nt_handle_to_host_fd_params __wine_unix_nt_handle_to_host_fd_params_t;
	typedef __wine_unix_openat_params __wine_unix_openat_params_t;
	typedef __wine_unix_close_params __wine_unix_close_params_t;
	typedef __wine_unix_readwritev_params __wine_unix_readwritev_params_t;
	typedef __wine_unix_preadwritev_params __wine_unix_preadwritev_params_t;
#endif

#ifdef WINE_UNIX_LIB
	/* unixlib (unixhost.cc) exports: */
	__WINE_UNIX_DLLEXPORT __wine_unixlib_entry_t const __wine_unix_call_funcs[];
	__WINE_UNIX_DLLEXPORT __wine_unix_status_t __wine_unix_lib_init(void) __WINE_UNIX_NOEXCEPT;
#if INTPTR_MAX >= INT64_MAX
	__WINE_UNIX_DLLEXPORT __wine_unixlib_entry_t const __wine_unix_call_wow64_funcs[];
#endif
#else
	/* PE side: ntdll-provided dispatcher. */
	typedef __wine_unix_status_t (__WINE_UNIX_DEFAULTCALL *__wine_unix_call_dispatcher_t)(__wine_unixlib_handle_t, unsigned int, void *);

	extern __wine_unixlib_handle_t __wine_unixlib_handle;
	extern __wine_unix_call_dispatcher_t __wine_unix_call_dispatcher;

	static inline __wine_unix_status_t __wine_unix_call(unsigned int code, void *args) __WINE_UNIX_NOEXCEPT
	{
		return __wine_unix_call_dispatcher(__wine_unixlib_handle, code, args);
	}

#define __WINE_UNIX_CALL(code, args) __wine_unix_call((code), (args))
#endif

#ifdef __cplusplus
}
#endif
