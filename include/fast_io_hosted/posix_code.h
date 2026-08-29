#pragma once

namespace fast_io
{

inline constexpr void throw_posix_error() FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::herbceptions::throws_errc_with_value(errno);
}

inline constexpr void throw_posix_error(int err) FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::herbceptions::throws_errc_with_value(err);
}

} // namespace fast_io