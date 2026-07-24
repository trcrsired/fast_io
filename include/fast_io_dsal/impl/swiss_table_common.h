#pragma once

namespace fast_io::details
{

struct swiss_table_imp_common
{
	::std::uint_least8_t *controls;
	::std::size_t cap;
	::std::size_t counts;
	::std::size_t leftmost;
};


template <::std::integral chtype>
struct
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	swiss_table_str_type_erased_imp_common
{
	::std::uint_least8_t *controls;
	::std::size_t cap;
	::std::size_t counts;
	::std::size_t leftmost;
	void *slots;
};

template <::std::integral chtype>
struct swiss_table_str_imp_common
{
	::std::uint_least8_t *controls;
	::std::size_t cap;
	::std::size_t counts;
	::std::size_t leftmost;
	::fast_io::details::associative_string<chtype> *slots;
};

inline constexpr ::std::size_t swiss_table_hash_h1(::std::uint_least64_t hash) noexcept
{
	return static_cast<::std::size_t>(hash >> 7u);
}

enum class swiss_table_ctrl : ::std::uint_least8_t
{
	empty = 0b10000000,
	deleted = 0b11111110,
	sentinel = 0b11111111
	//	full = 0
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
	if (!cap)
	{
		return {0u, false};
	}
	::std::size_t const capm1{static_cast<::std::size_t>(cap - 1u)};
	auto controls{imp.controls};
	auto slots{imp.slots};
	auto h1{::fast_io::details::swiss_table_hash_h1(hash)};
	auto h2{::fast_io::details::swiss_table_hash_h2(hash)};
	::std::size_t pos{h1 & capm1};
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

template <::std::integral chtype, typename T>
inline constexpr ::fast_io::details::swiss_table_find_result swiss_table_find_common_with_str(
	T const &imp,
	chtype const *keybase, ::std::size_t keylen, ::std::uint_least64_t hash) noexcept
{
	::fast_io::basic_string_view<chtype> key(keybase, keylen);
	return ::fast_io::details::swiss_table_find_common(imp, key, hash);
}

template <::std::integral chtype, typename hasher, typename T>
inline constexpr ::fast_io::details::swiss_table_find_result swiss_table_find_common_with_str_hashfunc_with_hasher(
	T const &imp,
	chtype const *keybase, ::std::size_t keylen, hasher hash) noexcept
{
	return ::fast_io::details::swiss_table_find_common_with_str(imp, keybase, keylen,
																hash.do_hash(reinterpret_cast<::std::byte const *>(keybase), reinterpret_cast<::std::byte const *>(keybase + keylen)));
}

inline constexpr bool str_swiss_table_need_grow(
	::std::size_t counts, ::std::size_t cap) noexcept
{
	::std::size_t high;
	auto const low{::fast_io::intrinsics::umul(cap, static_cast<::std::size_t>(7), high)};
	constexpr auto shift{static_cast<unsigned>(::std::numeric_limits<::std::size_t>::digits - 3u)};
	return ((high << shift) | (low >> 3u)) <= counts;
}

inline constexpr ::std::size_t str_swiss_table_grow_compute_newcap(::std::size_t oldcap) noexcept
{
	if (oldcap == 0)
	{
		return 8u;
	}

	// add overflow trapping here
	constexpr ::std::size_t mx{::std::numeric_limits<::std::size_t>::max()};
	constexpr ::std::size_t mxdv2{(mx - 1u) >> 1u};
	if (mxdv2 < oldcap)
	{
		::fast_io::fast_terminate();
	}
	return (oldcap << 1u);
}

template <bool isprev>
inline constexpr ::std::uint_least8_t const *swiss_table_iterator_common(::std::uint_least8_t const *controlpos) noexcept
{
	do
	{
		if constexpr (isprev)
		{
			--controlpos;
		}
		else
		{
			++controlpos;
		}
	} while ((*controlpos >> 7u));
	return controlpos;
}

} // namespace fast_io::details
