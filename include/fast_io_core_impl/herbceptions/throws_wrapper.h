#pragma once

namespace fast_io::herbceptions
{

template <typename T>
	requires ::std::is_enum_v<T>
inline constexpr void throws_errc_generic([[maybe_unused]] T ec) FAST_IO_HERBCEPTIONS_THROWS
{
#if defined(__HERBCEPTIONS__)
	throw throws ec;
#else
	::fast_io::fast_terminate();
#endif
}

inline constexpr void throws_errc([[maybe_unused]] ::std::errc ec) FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::herbceptions::throws_errc_generic(ec);
}

inline constexpr void throws_errc_with_value([[maybe_unused]] int ec) FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::herbceptions::throws_errc_generic(static_cast<::std::errc>(ec));
}

inline constexpr void throws_parse_errc([[maybe_unused]] ::fast_io::freestanding::parse_errc ec) FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::herbceptions::throws_errc_generic(static_cast<::fast_io::freestanding::parse_errc>(ec));
}

#if defined(_WIN32) || defined(__CYGWIN__)
inline constexpr void throws_nt_errc([[maybe_unused]] ::fast_io::freestanding::nt_errc ec) FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::herbceptions::throws_errc_generic(ec);
}
inline constexpr void throws_nt_errc_with_value([[maybe_unused]] ::std::uint_least32_t ec) FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::herbceptions::throws_errc_generic(static_cast<::fast_io::freestanding::nt_errc>(ec));
}

inline constexpr void throws_win32_errc([[maybe_unused]] ::fast_io::freestanding::win32_errc ec) FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::herbceptions::throws_errc_generic(ec);
}

inline constexpr void throws_win32_errc_with_value([[maybe_unused]] ::std::uint_least32_t ec) FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::herbceptions::throws_errc_generic(static_cast<::fast_io::freestanding::win32_errc>(ec));
}

inline constexpr void throws_com_errc([[maybe_unused]] ::fast_io::freestanding::com_errc ec) FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::herbceptions::throws_errc_generic(ec);
}

inline constexpr void throws_com_errc_with_value([[maybe_unused]] ::std::uint_least32_t ec) FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::herbceptions::throws_errc_generic(static_cast<::fast_io::freestanding::com_errc>(ec));
}

inline constexpr void throws_wine_errc([[maybe_unused]] ::fast_io::freestanding::wine_errc ec) FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::herbceptions::throws_errc_generic(ec);
}

inline constexpr void throws_wine_errc_with_value([[maybe_unused]] ::std::uint_least32_t ec) FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::herbceptions::throws_errc_generic(static_cast<::fast_io::freestanding::wine_errc>(ec));
}

#endif
} // namespace fast_io::herbceptions
