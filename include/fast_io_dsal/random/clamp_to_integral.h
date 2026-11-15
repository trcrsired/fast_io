#pragma once

#include "math.h"

#include <limits>
#include <type_traits>

namespace fast_io::random::details
{

// Port of libc++'s __max_representable_int_for_float.
template <typename IntT, typename FloatT>
inline constexpr IntT max_representable_int_for_float() noexcept
{
	static_assert(::std::is_floating_point_v<FloatT>, "must be a floating point type");
	static_assert(::std::is_integral_v<IntT>, "must be an integral type");
	static_assert(::std::numeric_limits<FloatT>::radix == 2,
				  "FloatT has incorrect radix");
	static_assert(::std::numeric_limits<IntT>::radix == 2,
				  "IntT has incorrect radix");
	if constexpr (::std::numeric_limits<FloatT>::digits >
				  ::std::numeric_limits<IntT>::digits)
	{
		return ::std::numeric_limits<IntT>::max();
	}
	else
	{
		constexpr int bits{::std::numeric_limits<IntT>::digits -
						   ::std::numeric_limits<FloatT>::digits};
		return static_cast<IntT>(
			(::std::numeric_limits<IntT>::max() >> bits) << bits);
	}
}

// Port of libc++'s __clamp_to_integral: convert r after clamping it to the
// integral type's representable range. r must not be NaN.
template <typename IntT, typename RealT>
inline constexpr IntT clamp_to_integral(RealT r) noexcept
{
	using IntLim = ::std::numeric_limits<IntT>;
	using RealLim = ::std::numeric_limits<RealT>;
	constexpr IntT max_val{details::max_representable_int_for_float<IntT, RealT>()};
	if (r >= details::math::nextafter(
				 static_cast<RealT>(max_val), RealLim::infinity()))
	{
		return IntLim::max();
	}
	else if (r <= static_cast<RealT>(IntLim::lowest()))
	{
		return IntLim::min();
	}
	return static_cast<IntT>(r);
}

} // namespace fast_io::random::details
