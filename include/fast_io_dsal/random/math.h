#pragma once

#include <concepts>
#include <limits>
#include <type_traits>

#if !defined(__clang__) && !defined(__GNUC__)
#include <cmath>
#endif

// The distributions need a handful of libm entry points. Libc++ routes these
// through __math::*, which is a thin type-dispatching wrapper over the
// compiler builtins; the same dispatch is reproduced here so the distributions
// stay a direct port. Builtins keep the math calls visible to the optimizer and
// avoid pulling <cmath> into freestanding builds. The fallback covers
// compilers without the GCC/Clang builtins (MSVC).
namespace fast_io::random::details::math
{

#if defined(__clang__) || defined(__GNUC__)

template <::std::floating_point T>
inline constexpr T exp(T x) noexcept
{
	if constexpr (::std::same_as<T, float>)
	{
		return __builtin_expf(x);
	}
	else if constexpr (::std::same_as<T, double>)
	{
		return __builtin_exp(x);
	}
	else
	{
		return __builtin_expl(x);
	}
}

template <::std::floating_point T>
inline constexpr T log(T x) noexcept
{
	if constexpr (::std::same_as<T, float>)
	{
		return __builtin_logf(x);
	}
	else if constexpr (::std::same_as<T, double>)
	{
		return __builtin_log(x);
	}
	else
	{
		return __builtin_logl(x);
	}
}

template <::std::floating_point T>
inline constexpr T log1p(T x) noexcept
{
	if constexpr (::std::same_as<T, float>)
	{
		return __builtin_log1pf(x);
	}
	else if constexpr (::std::same_as<T, double>)
	{
		return __builtin_log1p(x);
	}
	else
	{
		return __builtin_log1pl(x);
	}
}

template <::std::floating_point T>
inline constexpr T sqrt(T x) noexcept
{
	if constexpr (::std::same_as<T, float>)
	{
		return __builtin_sqrtf(x);
	}
	else if constexpr (::std::same_as<T, double>)
	{
		return __builtin_sqrt(x);
	}
	else
	{
		return __builtin_sqrtl(x);
	}
}

template <::std::floating_point T>
inline constexpr T pow(T x, T y) noexcept
{
	if constexpr (::std::same_as<T, float>)
	{
		return __builtin_powf(x, y);
	}
	else if constexpr (::std::same_as<T, double>)
	{
		return __builtin_pow(x, y);
	}
	else
	{
		return __builtin_powl(x, y);
	}
}

template <::std::floating_point T>
inline constexpr T tan(T x) noexcept
{
	if constexpr (::std::same_as<T, float>)
	{
		return __builtin_tanf(x);
	}
	else if constexpr (::std::same_as<T, double>)
	{
		return __builtin_tan(x);
	}
	else
	{
		return __builtin_tanl(x);
	}
}

template <::std::floating_point T>
inline constexpr T trunc(T x) noexcept
{
	if constexpr (::std::same_as<T, float>)
	{
		return __builtin_truncf(x);
	}
	else if constexpr (::std::same_as<T, double>)
	{
		return __builtin_trunc(x);
	}
	else
	{
		return __builtin_truncl(x);
	}
}

template <::std::floating_point T>
inline constexpr T fabs(T x) noexcept
{
	if constexpr (::std::same_as<T, float>)
	{
		return __builtin_fabsf(x);
	}
	else if constexpr (::std::same_as<T, double>)
	{
		return __builtin_fabs(x);
	}
	else
	{
		return __builtin_fabsl(x);
	}
}

template <::std::floating_point T>
inline constexpr bool isinf(T x) noexcept
{
	return __builtin_isinf(x);
}

template <::std::floating_point T>
inline constexpr T nextafter(T x, T y) noexcept
{
	if constexpr (::std::same_as<T, float>)
	{
		return __builtin_nextafterf(x, y);
	}
	else if constexpr (::std::same_as<T, double>)
	{
		return __builtin_nextafter(x, y);
	}
	else
	{
		return __builtin_nextafterl(x, y);
	}
}

// Only the value is used by the distributions; the sign is not, so the
// reentrant POSIX entry point buys nothing here.
inline constexpr double lgamma_r(double x) noexcept
{
	return __builtin_lgamma(x);
}

#else // !defined(__clang__) && !defined(__GNUC__)

template <::std::floating_point T>
inline constexpr T exp(T x) noexcept
{
	return ::std::exp(x);
}

template <::std::floating_point T>
inline constexpr T log(T x) noexcept
{
	return ::std::log(x);
}

template <::std::floating_point T>
inline constexpr T log1p(T x) noexcept
{
	return ::std::log1p(x);
}

template <::std::floating_point T>
inline constexpr T sqrt(T x) noexcept
{
	return ::std::sqrt(x);
}

template <::std::floating_point T>
inline constexpr T pow(T x, T y) noexcept
{
	return ::std::pow(x, y);
}

template <::std::floating_point T>
inline constexpr T tan(T x) noexcept
{
	return ::std::tan(x);
}

template <::std::floating_point T>
inline constexpr T trunc(T x) noexcept
{
	return ::std::trunc(x);
}

template <::std::floating_point T>
inline constexpr T fabs(T x) noexcept
{
	return ::std::fabs(x);
}

template <::std::floating_point T>
inline constexpr bool isinf(T x) noexcept
{
	return ::std::isinf(x);
}

template <::std::floating_point T>
inline constexpr T nextafter(T x, T y) noexcept
{
	return ::std::nextafter(x, y);
}

inline constexpr double lgamma_r(double x) noexcept
{
	return ::std::lgamma(x);
}

#endif

} // namespace fast_io::random::details::math
