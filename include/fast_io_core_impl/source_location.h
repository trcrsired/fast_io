#pragma once

namespace fast_io
{

template <::std::integral char_type>
	requires(sizeof(char_type) == 1)
inline constexpr reserve_scatters_size_result
	print_reserve_scatters_size(::fast_io::io_reserve_type_t<char_type, ::std::source_location>) noexcept
{
	constexpr ::std::size_t ulint32_rsv_size{
		print_reserve_size(::fast_io::io_reserve_type<char_type, ::std::uint_least32_t>)};
	constexpr ::std::size_t total_uint_least32_t_rsv_size{ulint32_rsv_size * 2 + 3};
	return {3, total_uint_least32_t_rsv_size};
}

namespace details
{

inline constexpr ::std::size_t prrsv_reserve_size_source_location_impl(::std::source_location const &location) noexcept
{
	constexpr ::std::size_t known_size_at_compilation{3zu +
													  (print_reserve_size(::fast_io::io_reserve_type<char8_t, ::std::uint_least32_t>) * 2zu)};
	::std::size_t filenamesz{::fast_io::cstr_len(location.file_name())};
	::std::size_t functionnamesz{::fast_io::cstr_len(location.function_name())};
	constexpr ::std::size_t szmx{::std::numeric_limits<::std::size_t>::max()};
	constexpr ::std::size_t szmx_noknown{szmx - known_size_at_compilation};
#if FAST_IO_HAS_BUILTIN(__builtin_add_overflow)
	::std::size_t total_sum FAST_IO_INDETERMINATE;
	if (__builtin_add_overflow(known_size_at_compilation, filenamesz, __builtin_addressof(total_sum)))
	{
		::fast_io::fast_terminate();
	}
	if (__builtin_add_overflow(total_sum, functionnamesz, __builtin_addressof(total_sum)))
	{
		::fast_io::fast_terminate();
	}
	return total_sum;
#else
	if (szmx_noknown < filenamesz)
	{
		::fast_io::fast_terminate();
	}
	if (static_cast<::std::size_t>(szmx_noknown - filenamesz) < functionnamesz)
	{
		::fast_io::fast_terminate();
	}
	return known_size_at_compilation + filenamesz + functionnamesz;
#endif
}

template <::std::integral char_type>
	requires(sizeof(char_type) == 1)
inline constexpr basic_reserve_scatters_define_result<char_type>
prrsv_reserve_scatters_source_location_define_impl(::fast_io::basic_io_scatter_t<char_type> *pscatters,
												   char_type *pbuffer, ::std::source_location const &location) noexcept
{
	using chartypeconstaliasptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= char_type const *;
	if constexpr (::std::same_as<char_type, char>)
	{
		char const *fnm{location.file_name()};
		*pscatters = ::fast_io::basic_io_scatter_t<char_type>{fnm, ::fast_io::cstr_len(fnm)};
	}
	else
	{
		char_type const *fnm{reinterpret_cast<chartypeconstaliasptr>(location.file_name())};
		*pscatters = ::fast_io::basic_io_scatter_t<char_type>{fnm, ::fast_io::cstr_len(fnm)};
	}
	++pscatters;
	char_type *iter{pbuffer};
	*iter = ::fast_io::char_literal_v<u8':', char_type>;
	++iter;
	iter = print_reserve_define(::fast_io::io_reserve_type<char_type, ::std::uint_least32_t>, iter, location.line());
	*iter = ::fast_io::char_literal_v<u8':', char_type>;
	++iter;
	iter = print_reserve_define(::fast_io::io_reserve_type<char_type, ::std::uint_least32_t>, iter, location.column());
	*iter = ::fast_io::char_literal_v<u8':', char_type>;
	++iter;
	*pscatters = ::fast_io::basic_io_scatter_t<char_type>{pbuffer, static_cast<::std::size_t>(iter - pbuffer)};
	++pscatters;
	if constexpr (::std::same_as<char_type, char>)
	{
		char const *fnm{location.function_name()};
		*pscatters = ::fast_io::basic_io_scatter_t<char_type>{fnm, ::fast_io::cstr_len(fnm)};
	}
	else
	{
		char_type const *fnm{reinterpret_cast<chartypeconstaliasptr>(location.function_name())};
		*pscatters = ::fast_io::basic_io_scatter_t<char_type>{fnm, ::fast_io::cstr_len(fnm)};
	}
	++pscatters;
	return {pscatters, iter};
}

} // namespace details

template <::std::integral char_type>
	requires(sizeof(char_type) == 1)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr ::std::size_t
print_reserve_size(::fast_io::io_reserve_type_t<char_type, ::std::source_location>, ::std::source_location const &location) noexcept
{
	return ::fast_io::details::prrsv_reserve_size_source_location_impl(location);
}

template <::std::integral char_type>
	requires(sizeof(char_type) == 1)
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
inline constexpr basic_reserve_scatters_define_result<char_type>
print_reserve_scatters_define(::fast_io::io_reserve_type_t<char_type, ::std::source_location>,
							  ::fast_io::basic_io_scatter_t<char_type> *pscatters, char_type *pbuffer,
							  ::std::source_location const &location) noexcept
{
	return ::fast_io::details::prrsv_reserve_scatters_source_location_define_impl(pscatters, pbuffer, location);
}

namespace manipulators
{

inline
#ifdef __cpp_consteval
	consteval
#else
	constexpr
#endif
	::std::source_location
	cur_src_loc(::std::source_location loc = ::std::source_location::current()) noexcept
{
	return loc;
}

} // namespace manipulators

} // namespace fast_io
