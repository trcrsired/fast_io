#pragma once

#include "../../../fast_io_dsal/impl/misc/push_macros.h"

namespace fast_io
{

namespace details
{

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

struct my_wine_at_open_parameter
{
	__wine_host_fd_t host_dirfd{};
	__wine_host_flags_t flags{};
	__wine_host_mode_t mode{};
	inline __wine_host_fd_t operator()(char const *filename, ::std::size_t filenamelen) const
		FAST_IO_HERBCEPTIONS_THROWS
	{
		return wine_openat_impl(host_dirfd, filename, filenamelen, flags, mode);
	}
};

/*
wineunix filenames are utf8 bytes like posix (the unixcall impl hands them to
openat, the nt impl converts to utf16), never utf16 like the win32 api — even
though the wine layer is compiled for a windows target.
*/
template <::fast_io::constructible_to_os_c_str T>
inline __wine_host_fd_t wine_openat_file_impl(__wine_host_fd_t host_dirfd, T const &t, open_mode om, perms pm)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::posix_api_common(
		t, my_wine_at_open_parameter{host_dirfd, ::fast_io::details::calculate_wine_open_mode(om),
									 static_cast<__wine_host_mode_t>(pm)});
}

template <::fast_io::constructible_to_os_c_str T>
inline __wine_host_fd_t wine_open_file_impl(T const &t, open_mode om, perms pm) FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::details::wine_openat_file_impl(0, t, om, pm);
}

} // namespace details

} // namespace fast_io

#include "../../../fast_io_dsal/impl/misc/pop_macros.h"
