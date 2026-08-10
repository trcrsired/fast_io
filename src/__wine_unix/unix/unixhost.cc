/*
The unix side of the fast_io wine_file layer. Compiled as a normal native shared
library (clang/gcc, -fPIC -shared) with -DWINE_UNIX_LIB; no wine toolchain needed.

Wine's loader dlopen's this .so and dlsym's "__wine_unix_call_funcs" (and
"__wine_unix_call_wow64_funcs" for a 32-bit PE on a 64-bit host) when the PE side asks
for it through NtQueryVirtualMemory(GetCurrentProcess(), &name, MemoryWineLoadUnixLibByName, ...).
See dlls/ntdll/unix/virtual.c in the wine source.
*/

#define WINE_UNIX_LIB 1
#include <__wine_unix/__wine_unix.h>
#include <__wine_unix/__wine_unix_errno.h>

#include <errno.h>
#include <fcntl.h>
#include <limits>
#include <stdlib.h>
#include <string.h>
#include <sys/uio.h>
#include <unistd.h>

namespace __wine_unix
{
namespace
{

inline __wine_unix_status_t host_fd_to_unix_fd(__wine_host_fd_t host_fd, int &unix_fd) noexcept
{
	unix_fd = -1;
	if (host_fd == 0)
	{
		return __WINE_UNIX_ERRNO_EBADF;
	}
	--host_fd;
	constexpr __wine_host_fd_t intmx{static_cast<__wine_host_fd_t>(::std::numeric_limits<int>::max())};
	if (intmx < host_fd)
	{
		return __WINE_UNIX_ERRNO_EBADF;
	}
	unix_fd = static_cast<int>(host_fd);
	return __WINE_UNIX_ERRNO_SUCCESS;
}

inline __wine_host_fd_t unix_fd_to_host_fd(int unix_fd) noexcept
{
	if (unix_fd < 0)
	{
		return 0;
	}
	return static_cast<__wine_host_fd_t>(unix_fd) + 1;
}

inline __wine_unix_status_t host_errno_to_wine_errno(int val) noexcept
{
	/*
	Todo: proper per-platform mapping. On linux the values already match
	the ones in __wine_unix_errno.h.
	*/
	return static_cast<__wine_unix_status_t>(val);
}

struct c_path_malloc_guard
{
	__wine_errno_t host_errno{__WINE_UNIX_ERRNO_EINVAL};
	char *filename_c_str{};
	c_path_malloc_guard() noexcept = default;
	explicit c_path_malloc_guard(__wine_errno_t err, char *ptr) noexcept
		: host_errno{err}, filename_c_str{ptr}
	{}
	c_path_malloc_guard(c_path_malloc_guard const &) = delete;
	c_path_malloc_guard &operator=(c_path_malloc_guard const &) = delete;
	~c_path_malloc_guard()
	{
		free(filename_c_str);
	}
};

inline c_path_malloc_guard c_path_common(char const *filename, size_t filenamelen) noexcept
{
	if (filenamelen == SIZE_MAX)
	{
		return {};
	}
	if (__builtin_strnlen(filename, filenamelen) != filenamelen)
	{
		return {};
	}
	char *newmem = static_cast<char *>(malloc(filenamelen + 1));
	if (newmem == nullptr)
	{
		return {__WINE_UNIX_ERRNO_ENOMEM, nullptr};
	}
	if (filenamelen)
	{
		memcpy(newmem, filename, filenamelen);
	}
	newmem[filenamelen] = 0;
	return {__WINE_UNIX_ERRNO_SUCCESS, newmem};
}

inline __wine_unix_status_t readwritev_result_common_split(ssize_t ret, __wine_unix_iovec_t const *iovs, size_t iovsize,
														   size_t &total, size_t &baseindex, size_t &index) noexcept
{
	if (ret == -1)
	{
		return host_errno_to_wine_errno(errno);
	}
	size_t uret{static_cast<size_t>(ret)};
	total = uret;
	baseindex = 0;
	index = 0;
	if (iovsize)
	{
		auto *const iovsed{iovs + iovsize};
		auto *i{iovs};
		size_t lastn{uret};
		for (; i != iovsed; ++i)
		{
			size_t const ilen{i->iov_len};
			if (lastn < ilen)
			{
				break;
			}
			lastn -= ilen;
		}
		baseindex = static_cast<size_t>(i - iovs);
		index = lastn;
	}
	return __WINE_UNIX_ERRNO_SUCCESS;
}

inline __wine_unix_iovec_t const *iovs_from_params(auto const &params) noexcept
{
	return reinterpret_cast<__wine_unix_iovec_t const *>(static_cast<::std::uintptr_t>(params.iovs));
}

template <typename Fn>
inline __wine_unix_status_t readwritev_common(__wine_unix_readwritev_params_t *params, Fn fn) noexcept
{
	int unix_fd{-1};
	if (auto const errcode{host_fd_to_unix_fd(params->host_fd, unix_fd)}; errcode)
	{
		return errcode;
	}
	size_t iovsize{static_cast<size_t>(params->iovsize)};
	constexpr int mxval{
#ifdef IOV_MAX
		IOV_MAX
#else
		INT_MAX
#endif
	};
	if (mxval < iovsize)
	{
		iovsize = mxval;
	}
	__wine_unix_iovec_t const *const iovs{iovs_from_params(*params)};
	auto ret = fn(unix_fd, reinterpret_cast<struct iovec const *>(iovs), static_cast<int>(iovsize));
	size_t total{};
	size_t baseindex{};
	size_t index{};
	auto const errcode{readwritev_result_common_split(ret, iovs, iovsize, total, baseindex, index)};
	params->total = static_cast<decltype(params->total)>(total);
	params->baseindex = static_cast<decltype(params->baseindex)>(baseindex);
	params->index = static_cast<decltype(params->index)>(index);
	return errcode;
}

template <typename Fn>
inline __wine_unix_status_t preadwritev_common(__wine_unix_preadwritev_params_t *params, Fn fn) noexcept
{
	if constexpr (sizeof(off_t) < sizeof(__wine_off_t))
	{
		constexpr off_t off_min{::std::numeric_limits<off_t>::min()};
		constexpr off_t off_max{::std::numeric_limits<off_t>::max()};
		if (params->offset < off_min || off_max < params->offset)
		{
			return __WINE_UNIX_ERRNO_EOVERFLOW;
		}
	}
	int unix_fd{-1};
	if (auto const errcode{host_fd_to_unix_fd(params->host_fd, unix_fd)}; errcode)
	{
		return errcode;
	}
	size_t iovsize{static_cast<size_t>(params->iovsize)};
	constexpr int mxval{
#ifdef IOV_MAX
		IOV_MAX
#else
		INT_MAX
#endif
	};
	if (mxval < iovsize)
	{
		iovsize = mxval;
	}
	__wine_unix_iovec_t const *const iovs{iovs_from_params(*params)};
	auto ret = fn(unix_fd, reinterpret_cast<struct iovec const *>(iovs), static_cast<int>(iovsize));
	size_t total{};
	size_t baseindex{};
	size_t index{};
	auto const errcode{readwritev_result_common_split(ret, iovs, iovsize, total, baseindex, index)};
	params->total = static_cast<decltype(params->total)>(total);
	params->baseindex = static_cast<decltype(params->baseindex)>(baseindex);
	params->index = static_cast<decltype(params->index)>(index);
	return errcode;
}

static __wine_unix_status_t unix_host_fd_to_unix_fd(void *args) noexcept
{
	auto *params{static_cast<__wine_unix_host_fd_to_unix_fd_params_t *>(args)};
	int unix_fd{-1};
	auto const errcode{host_fd_to_unix_fd(params->host_fd, unix_fd)};
	params->unix_fd = unix_fd;
	return errcode;
}

static __wine_unix_status_t unix_unix_fd_to_host_fd(void *args) noexcept
{
	auto *params{static_cast<__wine_unix_unix_fd_to_host_fd_params_t *>(args)};
	params->host_fd = unix_fd_to_host_fd(static_cast<int>(params->unix_fd));
	return __WINE_UNIX_ERRNO_SUCCESS;
}

static __wine_unix_status_t unix_host_fd_to_nt_handle(void *) noexcept
{
	return __WINE_UNIX_ERRNO_EOPNOTSUPP;
}

static __wine_unix_status_t unix_nt_handle_to_host_fd(void *) noexcept
{
	return __WINE_UNIX_ERRNO_EOPNOTSUPP;
}

static __wine_unix_status_t unix_openat(void *args) noexcept
{
	auto *params{static_cast<__wine_unix_openat_params_t *>(args)};
	int dirfd{-1};
	if (params->host_dirfd == 0)
	{
		dirfd = AT_FDCWD;
	}
	else if (auto const errcode{host_fd_to_unix_fd(params->host_dirfd, dirfd)}; errcode)
	{
		return errcode;
	}
	auto pathret{c_path_common(reinterpret_cast<char const *>(static_cast<::std::uintptr_t>(params->filename)),
							   static_cast<size_t>(params->filenamelen))};
	if (pathret.host_errno)
	{
		return pathret.host_errno;
	}
	int const unix_fd{::openat(dirfd, pathret.filename_c_str, static_cast<int>(params->flags),
							   static_cast<mode_t>(params->mode))};
	params->host_fd = static_cast<decltype(params->host_fd)>(unix_fd_to_host_fd(unix_fd));
	if (unix_fd == -1)
	{
		return host_errno_to_wine_errno(errno);
	}
	return __WINE_UNIX_ERRNO_SUCCESS;
}

static __wine_unix_status_t unix_close(void *args) noexcept
{
	auto *params{static_cast<__wine_unix_close_params_t *>(args)};
	int unix_fd{-1};
	if (auto const errcode{host_fd_to_unix_fd(params->host_fd, unix_fd)}; errcode)
	{
		return errcode;
	}
	if (::close(unix_fd) == -1)
	{
		return host_errno_to_wine_errno(errno);
	}
	return __WINE_UNIX_ERRNO_SUCCESS;
}

static __wine_unix_status_t unix_writev(void *args) noexcept
{
	auto *params{static_cast<__wine_unix_readwritev_params_t *>(args)};
	return readwritev_common(params, [](int fd, struct iovec const *iov, int iovcnt) noexcept {
		return ::writev(fd, iov, iovcnt);
	});
}

static __wine_unix_status_t unix_readv(void *args) noexcept
{
	auto *params{static_cast<__wine_unix_readwritev_params_t *>(args)};
	return readwritev_common(params, [](int fd, struct iovec const *iov, int iovcnt) noexcept {
		return ::readv(fd, iov, iovcnt);
	});
}

static __wine_unix_status_t unix_pwritev(void *args) noexcept
{
	auto *params{static_cast<__wine_unix_preadwritev_params_t *>(args)};
	return preadwritev_common(params, [params](int fd, struct iovec const *iov, int iovcnt) noexcept {
		return ::pwritev(fd, iov, iovcnt, static_cast<off_t>(params->offset));
	});
}

static __wine_unix_status_t unix_preadv(void *args) noexcept
{
	auto *params{static_cast<__wine_unix_preadwritev_params_t *>(args)};
	return preadwritev_common(params, [params](int fd, struct iovec const *iov, int iovcnt) noexcept {
		return ::preadv(fd, iov, iovcnt, static_cast<off_t>(params->offset));
	});
}

#if INTPTR_MAX >= INT64_MAX
/*
wow64 (32-bit PE on a 64-bit host) wrappers. args points at a *_params32 struct laid
out by a 32-bit compiler; we widen pointers/sizes into a 64-bit params struct, call the
64-bit implementation, and write the outputs back.
*/

static __wine_unix_status_t wow64_unix_host_fd_to_unix_fd(void *args) noexcept
{
	auto *params{static_cast<__wine_unix_host_fd_to_unix_fd_params32 *>(args)};
	int unix_fd{-1};
	auto const errcode{host_fd_to_unix_fd(params->host_fd, unix_fd)};
	params->unix_fd = unix_fd;
	return errcode;
}

static __wine_unix_status_t wow64_unix_unix_fd_to_host_fd(void *args) noexcept
{
	auto *params{static_cast<__wine_unix_unix_fd_to_host_fd_params32 *>(args)};
	params->host_fd = static_cast<__wine_unix_ptr32_t>(unix_fd_to_host_fd(params->unix_fd));
	return __WINE_UNIX_ERRNO_SUCCESS;
}

static __wine_unix_status_t wow64_unix_host_fd_to_nt_handle(void *) noexcept
{
	return __WINE_UNIX_ERRNO_EOPNOTSUPP;
}

static __wine_unix_status_t wow64_unix_nt_handle_to_host_fd(void *) noexcept
{
	return __WINE_UNIX_ERRNO_EOPNOTSUPP;
}

static __wine_unix_status_t wow64_unix_openat(void *args) noexcept
{
	auto *params32{static_cast<__wine_unix_openat_params32 *>(args)};
	__wine_unix_openat_params params{};
	params.host_dirfd = params32->host_dirfd;
	params.filename = reinterpret_cast<char const *>(static_cast<::std::uintptr_t>(params32->filename));
	params.filenamelen = params32->filenamelen;
	params.flags = params32->flags;
	params.mode = params32->mode;
	auto const errcode{unix_openat(&params)};
	params32->host_fd = static_cast<__wine_unix_ptr32_t>(params.host_fd);
	return errcode;
}

static __wine_unix_status_t wow64_unix_close(void *args) noexcept
{
	auto *params{static_cast<__wine_unix_close_params32 *>(args)};
	int unix_fd{-1};
	if (auto const errcode{host_fd_to_unix_fd(params->host_fd, unix_fd)}; errcode)
	{
		return errcode;
	}
	if (::close(unix_fd) == -1)
	{
		return host_errno_to_wine_errno(errno);
	}
	return __WINE_UNIX_ERRNO_SUCCESS;
}

static __wine_unix_status_t wow64_readwritev_common(void *args, bool write) noexcept
{
	auto *params32{static_cast<__wine_unix_readwritev_params32 *>(args)};
	__wine_unix_readwritev_params params{};
	params.host_fd = params32->host_fd;
	size_t const iovsize{params32->iovsize};
	struct iovec *iovs{nullptr};
	if (iovsize)
	{
		iovs = static_cast<struct iovec *>(malloc(sizeof(struct iovec) * iovsize));
		if (iovs == nullptr)
		{
			return __WINE_UNIX_ERRNO_ENOMEM;
		}
		auto const *src{reinterpret_cast<uint32_t const *>(static_cast<::std::uintptr_t>(params32->iovs))};
		for (size_t i{}; i != iovsize; ++i)
		{
			iovs[i].iov_base = reinterpret_cast<void *>(static_cast<::std::uintptr_t>(src[2 * i]));
			iovs[i].iov_len = src[2 * i + 1];
		}
	}
	params.iovs = reinterpret_cast<__wine_unix_iovec_t const *>(iovs);
	params.iovsize = iovsize;
	auto const errcode{write ? unix_writev(&params) : unix_readv(&params)};
	params32->total = static_cast<uint32_t>(params.total);
	params32->baseindex = static_cast<uint32_t>(params.baseindex);
	params32->index = static_cast<uint32_t>(params.index);
	free(iovs);
	return errcode;
}

static __wine_unix_status_t wow64_unix_writev(void *args) noexcept
{
	return wow64_readwritev_common(args, true);
}

static __wine_unix_status_t wow64_unix_readv(void *args) noexcept
{
	return wow64_readwritev_common(args, false);
}

static __wine_unix_status_t wow64_preadwritev_common(void *args, bool write) noexcept
{
	auto *params32{static_cast<__wine_unix_preadwritev_params32 *>(args)};
	__wine_unix_preadwritev_params params{};
	params.host_fd = params32->host_fd;
	params.offset = params32->offset;
	size_t const iovsize{params32->iovsize};
	struct iovec *iovs{nullptr};
	if (iovsize)
	{
		iovs = static_cast<struct iovec *>(malloc(sizeof(struct iovec) * iovsize));
		if (iovs == nullptr)
		{
			return __WINE_UNIX_ERRNO_ENOMEM;
		}
		auto const *src{reinterpret_cast<uint32_t const *>(static_cast<::std::uintptr_t>(params32->iovs))};
		for (size_t i{}; i != iovsize; ++i)
		{
			iovs[i].iov_base = reinterpret_cast<void *>(static_cast<::std::uintptr_t>(src[2 * i]));
			iovs[i].iov_len = src[2 * i + 1];
		}
	}
	params.iovs = reinterpret_cast<__wine_unix_iovec_t const *>(iovs);
	params.iovsize = iovsize;
	auto const errcode{write ? unix_pwritev(&params) : unix_preadv(&params)};
	params32->total = static_cast<uint32_t>(params.total);
	params32->baseindex = static_cast<uint32_t>(params.baseindex);
	params32->index = static_cast<uint32_t>(params.index);
	free(iovs);
	return errcode;
}

static __wine_unix_status_t wow64_unix_pwritev(void *args) noexcept
{
	return wow64_preadwritev_common(args, true);
}

static __wine_unix_status_t wow64_unix_preadv(void *args) noexcept
{
	return wow64_preadwritev_common(args, false);
}

#endif // INTPTR_MAX >= INT64_MAX

} // namespace

static_assert(sizeof(__wine_unix_iovec_t) == sizeof(struct iovec),
			  "__wine_unix_iovec_t must match struct iovec layout");

} // namespace __wine_unix

extern "C"
{
	__WINE_UNIX_DLLEXPORT __wine_unixlib_entry_t const __wine_unix_call_funcs[] = {
		::__wine_unix::unix_host_fd_to_unix_fd,
		::__wine_unix::unix_unix_fd_to_host_fd,
		::__wine_unix::unix_host_fd_to_nt_handle,
		::__wine_unix::unix_nt_handle_to_host_fd,
		::__wine_unix::unix_openat,
		::__wine_unix::unix_close,
		::__wine_unix::unix_writev,
		::__wine_unix::unix_readv,
		::__wine_unix::unix_pwritev,
		::__wine_unix::unix_preadv,
	};

#if INTPTR_MAX >= INT64_MAX
	__WINE_UNIX_DLLEXPORT __wine_unixlib_entry_t const __wine_unix_call_wow64_funcs[] = {
		::__wine_unix::wow64_unix_host_fd_to_unix_fd,
		::__wine_unix::wow64_unix_unix_fd_to_host_fd,
		::__wine_unix::wow64_unix_host_fd_to_nt_handle,
		::__wine_unix::wow64_unix_nt_handle_to_host_fd,
		::__wine_unix::wow64_unix_openat,
		::__wine_unix::wow64_unix_close,
		::__wine_unix::wow64_unix_writev,
		::__wine_unix::wow64_unix_readv,
		::__wine_unix::wow64_unix_pwritev,
		::__wine_unix::wow64_unix_preadv,
	};
#endif

	__WINE_UNIX_DLLEXPORT __wine_unix_status_t __wine_unix_lib_init(void) noexcept
	{
		return __WINE_UNIX_ERRNO_SUCCESS;
	}
} // extern "C"

static_assert(sizeof(__wine_unix_call_funcs) / sizeof(__wine_unixlib_entry_t) == __wine_unix_funcs_count,
			  "__wine_unix_call_funcs must match the __wine_unix_funcs enum");
#if INTPTR_MAX >= INT64_MAX
static_assert(sizeof(__wine_unix_call_wow64_funcs) / sizeof(__wine_unixlib_entry_t) == __wine_unix_funcs_count,
			  "__wine_unix_call_wow64_funcs must match the __wine_unix_funcs enum");
#endif
