#pragma once

#include <__wine_unix/__wine_unix.h>
#include "../../../fast_io_dsal/impl/misc/push_macros.h"

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

inline __wine_unix_rwv_result_t wine_writev_impl(__wine_host_fd_t host_fd, __wine_unix_iovec_t const *iovs,
												 ::std::size_t iovsize) FAST_IO_HERBCEPTIONS_THROWS
{
#if defined(__HERBCEPTIONS__)
	auto r{catch return_failure(__wine_unix_writev(host_fd, iovs, iovsize))};
	if (r.failed)
	{
		throw throws r.error;
	}
	return r.value;
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
	auto r{catch return_failure(__wine_unix_readv(host_fd, iovs, iovsize))};
	if (r.failed)
	{
		throw throws r.error;
	}
	return r.value;
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
	auto r{catch return_failure(__wine_unix_pwritev(host_fd, iovs, iovsize, offset))};
	if (r.failed)
	{
		throw throws r.error;
	}
	return r.value;
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
	auto r{catch return_failure(__wine_unix_preadv(host_fd, iovs, iovsize, offset))};
	if (r.failed)
	{
		throw throws r.error;
	}
	return r.value;
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

inline constexpr __wine_host_flags_t calculate_wine_open_mode(open_mode value) noexcept
{
	__wine_host_flags_t mode{__WINE_UNIX_O_NOFOLLOW};
	if ((value & open_mode::follow) != open_mode::none)
	{
		mode = {};
	}
	if ((value & open_mode::inherit) == open_mode::none)
	{
		mode |= __WINE_UNIX_O_CLOEXEC;
	}
	if ((value & open_mode::creat) != open_mode::none)
	{
		mode |= __WINE_UNIX_O_CREAT;
	}
	if ((value & open_mode::excl) != open_mode::none)
	{
		mode |= __WINE_UNIX_O_EXCL;
	}
	if ((value & open_mode::trunc) != open_mode::none)
	{
		mode |= __WINE_UNIX_O_TRUNC;
	}
	if ((value & open_mode::direct) != open_mode::none)
	{
		mode |= __WINE_UNIX_O_DIRECT;
	}
	if ((value & open_mode::sync) != open_mode::none)
	{
		mode |= __WINE_UNIX_O_SYNC;
	}
	if ((value & open_mode::dsync) != open_mode::none)
	{
		mode |= __WINE_UNIX_O_DSYNC;
	}
	if ((value & open_mode::no_ctty) != open_mode::none)
	{
		mode |= __WINE_UNIX_O_NOCTTY;
	}
	if ((value & open_mode::path) != open_mode::none)
	{
		mode |= __WINE_UNIX_O_PATH;
	}
	if ((value & open_mode::no_atime) != open_mode::none)
	{
		mode |= __WINE_UNIX_O_NOATIME;
	}
	if ((value & open_mode::no_block) != open_mode::none)
	{
		mode |= __WINE_UNIX_O_NONBLOCK;
	}
	if ((value & open_mode::temporary) != open_mode::none)
	{
		mode |= __WINE_UNIX_O_TMPFILE;
	}
	if ((value & open_mode::directory) != open_mode::none)
	{
		mode |= __WINE_UNIX_O_DIRECTORY;
	}
	mode |= __WINE_UNIX_O_LARGEFILE;

	using utype = typename ::std::underlying_type<open_mode>::type;
	constexpr auto supported_values{static_cast<utype>(open_mode::out) | static_cast<utype>(open_mode::app) |
									static_cast<utype>(open_mode::in)};
	switch (static_cast<utype>(value) & static_cast<utype>(supported_values))
	{
		// Action if file already exists;	Action if file does not exist;	c-style mode;	Explanation
		// Read from start;	Failure to open;	"r";	Open a file for reading
	case static_cast<utype>(open_mode::in):
		return mode | __WINE_UNIX_O_RDONLY;
		// Destroy contents;	Create new;	"w";	Create a file for writing
	case static_cast<utype>(open_mode::out):
		return mode | __WINE_UNIX_O_WRONLY | __WINE_UNIX_O_CREAT | __WINE_UNIX_O_TRUNC;
		// Append to file;	Create new;	"a";	Append to a file
	case static_cast<utype>(open_mode::app):
	case static_cast<utype>(open_mode::out) | static_cast<utype>(open_mode::app):
		return mode | __WINE_UNIX_O_WRONLY | __WINE_UNIX_O_CREAT | __WINE_UNIX_O_APPEND;
		// Read from start;	Error;	"r+";		Open a file for read/write
	case static_cast<utype>(open_mode::out) | static_cast<utype>(open_mode::in):
		return mode | __WINE_UNIX_O_RDWR;
		// Write to end;	Create new;	"a+";	Open a file for read/write
	case static_cast<utype>(open_mode::out) | static_cast<utype>(open_mode::in) |
		static_cast<utype>(open_mode::app):
	case static_cast<utype>(open_mode::in) | static_cast<utype>(open_mode::app):
		return mode | __WINE_UNIX_O_RDWR | __WINE_UNIX_O_CREAT | __WINE_UNIX_O_APPEND;
		// Destroy contents;	Error;	"wx";	Create a file for writing
	default:
		return mode;
	}
}

inline __wine_host_fd_t wine_open_file_impl(__wine_host_fd_t host_dirfd, char const *filename, ::std::size_t filenamelen,
											open_mode om, perms pm) FAST_IO_HERBCEPTIONS_THROWS
{
	return wine_openat_impl(host_dirfd, filename, filenamelen, calculate_wine_open_mode(om),
							static_cast<__wine_host_mode_t>(pm));
}

} // namespace details

template <::fast_io::wine_family family, ::std::integral char_type>
inline ::std::byte const *write_some_bytes_overflow_define(::fast_io::basic_wine_family_io_observer<family, char_type> wiob,
														   ::std::byte const *first,
														   ::std::byte const *last) FAST_IO_HERBCEPTIONS_THROWS
{
	__wine_unix_iovec_t iov{first, static_cast<::std::size_t>(last - first)};
	auto ret{::fast_io::details::wine_writev_impl(wiob.host_fd, __builtin_addressof(iov), 1)};
	return first + ret.total;
}

template <::fast_io::wine_family family, ::std::integral char_type>
inline ::std::byte *read_some_bytes_underflow_define(::fast_io::basic_wine_family_io_observer<family, char_type> wiob,
													 ::std::byte *first, ::std::byte *last) FAST_IO_HERBCEPTIONS_THROWS
{
	__wine_unix_iovec_t iov{first, static_cast<::std::size_t>(last - first)};
	auto ret{::fast_io::details::wine_readv_impl(wiob.host_fd, __builtin_addressof(iov), 1)};
	return first + ret.total;
}

template <::fast_io::wine_family family, ::std::integral char_type>
inline ::fast_io::io_scatter_status_t
scatter_write_some_bytes_overflow_define(::fast_io::basic_wine_family_io_observer<family, char_type> wiob,
										 ::fast_io::io_scatter_t const *pscatter,
										 ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
{
	auto ret{::fast_io::details::wine_writev_impl(
		wiob.host_fd, reinterpret_cast<::fast_io::details::wine_iovec_may_alias_const_ptr>(pscatter), n)};
	return {ret.baseindex, ret.index};
}

template <::fast_io::wine_family family, ::std::integral char_type>
inline ::fast_io::io_scatter_status_t
scatter_read_some_bytes_underflow_define(::fast_io::basic_wine_family_io_observer<family, char_type> wiob,
										 ::fast_io::io_scatter_t const *pscatter,
										 ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
{
	auto ret{::fast_io::details::wine_readv_impl(
		wiob.host_fd, reinterpret_cast<::fast_io::details::wine_iovec_may_alias_const_ptr>(pscatter), n)};
	return {ret.baseindex, ret.index};
}

template <::fast_io::wine_family family, ::std::integral char_type>
inline ::std::byte const *pwrite_some_bytes_overflow_define(::fast_io::basic_wine_family_io_observer<family, char_type> wiob,
															::std::byte const *first, ::std::byte const *last,
															::fast_io::intfpos_t off) FAST_IO_HERBCEPTIONS_THROWS
{
	__wine_unix_iovec_t iov{first, static_cast<::std::size_t>(last - first)};
	auto ret{::fast_io::details::wine_pwritev_impl(wiob.host_fd, __builtin_addressof(iov), 1,
												   static_cast<__wine_off_t>(off))};
	return first + ret.total;
}

template <::fast_io::wine_family family, ::std::integral char_type>
inline ::std::byte *pread_some_bytes_underflow_define(::fast_io::basic_wine_family_io_observer<family, char_type> wiob,
													  ::std::byte *first, ::std::byte *last,
													  ::fast_io::intfpos_t off) FAST_IO_HERBCEPTIONS_THROWS
{
	__wine_unix_iovec_t iov{first, static_cast<::std::size_t>(last - first)};
	auto ret{::fast_io::details::wine_preadv_impl(wiob.host_fd, __builtin_addressof(iov), 1,
												  static_cast<__wine_off_t>(off))};
	return first + ret.total;
}

template <::fast_io::wine_family family, ::std::integral char_type>
inline ::fast_io::io_scatter_status_t
scatter_pwrite_some_bytes_overflow_define(::fast_io::basic_wine_family_io_observer<family, char_type> wiob,
										  ::fast_io::io_scatter_t const *pscatter, ::std::size_t n,
										  ::fast_io::intfpos_t off) FAST_IO_HERBCEPTIONS_THROWS
{
	auto ret{::fast_io::details::wine_pwritev_impl(
		wiob.host_fd, reinterpret_cast<::fast_io::details::wine_iovec_may_alias_const_ptr>(pscatter), n,
		static_cast<__wine_off_t>(off))};
	return {ret.baseindex, ret.index};
}

template <::fast_io::wine_family family, ::std::integral char_type>
inline ::fast_io::io_scatter_status_t
scatter_pread_some_bytes_underflow_define(::fast_io::basic_wine_family_io_observer<family, char_type> wiob,
										  ::fast_io::io_scatter_t const *pscatter, ::std::size_t n,
										  ::fast_io::intfpos_t off) FAST_IO_HERBCEPTIONS_THROWS
{
	auto ret{::fast_io::details::wine_preadv_impl(
		wiob.host_fd, reinterpret_cast<::fast_io::details::wine_iovec_may_alias_const_ptr>(pscatter), n,
		static_cast<__wine_off_t>(off))};
	return {ret.baseindex, ret.index};
}

template <::fast_io::wine_family family>
struct wine_family_file_factory FAST_IO_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE
{
	using native_handle_type = ::fast_io::wine_host_fd_t;
	native_handle_type host_fd{};

	inline explicit constexpr wine_family_file_factory(native_handle_type v) noexcept
		: host_fd(v) {};
	inline wine_family_file_factory(wine_family_file_factory const &) = delete;
	inline wine_family_file_factory &operator=(wine_family_file_factory const &) = delete;
	inline ~wine_family_file_factory()
	{
		if (host_fd) [[likely]]
		{
			__wine_unix_close_returns_status(host_fd);
		}
	}
};

using wine_file_factory = wine_family_file_factory<::fast_io::wine_family::native>;

template <::fast_io::wine_family family, ::std::integral ch_type>
class basic_wine_family_file : public basic_wine_family_io_observer<family, ch_type>
{
public:
	using typename basic_wine_family_io_observer<family, ch_type>::char_type;
	using typename basic_wine_family_io_observer<family, ch_type>::input_char_type;
	using typename basic_wine_family_io_observer<family, ch_type>::output_char_type;
	using typename basic_wine_family_io_observer<family, ch_type>::native_handle_type;
	using file_factory_type = wine_family_file_factory<family>;
	using basic_wine_family_io_observer<family, ch_type>::native_handle;
	inline constexpr basic_wine_family_file() noexcept = default;
	template <typename native_hd>
		requires ::std::same_as<native_handle_type, ::std::remove_cvref_t<native_hd>>
	inline explicit constexpr basic_wine_family_file(native_hd host_fd1) noexcept
		: basic_wine_family_io_observer<family, ch_type>{host_fd1}
	{
	}
	inline explicit constexpr basic_wine_family_file(wine_family_file_factory<family> &&factory) noexcept
		: basic_wine_family_io_observer<family, ch_type>{factory.host_fd}
	{
		factory.host_fd = 0;
	}

	inline explicit basic_wine_family_file(char const *filename, open_mode om,
										   perms pm = static_cast<perms>(436)) FAST_IO_HERBCEPTIONS_THROWS
		: basic_wine_family_io_observer<family, ch_type>{::fast_io::details::wine_open_file_impl(
			  0, filename, ::fast_io::cstr_len(filename), om, pm)}
	{
	}

	inline explicit basic_wine_family_file(wine_at_entry wate, char const *filename, open_mode om,
										   perms pm = static_cast<perms>(436)) FAST_IO_HERBCEPTIONS_THROWS
		: basic_wine_family_io_observer<family, ch_type>{::fast_io::details::wine_open_file_impl(
			  wate.host_fd, filename, ::fast_io::cstr_len(filename), om, pm)}
	{
	}

	inline constexpr basic_wine_family_file(basic_wine_family_file const &) = delete;
	inline constexpr basic_wine_family_file &operator=(basic_wine_family_file const &) = delete;

	inline constexpr basic_wine_family_file(basic_wine_family_file &&__restrict other) noexcept
		: basic_wine_family_io_observer<family, ch_type>{other.release()}
	{
	}
	inline basic_wine_family_file &operator=(basic_wine_family_file &&__restrict other) noexcept
	{
		if (this->host_fd) [[likely]]
		{
			__wine_unix_close_returns_status(this->host_fd);
		}
		this->host_fd = other.release();
		return *this;
	}
	inline constexpr basic_wine_family_file(decltype(nullptr)) noexcept = delete;
	inline constexpr basic_wine_family_file(basic_wine_family_io_observer<family, ch_type>) noexcept = delete;
	inline constexpr basic_wine_family_file &operator=(basic_wine_family_io_observer<family, ch_type>) noexcept = delete;
	inline ~basic_wine_family_file()
	{
		if (this->host_fd) [[likely]]
		{
			__wine_unix_close_returns_status(this->host_fd);
		}
	}
};

template <::std::integral ch_type>
using basic_wine_io_observer = basic_wine_family_io_observer<::fast_io::wine_family::native, ch_type>;

template <::std::integral ch_type>
using basic_wine_file = basic_wine_family_file<::fast_io::wine_family::native, ch_type>;

using wine_io_observer = basic_wine_io_observer<char>;
using wine_file = basic_wine_file<char>;

namespace details
{
template <int which>
inline ::fast_io::wine_host_fd_t wine_get_std_host_fd() noexcept
{
	return __wine_unix_get_std_host_fd_returns_status(which).host_fd;
}
} // namespace details

/*
std streams through whichever wineunix.dll is loaded: unixcall impl returns
unix fd + 1, nt impl returns the process's Standard{Input,Output,Error}
handle — both wrapped as a host_fd.
*/
template <::std::integral char_type = char>
inline basic_wine_io_observer<char_type> wine_stdin() noexcept
{
	return {::fast_io::details::wine_get_std_host_fd<0>()};
}

template <::std::integral char_type = char>
inline basic_wine_io_observer<char_type> wine_stdout() noexcept
{
	return {::fast_io::details::wine_get_std_host_fd<1>()};
}

template <::std::integral char_type = char>
inline basic_wine_io_observer<char_type> wine_stderr() noexcept
{
	return {::fast_io::details::wine_get_std_host_fd<2>()};
}

namespace freestanding
{
template <::fast_io::wine_family family>
struct is_trivially_copyable_or_relocatable<wine_family_file_factory<family>>
{
	inline static constexpr bool value = true;
};

} // namespace freestanding
} // namespace fast_io

#include "../../../fast_io_dsal/impl/misc/pop_macros.h"
