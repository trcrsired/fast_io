#pragma once

namespace fast_io
{


namespace containers
{
template <::std::integral chtype, typename Allocator>
class basic_str_swiss_set
{
public:
	using char_type = ch_type;
	using string_view_type = ::fast_io::containers::basic_string_view<char_type>;
	using cstring_view_type = ::fast_io::containers::basic_cstring_view<char_type>;
	using allocator_type = Allocator;
	using size_type = ::std::size_t;
	using difference_type = ::std::ptrdiff_t;

	::fast_io::details::swiss_table_str_imp_common<char_type> imp;

	constexpr basic_str_swiss_set() noexcept = default;

#if 0
	constexpr bool insert_key(string_view_type key) noexcept
	{
	}
#endif
	constexpr bool contains(string_view_type key) const noexcept
	{
		return ::fast_io::containers::details::str_btree_contains(this->imp, key.ptr, key.n, ::fast_io::details::xxh3_64bits_fio(key.data(), key.size_bytes()));
	}
}
} // namespace containers

} // namespace fast_io