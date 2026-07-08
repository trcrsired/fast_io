#pragma once

namespace fast_io::details
{

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
		newcap = 7;
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
		newcap = (oldcap << 1u) + 1u;
	}

	auto newcontrols{typed_ctrl_allocator_type::allocate(newcap)};
	auto newslots{typed_slot_allocator_type::allocate(newcap)};

	for (::std::size_t i{}; i != newcap; ++i)
	{
		newcontrols[i] = static_cast<::std::uint_least8_t>(::fast_io::details::swiss_table_ctrl::empty);
	}

	imp.controls = newcontrols;
	imp.cap = newcap;
	imp.slots = newslots;

	// Rehash existing entries
	if (oldcap != 0)
	{
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
				auto pos{h1 & newcap};
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
			}
		}
		typed_ctrl_allocator_type::deallocate_n(oldcontrols, oldcap);
		typed_slot_allocator_type::deallocate_n(oldslots, oldcap);
	}
}

template <::std::integral chtype>
inline constexpr bool str_swiss_table_need_grow(
	::fast_io::details::swiss_table_str_imp_common<chtype> const &imp) noexcept
{
	::std::size_t deleted_count{};
	if constexpr (false)
	{
		for (::std::size_t i{}; i != imp.cap; ++i)
		{
			if (imp.controls[i] == static_cast<::std::uint_least8_t>(::fast_io::details::swiss_table_ctrl::deleted))
			{
				++deleted_count;
			}
		}
	}
	::std::size_t const occupied{imp.counts + deleted_count};
	::std::size_t high;
	auto const low{::fast_io::intrinsics::umul(occupied, static_cast<::std::size_t>(7), high)};
	constexpr auto shift{static_cast<unsigned>(::std::numeric_limits<::std::size_t>::digits - 3u)};
	return ((high << shift) | (low >> 3u)) >= imp.cap;
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
	++imp.counts;
}

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
			need_grow = this->imp.controls[pos] != static_cast<::std::uint_least8_t>(::fast_io::details::swiss_table_ctrl::empty) ||
						::fast_io::details::str_swiss_table_need_grow(this->imp);
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
};
} // namespace containers

} // namespace fast_io
