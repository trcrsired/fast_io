#pragma once

#include "../fast_io_dsal/impl/misc/push_macros.h"
#include "lcblob.h"

// locale-aware printing. imbue(loc, out) wraps an output stream so that
// scalar values are formatted with the locale's numeric conventions
// (thousands_sep + grouping from the lcblob); every other argument type
// prints exactly as it would without the wrapper.
//
//   ::fast_io::println(::fast_io::imbue(loc, obf), 1242141242124);
//
// The locale handle is a ::fast_io::i18n::locale const* — an immortal
// pointer into the fast_io_i18n cache (thread-local first, then the
// global map). It can be stored, copied and reused freely; locale data
// is never unloaded.

namespace fast_io
{

template <typename stm>
struct lc_imbuer
{
	using handle_type = stm;
	using char_type = typename handle_type::char_type;
	using output_char_type = char_type;
	::fast_io::i18n::locale const *locale{};
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
	[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address) >= 201803
	[[no_unique_address]]
#endif
#endif
	handle_type handle{};
};

template <typename stm>
inline constexpr lc_imbuer<stm> output_stream_ref_define(lc_imbuer<stm> t) noexcept
{
	return t;
}

template <typename stm>
	requires(::std::is_lvalue_reference_v<stm> || ::std::is_trivially_copyable_v<stm>)
inline constexpr auto imbue(::fast_io::i18n::locale const *locale, stm &&out) noexcept
{
	using reftype = decltype(::fast_io::operations::output_stream_ref(out));
	return lc_imbuer<reftype>{locale, ::fast_io::operations::output_stream_ref(out)};
}

namespace i18n::details
{

// decoded LC_NUMERIC for one print call: thousands_sep transcoded to the
// stream char_type (capped — a longer separator disables grouping rather
// than allocating), grouping is the raw i8 byte list from the blob
// (charset-neutral).
inline constexpr ::std::size_t lc_sep_capacity{16};

template <::std::integral char_type>
struct lc_numeric_ctx
{
	char_type sep[lc_sep_capacity]{};
	::std::size_t sep_len{};
	char8_t const *grouping{};
	::std::size_t grouping_len{};
};

// assemble width-W little-endian char units out of the flat byte pool
// (blob payloads are LE on every host)
template <::std::integral U>
inline ::std::size_t lc_le_units(char8_t const *p, ::std::size_t len, U *dst, ::std::size_t cap) noexcept
{
	constexpr ::std::size_t w{sizeof(U)};
	::std::size_t n{len / w};
	if (cap < n)
	{
		n = cap;
	}
	for (::std::size_t i{}; i < n; ++i)
	{
		::std::uint_least32_t v{};
		for (::std::size_t b{}; b < w; ++b)
		{
			v |= static_cast<::std::uint_least32_t>(p[i * w + b]) << (b * 8u);
		}
		dst[i] = static_cast<U>(v);
	}
	return n;
}

// stream char_type -> the blob section whose payload already is that
// type. char reads the charset slot (the locale's declared codeset),
// char8_t utf8, char16_t utf16, char32_t utf32; wchar_t follows its
// size. No transcoding happens at runtime.
template <::std::integral char_type>
inline constexpr ::fast_io::i18n::lcblob::blob_charset lc_charset_for{
	::std::same_as<char_type, char> ? ::fast_io::i18n::lcblob::blob_charset::charset
	: ::std::same_as<char_type, char8_t> ? ::fast_io::i18n::lcblob::blob_charset::utf8
	: (sizeof(char_type) == 2 ? ::fast_io::i18n::lcblob::blob_charset::utf16
							  : ::fast_io::i18n::lcblob::blob_charset::utf32)};

template <::std::integral char_type>
inline lc_numeric_ctx<char_type> lc_load_numeric(::fast_io::i18n::locale const *v) FAST_IO_HERBCEPTIONS_THROWS
{
	lc_numeric_ctx<char_type> ctx{};
	if (v == nullptr)
	{
		return ctx;
	}
	namespace lc = ::fast_io::i18n::lcblob;
	constexpr auto cs{lc_charset_for<char_type>};
	if (auto const f{::fast_io::i18n::locale_field(v, lc::lc_numeric, 1, cs)}; // thousands_sep
		f.rec != nullptr)
	{
		::fast_io::i18n::locale_slot s;
		lc::read_slot(f.rec, f.seg_end, f.seg_begin, s);
		if (s.tag == lc::slot_tag::string && s.len != 0 &&
			s.len / sizeof(char_type) <= lc_sep_capacity)
		{
			// payload units already are char_type — assemble LE, no codecvt
			ctx.sep_len = lc_le_units(s.ptr, static_cast<::std::size_t>(s.len), ctx.sep, lc_sep_capacity);
		}
	}
	if (auto const f{::fast_io::i18n::locale_field(v, lc::lc_numeric, 2, cs)}; // grouping
		f.rec != nullptr)
	{
		::fast_io::i18n::locale_slot s;
		lc::read_slot(f.rec, f.seg_end, f.seg_begin, s);
		if (s.tag == lc::slot_tag::bytes)
		{
			ctx.grouping = s.ptr;
			ctx.grouping_len = static_cast<::std::size_t>(s.len);
		}
	}
	return ctx;
}

// glibc grouping semantics: entry gi is the group size for group gi
// counting from the right; past the end the last value repeats; a 0
// repeats the previous value; 0xFF (CHAR_MAX) or 0-with-no-previous
// means no further grouping.
template <::std::integral char_type>
inline ::std::uint_least8_t lc_group_size(lc_numeric_ctx<char_type> const &ctx, ::std::size_t gi,
										  ::std::uint_least8_t prev) noexcept
{
	::std::uint_least8_t const e{static_cast<::std::uint_least8_t>(gi < ctx.grouping_len ? ctx.grouping[gi] : prev)};
	return e == 0 ? prev : e;
}

// total chars for ndigits digits with grouping separators
template <::std::integral char_type>
inline ::std::size_t lc_grouped_count(lc_numeric_ctx<char_type> const &ctx, ::std::size_t ndigits) noexcept
{
	::std::size_t i{}, seps{};
	::std::uint_least8_t prev{};
	for (::std::size_t gi{};; ++gi)
	{
		::std::uint_least8_t const e{lc_group_size(ctx, gi, prev)};
		if (e == 0 || e == 0xFF || ndigits - i <= e)
		{
			break;
		}
		i += e;
		++seps;
		prev = e;
	}
	return ndigits + seps * ctx.sep_len;
}

// write digits of u (base 10) with grouping backwards from
// first + lc_grouped_count(ctx, ndigits); returns that end pointer
template <bool full, ::std::integral char_type, ::fast_io::details::my_unsigned_integral T>
inline char_type *lc_grouped_write(lc_numeric_ctx<char_type> const &ctx, char_type *first, T u) noexcept
{
	constexpr ::std::size_t maxd{::fast_io::details::cal_max_int_size<T, 10>()};
	::std::size_t const nd{full ? maxd : ::fast_io::details::chars_len<10>(u)};
	char_type *const ed{first + lc_grouped_count(ctx, nd)};
	char_type *it{ed};
	::std::size_t i{};
	::std::uint_least8_t prev{};
	for (::std::size_t gi{};; ++gi)
	{
		::std::uint_least8_t const e{lc_group_size(ctx, gi, prev)};
		if (e == 0 || e == 0xFF)
		{
			for (; i != nd; ++i)
			{
				*--it = ::fast_io::char_literal_add<char_type>(u % 10u);
				u = static_cast<T>(u / 10u);
			}
			return ed;
		}
		for (::std::size_t j{}; j != e && i != nd; ++j, ++i)
		{
			*--it = ::fast_io::char_literal_add<char_type>(u % 10u);
			u = static_cast<T>(u / 10u);
		}
		if (i == nd)
		{
			return ed;
		}
		for (::std::size_t k{ctx.sep_len}; k-- != 0;)
		{
			*--it = ctx.sep[k];
		}
		prev = e;
	}
}

// sign + grouped/ungrouped digit emission for a base-10 scalar
template <bool showpos, bool full, ::std::integral char_type, typename int_type>
inline char_type *lc_print_int(lc_numeric_ctx<char_type> const &ctx, char_type *first, int_type t) noexcept
{
	using unsigned_type = ::fast_io::details::my_make_unsigned_t<int_type>;
	unsigned_type u{static_cast<unsigned_type>(t)};
	if constexpr (showpos)
	{
		if constexpr (::fast_io::details::my_unsigned_integral<int_type>)
		{
			*first = ::fast_io::char_literal_v<u8'+', char_type>;
		}
		else
		{
			if (t < 0)
			{
				*first = ::fast_io::char_literal_v<u8'-', char_type>;
				constexpr unsigned_type zero{};
				u = static_cast<unsigned_type>(zero - u);
			}
			else
			{
				*first = ::fast_io::char_literal_v<u8'+', char_type>;
			}
		}
		++first;
	}
	else if constexpr (::fast_io::details::my_signed_integral<int_type>)
	{
		if (t < 0)
		{
			*first = ::fast_io::char_literal_v<u8'-', char_type>;
			++first;
			constexpr unsigned_type zero{};
			u = static_cast<unsigned_type>(zero - u);
		}
	}
	if (ctx.grouping_len != 0 && ctx.sep_len != 0)
	{
		return lc_grouped_write<full>(ctx, first, u);
	}
	return ::fast_io::details::print_reserve_integral_withfull_main_impl<full, 10, false>(first, u);
}

// scalar_manip_t of an integral, decimal, non-alphabet type — the only
// case locale grouping applies to (mirrors printf %'d semantics)
template <typename T>
inline constexpr bool lc_grouped_scalar_v{false};

template <::fast_io::manipulators::scalar_flags flags, typename T>
inline constexpr bool lc_grouped_scalar_v<::fast_io::manipulators::scalar_manip_t<flags, T>>{
	::fast_io::details::my_integral<T> && flags.base == 10 && !flags.alphabet &&
	!::std::same_as<::std::remove_cv_t<T>, bool>};

} // namespace i18n::details

// ---------------------------------------------------------------------------
// locale-aware print hooks — the dynamic_reserve_printable analogue
// keyed on the decoded numeric context. print_reserve_size reports the
// exact byte count, print_reserve_define writes it; other types plug in
// by overloading these on the same ctx parameter.
// ---------------------------------------------------------------------------

template <::std::integral char_type, ::fast_io::manipulators::scalar_flags flags, typename T>
	requires(::fast_io::i18n::details::lc_grouped_scalar_v<
			 ::fast_io::manipulators::scalar_manip_t<flags, T>>)
inline constexpr ::std::size_t
print_reserve_size(::fast_io::i18n::details::lc_numeric_ctx<char_type> const &ctx,
				   ::fast_io::manipulators::scalar_manip_t<flags, T> t) noexcept
{
	using unsigned_type = ::fast_io::details::my_make_unsigned_t<T>;
	unsigned_type u{static_cast<unsigned_type>(t.reference)};
	::std::size_t sign{};
	if constexpr (flags.showpos)
	{
		sign = 1;
		if constexpr (::fast_io::details::my_signed_integral<T>)
		{
			if (t.reference < 0)
			{
				constexpr unsigned_type zero{};
				u = static_cast<unsigned_type>(zero - u);
			}
		}
	}
	else if constexpr (::fast_io::details::my_signed_integral<T>)
	{
		if (t.reference < 0)
		{
			sign = 1;
			constexpr unsigned_type zero{};
			u = static_cast<unsigned_type>(zero - u);
		}
	}
	::std::size_t const nd{flags.full ? ::fast_io::details::cal_max_int_size<T, 10>()
									  : ::fast_io::details::chars_len<10>(u)};
	return sign + ::fast_io::i18n::details::lc_grouped_count(ctx, nd);
}

template <::std::integral char_type, ::fast_io::manipulators::scalar_flags flags, typename T>
	requires(::fast_io::i18n::details::lc_grouped_scalar_v<
			 ::fast_io::manipulators::scalar_manip_t<flags, T>>)
inline constexpr char_type *
print_reserve_define(::fast_io::i18n::details::lc_numeric_ctx<char_type> const &ctx, char_type *iter,
					 ::fast_io::manipulators::scalar_manip_t<flags, T> t) noexcept
{
	return ::fast_io::i18n::details::lc_print_int<flags.showpos, flags.full>(ctx, iter, t.reference);
}

namespace i18n::details
{

// write one lc scalar to the stream — obuffer fast path first (reserve
// the exact size, write in place), stack buffer + scatter otherwise
template <typename output, ::fast_io::manipulators::scalar_flags flags, typename T>
inline constexpr void lc_write_scalar(lc_numeric_ctx<typename output::output_char_type> const &ctx, output out,
									  ::fast_io::manipulators::scalar_manip_t<flags, T> t)
	FAST_IO_HERBCEPTIONS_THROWS
{
	using char_type = typename output::output_char_type;
	::std::size_t const need{print_reserve_size(ctx, t)};
	if constexpr (::fast_io::operations::decay::defines::has_obuffer_basic_operations<output>)
	{
		auto const curr{obuffer_curr(out)};
		auto const ed{obuffer_end(out)};
		auto const diff{ed - curr};
		if (0 <= diff && need <= static_cast<::std::size_t>(diff)) [[likely]]
		{
			obuffer_set_curr(out, print_reserve_define(ctx, curr, t));
			return;
		}
		else if constexpr (::fast_io::operations::decay::defines::has_obuffer_flush_reserve_define<output>)
		{
			obuffer_flush_reserve_define(out, need);
			auto const ncurr{obuffer_curr(out)};
			obuffer_set_curr(out, print_reserve_define(ctx, ncurr, t));
			return;
		}
	}
	constexpr ::std::size_t ss{
		::fast_io::details::print_integer_reserved_size_cache<flags.base, flags.showbase, flags.showpos,
															::std::remove_cv_t<T>>};
	char_type buf[ss + ss * lc_sep_capacity];
	auto const it{print_reserve_define(ctx, buf, t)};
	::fast_io::operations::print_freestanding<false>(
		out, ::fast_io::basic_io_scatter_t<char_type>{buf, static_cast<::std::size_t>(it - buf)});
}

// index of the first arg that is an lc-grouped scalar;
// sizeof...(Args) when none — same template-for consteval pattern as
// details::first_print_define_index_range in the plain path
template <typename... Args>
inline consteval ::std::size_t lc_first_scalar_index() noexcept
{
	template for (constexpr auto i : ::fast_io::details::index_array_range<0, sizeof...(Args)>)
	{
		if constexpr (lc_grouped_scalar_v<::std::remove_cvref_t<Args...[i]>>)
		{
			return i;
		}
	}
	return sizeof...(Args);
}

template <typename... Args>
inline constexpr bool lc_any_scalar_v{lc_first_scalar_index<Args...>() != sizeof...(Args)};

// args are split at every lc scalar: contiguous non-lc segments go
// through print_freestanding_decay (the plain path keeps its reserve /
// scatter batching inside each segment), lc scalars write via ctx.
// The terminal call delegates to the plain path — it also emits '\n'.
template <bool line, typename output, typename... Args>
inline constexpr void lc_status_print_impl(lc_numeric_ctx<typename output::output_char_type> const &ctx,
										   output optstm, Args... args) FAST_IO_HERBCEPTIONS_THROWS
{
	constexpr ::std::size_t fpos{lc_first_scalar_index<Args...>()};
	if constexpr (fpos == sizeof...(Args))
	{
		return ::fast_io::operations::decay::print_freestanding_decay<line>(optstm, args...);
	}
	else
	{
		if constexpr (fpos != 0)
		{
			[&]<::std::size_t... pos>(::std::index_sequence<pos...>) FAST_IO_HERBCEPTIONS_THROWS {
				::fast_io::operations::decay::print_freestanding_decay<false>(optstm, args...[pos]...);
			}(::std::make_index_sequence<fpos>{});
		}
		lc_write_scalar(ctx, optstm, args...[fpos]);
		return [&]<::std::size_t... pos>(::std::index_sequence<pos...>) FAST_IO_HERBCEPTIONS_THROWS {
			return lc_status_print_impl<line>(ctx, optstm, args...[fpos + 1 + pos]...);
		}(::std::make_index_sequence<sizeof...(Args) - fpos - 1>{});
	}
}

} // namespace i18n::details

template <bool line, typename output, typename... Args>
inline constexpr void status_print_define(::fast_io::lc_imbuer<output> imb, Args... args)
	FAST_IO_HERBCEPTIONS_THROWS
{
	if constexpr (::fast_io::operations::decay::defines::has_output_or_io_stream_mutex_ref_define<output>)
	{
		::fast_io::operations::decay::stream_ref_decay_lock_guard lg{
			::fast_io::operations::decay::output_stream_mutex_ref_decay(imb.handle)};
		return status_print_define<line>(
			::fast_io::lc_imbuer{imb.locale,
								 ::fast_io::operations::decay::output_stream_unlocked_ref_decay(imb.handle)},
			args...);
	}
	else if constexpr (::fast_io::i18n::details::lc_any_scalar_v<Args...>)
	{
		using char_type = typename output::output_char_type;
		auto const ctx{::fast_io::i18n::details::lc_load_numeric<char_type>(imb.locale)};
		return ::fast_io::i18n::details::lc_status_print_impl<line>(ctx, imb.handle, args...);
	}
	else
	{
		return ::fast_io::operations::decay::print_freestanding_decay<line>(imb.handle, args...);
	}
}

} // namespace fast_io

#include "../fast_io_dsal/impl/misc/pop_macros.h"
