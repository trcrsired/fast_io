#pragma once

namespace fast_io::details
{
/*
The string is null terminated
*/
template <::std::integral chtype>
struct associative_string
{
	using char_type = chtype;
	using size_type = ::std::size_t;
	char_type const *ptr;
	size_type n;
	constexpr ::fast_io::basic_string_view<char_type> strvw() const noexcept
	{
		return ::fast_io::basic_string_view<char_type>(ptr, n);
	}
};

template <typename allocator_type, ::std::integral char_type>
inline constexpr ::fast_io::details::associative_string<char_type> create_associative_string(char_type const *p, ::std::size_t n) noexcept
{
	using typed_allocator_type = ::fast_io::typed_generic_allocator_adapter<allocator_type, char_type>;
	if (n == SIZE_MAX) [[unlikely]]
	{
		::fast_io::fast_terminate();
	}
	::std::size_t const np1{static_cast<::std::size_t>(n + 1u)};
	char_type *newp{typed_allocator_type::allocate(np1)};
	*::fast_io::details::non_overlapped_copy_n(p, n, newp) = 0;
	return {newp, n};
}

} // namespace fast_io::details