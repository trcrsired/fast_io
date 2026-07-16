#pragma once

namespace fast_io
{
template <::std::integral chtype, typename valtype>
struct basic_str_swiss_map_key_mapped_pair
{
    using char_type = chtype;
    using key_type = ::fast_io::containers::basic_cstring_view<char_type>;
    using mapped_type = valtype;
    ::fast_io::details::associative_string<chtype> ky;
    mapped_type val;
    constexpr key_type key() const noexcept
    {
        return ky.strvw();
    }
    constexpr mapped_type& mapped() noexcept
    {
        return val;
    }
    constexpr mapped_type const& mapped() const noexcept
    {
        return val;
    }
};

struct str_swiss_map_
}

namespace fast_io::details
{


template <::std::integral chtype, typename value_type, bool isconst>
struct str_swiss_map_iterator
{
    using value_type = ::fast_io::containers::str_swiss_map_key_mapped_pair<chtype, value_type>;
    using reference = ::std::conditional_t<isconst, value_type const&, value_type>;
	using iterator_tag = ::std::bidirectional_iterator_tag;
	using difference_type = ::std::ptrdiff_t;
	::std::uint_least8_t const *controlpos{};
	value_type const *slots{};
	constexpr str_swiss_map_key_mapped_pair& operator*() const noexcept
	{
		return *slots;
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
};

template <::std::integral chtype, typename value_type, bool isconst1, bool isconst2>
inline constexpr bool operator==(::fast_io::details::str_swiss_map_iterator<chtype, value_type, isconst1> a,
								 ::fast_io::details::str_swiss_map_iterator<chtype, value_type, isconst2> b) noexcept
{
	return a.controlpos == b.controlpos;
}

template <typename allocator_type, typename hasher, ::std::integral chtype>
inline constexpr void str_swiss_table_reserve_to_newcap(
	::fast_io::details::swiss_table_str_imp_common<chtype> &imp, ::std::size_t newcap, hasher hash) noexcept
{
	using char_type = chtype;
	using slot_type = ::fast_io::details::basic_str_swiss_map_key_mapped_pair<char_type, mappedtype>;
	using typed_slot_allocator_type = ::fast_io::typed_generic_allocator_adapter<allocator_type, slot_type>;
	using typed_ctrl_allocator_type = ::fast_io::typed_generic_allocator_adapter<allocator_type, ::std::uint_least8_t>;

	auto oldcontrols{imp.controls};
	auto oldslots{imp.slots};
	auto const oldcap{imp.cap};

	auto newcontrols{typed_ctrl_allocator_type::allocate(newcap + 1u)};
	auto newslots{typed_slot_allocator_type::allocate(newcap)};

	auto newcontrols_ed{newcontrols + newcap};
	for (auto i{newcontrols}; i != newcontrols_ed; ++i)
	{
		*i = static_cast<::std::uint_least8_t>(::fast_io::details::swiss_table_ctrl::empty);
	}
	*newcontrols_ed = 0u;

	imp.controls = newcontrols;
	imp.cap = newcap;
	imp.slots = newslots;

	// Rehash existing entries
	if (oldcap == 0)
	{
		imp.leftmost = 0;
	}
	else
	{
		::std::size_t leftmost{newcap};
		::std::size_t newcapm1{static_cast<::std::size_t>(newcap - 1u)};
		for (::std::size_t i{}; i != oldcap; ++i)
		{
			auto oldctrl{oldcontrols[i]};
			if (oldctrl != static_cast<::std::uint_least8_t>(::fast_io::details::swiss_table_ctrl::empty) &&
				oldctrl != static_cast<::std::uint_least8_t>(::fast_io::details::swiss_table_ctrl::deleted))
			{
				auto const &oldslot{oldslots[i]};
				auto const oldhash{hash.do_hash(reinterpret_cast<::std::byte const *>(oldslot.ptr), reinterpret_cast<::std::byte const *>(oldslot.ptr + oldslot.n))};
				auto h1{::fast_io::details::swiss_table_hash_h1(oldhash)};
				auto h2{::fast_io::details::swiss_table_hash_h2(oldhash)};
				auto pos{h1 & newcapm1};
				for (;;)
				{
					if (newcontrols[pos] == static_cast<::std::uint_least8_t>(::fast_io::details::swiss_table_ctrl::empty))
					{
						newcontrols[pos] = h2;
						newslots[pos] = oldslot;
						break;
					}
					if ((++pos) == newcap) [[unlikely]]
					{
						pos = 0;
					}
				}
				if (pos < leftmost)
				{
					leftmost = pos;
				}
			}
		}
		imp.leftmost = leftmost;
		typed_ctrl_allocator_type::deallocate_n(oldcontrols, oldcap + 1u);
		typed_slot_allocator_type::deallocate_n(oldslots, oldcap);
	}
}

template <typename allocator_type, typename hasher, ::std::integral chtype>
inline constexpr void str_swiss_table_reserve(
	::fast_io::details::swiss_table_str_imp_common<chtype> &imp, ::std::size_t n, hasher hash) noexcept
{
	if (n <= imp.counts)
	{
		return;
	}

	constexpr ::std::size_t mx{::std::numeric_limits<::std::size_t>::max()};
	constexpr ::std::size_t mxfactor{mx / 5 * 4};
	if (mxfactor < n)
	{
		::fast_io::fast_terminate();
	}
	::std::size_t const counts{imp.counts};
	::std::size_t high;
	auto const low{::fast_io::intrinsics::umul(imp.cap, static_cast<::std::size_t>(5), high)};
	constexpr unsigned shifter{2u};
	constexpr auto highshifter{static_cast<unsigned>(::std::numeric_limits<::std::size_t>::digits - shifter)};
	::std::size_t newcapnoceil{(high << highshifter) | (low >> shifter)};
	::std::size_t newcap{::std::bit_ceil(newcapnoceil)};
	if (!newcap)
	{
		::fast_io::fast_terminate();
	}
	if (newcap < 8)
	{
		newcap = 8;
	}
	::fast_io::details::str_swiss_table_reserve_to_newcap<allocator_type, hasher, chtype>(imp, newcap, hash);
}

template <typename allocator_type, typename hasher, ::std::integral chtype>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr void str_swiss_table_grow(
	::fast_io::details::swiss_table_str_imp_common<chtype> &imp, hasher hash) noexcept
{
	auto oldcap{imp.cap};
	::std::size_t newcap;
	if (oldcap == 0)
	{
		newcap = 8;
	}
	else
	{
		// add overflow trapping here
		constexpr ::std::size_t mx{::std::numeric_limits<::std::size_t>::max()};
		constexpr ::std::size_t mxdv2{(mx - 1u) >> 1u};
		if (mxdv2 < oldcap)
		{
			::fast_io::fast_terminate();
		}
		newcap = (oldcap << 1u);
	}
	::fast_io::details::str_swiss_table_reserve_to_newcap<allocator_type, hasher, chtype>(imp, newcap, hash);
}

template <::std::integral chtype>
inline constexpr bool str_swiss_table_need_grow(
	::fast_io::details::swiss_table_str_imp_common<chtype> const &imp) noexcept
{
	::std::size_t const counts{imp.counts};
	::std::size_t high;
	auto const low{::fast_io::intrinsics::umul(imp.cap, static_cast<::std::size_t>(7), high)};
	constexpr auto shift{static_cast<unsigned>(::std::numeric_limits<::std::size_t>::digits - 3u)};
	return ((high << shift) | (low >> 3u)) <= counts;
}

template <typename allocator_type, ::std::integral chtype>
inline constexpr void str_swiss_table_insert_key_internal(
	::fast_io::details::swiss_table_str_imp_common<chtype> &imp,
	::std::size_t pos, chtype const *keybase, ::std::size_t keylen,
	::std::uint_least64_t hash) noexcept
{
	using char_type = chtype;
	auto const h2{::fast_io::details::swiss_table_hash_h2(hash)};
	imp.controls[pos] = h2;
	imp.slots[pos] = ::fast_io::details::create_associative_string<allocator_type, char_type>(keybase, keylen);
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

template <bool needdestroy, typename allocator_type, ::std::integral chtype, typename mappedtype>
inline constexpr void str_swiss_table_clear_impl(
	::fast_io::details::swiss_table_str_imp_common<chtype> &imp) noexcept
{
	using char_type = chtype;
	using slot_type = ::fast_io::details::basic_str_swiss_map_key_mapped_pair<char_type, mappedtype>;
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
		auto &ci{controls[i]};
		auto ctrl{ci};
		if (ctrl != static_cast<::std::uint_least8_t>(::fast_io::details::swiss_table_ctrl::empty) &&
			ctrl != static_cast<::std::uint_least8_t>(::fast_io::details::swiss_table_ctrl::deleted))
		{
			auto si{slots[i]};
			::fast_io::details::deallocate_associative_string<allocator_type, char_type>(si.ptr, si.n);
			if constexpr (!needdestroy)
			{
				ci = static_cast<::std::uint_least8_t>(::fast_io::details::swiss_table_ctrl::deleted);
			}
		}
	}
	if constexpr (needdestroy)
	{
		typed_ctrl_allocator_type::deallocate_n(controls, static_cast<::std::size_t>(cap + 1u));
		typed_slot_allocator_type::deallocate_n(slots, cap);
		imp = {};
	}
	else
	{
		imp.counts = 0u;
		imp.leftmost = imp.cap;
	}
}

template <typename allocator_type, ::std::integral chtype>
inline constexpr ::std::size_t str_swiss_map_erase_rg(::fast_io::details::swiss_table_str_imp_common<chtype> &imp, ::std::size_t first, ::std::size_t last) noexcept
{
	if (first == last)
	{
		return first;
	}
	::std::size_t i{first};
	::std::size_t counting{};
	for (; i != last; ++counting)
	{
		auto si{imp.slots[i]};
		::fast_io::details::deallocate_associative_string<allocator_type, chtype>(si.ptr, si.n);
		auto controls{imp.controls};
		auto controlspos{controls + i};
		*controlspos = static_cast<::std::uint_least8_t>(::fast_io::details::swiss_table_ctrl::deleted);
		i = static_cast<::std::size_t>(::fast_io::details::swiss_table_iterator_common<false>(controlspos) - controls);
	}
	if (!(imp.counts -= counting) || imp.leftmost == first)
	{
		imp.leftmost = i;
	}
	return i;
}

template <bool compute_next, typename allocator_type, ::std::integral chtype>
inline constexpr ::std::conditional_t<compute_next, ::std::size_t, void> str_swiss_map_erase(::fast_io::details::swiss_table_str_imp_common<chtype> &imp, ::std::size_t pos) noexcept
{
	auto si{imp.slots[pos]};
	::fast_io::details::deallocate_associative_string<allocator_type, chtype>(si.ptr, si.n);
	auto controls{imp.controls};
	auto controlspos{controls + pos};
	*controlspos = static_cast<::std::uint_least8_t>(::fast_io::details::swiss_table_ctrl::deleted);
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
			auto leftmost{imp.leftmost};
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
inline constexpr bool str_swiss_map_erase_key(::fast_io::details::swiss_table_str_imp_common<chtype> &imp, chtype const *str, ::std::size_t strn, hasher hash) noexcept
{
	using char_type = chtype;
	using slot_type = ::fast_io::details::basic_str_swiss_map_key_mapped_pair<char_type, mappedtype>;
	using typed_slot_allocator_type = ::fast_io::typed_generic_allocator_adapter<allocator_type, slot_type>;
	using typed_ctrl_allocator_type = ::fast_io::typed_generic_allocator_adapter<allocator_type, ::std::uint_least8_t>;

	auto [pos, found] = ::fast_io::details::swiss_table_find_common_with_str_hashfunc_with_hasher<char_type>(
		imp, str, strn, hash);
	if (!found)
	{
		return false;
	}
	::fast_io::details::str_swiss_map_erase<false, allocator_type, chtype>(imp, pos);
	return true;
}

template <typename allocator_type, ::std::integral chtype, typename mappedtype>
inline constexpr ::fast_io::details::swiss_table_str_imp_common<chtype> str_swiss_map_clone(::fast_io::details::swiss_table_str_imp_common<chtype> const &other) noexcept
{
	using char_type = chtype;
	using slot_type = ::fast_io::details::basic_str_swiss_map_key_mapped_pair<char_type, mappedtype>;
	using typed_slot_allocator_type = ::fast_io::typed_generic_allocator_adapter<allocator_type, slot_type>;
	using typed_ctrl_allocator_type = ::fast_io::typed_generic_allocator_adapter<allocator_type, ::std::uint_least8_t>;

	auto const cap{other.cap};
	if (!cap)
	{
		return {};
	}
	auto othercontrols{other.controls};
	auto otherslots{other.slots};
	::std::size_t capp1{static_cast<::std::size_t>(cap + 1u)};
	auto controls{typed_ctrl_allocator_type::allocate(capp1)};
	auto slots{typed_slot_allocator_type::allocate(cap)};

	::fast_io::freestanding::non_overlapped_copy_n(othercontrols, capp1, controls);
	for (::std::size_t i{}; i != cap; ++i)
	{
		auto ctrl{othercontrols[i]};
		if (ctrl != static_cast<::std::uint_least8_t>(::fast_io::details::swiss_table_ctrl::empty) &&
			ctrl != static_cast<::std::uint_least8_t>(::fast_io::details::swiss_table_ctrl::deleted))
		{
			auto si{otherslots[i]};
			slots[i] = ::fast_io::details::create_associative_string<allocator_type, char_type>(si.ptr, si.n);
		}
	}
	return {controls, cap, other.counts, other.leftmost, slots};
}


template <::std::integral chtype>
struct str_swiss_table_insert_key_result
{
	::fast_io::details::str_swiss_map_iterator<chtype> position;
	bool inserted;
};

template <typename allocator_type, typename hasher, ::std::integral char_type>
constexpr ::fast_io::details::str_swiss_table_insert_key_result<char_type> str_swiss_table_insert_key_with_hash(::fast_io::details::swiss_table_str_imp_common<char_type> &imp, char_type const *key, ::std::size_t keyn, hasher hash) noexcept
{
	auto hval{hash.do_hash(reinterpret_cast<::std::byte const *>(key), reinterpret_cast<::std::byte const *>(key + keyn))};
	auto const result{::fast_io::details::swiss_table_find_common_with_str<char_type>(
		imp, key, keyn, hval)};
	::std::size_t pos{result.pos};
	if (result.found)
	{
		return {{imp.controls + pos, imp.slots + pos}, false};
	}
	if (::fast_io::details::str_swiss_table_need_grow(imp))
	{
		::fast_io::details::str_swiss_table_grow<allocator_type, hasher, char_type>(
			imp, hash);
		pos = ::fast_io::details::swiss_table_find_common_with_str<char_type>(
				  imp, key, keyn, hval)
				  .pos;
	}
	::fast_io::details::str_swiss_table_insert_key_internal<allocator_type, char_type>(
		imp, pos, key, keyn, hval);
	return {{imp.controls + pos, imp.slots + pos}, true};
}

} // namespace fast_io::details

namespace fast_io
{

namespace containers
{
template <::std::integral chtype, typename Hash, typename Allocator>
class basic_str_swiss_map
{
public:
	using char_type = chtype;
	using string_view_type = ::fast_io::containers::basic_string_view<char_type>;
	using cstring_view_type = ::fast_io::containers::basic_cstring_view<char_type>;
	using allocator_type = Allocator;
	using size_type = ::std::size_t;
	using difference_type = ::std::ptrdiff_t;
	using const_iterator = ::fast_io::details::str_swiss_map_iterator<chtype>;
	using iterator = const_iterator;
	using const_reverse_iterator = ::std::reverse_iterator<const_iterator>;
	using reverse_iterator = const_reverse_iterator;
	using insert_result_type = ::fast_io::details::str_swiss_table_insert_key_result<char_type>;
	using hasher = Hash;

	::fast_io::details::swiss_table_str_imp_common<char_type> imp{};
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

	constexpr basic_str_swiss_map(basic_str_swiss_map const &other) noexcept : imp(::fast_io::details::str_swiss_map_clone<allocator_type, chtype>(other.imp)), hash(other.hash) {};
	constexpr basic_str_swiss_map &operator=(basic_str_swiss_map const &other) noexcept
	{
		if (this != ::std::addressof(other))
		{
			auto tmp = ::fast_io::details::str_swiss_map_clone<allocator_type, chtype>(other.imp);
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
	template <::std::ranges::range R>
	constexpr void construct_with_range_common(R &&rg) noexcept
	{
		construct_range_destroyer des(this);
		if constexpr (::std::ranges::sized_range<R>)
		{
			this->reserve(::std::ranges::size(rg));
		}
		for (auto const &e : rg)
		{
			this->insert_key(e);
		}
		des.ptr = nullptr;
	}

public:
	explicit constexpr basic_str_swiss_map(::std::initializer_list<string_view_type> ilist) noexcept
	{
		this->construct_with_range_common(ilist);
	}

	template <::std::ranges::range R>
	explicit constexpr basic_str_swiss_map(::fast_io::freestanding::from_range_t, R &&rg) noexcept(::std::is_nothrow_constructible_v<string_view_type, ::std::ranges::range_value_t<R>>)
	{
		this->construct_with_range_common(::std::forward<R>(rg));
	}

	explicit constexpr basic_str_swiss_map(::fast_io::from_hasher_t, hasher h, ::std::initializer_list<string_view_type> ilist) noexcept
		: hash(h)
	{
		this->construct_with_range_common(ilist);
	}

	template <::std::ranges::range R>
	explicit constexpr basic_str_swiss_map(::fast_io::from_hasher_t, hasher h, ::fast_io::freestanding::from_range_t, R &&rg) noexcept(::std::is_nothrow_constructible_v<string_view_type, ::std::ranges::range_value_t<R>>)
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
			::fast_io::details::str_swiss_table_clear_impl<true, allocator_type, char_type>(this->imp);
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
		::fast_io::details::str_swiss_table_clear_impl<false, allocator_type, char_type>(this->imp);
	}

	constexpr void clear_destroy() noexcept
	{
		::fast_io::details::str_swiss_table_clear_impl<true, allocator_type, char_type>(this->imp);
		this->imp = {};
	}

	constexpr ~basic_str_swiss_map()
	{
		::fast_io::details::str_swiss_table_clear_impl<true, allocator_type, char_type>(this->imp);
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

	constexpr insert_result_type insert_key(string_view_type key) noexcept
	{
		return ::fast_io::details::str_swiss_table_insert_key_with_hash<allocator_type, hasher, char_type>(
			this->imp, key.ptr, key.n, hash);
	}

	template <::std::ranges::range R>
	constexpr void insert_range(R &&rg) noexcept(::std::is_nothrow_constructible_v<string_view_type, ::std::ranges::range_value_t<R>>)
	{
		for (auto const &e : rg)
		{
			this->insert_key(e);
		}
	}

	constexpr bool contains(string_view_type key) const noexcept
	{
		return ::fast_io::details::swiss_table_find_common_with_str_hashfunc_with_hasher<char_type>(
				   this->imp, key.ptr, key.n, hash)
			.found;
	}
	constexpr iterator find_key(string_view_type key) const noexcept
	{
		auto [pos, found] = ::fast_io::details::swiss_table_find_common_with_str_hashfunc_with_hasher<char_type>(
			this->imp, key.ptr, key.n, hash);
		if (found)
		{
			return {this->imp.controls + pos, this->imp.slots + pos};
		}
		return cend();
	}
	constexpr size_type count(string_view_type key) const noexcept
	{
		return this->contains(key);
	}
	constexpr bool is_empty() const noexcept
	{
		return !this->imp.counts;
	}

	constexpr size_type erase_key(string_view_type key) noexcept
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
		auto next{::fast_io::details::str_swiss_map_erase_rg<true, allocator_type, char_type>(this->imp, static_cast<::std::size_t>(first.controlpos - controls), static_cast<::std::size_t>(last.controlpos - controls))};
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
	constexpr iterator begin() const noexcept
	{
		return this->cbegin();
	}
	constexpr iterator end() const noexcept
	{
		return this->cend();
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

	constexpr size_type reserve(size_type n) noexcept
	{
		::fast_io::details::str_swiss_table_reserve<allocator_type, char_type>(this->imp, n);
	}
	constexpr void swap(basic_str_swiss_map &other) noexcept
	{
		::std::ranges::swap(this->imp, other.imp);
		::std::ranges::swap(this->hash, other.hash);
	}
};

template <::std::integral chtype, typename Hash, typename Allocator>
constexpr void swap(basic_str_swiss_map<chtype, Hash, Allocator> &a, basic_str_swiss_map<chtype, Hash, Allocator> &b) noexcept
{
	a.swap(b);
}

template <::std::integral chtype, typename Hash, typename Allocator>
constexpr bool operator==(basic_str_swiss_map<chtype, Hash, Allocator> const &lhs, basic_str_swiss_map<chtype, Hash, Allocator> const &rhs) noexcept
{
	if (lhs.size() != rhs.size())
	{
		return false;
	}
	return ::std::is_permutation(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
}

} // namespace containers

} // namespace fast_io
