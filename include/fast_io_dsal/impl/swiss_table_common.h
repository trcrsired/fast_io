#pragma once

namespace fast_io::details
{

struct swiss_table_imp_common
{
	::std::uint_least8_t *controls;
	::std::size_t cap;
	::std::size_t counts;
};

template <::std::integral chtype>
struct swiss_table_str_imp_common
{
	::std::uint_least8_t *controls;
	::std::size_t cap;
	::std::size_t counts;
	::fast_io::details::associative_string<chtype> *slots;
};

inline constexpr ::std::uint_least64_t swiss_table_hash_h1(::std::uint_least64_t hash) noexcept
{
	return hash >> 7u;
}

enum class swiss_table_ctrl : ::std::uint_least8_t
{
	empty = 0b10000000,
	deleted = 0b11111110,
	sentinel = 0b11111111
};

inline constexpr ::std::uint_least8_t swiss_table_hash_h2(::std::uint_least64_t hash) noexcept
{
	return static_cast<::std::uint_least8_t>(hash & 0x7F);
}

struct swiss_table_find_result
{
	::std::size_t pos;
	bool found;
};

template <typename imptype, typename keytype>
inline constexpr ::fast_io::details::swiss_table_find_result swiss_table_find_common(imptype const &imp,
																					 keytype key, ::std::uint_least64_t hash) noexcept
{
	::std::size_t const cap{imp.cap};
	auto controls{imp.controls};
	auto slots{imp.slots};
	auto h1{::fast_io::details::swiss_table_hash_h1(hash)};
	auto h2{::fast_io::details::swiss_table_hash_h2(hash)};
	::std::size_t pos{h1 & cap};
	for (;;)
	{
		auto controlspos{controls[pos]};
		if (h2 == controlspos && key == slots[pos].key())
		{
			return {pos, true};
		}
		if (controlspos == static_cast<::std::uint_least8_t>(::fast_io::details::swiss_table_ctrl::empty))
		{
			return {pos, false};
		}
		if ((++pos) == cap) [[unlikely]]
		{
			pos = 0;
		}
	}
}

template <::std::integral chtype>
inline constexpr ::fast_io::details::swiss_table_find_result swiss_table_find_common_with_str(
	::fast_io::details::swiss_table_str_imp_common<chtype> const &imp,
	chtype const *keybase, ::std::size_t keylen, ::std::uint_least64_t hash) noexcept
{
	::fast_io::basic_string_view<chtype> key(keybase, keylen);
	return ::fast_io::details::swiss_table_find_common(imp, key, hash);
}

} // namespace fast_io::details
