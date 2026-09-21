#pragma once

namespace fast_io::details
{

/*
 * Control-byte layout follows abseil's raw_hash_set:
 *
 *   ctrl[0 .. cap)                    real bytes (empty/deleted/h2 of full slot)
 *   ctrl[cap]                         sentinel
 *   ctrl[cap+1 .. cap+1+cloned)       clones of ctrl[0 .. cloned)
 *
 * where cloned = swiss_table_cloned_counts == group_counts - 1, so that a
 * group load starting at any probe offset stays in bounds. Capacities are
 * always 2^m - 1 (or 0) so that `cap` itself is the probe mask: probe offsets
 * and clone mirroring are computed with `& cap`.
 */

inline constexpr ::std::size_t swiss_table_group_counts{16u};
inline constexpr ::std::size_t swiss_table_cloned_counts{swiss_table_group_counts - 1u};
inline constexpr ::std::size_t swiss_table_min_capacity{swiss_table_cloned_counts};

enum class swiss_table_ctrl : ::std::uint_least8_t
{
	empty = 0b10000000,
	deleted = 0b11111110,
	sentinel = 0b11111111
};

inline constexpr ::std::size_t swiss_table_hash_h1(::std::uint_least64_t hash) noexcept
{
	return static_cast<::std::size_t>(hash >> 7u);
}

inline constexpr ::std::uint_least8_t swiss_table_hash_h2(::std::uint_least64_t hash) noexcept
{
	return static_cast<::std::uint_least8_t>(hash & 0x7Fu);
}

inline constexpr bool swiss_table_ctrl_is_full(::std::uint_least8_t ctrl) noexcept
{
	return static_cast<signed char>(ctrl) >= 0;
}

inline constexpr bool swiss_table_ctrl_is_empty(::std::uint_least8_t ctrl) noexcept
{
	return ctrl == static_cast<::std::uint_least8_t>(::fast_io::details::swiss_table_ctrl::empty);
}

inline constexpr bool swiss_table_ctrl_is_deleted(::std::uint_least8_t ctrl) noexcept
{
	return ctrl == static_cast<::std::uint_least8_t>(::fast_io::details::swiss_table_ctrl::deleted);
}

inline constexpr bool swiss_table_ctrl_is_empty_or_deleted(::std::uint_least8_t ctrl) noexcept
{
	return static_cast<signed char>(ctrl) <
		   static_cast<signed char>(::fast_io::details::swiss_table_ctrl::sentinel);
}

template <::std::integral T>
inline constexpr ::fast_io::intrinsics::simd_vector<T, ::fast_io::details::swiss_table_group_counts>
swiss_table_group_splat(T v) noexcept
{
	::fast_io::intrinsics::simd_vector<T, ::fast_io::details::swiss_table_group_counts> pat{};
	for (::std::size_t i{}; i != ::fast_io::details::swiss_table_group_counts; ++i)
	{
		pat.value[i] = v;
	}
	return pat;
}

// Bit i of the result is the most significant bit of byte lane i.
template <::std::integral T>
inline constexpr ::std::uint_least32_t swiss_table_group_bitmask(
	::fast_io::intrinsics::simd_vector<T, ::fast_io::details::swiss_table_group_counts> const &v) noexcept
{
	static_assert(sizeof(T) == sizeof(char));
#if defined(__SSE2__) && defined(__x86_64__) && __has_cpp_attribute(__gnu__::__vector_size__) && \
	FAST_IO_HAS_BUILTIN(__builtin_ia32_pmovmskb128)
#if __cpp_if_consteval >= 202106L
	if !consteval
#else
	if (!__builtin_is_constant_evaluated())
#endif
	{
		using x86_64_v16qi [[__gnu__::__vector_size__(16)]] = char;
		return static_cast<::std::uint_least32_t>(__builtin_ia32_pmovmskb128((x86_64_v16qi)v.value));
	}
#endif
	auto const arr{static_cast<::fast_io::intrinsics::simd_vector<::std::uint_least64_t, 2>>(v)};
	::std::uint_least64_t lo{arr[0]};
	::std::uint_least64_t hi{arr[1]};
	if constexpr (::std::endian::native != ::std::endian::little)
	{
		lo = ::fast_io::byte_swap(lo);
		hi = ::fast_io::byte_swap(hi);
	}
	constexpr ::std::uint_least64_t msbs{0x8080808080808080ULL};
	constexpr ::std::uint_least64_t gatherer{0x0002040810204081ULL};
	return static_cast<::std::uint_least32_t>(
		(((lo & msbs) * gatherer) >> 56u) |
		((((hi & msbs) * gatherer) >> 56u) << 8u));
}

struct swiss_table_group
{
	::fast_io::intrinsics::simd_vector<::std::uint_least8_t, ::fast_io::details::swiss_table_group_counts> ctrlv;

	inline explicit swiss_table_group(::std::uint_least8_t const *pos) noexcept
	{
		ctrlv.load(pos);
	}

	inline constexpr ::std::uint_least32_t match(::std::uint_least8_t h2) const noexcept
	{
		return ::fast_io::details::swiss_table_group_bitmask(
			ctrlv == ::fast_io::details::swiss_table_group_splat(h2));
	}

	inline constexpr ::std::uint_least32_t mask_empty() const noexcept
	{
		return ::fast_io::details::swiss_table_group_bitmask(
			ctrlv == ::fast_io::details::swiss_table_group_splat(
						 static_cast<::std::uint_least8_t>(::fast_io::details::swiss_table_ctrl::empty)));
	}

	inline constexpr ::std::uint_least32_t mask_empty_or_deleted() const noexcept
	{
		using svec_type = ::fast_io::intrinsics::simd_vector<signed char, ::fast_io::details::swiss_table_group_counts>;
		auto const cmp{static_cast<svec_type>(ctrlv) <
					   ::fast_io::details::swiss_table_group_splat<signed char>(static_cast<signed char>(-1))};
		return ::fast_io::details::swiss_table_group_bitmask(cmp);
	}

	// special (empty/deleted/sentinel) -> empty, full -> deleted.
	// Used by drop-deletes rehash before reinsertion.
	inline constexpr void convert_special_to_empty_and_full_to_deleted(::std::uint_least8_t *dst) const noexcept
	{
		for (::std::size_t i{}; i != ::fast_io::details::swiss_table_group_counts; ++i)
		{
			dst[i] = ::fast_io::details::swiss_table_ctrl_is_full(ctrlv[i])
						 ? static_cast<::std::uint_least8_t>(::fast_io::details::swiss_table_ctrl::deleted)
						 : static_cast<::std::uint_least8_t>(::fast_io::details::swiss_table_ctrl::empty);
		}
	}
};

// Triangular quadratic probe sequence: offset advances by 0, 16, 48, 80, ...
// (index grows by the group width each step), all mod cap+1 via & cap.
struct swiss_table_probe_seq
{
	::std::size_t cap;
	::std::size_t offset;
	::std::size_t index{};

	inline constexpr ::std::size_t offset_at(::std::size_t i) const noexcept
	{
		return (offset + i) & cap;
	}

	inline constexpr void next() noexcept
	{
		index += ::fast_io::details::swiss_table_group_counts;
		offset = (offset + index) & cap;
	}
};

// Writes a control byte and maintains the clone region.
inline constexpr void swiss_table_set_ctrl(::std::uint_least8_t *controls, ::std::size_t cap,
										   ::std::size_t i, ::std::uint_least8_t v) noexcept
{
	controls[i] = v;
	constexpr ::std::size_t cloned{::fast_io::details::swiss_table_cloned_counts};
	controls[((i - cloned) & cap) + cloned] = v;
}

struct swiss_table_imp_common
{
	::std::uint_least8_t *controls;
	::std::size_t cap;
	::std::size_t counts;
	::std::size_t leftmost;
	::std::size_t growth_left;
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
	::std::size_t growth_left;
	void *slots;
};

template <::std::integral chtype>
struct swiss_table_str_imp_common
{
	::std::uint_least8_t *controls;
	::std::size_t cap;
	::std::size_t counts;
	::std::size_t leftmost;
	::std::size_t growth_left;
	::fast_io::details::associative_string<chtype> *slots;
};

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
	auto controls{imp.controls};
	auto slots{imp.slots};
	auto const h2{::fast_io::details::swiss_table_hash_h2(hash)};
	::fast_io::details::swiss_table_probe_seq seq{cap, ::fast_io::details::swiss_table_hash_h1(hash) & cap, {}};
	for (;;)
	{
		::fast_io::details::swiss_table_group const group{controls + seq.offset};
		for (auto m{group.match(h2)}; m; m &= static_cast<::std::uint_least32_t>(m - 1u))
		{
			auto const pos{seq.offset_at(static_cast<::std::size_t>(::std::countr_zero(m)))};
			if (key == slots[pos].key())
			{
				return {pos, true};
			}
		}
		if (auto const emptymask{group.mask_empty()}; emptymask)
		{
			return {seq.offset_at(static_cast<::std::size_t>(::std::countr_zero(emptymask))), false};
		}
		seq.next();
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

// First non-full (empty or deleted) position in the probe sequence of hash.
// Used for insertion and for rehashing.
inline constexpr ::std::size_t swiss_table_find_first_non_full(::std::uint_least8_t const *controls, ::std::size_t cap,
															   ::std::uint_least64_t hash) noexcept
{
	::fast_io::details::swiss_table_probe_seq seq{cap, ::fast_io::details::swiss_table_hash_h1(hash) & cap, {}};
	for (;;)
	{
		if (auto const mask{::fast_io::details::swiss_table_group{controls + seq.offset}.mask_empty_or_deleted()}; mask)
		{
			return seq.offset_at(static_cast<::std::size_t>(::std::countr_zero(mask)));
		}
		seq.next();
	}
}

// Whether the erased slot's probe cluster never overflowed a group boundary.
// If true the slot can be marked empty instead of deleted. References abseil's
// was_never_full.
inline constexpr bool swiss_table_was_never_full(::std::uint_least8_t const *controls, ::std::size_t cap,
												 ::std::size_t index) noexcept
{
	constexpr ::std::size_t group_counts{::fast_io::details::swiss_table_group_counts};
	if (cap <= group_counts)
	{
		return true;
	}
	auto const index_before{static_cast<::std::size_t>((index - group_counts) & cap)};
	auto const empty_after{::fast_io::details::swiss_table_group{controls + index}.mask_empty()};
	auto const empty_before{::fast_io::details::swiss_table_group{controls + index_before}.mask_empty()};
	return empty_after && empty_before &&
		   (static_cast<::std::size_t>(::std::countr_zero(empty_after)) +
			static_cast<::std::size_t>(::std::countl_zero(static_cast<::std::uint16_t>(empty_before)))) < group_counts;
}

// Marks slot pos as erased: empty when the slot was never full (reclaimable),
// deleted otherwise. Maintains the clone region and growth_left.
template <typename imptype>
inline constexpr void swiss_table_erase_meta_only(imptype &imp, ::std::size_t pos) noexcept
{
	if (::fast_io::details::swiss_table_was_never_full(imp.controls, imp.cap, pos))
	{
		::fast_io::details::swiss_table_set_ctrl(imp.controls, imp.cap, pos,
												 static_cast<::std::uint_least8_t>(::fast_io::details::swiss_table_ctrl::empty));
		++imp.growth_left;
	}
	else
	{
		::fast_io::details::swiss_table_set_ctrl(imp.controls, imp.cap, pos,
												 static_cast<::std::uint_least8_t>(::fast_io::details::swiss_table_ctrl::deleted));
	}
}

// Number of insertions a table of capacity cap can hold before needing to
// grow or rehash (abseil CapacityToGrowth): cap-1 for small tables so that a
// probe sequence always finds an empty slot, cap*7/8 for large ones.
inline constexpr ::std::size_t swiss_table_capacity_to_growth(::std::size_t cap) noexcept
{
	if (!cap)
	{
		return 0u;
	}
	constexpr ::std::size_t kmax_capacity_for_load_factor_one{::fast_io::details::swiss_table_group_counts * 4u - 1u};
	if (cap <= kmax_capacity_for_load_factor_one)
	{
		return cap - 1u;
	}
	return cap - (cap >> 3u);
}

// Smallest valid capacity whose growth bound can hold `size` elements.
// Ports abseil SizeToCapacity.
inline constexpr ::std::size_t swiss_table_size_to_capacity(::std::size_t size) noexcept
{
	if (!size)
	{
		return 0u;
	}
	constexpr ::std::size_t digits{static_cast<::std::size_t>(::std::numeric_limits<::std::size_t>::digits)};
	constexpr ::std::size_t kmax_capacity_for_load_factor_one{::fast_io::details::swiss_table_group_counts * 4u - 1u};
	if (size >= (::std::numeric_limits<::std::size_t>::max() >> 3u))
	{
		::fast_io::fast_terminate();
	}
	auto leading_zeros{::std::countl_zero(size + (size >= ::fast_io::details::swiss_table_group_counts / 2u))};
	::std::size_t cap;
	if (size < kmax_capacity_for_load_factor_one)
	{
		cap = (~::std::size_t{}) >> leading_zeros;
	}
	else
	{
		constexpr ::std::size_t klast3bits{::std::size_t{7u} << (digits - 3u)};
		auto const max_size_for_next_capacity{static_cast<::std::size_t>(klast3bits >> leading_zeros)};
		leading_zeros -= static_cast<int>(size > max_size_for_next_capacity);
		cap = (~::std::size_t{}) >> leading_zeros;
	}
	return cap < ::fast_io::details::swiss_table_min_capacity ? ::fast_io::details::swiss_table_min_capacity : cap;
}

inline constexpr ::std::size_t str_swiss_table_grow_compute_newcap(::std::size_t oldcap) noexcept
{
	if (oldcap == 0)
	{
		return ::fast_io::details::swiss_table_min_capacity;
	}
	constexpr ::std::size_t mx{::std::numeric_limits<::std::size_t>::max()};
	constexpr ::std::size_t mxdv2{(mx - 1u) >> 1u};
	if (mxdv2 < oldcap)
	{
		::fast_io::fast_terminate();
	}
	return (oldcap << 1u) | 1u;
}

inline constexpr ::std::size_t str_swiss_table_reserve_compute_newcap(::std::size_t n) noexcept
{
	return ::fast_io::details::swiss_table_size_to_capacity(n);
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
	} while (::fast_io::details::swiss_table_ctrl_is_empty_or_deleted(*controlpos));
	return controlpos;
}

} // namespace fast_io::details
