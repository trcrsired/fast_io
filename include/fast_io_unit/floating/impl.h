#pragma once

#include "punning.h"
#include "hexfloat.h"
#include "roundtrip.h"

namespace fast_io
{

template <::std::integral char_type, manipulators::scalar_flags flags, details::my_floating_point flt>
	requires(flags.base == 10)
inline constexpr ::std::size_t
print_reserve_size(io_reserve_type_t<char_type, manipulators::scalar_manip_t<flags, flt>>) noexcept
{
	static_assert(manipulators::floating_format::general == flags.floating ||
				  manipulators::floating_format::scientific == flags.floating ||
				  manipulators::floating_format::fixed == flags.floating ||
				  manipulators::floating_format::decimal == flags.floating ||
				  manipulators::floating_format::hexfloat == flags.floating);
	using trait = ::fast_io::details::iec559_traits<flt>;
	if constexpr (flags.floating == manipulators::floating_format::hexfloat)
	{
		if constexpr (::std::same_as<::std::remove_cvref_t<flt>, long double>
#if defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)
					  || ::std::same_as<::std::remove_cvref_t<flt>, __float128>
#endif
		)
		{
#if (defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)) && defined(__SIZEOF_INT128__)
			if constexpr (sizeof(flt) > sizeof(double))
			{
				return details::print_rsvhexfloat_size_cache<flags.showbase, __uint128_t>;
			}
			else
#endif
				return details::print_rsvhexfloat_size_cache<flags.showbase,
															 typename details::iec559_traits<double>::mantissa_type>;
		}
		else
		{
			return details::print_rsvhexfloat_size_cache<flags.showbase, typename trait::mantissa_type>;
		}
	}
	else
	{
		if constexpr (::std::same_as<::std::remove_cvref_t<flt>, long double> &&
					  sizeof(flt) == sizeof(double)) // this is the case on xxx-windows-msvc
		{
			return details::print_rsv_cache<double, flags.floating>;
		}
		using rsvflt_traits = details::iec559_traits<::std::remove_cvref_t<flt>>;
		static_assert(rsvflt_traits::mbits == 7u || rsvflt_traits::mbits == 10u ||
						  rsvflt_traits::mbits == 23u || rsvflt_traits::mbits == 52u ||
						  rsvflt_traits::mbits == 63u || rsvflt_traits::mbits == 112u,
					  "unsupported floating-point format for shortest conversion");
		return details::print_rsv_cache<::std::remove_cvref_t<flt>, flags.floating>;
	}
}

template <::std::integral char_type, manipulators::scalar_flags flags, details::my_floating_point flt>
	requires(flags.base == 10)
inline constexpr char_type *print_reserve_define(io_reserve_type_t<char_type, manipulators::scalar_manip_t<flags, flt>>,
												 char_type *iter, manipulators::scalar_manip_t<flags, flt> f) noexcept
{
	static_assert(manipulators::floating_format::general == flags.floating ||
				  manipulators::floating_format::scientific == flags.floating ||
				  manipulators::floating_format::fixed == flags.floating ||
				  manipulators::floating_format::decimal == flags.floating ||
				  manipulators::floating_format::hexfloat == flags.floating);
	if constexpr (flags.floating == manipulators::floating_format::hexfloat)
	{
		if constexpr (::std::same_as<::std::remove_cvref_t<flt>, long double>
#if defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)
					  || ::std::same_as<::std::remove_cvref_t<flt>, __float128>
#endif
		)
		{
#if (defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)) && defined(__SIZEOF_INT128__)
			if constexpr (sizeof(flt) > sizeof(double))
			{
				return details::print_rsvhexfloat_define_impl<flags.showbase, flags.uppercase_showbase, flags.showpos,
															  flags.uppercase, flags.uppercase_e, flags.comma>(
					iter, static_cast<__float128>(f.reference));
			}
			else
#endif
				return details::print_rsvhexfloat_define_impl<flags.showbase, flags.uppercase_showbase, flags.showpos,
															  flags.uppercase, flags.uppercase_e, flags.comma>(
					iter, static_cast<double>(f.reference));
		}
		else
		{
			return details::print_rsvhexfloat_define_impl<flags.showbase, flags.uppercase_showbase, flags.showpos,
														  flags.uppercase, flags.uppercase_e, flags.comma>(iter,
																										   f.reference);
		}
	}
	else
	{
		if constexpr (::std::same_as<::std::remove_cvref_t<flt>, long double> &&
					  sizeof(flt) == sizeof(double)) // this is the case on xxx-windows-msvc
		{
			return details::print_rsvflt_define_impl<flags.showpos, flags.uppercase, flags.uppercase_e, flags.comma,
													 flags.floating>(iter, static_cast<double>(f.reference));
		}
		else
		{
			// this is the case for every other platform, including xxx-windows-gnu
			using rsvflt_traits = details::iec559_traits<::std::remove_cvref_t<flt>>;
			static_assert(rsvflt_traits::mbits == 7u || rsvflt_traits::mbits == 10u ||
							  rsvflt_traits::mbits == 23u || rsvflt_traits::mbits == 52u ||
							  rsvflt_traits::mbits == 63u || rsvflt_traits::mbits == 112u,
						  "unsupported floating-point format for shortest conversion");
			return details::print_rsvflt_define_impl<flags.showpos, flags.uppercase, flags.uppercase_e, flags.comma,
													 flags.floating>(iter, f.reference);
		}
	}
}

#if 0
template <::std::integral char_type, manipulators::scalar_flags flags, details::my_floating_point flt>
	requires(flags.base == 10)
inline constexpr ::std::size_t
print_reserve_size(io_reserve_type_t<char_type, manipulators::scalar_manip_precision_t<flags, flt>>) noexcept
{
	static_assert(manipulators::floating_format::general == flags.floating ||
				  manipulators::floating_format::scientific == flags.floating ||
				  manipulators::floating_format::fixed == flags.floating ||
				  manipulators::floating_format::decimal == flags.floating ||
				  manipulators::floating_format::hexfloat == flags.floating);
	using trait = ::fast_io::details::iec559_traits<flt>;
	if constexpr (flags.floating == manipulators::floating_format::hexfloat)
	{
		if constexpr (::std::same_as<::std::remove_cvref_t<flt>, long double>
#if defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)
					  || ::std::same_as<::std::remove_cvref_t<flt>, __float128>
#endif
		)
		{
#if (defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)) && defined(__SIZEOF_INT128__)
			if constexpr (sizeof(flt) > sizeof(double))
			{
				return details::print_rsvhexfloat_size_cache<flags.showbase, __uint128_t>;
			}
			else
#endif
				return details::print_rsvhexfloat_size_cache<flags.showbase,
															 typename details::iec559_traits<double>::mantissa_type>;
		}
		else
		{
			return details::print_rsvhexfloat_size_cache<flags.showbase, typename trait::mantissa_type>;
		}
	}
	else
	{
		if constexpr (::std::same_as<::std::remove_cvref_t<flt>, long double> &&
					  sizeof(flt) == sizeof(double)) // this is the case on xxx-windows-msvc
		{
			return details::print_rsv_cache<double, flags.floating>;
		}
		using rsvflt_traits = details::iec559_traits<::std::remove_cvref_t<flt>>;
		static_assert(rsvflt_traits::mbits == 7u || rsvflt_traits::mbits == 10u ||
						  rsvflt_traits::mbits == 23u || rsvflt_traits::mbits == 52u ||
						  rsvflt_traits::mbits == 63u || rsvflt_traits::mbits == 112u,
					  "unsupported floating-point format for shortest conversion");
		return details::print_rsv_cache<::std::remove_cvref_t<flt>, flags.floating>;
	}
}

template <::std::integral char_type, manipulators::scalar_flags flags, details::my_floating_point flt>
	requires(flags.base == 10)
inline constexpr char_type *print_reserve_define(io_reserve_type_t<char_type, manipulators::scalar_manip_precision_t<flags, flt>>,
												 char_type *iter, manipulators::scalar_manip_precision_t<flags, flt> f) noexcept
{
	static_assert(manipulators::floating_format::general == flags.floating ||
				  manipulators::floating_format::scientific == flags.floating ||
				  manipulators::floating_format::fixed == flags.floating ||
				  manipulators::floating_format::decimal == flags.floating ||
				  manipulators::floating_format::hexfloat == flags.floating);
	if constexpr (flags.floating == manipulators::floating_format::hexfloat)
	{
		if constexpr (::std::same_as<::std::remove_cvref_t<flt>, long double>
#if defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)
					  || ::std::same_as<::std::remove_cvref_t<flt>, __float128>
#endif
		)
		{
#if (defined(__SIZEOF_FLOAT128__) || defined(__FLOAT128__)) && defined(__SIZEOF_INT128__)
			if constexpr (sizeof(flt) > sizeof(double))
			{
				return details::print_rsvhexfloat_define_impl<flags.showbase, flags.uppercase_showbase, flags.showpos,
															  flags.uppercase, flags.uppercase_e, flags.comma>(
					iter, static_cast<__float128>(f.reference));
			}
			else
#endif
				return details::print_rsvhexfloat_define_impl<flags.showbase, flags.uppercase_showbase, flags.showpos,
															  flags.uppercase, flags.uppercase_e, flags.comma>(
					iter, static_cast<double>(f.reference));
		}
		else
		{
			return details::print_rsvhexfloat_define_impl<flags.showbase, flags.uppercase_showbase, flags.showpos,
														  flags.uppercase, flags.uppercase_e, flags.comma>(iter,
																										   f.reference);
		}
	}
	else
	{
		if constexpr (::std::same_as<::std::remove_cvref_t<flt>, long double> &&
					  sizeof(flt) == sizeof(double)) // this is the case on xxx-windows-msvc
		{
			return details::print_rsvflt_define_impl<flags.showpos, flags.uppercase, flags.uppercase_e, flags.comma,
													 flags.floating>(iter, static_cast<double>(f.reference));
		}
		else
		{
			// this is the case for every other platform, including xxx-windows-gnu
			using rsvflt_traits = details::iec559_traits<::std::remove_cvref_t<flt>>;
			static_assert(rsvflt_traits::mbits == 7u || rsvflt_traits::mbits == 10u ||
							  rsvflt_traits::mbits == 23u || rsvflt_traits::mbits == 52u ||
							  rsvflt_traits::mbits == 63u || rsvflt_traits::mbits == 112u,
						  "unsupported floating-point format for shortest conversion");
			return details::print_rsvflt_define_impl<flags.showpos, flags.uppercase, flags.uppercase_e, flags.comma,
													 flags.floating>(iter, f.reference);
		}
	}
}
#endif
} // namespace fast_io
