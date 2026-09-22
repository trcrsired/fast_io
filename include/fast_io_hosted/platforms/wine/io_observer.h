#pragma once

#include "../../../fast_io_dsal/impl/misc/push_macros.h"

#include <__wine_unix/__wine_unix.h>

namespace fast_io
{

enum class wine_family : ::std::uint_least32_t
{
	unspecified_host = 0,
	native = unspecified_host
};

using wine_host_fd_t = __wine_host_fd_t;

struct wine_at_entry
{
	using native_handle_type = ::fast_io::wine_host_fd_t;
	native_handle_type host_fd{};
	inline explicit constexpr wine_at_entry() noexcept = default;
	inline explicit constexpr wine_at_entry(native_handle_type hfd) noexcept
		: host_fd(hfd)
	{
	}
};

/*
host_dirfd == 0 means "current working directory" in the wineunix abi
(AT_FDCWD on the unix side, no RootDirectory on the nt side).
*/
inline constexpr wine_at_entry wine_at_fdcwd() noexcept
{
	return wine_at_entry{};
}

template <::fast_io::wine_family family, ::std::integral ch_type>
class basic_wine_family_io_observer
{
public:
	using char_type = ch_type;
	using input_char_type = char_type;
	using output_char_type = char_type;
	using native_handle_type = ::fast_io::wine_host_fd_t;
	native_handle_type host_fd{}; // unixcall impl: unix fd + 1. nt impl: raw HANDLE. 0 = no fd.
	inline constexpr native_handle_type native_handle() const noexcept
	{
		return host_fd;
	}
	inline explicit constexpr operator bool() const noexcept
	{
		return host_fd;
	}
	inline constexpr native_handle_type release() noexcept
	{
		auto temp{host_fd};
		host_fd = 0;
		return temp;
	}
};

template <::fast_io::wine_family family, ::std::integral ch_type>
inline constexpr bool operator==(basic_wine_family_io_observer<family, ch_type> a,
								 basic_wine_family_io_observer<family, ch_type> b) noexcept
{
	return a.host_fd == b.host_fd;
}

#if __cpp_impl_three_way_comparison >= 201907L
template <::fast_io::wine_family family, ::std::integral ch_type>
inline constexpr auto operator<=>(basic_wine_family_io_observer<family, ch_type> a,
								  basic_wine_family_io_observer<family, ch_type> b) noexcept
{
	return a.host_fd <=> b.host_fd;
}
#endif

template <::fast_io::wine_family family, ::std::integral ch_type>
inline constexpr wine_at_entry at(basic_wine_family_io_observer<family, ch_type> wiob) noexcept
{
	return wine_at_entry{wiob.host_fd};
}

template <::fast_io::wine_family family, ::std::integral ch_type>
inline constexpr basic_wine_family_io_observer<family, ch_type>
io_stream_ref_define(basic_wine_family_io_observer<family, ch_type> other) noexcept
{
	return other;
}

template <::fast_io::wine_family family, ::std::integral ch_type>
inline constexpr basic_wine_family_io_observer<family, char>
io_bytes_stream_ref_define(basic_wine_family_io_observer<family, ch_type> other) noexcept
{
	return {other.host_fd};
}

namespace details
{

[[noreturn]] inline void throw_wine_error_status(__wine_unix_status_t status) FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::herbceptions::throws_errc_generic(
		static_cast<__wine_unix_errc>(static_cast<::std::uint_least32_t>(status)));
}

inline __wine_host_fd_t wine_openat_impl(__wine_host_fd_t host_dirfd, char const *filename, ::std::size_t filenamelen,
										 __wine_host_flags_t flags, __wine_host_mode_t mode) FAST_IO_HERBCEPTIONS_THROWS
{
#if defined(__HERBCEPTIONS__)
	// clang backend crashes on try() when the api value type is i64; catch
	// return_failure + throw throws lowers to the same thing without the bug
	auto r{catch return_failure(__wine_unix_openat(host_dirfd, filename, filenamelen, flags, mode))};
	if (r.failed)
	{
		throw throws r.error;
	}
	return r.value;
#else
	auto ret{__wine_unix_openat_returns_status(host_dirfd, filename, filenamelen, flags, mode)};
	if (ret.status)
	{
		throw_wine_error_status(ret.status);
	}
	return ret.host_fd;
#endif
}

inline ::std::size_t wine_write_impl(__wine_host_fd_t host_fd, void const *buf,
									 ::std::size_t len) FAST_IO_HERBCEPTIONS_THROWS
{
#if defined(__HERBCEPTIONS__)
	auto ret{try(__wine_unix_write(host_fd, buf, len))};
	return ret.total;
#else
	auto ret{__wine_unix_write_returns_status(host_fd, buf, len)};
	if (ret.status)
	{
		throw_wine_error_status(ret.status);
	}
	return ret.total;
#endif
}

inline ::std::size_t wine_read_impl(__wine_host_fd_t host_fd, void *buf,
									::std::size_t len) FAST_IO_HERBCEPTIONS_THROWS
{
#if defined(__HERBCEPTIONS__)
	auto ret{try(__wine_unix_read(host_fd, buf, len))};
	return ret.total;
#else
	auto ret{__wine_unix_read_returns_status(host_fd, buf, len)};
	if (ret.status)
	{
		throw_wine_error_status(ret.status);
	}
	return ret.total;
#endif
}

inline __wine_unix_rwv_result_t wine_writev_impl(__wine_host_fd_t host_fd, __wine_unix_iovec_t const *iovs,
												 ::std::size_t iovsize) FAST_IO_HERBCEPTIONS_THROWS
{
#if defined(__HERBCEPTIONS__)
	return try(__wine_unix_writev(host_fd, iovs, iovsize));
#else
	auto ret{__wine_unix_writev_returns_status(host_fd, iovs, iovsize)};
	if (ret.status)
	{
		throw_wine_error_status(ret.status);
	}
	return {ret.total, ret.baseindex, ret.index};
#endif
}

inline __wine_unix_rwv_result_t wine_readv_impl(__wine_host_fd_t host_fd, __wine_unix_iovec_t const *iovs,
												::std::size_t iovsize) FAST_IO_HERBCEPTIONS_THROWS
{
#if defined(__HERBCEPTIONS__)
	return try(__wine_unix_readv(host_fd, iovs, iovsize));
#else
	auto ret{__wine_unix_readv_returns_status(host_fd, iovs, iovsize)};
	if (ret.status)
	{
		throw_wine_error_status(ret.status);
	}
	return {ret.total, ret.baseindex, ret.index};
#endif
}

inline __wine_unix_rwv_result_t wine_pwritev_impl(__wine_host_fd_t host_fd, __wine_unix_iovec_t const *iovs,
												  ::std::size_t iovsize, __wine_off_t offset) FAST_IO_HERBCEPTIONS_THROWS
{
#if defined(__HERBCEPTIONS__)
	return try(__wine_unix_pwritev(host_fd, iovs, iovsize, offset));
#else
	auto ret{__wine_unix_pwritev_returns_status(host_fd, iovs, iovsize, offset)};
	if (ret.status)
	{
		throw_wine_error_status(ret.status);
	}
	return {ret.total, ret.baseindex, ret.index};
#endif
}

inline __wine_unix_rwv_result_t wine_preadv_impl(__wine_host_fd_t host_fd, __wine_unix_iovec_t const *iovs,
												 ::std::size_t iovsize, __wine_off_t offset) FAST_IO_HERBCEPTIONS_THROWS
{
#if defined(__HERBCEPTIONS__)
	return try(__wine_unix_preadv(host_fd, iovs, iovsize, offset));
#else
	auto ret{__wine_unix_preadv_returns_status(host_fd, iovs, iovsize, offset)};
	if (ret.status)
	{
		throw_wine_error_status(ret.status);
	}
	return {ret.total, ret.baseindex, ret.index};
#endif
}

using wine_iovec_may_alias_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	= __wine_unix_iovec_t const *;

} // namespace details

} // namespace fast_io

#include "../../../fast_io_dsal/impl/misc/pop_macros.h"
