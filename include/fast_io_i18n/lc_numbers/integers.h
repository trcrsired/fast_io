#pragma once


namespace fast_io
{

namespace details
{

// how many separators does an ndigits-digit number get
template <::std::integral char_type>
inline ::std::size_t lc_grouped_count(lc_ctx<char_type> const *ctx,
									  ::std::size_t ndigits) noexcept
{
	::std::size_t seps{};
	::std::uint_least8_t prev{};
	for (::std::size_t rem{ndigits}, gi{}; rem != 0; ++gi)
	{
		auto const g{lc_group_size(ctx, gi, prev)};
		if (g == 0)
		{
			break;
		}
		prev = g;
		if (rem <= g)
		{
			break;
		}
		rem -= g;
		++seps;
	}
	return seps;
}

// digits written most-significant-first with the separator inserted at
// each group boundary (group sizes count from the right). u is the
// magnitude — the sign, if any, is emitted by the caller. full pads
// with leading zeros to the type's maximum digit count.
template <bool full, ::std::integral char_type, ::fast_io::details::my_unsigned_integral T>
inline char_type *lc_grouped_write(lc_ctx<char_type> const *ctx,
								   char_type *first, T u) noexcept
{
	using int_type = ::std::remove_cv_t<T>;
	int_type tmp[::fast_io::details::cal_max_int_size<int_type, 10>()];
	int_type *tmpend{tmp};
	do
	{
		*tmpend++ = u % 10u;
		u /= 10u;
	} while (u != 0);
	::std::size_t nd{static_cast<::std::size_t>(tmpend - tmp)};
	if constexpr (full)
	{
		nd = sizeof(tmp) / sizeof(int_type);
		// leading zeros fill the remaining most-significant digits
		for (auto p{tmpend}; p != tmp + nd; ++p)
		{
			*p = int_type{};
		}
		tmpend = tmp + nd;
	}
	// group boundaries: a separator goes between the digits when the
	// remaining count equals a cumulative group sum counted from the
	// right — for 10 digits with grouping 3 the bounds are 9,6,3
	// ("1|234|567|890"), descending
	::std::size_t bounds[32];
	::std::size_t nb{};
	{
		::std::uint_least8_t prev{};
		::std::size_t cum{};
		for (::std::size_t gi{}; nb < 32; ++gi)
		{
			auto const g{lc_group_size(ctx, gi, prev)};
			if (g == 0)
			{
				break;
			}
			prev = g;
			if (cum + g >= nd)
			{
				break; // the most-significant group needs no leading sep
			}
			cum += g;
			bounds[nb++] = cum;
		}
	}
	::std::size_t bi{};
	for (::std::size_t pos{nd}; pos != 0; --pos)
	{
		*first++ = ::fast_io::char_literal_add<char_type>(tmp[pos - 1]);
		if (bi < nb && pos - 1 == bounds[nb - 1 - bi])
		{
			::fast_io::details::my_memcpy(first, ctx->sep,
										  ctx->sep_len * sizeof(char_type));
			first += ctx->sep_len;
			++bi;
		}
	}
	return first;
}

// full signed-decimal emit with optional showpos sign
template <bool showpos, bool full, ::std::integral char_type, ::fast_io::details::my_integral T>
inline char_type *lc_print_int(lc_ctx<char_type> const *ctx,
							   char_type *first, T t) noexcept
{
	using int_type = ::std::remove_cv_t<T>;
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
	if (ctx->grouping_len != 0 && ctx->sep_len != 0)
	{
		return lc_grouped_write<full>(ctx, first, u);
	}
	return ::fast_io::details::print_reserve_integral_withfull_main_impl<full, 10, false>(first, u);
}

} // namespace details

// scalar_manip_t of an integral, decimal, non-alphabet type — the only
// case locale grouping applies to (mirrors printf %'d semantics)
template <typename T>
inline constexpr bool lc_grouped_scalar_v{false};

template <::fast_io::manipulators::scalar_flags flags, typename T>
inline constexpr bool lc_grouped_scalar_v<::fast_io::manipulators::scalar_manip_t<flags, T>>{
	::fast_io::details::my_integral<T> && flags.base == 10 && !flags.alphabet &&
	!::std::same_as<::std::remove_cv_t<T>, bool>};

// locale print hooks keyed on lc_ctx — the dynamic_reserve_printable
// analogue. print_reserve_size reports the exact char count,
// print_reserve_define writes it.
template <::std::integral char_type, ::fast_io::manipulators::scalar_flags flags, typename T>
	requires(::fast_io::lc_grouped_scalar_v<
			 ::fast_io::manipulators::scalar_manip_t<flags, T>>)
inline constexpr ::std::size_t
print_reserve_size(lc_ctx<char_type> const *ctx,
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
	return sign + nd +
		   ::fast_io::details::lc_grouped_count(ctx, nd) * ctx->sep_len;
}

template <::std::integral char_type, ::fast_io::manipulators::scalar_flags flags, typename T>
	requires(::fast_io::lc_grouped_scalar_v<
			 ::fast_io::manipulators::scalar_manip_t<flags, T>>)
inline constexpr char_type *
print_reserve_define(lc_ctx<char_type> const *ctx, char_type *iter,
					 ::fast_io::manipulators::scalar_manip_t<flags, T> t) noexcept
{
	return ::fast_io::details::lc_print_int<flags.showpos, flags.full>(ctx, iter, t.reference);
}

} // namespace fast_io

