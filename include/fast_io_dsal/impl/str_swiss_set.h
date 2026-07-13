#pragma once

namespace fast_io::details
{

template <::std::integral chtype>
struct str_swiss_set_iterator
{
	using value_type = ::fast_io::containers::basic_cstring_view<chtype>;
	using iterator_tag = ::std::bidirectional_iterator_tag;
	using difference_type = ::std::ptrdiff_t;
	::std::uint_least8_t const *controlpos{};
	::std::uint_least8_t const *controls{};
	::fast_io::details::associative_string<chtype> const *slots{};
	constexpr value_type operator*() const noexcept
	{
		return slots[static_cast<::std::size_t>(controlpos - controls)].strvw();
	}
	constexpr str_swiss_set_iterator &operator++() noexcept
	{
		this->controlpos = ::fast_io::details::swiss_table_iterator_common<false>(this->controlpos);
		return *this;
	}
	constexpr str_swiss_set_iterator operator++(int) noexcept
	{
		auto temp{*this};
		++*this;
		return temp;
	}
	constexpr str_swiss_set_iterator &operator--() noexcept
	{
		this->controlpos = ::fast_io::details::swiss_table_iterator_common<true>(this->controlpos);
		return *this;
	}
	constexpr str_swiss_set_iterator operator--(int) noexcept
	{
		auto temp{*this};
		--*this;
		return temp;
	}
};

template <::std::integral chtype>
inline constexpr bool operator==(::fast_io::details::str_swiss_set_iterator<chtype> a,
								 ::fast_io::details::str_swiss_set_iterator<chtype> b) noexcept
{
	return a.controlpos == b.controlpos;
}

template <::std::integral chtype>
inline constexpr bool operator!=(::fast_io::details::str_swiss_set_iterator<chtype> a,
								 ::fast_io::details::str_swiss_set_iterator<chtype> b) noexcept
{
	return !(a == b);
}

template <typename allocator_type, ::std::integral chtype>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr void str_swiss_table_grow(
	::fast_io::details::swiss_table_str_imp_common<chtype> &imp) noexcept
{
	using char_type = chtype;
	using slot_type = ::fast_io::details::associative_string<char_type>;
	using typed_slot_allocator_type = ::fast_io::typed_generic_allocator_adapter<allocator_type, slot_type>;
	using typed_ctrl_allocator_type = ::fast_io::typed_generic_allocator_adapter<allocator_type, ::std::uint_least8_t>;

	auto oldcontrols{imp.controls};
	auto oldslots{imp.slots};
	auto const oldcap{imp.cap};

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
#if 0
		imp.rightmost = 0;
#endif
	}
	else
	{
		::std::size_t leftmost{newcap};
#if 0
		::std::size_t rightmost{};
#endif
		::std::size_t newcapm1{static_cast<::std::size_t>(newcap - 1u)};
		for (::std::size_t i{}; i != oldcap; ++i)
		{
			auto oldctrl{oldcontrols[i]};
			if (oldctrl != static_cast<::std::uint_least8_t>(::fast_io::details::swiss_table_ctrl::empty) &&
				oldctrl != static_cast<::std::uint_least8_t>(::fast_io::details::swiss_table_ctrl::deleted))
			{
				auto const &oldslot{oldslots[i]};
				auto const oldhash{::fast_io::details::rapidhash_64bits_fio(oldslot.ptr, oldslot.n * sizeof(char_type))};
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
#if 0
				if (rightmost < pos)
				{
					rightmost = pos;
				}
#endif
			}
		}
		imp.leftmost = leftmost;
#if 0
		imp.rightmost = static_cast<::std::size_t>(rightmost + 1u);
#endif
		typed_ctrl_allocator_type::deallocate_n(oldcontrols, oldcap + 1u);
		typed_slot_allocator_type::deallocate_n(oldslots, oldcap);
	}
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
inline constexpr void str_swiss_table_insert_key(
	::fast_io::details::swiss_table_str_imp_common<chtype> &imp,
	::std::size_t pos, chtype const *keybase, ::std::size_t keylen,
	::std::uint_least64_t hash) noexcept
{
	using char_type = chtype;
	auto const h2{::fast_io::details::swiss_table_hash_h2(hash)};
	imp.controls[pos] = h2;
	imp.slots[pos] = ::fast_io::details::create_associative_string<allocator_type, char_type>(keybase, keylen);
	auto newleftmost{pos};
#if 0
	auto newrightmost{static_cast<::std::size_t>(pos + 1u)};
#endif
	auto counts{imp.counts};
	if (counts)
	{
		if (newleftmost < imp.leftmost)
		{
			imp.leftmost = newleftmost;
		}
#if 0
		if (imp.leftmost < newrightmost)
		{
			imp.rightmost = newrightmost;
		}
#endif
	}
	else
	{
		imp.leftmost = newleftmost;
#if 0
		imp.rightmost = newrightmost;
#endif
	}
	imp.counts = static_cast<::std::size_t>(counts + 1u);
}
#if 0
template <typename allocator_type, ::std::integral chtype>
inline constexpr void str_swiss_table_insert_key(
	::fast_io::details::swiss_table_str_imp_common<chtype> &imp,
	::std::size_t pos, chtype const *keybase, ::std::size_t keylen,
	::std::uint_least64_t hash) noexcept
{
	using char_type = chtype;
	auto const h2{::fast_io::details::swiss_table_hash_h2(hash)};
	imp.controls[pos] = h2;
	imp.slots[pos] = ::fast_io::details::create_associative_string<allocator_type, char_type>(keybase, keylen);
	++imp.counts;
}
#endif
template <typename allocator_type, ::std::integral chtype>
inline constexpr void str_swiss_table_destroy_impl(
	::fast_io::details::swiss_table_str_imp_common<chtype> &imp) noexcept
{
	using char_type = chtype;
	using slot_type = ::fast_io::details::associative_string<char_type>;
	using typed_slot_allocator_type = ::fast_io::typed_generic_allocator_adapter<allocator_type, slot_type>;
	using typed_ctrl_allocator_type = ::fast_io::typed_generic_allocator_adapter<allocator_type, ::std::uint_least8_t>;

	auto controls{imp.controls};
	auto slots{imp.slots};
	auto const cap{imp.cap};
	if (cap != 0)
	{
		for (::std::size_t i{}; i != cap; ++i)
		{
			auto ctrl{controls[i]};
			if (ctrl != static_cast<::std::uint_least8_t>(::fast_io::details::swiss_table_ctrl::empty) &&
				ctrl != static_cast<::std::uint_least8_t>(::fast_io::details::swiss_table_ctrl::deleted))
			{
				::fast_io::details::deallocate_associative_string<allocator_type, char_type>(slots[i].ptr, slots[i].n);
			}
		}
		typed_ctrl_allocator_type::deallocate_n(controls, cap);
		typed_slot_allocator_type::deallocate_n(slots, cap);
		imp = {};
	}
}

} // namespace fast_io::details

namespace fast_io
{

namespace containers
{
template <::std::integral chtype, typename Allocator>
class basic_str_swiss_set
{
public:
	using char_type = chtype;
	using string_view_type = ::fast_io::containers::basic_string_view<char_type>;
	using cstring_view_type = ::fast_io::containers::basic_cstring_view<char_type>;
	using allocator_type = Allocator;
	using size_type = ::std::size_t;
	using difference_type = ::std::ptrdiff_t;
	using const_iterator = ::fast_io::details::str_swiss_set_iterator<chtype>;
	using iterator = const_iterator;
#if 0
	using const_reverse_iterator = ::std::reverse_iterator<const_iterator>;
	using reverse_iterator = const_reverse_iterator;
#endif

	::fast_io::details::swiss_table_str_imp_common<char_type> imp{};

	constexpr basic_str_swiss_set() noexcept = default;

	constexpr basic_str_swiss_set(basic_str_swiss_set const &) noexcept = delete;
	constexpr basic_str_swiss_set &operator=(basic_str_swiss_set const &) noexcept = delete;

	constexpr basic_str_swiss_set(basic_str_swiss_set &&other) noexcept
		: imp{other.imp}
	{
		other.imp = {};
	}

	constexpr basic_str_swiss_set &operator=(basic_str_swiss_set &&other) noexcept
	{
		if (this != ::std::addressof(other))
		{
			::fast_io::details::str_swiss_table_destroy_impl<allocator_type, char_type>(this->imp);
			this->imp = other.imp;
			other.imp = {};
		}
		return *this;
	}

	constexpr ~basic_str_swiss_set()
	{
		::fast_io::details::str_swiss_table_destroy_impl<allocator_type, char_type>(this->imp);
	}

	constexpr size_type size() const noexcept
	{
		return this->imp.counts;
	}

	constexpr bool insert_key(string_view_type key) noexcept
	{
		auto const hash{::fast_io::details::rapidhash_64bits_fio(key.data(), key.size_bytes())};
		bool need_grow{this->imp.cap == 0};
		::std::size_t pos{};
		if (!need_grow)
		{
			auto const result{::fast_io::details::swiss_table_find_common_with_str<char_type>(
				this->imp, key.ptr, key.n, hash)};
			if (result.found)
			{
				return false;
			}
			pos = result.pos;
			if constexpr (false)
			{
				need_grow = this->imp.controls[pos] != static_cast<::std::uint_least8_t>(::fast_io::details::swiss_table_ctrl::empty) ||
							::fast_io::details::str_swiss_table_need_grow(this->imp);
			}
			else
			{
				need_grow = ::fast_io::details::str_swiss_table_need_grow(this->imp);
			}
		}
		if (need_grow) [[unlikely]]
		{
			::fast_io::details::str_swiss_table_grow<allocator_type, char_type>(
				this->imp);
			pos = ::fast_io::details::swiss_table_find_common_with_str<char_type>(
					  this->imp, key.ptr, key.n, hash)
					  .pos;
		}
		::fast_io::details::str_swiss_table_insert_key<allocator_type, char_type>(
			this->imp, pos, key.ptr, key.n, hash);
		return true;
	}
	constexpr bool contains(string_view_type key) const noexcept
	{
		if (this->imp.cap == 0)
		{
			return false;
		}
		return ::fast_io::details::swiss_table_find_common_with_str<char_type>(
				   this->imp, key.ptr, key.n,
				   ::fast_io::details::rapidhash_64bits_fio(key.data(), key.size_bytes()))
			.found;
	}
	constexpr size_type count(string_view_type key) const noexcept
	{
		return this->contains(key);
	}
	constexpr bool is_empty() const noexcept
	{
		return !this->imp.counts;
	}
#if 0
	constexpr size_type erase_key(string_view_type key) const noexcept
	{
		if (this->imp.cap == 0)
		{
			return 0;
		}
	}
#endif
	constexpr const_iterator cbegin() const noexcept
	{
		auto controls{this->imp.controls};
		return {controls + this->imp.leftmost, controls, this->imp.slots};
	}
	constexpr const_iterator cend() const noexcept
	{
		auto controls{this->imp.controls};
#if 0
		return {controls + this->imp.rightmost, controls, this->imp.slots};
#else
		return {controls + this->imp.cap, controls, this->imp.slots};
#endif
	}
	constexpr iterator begin() const noexcept
	{
		return this->cbegin();
	}
	constexpr iterator end() const noexcept
	{
		return this->cend();
	}
};
} // namespace containers

} // namespace fast_io
