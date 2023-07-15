﻿#pragma once

namespace fast_io {

template <std::integral ch_type>
struct basic_ibuffer_view {
	using char_type = ch_type;	
	using input_char_type = ch_type;
	char_type const* begin_ptr{};
	char_type const* curr_ptr{};
	char_type const* end_ptr{};
	constexpr basic_ibuffer_view() noexcept = default;
	template <::std::contiguous_iterator Iter>
		requires std::same_as<std::remove_cvref_t<::std::iter_value_t<Iter>>, char_type>
	constexpr basic_ibuffer_view(Iter first, Iter last) noexcept : begin_ptr{::std::to_address(first)}, curr_ptr{begin_ptr}, end_ptr{curr_ptr + (last - first)} {}
	template <std::ranges::contiguous_range rg>
		requires(std::same_as<::std::ranges::range_value_t<rg>, char_type> && !::std::is_array_v<std::remove_cvref_t<rg>>)
	explicit constexpr basic_ibuffer_view(rg& r) noexcept : basic_ibuffer_view(::std::ranges::cbegin(r), ::std::ranges::cend(r)) {}
	constexpr void clear() noexcept {
		curr_ptr = begin_ptr;
	}
};

#if 0
template<std::integral ch_type,::std::contiguous_iterator Iter>
requires std::same_as<::std::iter_value_t<Iter>,ch_type>
[[nodiscard]] inline constexpr Iter read(basic_ibuffer_view<ch_type>& view,Iter first,Iter last) noexcept
{
	auto diff{last-first};
	auto view_diff{view.end_ptr-view.curr_ptr};
	if(view_diff<diff)
		diff=view_diff;
	auto it{::fast_io::details::non_overlapped_copy_n(view.curr_ptr,static_cast<std::size_t>(view_diff),first)};
	view.curr_ptr+=diff;
	return it;
}
#endif

template <std::integral ch_type>
[[nodiscard]] constexpr ch_type const* ibuffer_begin(basic_ibuffer_view<ch_type>& view) noexcept {
	return view.begin_ptr;
}

template <std::integral ch_type>
[[nodiscard]] constexpr ch_type const* ibuffer_curr(basic_ibuffer_view<ch_type>& view) noexcept {
	return view.curr_ptr;
}

template <std::integral ch_type>
[[nodiscard]] constexpr ch_type const* ibuffer_end(basic_ibuffer_view<ch_type>& view) noexcept {
	return view.end_ptr;
}

template <std::integral ch_type>
constexpr void ibuffer_set_curr(basic_ibuffer_view<ch_type>& view, ch_type const* ptr) noexcept {
	view.curr_ptr = ptr;
}

template <std::integral ch_type>
[[nodiscard]] constexpr bool ibuffer_underflow(basic_ibuffer_view<ch_type>&) noexcept {
	return false;
}

template <std::integral ch_type>
inline constexpr bool ibuffer_underflow_never(basic_ibuffer_view<ch_type>&) noexcept {
	return true;
}

template <std::integral ch_type>
struct basic_obuffer_view {
	using char_type = ch_type;
	using output_char_type = ch_type;
	char_type *begin_ptr{}, *curr_ptr{}, *end_ptr{};
	constexpr basic_obuffer_view() noexcept = default;
	template <::std::contiguous_iterator Iter>
		requires std::same_as<::std::iter_value_t<Iter>, char_type>
	constexpr basic_obuffer_view(Iter first, Iter last) noexcept : begin_ptr{::std::to_address(first)},
																   curr_ptr{begin_ptr},
																   end_ptr{::std::to_address(last)} {}
	template <std::ranges::contiguous_range rg>
		requires(std::same_as<::std::ranges::range_value_t<rg>, char_type> && !::std::is_array_v<std::remove_cvref_t<rg>>)
	explicit constexpr basic_obuffer_view(rg& r) noexcept : basic_obuffer_view(::std::ranges::begin(r), ::std::ranges::end(r)) {}
	constexpr void clear() noexcept {
		curr_ptr = begin_ptr;
	}
	constexpr char_type const* cbegin() const noexcept {
		return begin_ptr;
	}
	constexpr char_type const* cend() const noexcept {
		return curr_ptr;
	}
	constexpr char_type const* begin() const noexcept {
		return begin_ptr;
	}
	constexpr char_type const* end() const noexcept {
		return curr_ptr;
	}
	constexpr char_type* begin() noexcept {
		return begin_ptr;
	}
	constexpr char_type* end() noexcept {
		return curr_ptr;
	}

	constexpr char_type const* data() const noexcept {
		return begin_ptr;
	}
	constexpr char_type* data() noexcept {
		return begin_ptr;
	}

	constexpr std::size_t size() const noexcept {
		return static_cast<std::size_t>(curr_ptr - begin_ptr);
	}

	constexpr std::size_t capacity() const noexcept {
		return static_cast<std::size_t>(end_ptr - begin_ptr);
	}
};

#if 0
template<std::integral ch_type,::std::contiguous_iterator Iter>
requires std::same_as<::std::iter_value_t<Iter>,ch_type>
inline constexpr void write(basic_obuffer_view<ch_type>& view,Iter first,Iter last) noexcept
{
	auto diff{last-first};
	auto view_diff{view.end_ptr-view.curr_ptr};
	if(view_diff<diff)
		fast_terminate();
	view.curr_ptr=::fast_io::details::non_overlapped_copy_n(first,static_cast<std::size_t>(diff),view.curr_ptr);
}
#endif

template <std::integral ch_type>
[[nodiscard]] constexpr ch_type* obuffer_begin(basic_obuffer_view<ch_type>& view) noexcept {
	return view.begin_ptr;
}

template <std::integral ch_type>
[[nodiscard]] constexpr ch_type* obuffer_curr(basic_obuffer_view<ch_type>& view) noexcept {
	return view.curr_ptr;
}

template <std::integral ch_type>
[[nodiscard]] constexpr ch_type* obuffer_end(basic_obuffer_view<ch_type>& view) noexcept {
	return view.end_ptr;
}

template <std::integral ch_type>
constexpr void obuffer_set_curr(basic_obuffer_view<ch_type>& view, ch_type* ptr) noexcept {
	view.curr_ptr = ptr;
}

template <std::integral ch_type>
constexpr void obuffer_overflow(basic_obuffer_view<ch_type>&, ch_type) noexcept {
	fast_terminate();
}

template <std::integral ch_type>
inline constexpr bool obuffer_overflow_never(basic_obuffer_view<ch_type>&) noexcept {
	return true;
}

using ibuffer_view = basic_ibuffer_view<char>;
using wibuffer_view = basic_ibuffer_view<wchar_t>;
using u8ibuffer_view = basic_ibuffer_view<char8_t>;
using u16ibuffer_view = basic_ibuffer_view<char16_t>;
using u32ibuffer_view = basic_ibuffer_view<char32_t>;
using obuffer_view = basic_obuffer_view<char>;
using wobuffer_view = basic_obuffer_view<wchar_t>;
using u8obuffer_view = basic_obuffer_view<char8_t>;
using u16obuffer_view = basic_obuffer_view<char16_t>;
using u32obuffer_view = basic_obuffer_view<char32_t>;

template <std::integral ch_type>
inline constexpr basic_ibuffer_view<ch_type> input_stream_ref_define(basic_ibuffer_view<ch_type> biv) noexcept {
	return biv;
}

template <std::integral ch_type>
inline constexpr basic_obuffer_view<ch_type> output_stream_ref_define(basic_obuffer_view<ch_type> bov) noexcept {
	return bov;
}

namespace details {
template <std::integral ch_type>
inline constexpr ::std::byte* biv_read_impl(basic_ibuffer_view<ch_type>& biv, ::std::byte* first, ::std::byte* last) {
	biv.curr_ptr = ::fast_io::details::non_overlapped_copy_n(biv.curr_ptr, biv.end_ptr, first);
	return biv.curr_ptr;
}

template <std::integral ch_type>
inline constexpr ::std::byte const* bov_write_impl(basic_obuffer_view<ch_type>& bov, ::std::byte const* first, ::std::byte const* last) {
	bov.curr_ptr = ::fast_io::details::non_overlapped_copy_n(first, last, bov.curr_ptr);
	return bov.curr_ptr;
}
}

template <std::integral ch_type>
inline constexpr ::std::byte* read_until_eof_underflow_define(basic_ibuffer_view<ch_type>& biv, ::std::byte* first, ::std::byte* last) {
	return ::fast_io::details::biv_read_impl(biv, first, last);
}

template <std::integral ch_type>
inline constexpr ::std::byte const* write_until_eof_overflow_define(basic_obuffer_view<ch_type>& bov, ::std::byte const* first, ::std::byte const* last) {
	return ::fast_io::details::bov_write_impl(bov, first, last);
}

namespace details {
template <std::integral ch_type>
inline constexpr ::std::byte* biv_pread_impl(basic_ibuffer_view<ch_type>& biv, ::std::byte* first, ::std::byte* last, ::fast_io::intfpos_t off) {
	return ::fast_io::details::non_overlapped_copy_n(biv.first + off, biv.end_ptr, first);
}

template <std::integral ch_type>
inline constexpr ::std::byte const* bov_pwrite_impl(basic_obuffer_view<ch_type>& bov, ::std::byte const* first, ::std::byte const* last, ::fast_io::intfpos_t off) {
	return ::fast_io::details::non_overlapped_copy_n(first, last, bov.first + off);
}
}

template <std::integral ch_type>
inline constexpr ::std::byte* pread_until_eof_underflow_define(basic_ibuffer_view<ch_type>& biv, ::std::byte* first, ::std::byte* last, ::fast_io::intfpos_t off) {
	return ::fast_io::details::biv_pread_impl(biv, first, last, off);
}

template <std::integral ch_type>
inline constexpr ::std::byte const* pwrite_until_eof_overflow_define(basic_obuffer_view<ch_type>& bov, ::std::byte const* first, ::std::byte const* last, ::fast_io::intfpos_t off) {
	return ::fast_io::details::bov_pwrite_impl(bov, first, last, off);
}

namespace details {
template <std::integral ch_type>
inline constexpr ::fast_io::intfpos_t biv_seek_impl(basic_ibuffer_view<ch_type>& biv, ::fast_io::intfpos_t off, ::fast_io::seekdir skd) {
	switch (skd) {
	case fast_io::seekdir::beg:
		biv.curr_ptr = biv.begin_ptr + off;
		return biv.curr_ptr;
	case fast_io::seekdir::cur:
		biv.curr_ptr = biv.curr_ptr + off;
		return biv.curr_ptr;
	case fast_io::seekdir::end:
		biv.curr_ptr = biv.end_ptr + off;
		return biv.curr_ptr;
	default:
		::std::unreachable();
	}
}

template <std::integral ch_type>
inline constexpr ::fast_io::intfpos_t bov_seek_impl(basic_obuffer_view<ch_type>& bov, ::fast_io::intfpos_t off, ::fast_io::seekdir skd) {
	switch (skd) {
	case fast_io::seekdir::beg:
		bov.curr_ptr = bov.begin_ptr + off;
		return bov.curr_ptr;
	case fast_io::seekdir::cur:
		bov.curr_ptr = bov.curr_ptr + off;
		return bov.curr_ptr;
	case fast_io::seekdir::end:
		bov.curr_ptr = bov.end_ptr + off;
		return bov.curr_ptr;
	default:
		::std::unreachable();
	}
}
}

template <std::integral ch_type>
inline constexpr ::fast_io::intfpos_t input_stream_seek_define(basic_ibuffer_view<ch_type>& biv, ::fast_io::intfpos_t off, ::fast_io::seekdir skd) {
	return ::fast_io::details::biv_seek_impl(biv, off, skd);
}

template <std::integral ch_type>
inline constexpr ::fast_io::intfpos_t output_stream_seek_define(basic_obuffer_view<ch_type>& bov, ::fast_io::intfpos_t off, ::fast_io::seekdir skd) {
	return ::fast_io::details::bov_seek_impl(bov, off, skd);
}
}
