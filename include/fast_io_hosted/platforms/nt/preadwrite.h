#pragma once

namespace fast_io::win32::nt::details
{

template <::fast_io::nt_family family>
inline ::std::uint_least64_t nt_calculate_current_file_offset(void *__restrict handle)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::win32::nt::io_status_block block;
	::std::uint_least64_t fps{};
	auto status{::fast_io::win32::nt::nt_query_information_file<family == ::fast_io::nt_family::zw>(handle, __builtin_addressof(block), __builtin_addressof(fps),
																									static_cast<::std::uint_least32_t>(sizeof(::std::uint_least64_t)),
																									::fast_io::win32::nt::file_information_class::FilePositionInformation)};
	if (status)
	{
		::fast_io::herbceptions::throws_nt_errc_with_value(status);
	}
	return fps;
}

inline ::std::int_least64_t nt_calculate_offset_impl(::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS
{
	if constexpr (sizeof(::fast_io::intfpos_t) > sizeof(::std::int_least64_t))
	{
		[[maybe_unused]] constexpr ::std::int_least64_t mn{::std::numeric_limits<::std::int_least64_t>::min()};
		constexpr ::std::int_least64_t mx{::std::numeric_limits<::std::int_least64_t>::max()};
		if (off < 0 || off > mx)
		{
			::fast_io::herbceptions::throws_nt_errc_with_value(0xC0000106);
		}
	}
	return static_cast<::std::int_least64_t>(off);
}

template <nt_family family>
inline ::std::byte *nt_read_pread_some_bytes_common_impl(void *__restrict handle, ::std::byte *first, ::std::byte *last,
														 ::std::int_least64_t *pbyteoffset)
	FAST_IO_HERBCEPTIONS_THROWS
{
	// some poeple in zwclose7 forum said we do not need to initialize io_status_block
	::fast_io::win32::nt::io_status_block block;
	auto const status{::fast_io::win32::nt::nt_read_file<family == ::fast_io::nt_family::zw>(handle, nullptr, nullptr, nullptr, __builtin_addressof(block), first,
																							 ::fast_io::details::read_write_bytes_compute<::std::uint_least32_t>(first, last), pbyteoffset, nullptr)};
	if (status) [[unlikely]]
	{
		if (status == 0xC0000011 /*file*/ || status == 0xC000014B /*pipe*/) [[likely]]
		{
			return first;
		}
		::fast_io::herbceptions::throws_nt_errc_with_value(status);
	}
	return first + block.Information;
}

template <nt_family family>
inline ::std::byte *nt_read_some_bytes_impl(void *__restrict handle, ::std::byte *first, ::std::byte *last)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::win32::nt::details::nt_read_pread_some_bytes_common_impl<family>(handle, first, last, nullptr);
}

template <nt_family family>
inline ::std::byte *nt_pread_some_bytes_impl(void *__restrict handle, ::std::byte *first, ::std::byte *last,
											 ::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS
{
	// The difference between P-series functions in Windows synchronization mode and POSIX is that under Windows,
	// the functions will advance the position by the number of bytes written or read after each write/read operation.

	::std::int_least64_t offs{nt_calculate_offset_impl(off)};
	return ::fast_io::win32::nt::details::nt_read_pread_some_bytes_common_impl<family>(handle, first, last,
																					   __builtin_addressof(offs));
}

template <nt_family family>
inline ::std::byte const *nt_write_pwrite_some_bytes_common_impl(void *__restrict handle, ::std::byte const *first,
																 ::std::byte const *last,
																 ::std::int_least64_t *pbyteoffset)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::win32::nt::io_status_block block;
	auto const status{::fast_io::win32::nt::nt_write_file<family == ::fast_io::nt_family::zw>(handle, nullptr, nullptr, nullptr, __builtin_addressof(block), first,
																							  ::fast_io::details::read_write_bytes_compute<::std::uint_least32_t>(first, last), pbyteoffset, nullptr)};
	if (status)
	{
		::fast_io::herbceptions::throws_nt_errc_with_value(status);
	}
	return first + block.Information;
}

template <nt_family family>
inline ::std::byte const *nt_write_some_bytes_impl(void *__restrict handle, ::std::byte const *first,
												   ::std::byte const *last)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::win32::nt::details::nt_write_pwrite_some_bytes_common_impl<family>(handle, first, last, nullptr);
}

template <nt_family family>
inline ::std::byte const *nt_pwrite_some_bytes_impl(void *__restrict handle, ::std::byte const *first,
													::std::byte const *last, ::fast_io::intfpos_t off)
	FAST_IO_HERBCEPTIONS_THROWS
{
	// The difference between P-series functions in Windows synchronization mode and POSIX is that under Windows,
	// the functions will advance the position by the number of bytes written or read after each write/read operation.

	::std::int_least64_t offs{nt_calculate_offset_impl(off)};
	return ::fast_io::win32::nt::details::nt_write_pwrite_some_bytes_common_impl<family>(handle, first, last,
																						 __builtin_addressof(offs));
}

} // namespace fast_io::win32::nt::details
