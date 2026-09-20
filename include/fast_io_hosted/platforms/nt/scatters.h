#pragma once

namespace fast_io::win32::nt::details
{

template <typename T>
using nt_buffer_alloc_ptr = ::fast_io::details::buffer_alloc_arr_ptr<
	T,
	false,
	::fast_io::generic_allocator_adapter<
#if defined(__WINE__) && !defined(__CYGWIN__)
		::fast_io::native_thread_local_allocator
#else
		::fast_io::nt_rtlallocateheap_allocator
#endif
		>>;

template <::fast_io::nt_family family>
inline ::std::byte const *nt_write_pwrite_some_thunk(void *__restrict handle, ::std::byte const *first,
													 ::std::byte const *last, ::std::int_least64_t *__restrict pbyteoffset) FAST_IO_HERBCEPTIONS_THROWS
{
	::std::int_least64_t off{};
	if (pbyteoffset != nullptr && 0 <= (off = *pbyteoffset))
	{
		auto request{static_cast<::std::int_least64_t>(last - first)};
#if FAST_IO_HAS_BUILTIN(__builtin_add_overflow)
		::std::int_least64_t nxt;
		if (__builtin_add_overflow(off, request, __builtin_addressof(nxt))) [[unlikely]]
#else
		constexpr ::std::int_least64_t mx{::std::numeric_limits<::std::int_least64_t>::max()};
		if (mx - off < request) [[unlikely]]
#endif
		{
			::fast_io::herbceptions::throws_nt_errc_with_value(0xC0000095); // STATUS_INTEGER_OVERFLOW
		}
	}
	auto written{::fast_io::nt::details::nt_write_pwrite_some_bytes_common_impl<family>(
		handle, first, last, pbyteoffset == nullptr ? nullptr : __builtin_addressof(off))};
	if (pbyteoffset != nullptr && 0 <= off)
	{
		*pbyteoffset = off + static_cast<::std::int_least64_t>(written - first);
	}
	return written;
}

template <::fast_io::nt_family family>
inline void nt_write_pwrite_all_thunk(void *__restrict handle, ::std::byte const *first,
									  ::std::byte const *last, ::std::int_least64_t *__restrict pbyteoffset) FAST_IO_HERBCEPTIONS_THROWS
{
	while (first != last)
	{
		auto written{::fast_io::nt::details::nt_write_pwrite_some_thunk<family>(handle, first, last, pbyteoffset)};
		if (written == first) [[unlikely]]
		{
			::fast_io::herbceptions::throws_nt_errc_with_value(0xC0000185); // STATUS_IO_DEVICE_ERROR
		}
		first = written;
	}
}

// The kernel only takes a per-call ByteOffset; it cannot advance an offset across the
// chunked syscalls this emulation issues, so the offset accumulation and the pre-write
// overflow check live here. Negative ByteOffset values (-1/-2 special positions) pass through.
template <::fast_io::nt_family family>
struct nt_scatter_write_pwrite_chunk_impl
{
	::std::int_least64_t *pbyteoffset{};
	inline ::std::byte const *operator()(void *__restrict handle, ::std::byte const *first,
										 ::std::byte const *last) const FAST_IO_HERBCEPTIONS_THROWS
	{
		return ::fast_io::nt::details::nt_write_pwrite_some_thunk<family>(handle, first, last, pbyteoffset);
	}
};

// write-all variant: a chunk is only done when fully written; a syscall round making no
// progress means the write stopped -> STATUS_IO_DEVICE_ERROR.
template <::fast_io::nt_family family>
struct nt_scatter_write_all_pwrite_chunk_impl
{
	::std::int_least64_t *pbyteoffset{};
	inline void operator()(void *__restrict handle, ::std::byte const *first,
						   ::std::byte const *last) const FAST_IO_HERBCEPTIONS_THROWS
	{
		::fast_io::nt::details::nt_write_pwrite_all_thunk<family>(handle, first, last, pbyteoffset);
	}
};

template <::fast_io::nt_family family>
inline ::fast_io::io_scatter_status_t nt_scatter_write_pwrite_some_bytes_overflow_define_impl(void *handle,
																							  ::fast_io::io_scatter_t const *pscatters, ::std::size_t n, ::std::int_least64_t *__restrict pbyteoffset) FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::details::scatter_write_pwrite_some_bytes_common<
		::fast_io::win32::nt::details::nt_buffer_alloc_ptr<::std::byte>>(
		handle, pscatters, n, ::fast_io::nt::details::nt_scatter_write_pwrite_chunk_impl<family>{pbyteoffset});
}

template <::fast_io::nt_family family>
inline ::fast_io::io_scatter_status_t nt_scatter_write_some_bytes_overflow_define_impl(void *handle,
																					   ::fast_io::io_scatter_t const *pscatters, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::nt::details::nt_scatter_write_pwrite_some_bytes_overflow_define_impl<family>(handle, pscatters, n, nullptr);
}
template <::fast_io::nt_family family>
inline ::fast_io::io_scatter_status_t nt_scatter_pwrite_some_bytes_overflow_define_impl(void *handle,
																						::fast_io::io_scatter_t const *pscatters, ::std::size_t n, ::fast_io::intfpos_t byteoffset) FAST_IO_HERBCEPTIONS_THROWS
{
	::std::int_least64_t offs{::fast_io::nt::details::nt_calculate_offset_impl(byteoffset)};
	return ::fast_io::nt::details::nt_scatter_write_pwrite_some_bytes_overflow_define_impl<family>(handle, pscatters, n, __builtin_addressof(offs));
}

template <::fast_io::nt_family family>
inline void nt_scatter_write_pwrite_all_bytes_overflow_define_impl(void *handle,
																   ::fast_io::io_scatter_t const *pscatters, ::std::size_t n, ::std::int_least64_t *__restrict pbyteoffset) FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::details::scatter_write_pwrite_all_bytes_common<
		::fast_io::win32::nt::details::nt_buffer_alloc_ptr<::std::byte>>(
		handle, pscatters, n, ::fast_io::nt::details::nt_scatter_write_all_pwrite_chunk_impl<family>{pbyteoffset});
}

template <::fast_io::nt_family family>
inline void nt_scatter_write_all_bytes_overflow_define_impl(void *handle,
															::fast_io::io_scatter_t const *pscatters, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::nt::details::nt_scatter_write_pwrite_all_bytes_overflow_define_impl<family>(handle, pscatters, n, nullptr);
}

template <::fast_io::nt_family family>
inline void nt_scatter_pwrite_all_bytes_overflow_define_impl(void *handle,
															 ::fast_io::io_scatter_t const *pscatters, ::std::size_t n, ::fast_io::intfpos_t byteoffset) FAST_IO_HERBCEPTIONS_THROWS
{
	::std::int_least64_t offs{::fast_io::nt::details::nt_calculate_offset_impl(byteoffset)};
	return ::fast_io::nt::details::nt_scatter_write_pwrite_all_bytes_overflow_define_impl<family>(handle, pscatters, n, __builtin_addressof(offs));
}

} // namespace fast_io::win32::nt::details

namespace fast_io
{

template <::fast_io::nt_family family, ::std::integral char_type>
inline ::fast_io::io_scatter_status_t
scatter_write_some_bytes_overflow_define(::fast_io::basic_nt_family_io_observer<family, char_type> niob,
										 ::fast_io::io_scatter_t const *pscatters, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::nt::details::nt_scatter_write_some_bytes_overflow_define_impl<family>(niob.handle, pscatters, n);
}

template <::fast_io::nt_family family, ::std::integral char_type>
inline ::fast_io::io_scatter_status_t
scatter_pwrite_some_bytes_overflow_define(::fast_io::basic_nt_family_io_observer<family, char_type> niob,
										  ::fast_io::io_scatter_t const *pscatters, ::std::size_t n,
										  ::fast_io::intfpos_t byteoffset) FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::nt::details::nt_scatter_pwrite_some_bytes_overflow_define_impl<family>(niob.handle, pscatters, n, byteoffset);
}

template <::fast_io::nt_family family, ::std::integral char_type>
inline void
scatter_write_all_bytes_overflow_define(::fast_io::basic_nt_family_io_observer<family, char_type> niob,
										::fast_io::io_scatter_t const *pscatters, ::std::size_t n)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::nt::details::nt_scatter_write_all_bytes_overflow_define_impl<family>(niob.handle, pscatters, n);
}

template <::fast_io::nt_family family, ::std::integral char_type>
inline void
scatter_pwrite_all_bytes_overflow_define(::fast_io::basic_nt_family_io_observer<family, char_type> niob,
										 ::fast_io::io_scatter_t const *pscatters, ::std::size_t n, ::fast_io::intfpos_t byteoffset)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::nt::details::nt_scatter_pwrite_all_bytes_overflow_define_impl<family>(niob.handle, pscatters, n, byteoffset);
}

} // namespace fast_io
