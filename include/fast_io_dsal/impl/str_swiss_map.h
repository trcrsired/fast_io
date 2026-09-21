#pragma once

namespace fast_io::containers
{
template <::std::integral chtype, typename valtype>
struct basic_str_swiss_map_key_mapped_pair
{
	using char_type = chtype;
	using key_type = ::fast_io::containers::basic_cstring_view<char_type>;
	using mapped_type = valtype;
	::fast_io::details::associative_string<chtype> ky;
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
	[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address) >= 201803
	[[no_unique_address]]
#endif
#endif
	mapped_type val;
	constexpr key_type key() const noexcept
	{
		return ky.strvw();
	}
	constexpr mapped_type &mapped() noexcept
	{
		return val;
	}
	constexpr mapped_type const &mapped() const noexcept
	{
		return val;
	}
};

template <::std::integral chtype, typename mappedtype>
struct basic_str_swiss_map_key_mapped_initializer_list_pair
{
	using char_type = chtype;
	using key_type = ::fast_io::containers::basic_string_view<char_type>;
	using mapped_type = mappedtype;

	::fast_io::containers::basic_string_view<chtype> ky;
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
	[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address) >= 201803
	[[no_unique_address]]
#endif
#endif
	mappedtype val;
	constexpr key_type key() const noexcept
	{
		return ky;
	}
	constexpr mapped_type &mapped() noexcept
	{
		return val;
	}
	constexpr mapped_type const &mapped() const noexcept
	{
		return val;
	}
};

template <::std::integral chtype, typename mappedtype>
inline constexpr bool operator==(::fast_io::containers::basic_str_swiss_map_key_mapped_pair<chtype, mappedtype> const &a,
								 ::fast_io::containers::basic_str_swiss_map_key_mapped_pair<chtype, mappedtype> const &b) noexcept
{
	return a.key() == b.key() && a.mapped() == b.mapped();
}

} // namespace fast_io::containers

namespace fast_io::details
{

template <::std::integral chtype, typename mappedtype>
struct str_swiss_map_imp_common
{
	::std::uint_least8_t *controls;
	::std::size_t cap;
	::std::size_t counts;
	::std::size_t leftmost;
	::std::size_t growth_left;
	::fast_io::containers::basic_str_swiss_map_key_mapped_pair<chtype, mappedtype> *slots;
};

template <::std::integral chtype, typename mappedtype, bool isconst>
struct str_swiss_map_iterator
{
	using value_type = ::fast_io::containers::basic_str_swiss_map_key_mapped_pair<chtype, mappedtype>;
	using const_reference = value_type const &;
	using reference = ::std::conditional_t<isconst, const_reference, value_type &>;
	using iterator_tag = ::std::bidirectional_iterator_tag;
	using difference_type = ::std::ptrdiff_t;
	using const_pointer = value_type const *;
	using pointer = ::std::conditional_t<isconst, const_pointer, value_type *>;
	::std::uint_least8_t const *controlpos{};
	value_type *slots{};
	constexpr reference operator*() const noexcept
	{
		return *slots;
	}
	constexpr pointer operator->() const noexcept
	{
		return slots;
	}
	constexpr str_swiss_map_iterator &operator++() noexcept
	{
		auto oldcontrolpos{this->controlpos};
		auto newcontrolpos{::fast_io::details::swiss_table_iterator_common<false>(oldcontrolpos)};
		this->controlpos = newcontrolpos;
		auto diff{newcontrolpos - oldcontrolpos};
#if __has_cpp_attribute(assume)
		[[assume(0 < diff)]];
#endif
		this->slots += static_cast<::std::size_t>(diff);
		return *this;
	}
	constexpr str_swiss_map_iterator operator++(int) noexcept
	{
		auto temp{*this};
		++*this;
		return temp;
	}
	constexpr str_swiss_map_iterator &operator--() noexcept
	{
		auto oldcontrolpos{this->controlpos};
		auto newcontrolpos{::fast_io::details::swiss_table_iterator_common<true>(oldcontrolpos)};
		this->controlpos = newcontrolpos;
		auto diff{oldcontrolpos - newcontrolpos};
#if __has_cpp_attribute(assume)
		[[assume(0 < diff)]];
#endif
		this->slots -= static_cast<::std::size_t>(diff);
		return *this;
	}
	constexpr str_swiss_map_iterator operator--(int) noexcept
	{
		auto temp{*this};
		--*this;
		return temp;
	}
	constexpr operator str_swiss_map_iterator<chtype, mappedtype, true>() const noexcept
		requires(!isconst)
	{
		return {this->controlpos, this->slots};
	}
};

template <::std::integral chtype, typename mappedtype, bool isconst1, bool isconst2>
inline constexpr bool operator==(::fast_io::details::str_swiss_map_iterator<chtype, mappedtype, isconst1> a,
								 ::fast_io::details::str_swiss_map_iterator<chtype, mappedtype, isconst2> b) noexcept
{
	return a.controlpos == b.controlpos;
}


template <::std::integral chtype, typename mappedtype>
struct str_swiss_map_insert_key_result
{
	::fast_io::details::str_swiss_map_iterator<chtype, mappedtype, false> position;
	bool inserted;
};

template <typename hvaltype, ::std::integral chtype, typename mappedtype>
struct str_swiss_map_insert_key_result_with_hval
{
	hvaltype hval;
	::std::size_t position;
	bool inserted;
};

template <typename allocator_type, typename hasher, ::std::integral chtype, typename mappedtype>
inline constexpr void str_swiss_map_reserve_to_newcap(
	::fast_io::details::str_swiss_map_imp_common<chtype, mappedtype> &imp, ::std::size_t newcap, hasher hash) noexcept
{
	using char_type = chtype;
	using slot_type = ::fast_io::containers::basic_str_swiss_map_key_mapped_pair<char_type, mappedtype>;
	using typed_slot_allocator_type = ::fast_io::typed_generic_allocator_adapter<allocator_type, slot_type>;
	using typed_ctrl_allocator_type = ::fast_io::typed_generic_allocator_adapter<allocator_type, ::std::uint_least8_t>;

	auto oldcontrols{imp.controls};
	auto oldslots{imp.slots};
	auto const oldcap{imp.cap};

	auto newcontrols{typed_ctrl_allocator_type::allocate(static_cast<::std::size_t>(newcap + ::fast_io::details::swiss_table_group_counts))};
	auto newslots{typed_slot_allocator_type::allocate(newcap)};

	::fast_io::freestanding::my_memset(newcontrols,
									   static_cast<int>(::fast_io::details::swiss_table_ctrl::empty),
									   static_cast<::std::size_t>(newcap + ::fast_io::details::swiss_table_group_counts));
	newcontrols[newcap] = static_cast<::std::uint_least8_t>(::fast_io::details::swiss_table_ctrl::sentinel);

	imp.controls = newcontrols;
	imp.cap = newcap;
	imp.slots = newslots;

	// Rehash existing entries
	if (oldcap == 0)
	{
		imp.leftmost = newcap;
	}
	else
	{
		::std::size_t leftmost{newcap};
		for (::std::size_t i{}; i != oldcap; ++i)
		{
			if (::fast_io::details::swiss_table_ctrl_is_full(oldcontrols[i]))
			{
				auto &oldslot{oldslots[i]};
				auto const oldhash{hash.do_hash(reinterpret_cast<::std::byte const *>(oldslot.ky.ptr), reinterpret_cast<::std::byte const *>(oldslot.ky.ptr + oldslot.ky.n))};
				auto pos{::fast_io::details::swiss_table_find_first_non_full(newcontrols, newcap, oldhash)};
				::fast_io::details::swiss_table_set_ctrl(newcontrols, newcap, pos,
														 ::fast_io::details::swiss_table_hash_h2(oldhash));
				::std::construct_at(newslots + pos, ::std::move(oldslot));
				if constexpr (!::std::is_trivially_destructible_v<mappedtype>)
				{
					::std::destroy_at(__builtin_addressof(oldslot));
				}
				if (pos < leftmost)
				{
					leftmost = pos;
				}
			}
		}
		imp.leftmost = leftmost;
		typed_ctrl_allocator_type::deallocate_n(oldcontrols, static_cast<::std::size_t>(oldcap + ::fast_io::details::swiss_table_group_counts));
		typed_slot_allocator_type::deallocate_n(oldslots, oldcap);
	}
	imp.growth_left = ::fast_io::details::swiss_table_capacity_to_growth(newcap) - imp.counts;
}

template <typename allocator_type, typename hasher, ::std::integral chtype, typename mappedtype>
inline constexpr void str_swiss_map_reserve(
	::fast_io::details::str_swiss_map_imp_common<chtype, mappedtype> &imp, ::std::size_t n, hasher hash) noexcept
{
	if (n <= imp.counts + imp.growth_left)
	{
		return;
	}
	::std::size_t newcap{::fast_io::details::str_swiss_table_reserve_compute_newcap(n)};
	if (newcap <= imp.cap)
	{
		return;
	}
	::fast_io::details::str_swiss_map_reserve_to_newcap<allocator_type, hasher, chtype>(imp, newcap, hash);
}

template <typename allocator_type, typename hasher, ::std::integral chtype, typename mappedtype>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr void str_swiss_map_grow(
	::fast_io::details::str_swiss_map_imp_common<chtype, mappedtype> &imp, hasher hash) noexcept
{
	::fast_io::details::str_swiss_map_reserve_to_newcap<allocator_type, hasher, chtype>(
		imp,
		::fast_io::details::str_swiss_table_grow_compute_newcap(imp.cap), hash);
}


#if 0
template <typename allocator_type, ::std::integral chtype>
inline void str_swiss_map_insert_key_internal_trivial(
	::fast_io::details::swiss_table_str_type_erased_imp_common<chtype> &imp,
	::std::size_t pos, chtype const *keybase, ::std::size_t keylen, ::std::uint_least64_t hash,
	void const* val, ::std::size_t valsz) noexcept
{
	using char_type = chtype;
	auto const h2{::fast_io::details::swiss_table_hash_h2(hash)};
	imp.controls[pos] = h2;
	auto &slot{imp.slots[pos]};

	slot.ky = ::fast_io::details::create_associative_string<allocator_type, char_type>(keybase, keylen);
	if (valsz) [[likely]]
		::fast_io::freestanding::memcpy(__builtin_addressof(slot.val), val, valsz);

//	::std::construct_at(__builtin_addressof(slot.val), ::std::move(val));
	auto newleftmost{pos};
	auto counts{imp.counts};
	if (counts)
	{
		if (newleftmost < imp.leftmost)
		{
			imp.leftmost = newleftmost;
		}
	}
	else
	{
		imp.leftmost = newleftmost;
	}
	imp.counts = static_cast<::std::size_t>(counts + 1u);
}
#endif

template <typename allocator_type, ::std::integral chtype, typename mappedtype>
inline constexpr void str_swiss_map_insert_key_internal_computed_slot_controls(
	::fast_io::details::str_swiss_map_imp_common<chtype, mappedtype> &imp,
	::std::size_t pos, chtype const *keybase, ::std::size_t keylen, ::std::uint_least64_t hash, ::std::uint_least8_t *pcontrol,
	::fast_io::containers::basic_str_swiss_map_key_mapped_pair<chtype, mappedtype> *pslot) noexcept
{
	using char_type = chtype;
	auto const h2{::fast_io::details::swiss_table_hash_h2(hash)};
	if (::fast_io::details::swiss_table_ctrl_is_empty(*pcontrol))
	{
		--imp.growth_left;
	}
	::fast_io::details::swiss_table_set_ctrl(imp.controls, imp.cap, pos, h2);
	pslot->ky = ::fast_io::details::create_associative_string<allocator_type, char_type>(keybase, keylen);
	if (pos < imp.leftmost)
	{
		imp.leftmost = pos;
	}
	++imp.counts;
}

template <typename allocator_type, ::std::integral chtype, typename mappedtype>
inline constexpr void str_swiss_map_insert_key_internal(
	::fast_io::details::str_swiss_map_imp_common<chtype, mappedtype> &imp,
	::std::size_t pos, chtype const *keybase, ::std::size_t keylen, ::std::uint_least64_t hash) noexcept
{
	::fast_io::details::str_swiss_map_insert_key_internal_computed_slot_controls<allocator_type>(imp, pos, keybase, keylen, hash, imp.controls + pos, imp.slots + pos);
}

// Drops tombstones by rehashing the table in place without resizing.
// References abseil DropDeletesWithoutResize.
template <typename allocator_type, typename hasher, ::std::integral chtype, typename mappedtype>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr void str_swiss_map_rehash_in_place(
	::fast_io::details::str_swiss_map_imp_common<chtype, mappedtype> &imp, hasher hash) noexcept
{
	auto const controls{imp.controls};
	auto const slots{imp.slots};
	auto const cap{imp.cap};
	// special -> empty, full -> deleted (live slots are now tombstones)
	for (::std::size_t i{}; i != cap; ++i)
	{
		auto &ci{controls[i]};
		ci = ::fast_io::details::swiss_table_ctrl_is_full(ci)
				 ? static_cast<::std::uint_least8_t>(::fast_io::details::swiss_table_ctrl::deleted)
				 : static_cast<::std::uint_least8_t>(::fast_io::details::swiss_table_ctrl::empty);
	}
	controls[cap] = static_cast<::std::uint_least8_t>(::fast_io::details::swiss_table_ctrl::sentinel);
	__builtin_memcpy(controls + cap + 1u, controls, ::fast_io::details::swiss_table_cloned_counts);

	auto const probe_index{[cap](::std::size_t pos, ::std::size_t offset) noexcept {
		return ((pos - offset) & cap) / ::fast_io::details::swiss_table_group_counts;
	}};
	::std::size_t leftmost{cap};
	for (::std::size_t i{}; i != cap; ++i)
	{
		if (!::fast_io::details::swiss_table_ctrl_is_deleted(controls[i]))
		{
			continue;
		}
		for (;;)
		{
			auto const h{hash.do_hash(reinterpret_cast<::std::byte const *>(slots[i].ky.ptr),
									  reinterpret_cast<::std::byte const *>(slots[i].ky.ptr + slots[i].ky.n))};
			auto const offset{::fast_io::details::swiss_table_hash_h1(h) & cap};
			auto const target{::fast_io::details::swiss_table_find_first_non_full(controls, cap, h)};
			if (probe_index(target, offset) == probe_index(i, offset))
			{
				// target is within the same group: the slot stays
				::fast_io::details::swiss_table_set_ctrl(controls, cap, i,
														 ::fast_io::details::swiss_table_hash_h2(h));
				if (i < leftmost)
				{
					leftmost = i;
				}
				break;
			}
			if (::fast_io::details::swiss_table_ctrl_is_empty(controls[target]))
			{
				::fast_io::details::swiss_table_set_ctrl(controls, cap, target,
														 ::fast_io::details::swiss_table_hash_h2(h));
				::std::construct_at(slots + target, ::std::move(slots[i]));
				if constexpr (!::std::is_trivially_destructible_v<mappedtype>)
				{
					::std::destroy_at(__builtin_addressof(slots[i].val));
				}
				::fast_io::details::swiss_table_set_ctrl(controls, cap, i,
														 static_cast<::std::uint_least8_t>(::fast_io::details::swiss_table_ctrl::empty));
				if (target < leftmost)
				{
					leftmost = target;
				}
				break;
			}
			// swap element at i into its probe position and reprocess i
			::fast_io::details::swiss_table_set_ctrl(controls, cap, target,
													 ::fast_io::details::swiss_table_hash_h2(h));
			{
				auto tmp{::std::move(slots[i])};
				slots[i] = ::std::move(slots[target]);
				slots[target] = ::std::move(tmp);
			}
			if (target < leftmost)
			{
				leftmost = target;
			}
		}
	}
	imp.leftmost = leftmost;
	imp.growth_left = ::fast_io::details::swiss_table_capacity_to_growth(cap) - imp.counts;
}

template <bool needdestroy, typename allocator_type, ::std::integral chtype, typename mappedtype>
inline constexpr void str_swiss_map_clear_impl(
	::fast_io::details::str_swiss_map_imp_common<chtype, mappedtype> &imp) noexcept
{
	using char_type = chtype;
	using slot_type = ::fast_io::containers::basic_str_swiss_map_key_mapped_pair<char_type, mappedtype>;
	using typed_slot_allocator_type = ::fast_io::typed_generic_allocator_adapter<allocator_type, slot_type>;
	using typed_ctrl_allocator_type = ::fast_io::typed_generic_allocator_adapter<allocator_type, ::std::uint_least8_t>;

	auto const cap{imp.cap};
	if (!cap)
	{
		return;
	}
	auto controls{imp.controls};
	auto slots{imp.slots};
	for (::std::size_t i{}; i != cap; ++i)
	{
		if (::fast_io::details::swiss_table_ctrl_is_full(controls[i]))
		{
			auto &si{slots[i]};
			::fast_io::details::deallocate_associative_string<allocator_type, char_type>(si.ky.ptr, si.ky.n);
			if constexpr (!::std::is_trivially_destructible_v<mappedtype>)
			{
				::std::destroy_at(__builtin_addressof(si.val));
			}
		}
	}
	if constexpr (needdestroy)
	{
		typed_ctrl_allocator_type::deallocate_n(controls, static_cast<::std::size_t>(cap + ::fast_io::details::swiss_table_group_counts));
		typed_slot_allocator_type::deallocate_n(slots, cap);
		imp = {};
	}
	else
	{
		::fast_io::freestanding::my_memset(controls,
										   static_cast<int>(::fast_io::details::swiss_table_ctrl::empty),
										   static_cast<::std::size_t>(cap + ::fast_io::details::swiss_table_group_counts));
		controls[cap] = static_cast<::std::uint_least8_t>(::fast_io::details::swiss_table_ctrl::sentinel);
		imp.counts = 0u;
		imp.leftmost = cap;
		imp.growth_left = ::fast_io::details::swiss_table_capacity_to_growth(cap);
	}
}

template <typename allocator_type, ::std::integral chtype, typename mappedtype>
inline constexpr ::std::size_t str_swiss_map_erase_rg(::fast_io::details::str_swiss_map_imp_common<chtype, mappedtype> &imp, ::std::size_t first, ::std::size_t last) noexcept
{
	if (first == last)
	{
		return first;
	}
	::std::size_t i{first};
	::std::size_t counting{};
	for (; i != last; ++counting)
	{
		auto &si{imp.slots[i]};
		::fast_io::details::deallocate_associative_string<allocator_type, chtype>(si.ky.ptr, si.ky.n);
		if constexpr (!::std::is_trivially_destructible_v<mappedtype>)
		{
			::std::destroy_at(__builtin_addressof(si.val));
		}
		::fast_io::details::swiss_table_erase_meta_only(imp, i);
		i = static_cast<::std::size_t>(::fast_io::details::swiss_table_iterator_common<false>(imp.controls + i) - imp.controls);
	}
	if (!(imp.counts -= counting) || imp.leftmost == first)
	{
		imp.leftmost = i;
	}
	return i;
}

template <bool compute_next, typename allocator_type, ::std::integral chtype, typename mappedtype>
inline constexpr ::std::conditional_t<compute_next, ::std::size_t, void> str_swiss_map_erase(::fast_io::details::str_swiss_map_imp_common<chtype, mappedtype> &imp, ::std::size_t pos) noexcept
{
	auto &si{imp.slots[pos]};
	::fast_io::details::deallocate_associative_string<allocator_type, chtype>(si.ky.ptr, si.ky.n);
	if constexpr (!::std::is_trivially_destructible_v<mappedtype>)
	{
		::std::destroy_at(__builtin_addressof(si.val));
	}
	::fast_io::details::swiss_table_erase_meta_only(imp, pos);
	auto controls{imp.controls};
	auto controlspos{controls + pos};
	if constexpr (compute_next)
	{
		::std::size_t next{imp.cap};
		if (--imp.counts)
		{
			next = static_cast<::std::size_t>(::fast_io::details::swiss_table_iterator_common<false>(controlspos) - controls);
			if (imp.leftmost == pos)
			{
				imp.leftmost = next;
			}
		}
		else
		{
			imp.leftmost = next;
		}
		return next;
	}
	else
	{
		if (--imp.counts)
		{
			if (imp.leftmost == pos)
			{
				imp.leftmost = static_cast<::std::size_t>(::fast_io::details::swiss_table_iterator_common<false>(controlspos) - controls);
			}
		}
		else
		{
			imp.leftmost = imp.cap;
		}
	}
}

template <typename allocator_type, typename hasher, ::std::integral chtype, typename mappedtype>
inline constexpr bool str_swiss_map_erase_key(::fast_io::details::str_swiss_map_imp_common<chtype, mappedtype> &imp, chtype const *str, ::std::size_t strn, hasher hash) noexcept
{
	auto [pos, found] = ::fast_io::details::swiss_table_find_common_with_str_hashfunc_with_hasher<chtype>(
		imp, str, strn, hash);
	if (!found)
	{
		return false;
	}
	::fast_io::details::str_swiss_map_erase<false, allocator_type, chtype>(imp, pos);
	return true;
}

template <typename allocator_type, ::std::integral chtype, typename mappedtype>
inline constexpr ::fast_io::details::str_swiss_map_imp_common<chtype, mappedtype> str_swiss_map_clone(::fast_io::details::str_swiss_map_imp_common<chtype, mappedtype> const &other) noexcept
{
	using char_type = chtype;
	using mapped_type = mappedtype;
	using slot_type = ::fast_io::containers::basic_str_swiss_map_key_mapped_pair<char_type, mappedtype>;
	using typed_slot_allocator_type = ::fast_io::typed_generic_allocator_adapter<allocator_type, slot_type>;
	using typed_ctrl_allocator_type = ::fast_io::typed_generic_allocator_adapter<allocator_type, ::std::uint_least8_t>;

	auto const cap{other.cap};
	if (!cap)
	{
		return {};
	}
	auto othercontrols{other.controls};
	auto otherslots{other.slots};
	::std::size_t const ctrlsz{static_cast<::std::size_t>(cap + ::fast_io::details::swiss_table_group_counts)};
	auto controls{typed_ctrl_allocator_type::allocate(ctrlsz)};
	auto slots{typed_slot_allocator_type::allocate(cap)};

	::fast_io::freestanding::non_overlapped_copy_n(othercontrols, ctrlsz, controls);
	for (::std::size_t i{}; i != cap; ++i)
	{
		if (::fast_io::details::swiss_table_ctrl_is_full(othercontrols[i]))
		{
			auto &othersi{otherslots[i]};
			auto &si{slots[i]};
			si.ky = ::fast_io::details::create_associative_string<allocator_type, char_type>(othersi.ky.ptr, othersi.ky.n);
			::std::construct_at(__builtin_addressof(si.val), othersi.val);
		}
	}
	return {controls, cap, other.counts, other.leftmost, other.growth_left, slots};
}

template <typename allocator_type, typename hasher, ::std::integral char_type, typename mappedtype>
constexpr ::fast_io::details::str_swiss_map_insert_key_result_with_hval<typename hasher::digest_type, char_type, mappedtype> str_swiss_map_insert_key_with_hash_no_insert(::fast_io::details::str_swiss_map_imp_common<char_type, mappedtype> &imp, char_type const *key, ::std::size_t keyn, hasher hash) noexcept
{
	auto hval{hash.do_hash(reinterpret_cast<::std::byte const *>(key), reinterpret_cast<::std::byte const *>(key + keyn))};
	auto const result{::fast_io::details::swiss_table_find_common_with_str<char_type>(
		imp, key, keyn, hval)};
	::std::size_t pos{result.pos};
	bool inserted{!result.found};
	if (inserted) [[likely]]
	{
		if (!imp.growth_left)
		{
			if (imp.counts < ::fast_io::details::swiss_table_capacity_to_growth(imp.cap))
			{
				// tombstones consume the growth budget: rehash in place
				::fast_io::details::str_swiss_map_rehash_in_place<allocator_type, hasher, char_type>(
					imp, hash);
			}
			else
			{
				::fast_io::details::str_swiss_map_grow<allocator_type, hasher, char_type>(
					imp, hash);
			}
			pos = ::fast_io::details::swiss_table_find_first_non_full(imp.controls, imp.cap, hval);
		}
		else if (::fast_io::details::swiss_table_capacity_to_growth(imp.cap) != imp.growth_left + imp.counts)
		{
			// tombstones exist: reuse the first non-full slot in probe order
			pos = ::fast_io::details::swiss_table_find_first_non_full(imp.controls, imp.cap, hval);
		}
	}
	return {hval, pos, inserted};
}

template <typename allocator_type, typename hasher, ::std::integral char_type, typename mappedtype>
constexpr ::fast_io::details::str_swiss_map_insert_key_result<char_type, mappedtype> str_swiss_map_insert_key_with_hash(::fast_io::details::str_swiss_map_imp_common<char_type, mappedtype> &imp, char_type const *key, ::std::size_t keyn, hasher hash) noexcept
{
	auto [hval, pos, inserted] = ::fast_io::details::str_swiss_map_insert_key_with_hash_no_insert<allocator_type, hasher, char_type>(imp, key, keyn, hash);
	if (inserted)
	{
		::fast_io::details::str_swiss_map_insert_key_internal<allocator_type, char_type>(
			imp, pos, key, keyn, hval);
	}
	return {{imp.controls + pos, imp.slots + pos}, inserted};
}

template <typename chtype, typename valtype, typename R>
concept str_swiss_map_range_has_key_mapped_val = ::std::integral<chtype> &&
												 ::std::ranges::range<R> &&
												 requires(::std::ranges::range_value_t<R> rgval) {
													 { rgval.key() } -> ::std::convertible_to<::fast_io::containers::basic_string_view<chtype>>;
													 { rgval.mapped() } -> ::std::constructible_from<valtype>;
												 };

template <typename chtype, typename valtype, typename R>
concept str_swiss_map_range_has_key_mapped_noexcept = str_swiss_map_range_has_key_mapped_val<chtype, valtype, R> &&
													  ::std::is_nothrow_constructible_v<valtype, decltype(::std::declval<::std::ranges::range_value_t<R>>().mapped())> &&
													  ::std::is_nothrow_constructible_v<::fast_io::containers::basic_string_view<chtype>, decltype(::std::declval<::std::ranges::range_value_t<R>>().key())>;

} // namespace fast_io::details

namespace fast_io
{

namespace containers
{
template <::std::integral chtype, ::std::movable mappedtype, typename Hash, typename Allocator>
	requires(::std::is_nothrow_move_constructible_v<mappedtype>)
class basic_str_swiss_map
{
public:
	using char_type = chtype;
	using mapped_type = mappedtype;
	using key_string_view_type = ::fast_io::containers::basic_string_view<char_type>;
	using key_cstring_view_type = ::fast_io::containers::basic_cstring_view<char_type>;
	using allocator_type = Allocator;
	using size_type = ::std::size_t;
	using difference_type = ::std::ptrdiff_t;
	using const_iterator = ::fast_io::details::str_swiss_map_iterator<chtype, mappedtype, true>;
	using iterator = ::fast_io::details::str_swiss_map_iterator<chtype, mappedtype, false>;
	using const_reverse_iterator = ::std::reverse_iterator<const_iterator>;
	using reverse_iterator = ::std::reverse_iterator<iterator>;
	using insert_result_type = ::fast_io::details::str_swiss_map_insert_key_result<char_type, mapped_type>;
	using hasher = Hash;
	using initializer_list_pair_type = ::fast_io::containers::basic_str_swiss_map_key_mapped_initializer_list_pair<char_type, mapped_type>;

	::fast_io::details::str_swiss_map_imp_common<char_type, mapped_type> imp{};
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
	[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address)
	[[no_unique_address]]
#endif
#endif
	hasher hash{};

	constexpr basic_str_swiss_map() noexcept = default;

	explicit constexpr basic_str_swiss_map(::fast_io::freestanding::from_hasher_t, hasher h) noexcept : hash(h)
	{}
	constexpr basic_str_swiss_map(basic_str_swiss_map const &other) noexcept : imp(::fast_io::details::str_swiss_map_clone<allocator_type, chtype, mappedtype>(other.imp)), hash(other.hash) {};
	constexpr basic_str_swiss_map &operator=(basic_str_swiss_map const &other) noexcept
	{
		if (this != ::std::addressof(other))
		{
			auto tmp = ::fast_io::details::str_swiss_map_clone<allocator_type, chtype, mappedtype>(other.imp);
			this->clear_destroy();
			this->imp = tmp;
			this->hash = other.hash;
		}
		return *this;
	}

private:
	struct construct_range_destroyer
	{
		basic_str_swiss_map *ptr{};
		explicit constexpr construct_range_destroyer(basic_str_swiss_map *p) noexcept
			: ptr{p}
		{}

		construct_range_destroyer(construct_range_destroyer const &) = delete;
		construct_range_destroyer &operator=(construct_range_destroyer const &) = delete;

		constexpr ~construct_range_destroyer()
		{
			if (ptr)
			{
				ptr->clear_destroy();
			}
		}
	};
	template <::std::input_or_output_iterator Iter, ::std::sentinel_for<Iter> Sen>
	constexpr void construct_range_common_with_n(Iter first, Sen last, ::std::size_t sz)
	{
		construct_range_destroyer des(this);
		this->reserve(sz);
		for (; first != last; ++first)
		{
			this->insert_key(first->key(), first->mapped());
		}
		des.ptr = nullptr;
	}
	template <::std::input_or_output_iterator Iter, ::std::sentinel_for<Iter> Sen>
	constexpr void construct_range_common(Iter first, Sen last)
	{
		construct_range_destroyer des(this);
		for (; first != last; ++first)
		{
			this->insert_key(first->key(), first->mapped());
		}
		des.ptr = nullptr;
	}

	template <::std::ranges::range R>
	constexpr void construct_with_range_common(R &&rg) noexcept
	{
		if constexpr (::std::ranges::sized_range<R>)
		{
			this->construct_range_common_with_n(::std::ranges::begin(rg),
												::std::ranges::end(rg), ::std::ranges::size(rg));
		}
		else
		{
			this->construct_range_common(::std::ranges::begin(rg),
										 ::std::ranges::end(rg));
		}
	}

public:
	explicit constexpr basic_str_swiss_map(::std::initializer_list<initializer_list_pair_type> ilist) noexcept
	{
		this->construct_with_range_common(ilist);
	}
	template <::std::ranges::range R>
		requires(::fast_io::details::str_swiss_map_range_has_key_mapped_val<chtype, mappedtype, R>)
	explicit constexpr basic_str_swiss_map(::fast_io::freestanding::from_range_t, R &&rg) noexcept(::fast_io::details::str_swiss_map_range_has_key_mapped_noexcept<chtype, mappedtype, R>)
	{
		this->construct_with_range_common(::std::forward<R>(rg));
	}
	explicit constexpr basic_str_swiss_map(::fast_io::from_hasher_t, hasher h, ::std::initializer_list<initializer_list_pair_type> ilist) noexcept
		: hash(h)
	{
		this->construct_with_range_common(ilist);
	}
	template <::std::ranges::range R>
		requires(::fast_io::details::str_swiss_map_range_has_key_mapped_val<chtype, mappedtype, R>)
	explicit constexpr basic_str_swiss_map(::fast_io::from_hasher_t, hasher h, ::fast_io::freestanding::from_range_t, R &&rg) noexcept(::fast_io::details::str_swiss_map_range_has_key_mapped_noexcept<chtype, mappedtype, R>)
		: hash(h)
	{
		this->construct_with_range_common(::std::forward<R>(rg));
	}
	constexpr basic_str_swiss_map(basic_str_swiss_map &&other) noexcept
		: imp{other.imp}, hash(::std::move(other.hash))
	{
		other.imp = {};
	}

	constexpr basic_str_swiss_map &operator=(basic_str_swiss_map &&other) noexcept
	{
		if (this != ::std::addressof(other))
		{
			::fast_io::details::str_swiss_map_clear_impl<true, allocator_type, char_type>(this->imp);
			this->imp = other.imp;
			other.imp = {};
			this->hash = ::std::move(other.hash);
		}
		return *this;
	}
	constexpr hasher hash_function() const noexcept
	{
		return hash;
	}

	constexpr void clear() noexcept
	{
		::fast_io::details::str_swiss_map_clear_impl<false, allocator_type, char_type>(this->imp);
	}

	constexpr void clear_destroy() noexcept
	{
		::fast_io::details::str_swiss_map_clear_impl<true, allocator_type, char_type>(this->imp);
		this->imp = {};
	}

	constexpr ~basic_str_swiss_map()
	{
		::fast_io::details::str_swiss_map_clear_impl<true, allocator_type, char_type>(this->imp);
	}

	constexpr size_type size() const noexcept
	{
		return this->imp.counts;
	}

	constexpr size_type capacity() const noexcept
	{
		return this->imp.cap;
	}

	static inline constexpr size_type max_size() noexcept
	{
		constexpr size_type val{static_cast<size_type>((::std::numeric_limits<size_type>::max() >> 1u)) / (sizeof(::fast_io::details::associative_string<chtype>) + 1u)};
		return val;
	}

private:
	struct emplace_key_guard
	{
		::fast_io::details::str_swiss_map_imp_common<char_type, mapped_type> *pimp{};
		::std::size_t oldleftmost;
	};
	template <typename... Args>
		requires ::std::constructible_from<mapped_type, Args...>
	constexpr insert_result_type emplace_key_common(char_type const *key, ::std::size_t keyn, Args &&...args) noexcept(::std::is_nothrow_constructible_v<mapped_type, Args...>)
	{
		if constexpr (::std::is_nothrow_constructible_v<mapped_type, Args...>)
		{
			auto res = ::fast_io::details::str_swiss_map_insert_key_with_hash<allocator_type, hasher, char_type>(
				this->imp, key, keyn, hash);
			if (res.inserted)
			{
				::std::construct_at(__builtin_addressof(res.position.slots->val), ::std::forward<Args>(args)...);
			}
			return res;
		}
		else
		{
			auto [hval, pos, inserted] = ::fast_io::details::str_swiss_map_insert_key_with_hash_no_insert<allocator_type, hasher, char_type>(
				this->imp, key, keyn, hash);
			auto ctrlptr{this->imp.controls + pos};
			auto slotptr{this->imp.slots + pos};
			if (inserted)
			{
				::std::construct_at(__builtin_addressof(slotptr->val), ::std::forward<Args>(args)...);
				::fast_io::details::str_swiss_map_insert_key_internal_computed_slot_controls<allocator_type, char_type>(
					this->imp, pos, key, keyn, hval, ctrlptr, slotptr);
			}
			return {{ctrlptr, slotptr}, inserted};
		}
	}

public:
	template <typename... Args>
		requires ::std::constructible_from<mapped_type, Args...>
	constexpr insert_result_type emplace_key(key_string_view_type key, Args &&...args) noexcept(::std::is_nothrow_constructible_v<mapped_type, Args...>)
	{
		return this->emplace_key_common(key.ptr, key.n, ::std::forward<Args>(args)...);
	}

	constexpr insert_result_type insert_key(key_string_view_type key, mapped_type const &mapval) noexcept(::std::is_nothrow_copy_constructible_v<mapped_type>)
	{
		return this->emplace_key_common(key.ptr, key.n, mapval);
	}
	constexpr insert_result_type insert_key(key_string_view_type key, mapped_type &&mapval) noexcept
	{
		return this->emplace_key_common(key.ptr, key.n, ::std::move(mapval));
	}

	template <typename... Args>
		requires ::std::constructible_from<mapped_type, Args...>
	constexpr insert_result_type emplace_key_or_assign(key_string_view_type key, Args &&...args) noexcept(::std::is_nothrow_copy_constructible_v<mapped_type>)
	{
		if constexpr (::std::is_nothrow_constructible_v<mapped_type, Args...>)
		{
			auto res = ::fast_io::details::str_swiss_map_insert_key_with_hash<allocator_type, hasher, char_type>(
				this->imp, key.ptr, key.n, hash);

			if constexpr (::std::is_trivially_destructible_v<mapped_type>)
			{
				::std::construct_at(__builtin_addressof(res.position.slots->val), ::std::forward<Args>(args)...);
			}
			else
			{
				bool inserted{res.inserted};
				auto newptr{__builtin_addressof(res.position.slots->val)};
				if (!inserted)
				{
					::std::destroy_at(newptr);
				}
				::std::construct_at(newptr, mapped_type(::std::forward<Args>(args)...));
			}

			return res;
		}
		else
		{
			auto [hval, pos, inserted] = ::fast_io::details::str_swiss_map_insert_key_with_hash_no_insert<allocator_type, hasher, char_type>(
				this->imp, key.ptr, key.n, hash);
			auto ctrlptr{this->imp.controls + pos};
			auto slotptr{this->imp.slots + pos};
			auto newptr{__builtin_addressof(slotptr->val)};
			if (inserted)
			{
				::std::construct_at(newptr, ::std::forward<Args>(args)...);
				::fast_io::details::str_swiss_map_insert_key_internal_computed_slot_controls<allocator_type, char_type>(
					this->imp, pos, key.ptr, key.n, hval, ctrlptr, slotptr);
			}
			else
			{
				::std::destroy_at(newptr);
				::std::construct_at(newptr, mapped_type(::std::forward<Args>(args)...));
			}
			return {{ctrlptr, slotptr}, inserted};
		}
	}

	constexpr insert_result_type insert_key_or_assign(key_string_view_type key, mapped_type const &mapval) noexcept(::std::is_nothrow_copy_constructible_v<mapped_type>)
	{
		return this->emplace_key_or_assign(key, mapval);
	}
	constexpr insert_result_type insert_key_or_assign(key_string_view_type key, mapped_type &&mapval) noexcept
	{
		return this->emplace_key_or_assign(key, ::std::move(mapval));
	}

private:
	template <::std::input_or_output_iterator Iter, ::std::sentinel_for<Iter> Sen>
	constexpr void insert_range_common(Iter first, Sen last)
	{
		for (; first != last; ++first)
		{
			this->insert_key(first->key(), first->mapped());
		}
	}

public:
	template <::std::ranges::range R>
		requires(::fast_io::details::str_swiss_map_range_has_key_mapped_val<chtype, mappedtype, R>)
	constexpr void insert_range(R &&rg) noexcept(::fast_io::details::str_swiss_map_range_has_key_mapped_noexcept<chtype, mappedtype, R>)
	{
		this->insert_range_common(::std::ranges::begin(rg), ::std::ranges::end(rg));
	}

	constexpr void insert_range(::std::initializer_list<initializer_list_pair_type> ilist) noexcept
	{
		this->insert_range_common(ilist.begin(), ilist.end());
	}


	constexpr bool contains(key_string_view_type key) const noexcept
	{
		return ::fast_io::details::swiss_table_find_common_with_str_hashfunc_with_hasher<char_type>(
				   this->imp, key.ptr, key.n, hash)
			.found;
	}
	constexpr const_iterator find_key(key_string_view_type key) const noexcept
	{
		auto [pos, found] = ::fast_io::details::swiss_table_find_common_with_str_hashfunc_with_hasher<char_type>(
			this->imp, key.ptr, key.n, hash);
		if (found)
		{
			return {this->imp.controls + pos, this->imp.slots + pos};
		}
		return cend();
	}
	constexpr size_type count(key_string_view_type key) const noexcept
	{
		return this->contains(key);
	}
	constexpr bool is_empty() const noexcept
	{
		return !this->imp.counts;
	}
	constexpr size_type erase_key(key_string_view_type key) noexcept
	{
		return ::fast_io::details::str_swiss_map_erase_key<allocator_type, hasher>(this->imp, key.data(), key.size(), hash);
	}
	constexpr iterator erase(const_iterator iter) noexcept
	{
		auto next{::fast_io::details::str_swiss_map_erase<true, allocator_type, char_type>(this->imp, static_cast<::std::size_t>(iter.controlpos - this->imp.controls))};
		return {this->imp.controls + next, this->imp.slots + next};
	}
	constexpr iterator erase(const_iterator first, const_iterator last) noexcept
	{
		auto controls{this->imp.controls};
		auto next{::fast_io::details::str_swiss_map_erase_rg<allocator_type, char_type, mapped_type>(this->imp, static_cast<::std::size_t>(first.controlpos - controls), static_cast<::std::size_t>(last.controlpos - controls))};
		return {this->imp.controls + next, this->imp.slots + next};
	}
	constexpr const_iterator cbegin() const noexcept
	{
		auto leftmost{this->imp.leftmost};
		return {this->imp.controls + leftmost, this->imp.slots + leftmost};
	}
	constexpr const_iterator cend() const noexcept
	{
		auto cap{this->imp.cap};
		return {this->imp.controls + cap, this->imp.slots + cap};
	}
	constexpr const_iterator begin() const noexcept
	{
		return this->cbegin();
	}
	constexpr const_iterator end() const noexcept
	{
		return this->cend();
	}
	constexpr iterator begin() noexcept
	{
		auto leftmost{this->imp.leftmost};
		return {this->imp.controls + leftmost, this->imp.slots + leftmost};
	}
	constexpr iterator end() noexcept
	{
		auto cap{this->imp.cap};
		return {this->imp.controls + cap, this->imp.slots + cap};
	}

	constexpr const_reverse_iterator crbegin() const noexcept
	{
		return const_reverse_iterator(cend());
	}
	constexpr const_reverse_iterator crend() const noexcept
	{
		return const_reverse_iterator(cbegin());
	}
	constexpr reverse_iterator rbegin() const noexcept
	{
		return this->crbegin();
	}
	constexpr reverse_iterator rend() const noexcept
	{
		return this->crend();
	}
	constexpr void reserve(size_type n) noexcept
	{
		::fast_io::details::str_swiss_map_reserve<allocator_type, hasher, char_type>(this->imp, n, hash);
	}
	constexpr void swap(basic_str_swiss_map &other) noexcept
	{
		::std::ranges::swap(this->imp, other.imp);
		::std::ranges::swap(this->hash, other.hash);
	}
};

template <::std::integral chtype, typename mappedtype, typename Hash, typename Allocator>
constexpr void swap(basic_str_swiss_map<chtype, mappedtype, Hash, Allocator> &a, basic_str_swiss_map<chtype, mappedtype, Hash, Allocator> &b) noexcept
{
	a.swap(b);
}

template <::std::integral chtype, typename mappedtype, typename Hash, typename Allocator>
constexpr bool operator==(basic_str_swiss_map<chtype, mappedtype, Hash, Allocator> const &lhs, basic_str_swiss_map<chtype, mappedtype, Hash, Allocator> const &rhs) noexcept
{
	if (lhs.size() != rhs.size())
	{
		return false;
	}
	return ::std::is_permutation(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
}

} // namespace containers

} // namespace fast_io
