#pragma once

namespace fast_io
{

inline void throw_win32_error() FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::herbceptions::throws_win32_errc_with_value(::fast_io::win32::GetLastError());
}

inline void throw_win32_error([[maybe_unused]] ::std::uint_least32_t err) FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::herbceptions::throws_win32_errc_with_value(err);
}

} // namespace fast_io
