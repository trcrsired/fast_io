#pragma once

namespace fast_io
{

namespace details
{

inline ::fast_io::io_scatter_status_t posix_scatter_read_bytes_impl(int fd,
	::fast_io::io_scatter_t const *pscatter,::std::size_t n)
{
#if defined(__wasi__)
	using iovec_may_alias_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	= __wasi_iovec_t const*;
	::std::size_t ret;
	auto val{noexcept_call(__wasi_fd_read,
		reinterpret_cast<iovec_may_alias_const_ptr>(pscatter),
		static_cast<::std::size_t>(last-first),__builtin_addressof(ret))};
	if(val)
	{
		::fast_io::throw_posix_error(val);
	}
#else
	using iovec_may_alias_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	= struct iovec const*;

	auto ret{::fast_io::noexcept_call(::readv,
		reinterpret_cast<iovec_may_alias_const_ptr>(pscatter),
		static_cast<::std::size_t>(last-first))};
	if(ret<0)
	{
		::fast_io::throw_posix_error();
	}
#endif
	return scatter_size_to_status(static_cast<::std::size_t>(ret),pscatter,n);
}

inline ::fast_io::io_scatter_status_t posix_scatter_write_bytes_impl(int fd,
	::fast_io::io_scatter_t const *pscatter,::std::size_t n)
{

#if defined(__wasi__)
	using iovec_may_alias_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	= __wasi_iovec_t const*;
	::std::size_t ret;
	auto val{noexcept_call(__wasi_fd_write,
		reinterpret_cast<iovec_may_alias_const_ptr>(pscatter),
		static_cast<::std::size_t>(last-first),__builtin_addressof(ret))};
	if(val)
	{
		::fast_io::throw_posix_error(val);
	}
#else
	using iovec_may_alias_const_ptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	= struct iovec const*;

	auto ret{::fast_io::noexcept_call(::writev,
		reinterpret_cast<iovec_may_alias_const_ptr>(pscatter),
		static_cast<::std::size_t>(last-first))};
	if(ret<0)
	{
		::fast_io::throw_posix_error();
	}
#endif
	return scatter_size_to_status(static_cast<::std::size_t>(ret),pscatter,n);
}

}

template<::std::integral char_type>
inline ::fast_io::io_scatter_status_t scatter_read_some_bytes_underflow_define(
::fast_io::basic_posix_io_observer<char_type> piob,
::fast_io::io_scatter_t const *pscatters,::std::size_t n)
{
	return ::fast_io::details::posix_scatter_read_bytes_impl(piob.fd,pscatters,n);
}

template<::std::integral char_type>
inline ::fast_io::io_scatter_status_t scatter_write_some_bytes_overflow_define(
::fast_io::basic_posix_io_observer<char_type> piob,
::fast_io::io_scatter_t const *pscatters,::std::size_t n)
{
	return ::fast_io::details::posix_scatter_write_bytes_impl(piob.fd,pscatters,n);
}

}
