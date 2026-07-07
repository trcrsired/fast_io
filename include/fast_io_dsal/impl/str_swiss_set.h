#pragma once

namespace fast_io::containers::details
{

struct str_swiss_table_insert_pos_result
{
	::std::size_t pos;
	bool found;
};

template <::std::integral chtype>
inline constexpr str_swiss_table_insert_pos_result str_swiss_table_find_insert_position(
	::fast_io::details::swiss_table_str_imp_common<chtype> const &imp,
	chtype const *keybase, ::std::size_t keylen, ::std::uint_least64_t hash) noexcept
{
	::fast_io::basic_string_view<chtype> key(keybase, keylen);
	::std::size_t const cap{imp.cap};
	auto controls{imp.controls};
	auto slots{imp.slots};
	auto h1{::fast_io::details::swiss_table_hash_h1(hash)};
	auto h2{::fast_io::details::swiss_table_hash_h2(hash)};
	::std::size_t pos{h1 % cap};
	auto const start_pos{pos};
	::std::size_t first_deleted{::fast_io::containers::npos};
	for (;;)
	{
		auto controlspos{controls[pos]};
		if (h2 == controlspos && key == slots[pos].key())
		{
			return {pos, true};
		}
		if (controlspos == static_cast<::std::uint_least8_t>(::fast_io::details::swiss_table_ctrl::empty))
		{
			if (first_deleted != ::fast_io::containers::npos) [[unlikely]]
			{
				return {first_deleted, false};
			}
			return {pos, false};
		}
		if (controlspos == static_cast<::std::uint_least8_t>(::fast_io::details::swiss_table_ctrl::deleted) &&
			first_deleted == ::fast_io::containers::npos)
		{
			first_deleted = pos;
		}
		if ((++pos) == cap) [[unlikely]]
		{
			pos = 0;
		}
		if (pos == start_pos) [[unlikely]]
		{
			return {::fast_io::containers::npos, false};
		}
	}
}

template <typename allocator_type, ::std::integral chtype>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr bool str_swiss_table_insert_key_cold(
	::fast_io::details::swiss_table_str_imp_common<chtype> &imp,
	chtype const *keybase, ::std::size_t keylen, ::std::uint_least64_t hash) noexcept
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
		newcap = oldcap * 2;
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
				auto const oldhash{::fast_io::details::xxh3_64bits_fio(oldslot.ptr, oldslot.n * sizeof(char_type))};
				auto h1{::fast_io::details::swiss_table_hash_h1(oldhash)};
				auto h2{::fast_io::details::swiss_table_hash_h2(oldhash)};
				auto pos{h1 % newcap};
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

	// Now insert the new key
	auto h1{::fast_io::details::swiss_table_hash_h1(hash)};
	auto h2{::fast_io::details::swiss_table_hash_h2(hash)};
	auto pos{h1 % newcap};
	for (;;)
	{
		if (newcontrols[pos] == static_cast<::std::uint_least8_t>(::fast_io::details::swiss_table_ctrl::empty))
		{
			newcontrols[pos] = h2;
			newslots[pos] = ::fast_io::details::create_associative_string<allocator_type, char_type>(keybase, keylen);
			return true;
		}
		if ((++pos) == newcap) [[unlikely]]
		{
			pos = 0;
		}
	}
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
		imp.controls = nullptr;
		imp.cap = 0;
		imp.slots = nullptr;
	}
}

} // namespace fast_io::containers::details

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

	::fast_io::details::swiss_table_str_imp_common<char_type> imp;

	constexpr basic_str_swiss_set() noexcept = default;

	constexpr basic_str_swiss_set(basic_str_swiss_set const &) noexcept = delete;
	constexpr basic_str_swiss_set &operator=(basic_str_swiss_set const &) noexcept = delete;

	constexpr basic_str_swiss_set(basic_str_swiss_set &&other) noexcept
		: imp{other.imp}
	{
		other.imp.controls = nullptr;
		other.imp.cap = 0;
		other.imp.slots = nullptr;
	}

	constexpr basic_str_swiss_set &operator=(basic_str_swiss_set &&other) noexcept
	{
		if (this != ::std::addressof(other))
		{
			::fast_io::containers::details::str_swiss_table_destroy_impl<allocator_type, char_type>(this->imp);
			this->imp = other.imp;
			other.imp.controls = nullptr;
			other.imp.cap = 0;
			other.imp.slots = nullptr;
		}
		return *this;
	}

	constexpr ~basic_str_swiss_set()
	{
		::fast_io::containers::details::str_swiss_table_destroy_impl<allocator_type, char_type>(this->imp);
	}

	constexpr bool insert_key(string_view_type key) noexcept
	{
		auto const hash{::fast_io::details::xxh3_64bits_fio(key.data(), key.size_bytes())};
		if (this->imp.cap == 0) [[unlikely]]
		{
			return ::fast_io::containers::details::str_swiss_table_insert_key_cold<allocator_type, char_type>(
				this->imp, key.ptr, key.n, hash);
		}
		auto result{::fast_io::containers::details::str_swiss_table_find_insert_position<char_type>(
			this->imp, key.ptr, key.n, hash)};
		if (result.found)
		{
			return false;
		}
		if (result.pos == ::fast_io::containers::npos) [[unlikely]]
		{
			return ::fast_io::containers::details::str_swiss_table_insert_key_cold<allocator_type, char_type>(
				this->imp, key.ptr, key.n, hash);
		}
		auto h2{::fast_io::details::swiss_table_hash_h2(hash)};
		this->imp.controls[result.pos] = h2;
		this->imp.slots[result.pos] = ::fast_io::details::create_associative_string<allocator_type, char_type>(key.ptr, key.n);
		return true;
	}
	constexpr bool contains(string_view_type key) const noexcept
	{
		if (this->imp.cap == 0)
		{
			return false;
		}
		return ::fast_io::containers::details::str_swiss_table_find_common_with_str<char_type>(
				   this->imp, key.ptr, key.n,
				   ::fast_io::details::xxh3_64bits_fio(key.data(), key.size_bytes())) != ::fast_io::containers::npos;
	}
};
} // namespace containers

} // namespace fast_io
