#pragma once

namespace fast_io::details::algorithms
{

template<typename T>
concept non_bool_integral =
  ::std::integral<T> && !::std::same_as<::std::remove_cv_t<T>, bool>;

// Magnitude as an unsigned value. `std::abs` is unusable here: applying it to
// the most negative signed value overflows, while the unsigned conversion is
// well defined and yields the correct magnitude.
template<::std::integral T>
inline constexpr ::std::make_unsigned_t<T> integral_magnitude(T value) noexcept
{
  using U = ::std::make_unsigned_t<T>;
  if constexpr (::std::is_signed_v<T>)
  {
    return value < 0 ? U(0) - static_cast<U>(value) : static_cast<U>(value);
  }
  else
  {
    return static_cast<U>(value);
  }
}

template<typename U>
inline constexpr U gcd_unsigned(U m, U n) noexcept
{
  while (n != 0)
  {
    U const remainder{m % n};
    m = n;
    n = remainder;
  }
  return m;
}

} // namespace fast_io::details::algorithms

namespace fast_io::algorithms
{

// Greatest common divisor of the magnitudes of `m` and `n`. `gcd(0, 0)` is 0.
template<::fast_io::details::algorithms::non_bool_integral T>
inline constexpr T gcd(T m, T n) noexcept
{
  using U = ::std::make_unsigned_t<T>;
  U const result{::fast_io::details::algorithms::gcd_unsigned(
    ::fast_io::details::algorithms::integral_magnitude(m),
    ::fast_io::details::algorithms::integral_magnitude(n))};
  return static_cast<T>(result);
}

// Least common multiple of the magnitudes of `m` and `n`. Zero when either
// operand is zero. The result is undefined if it is not representable in `T`,
// matching the standard contract.
template<::fast_io::details::algorithms::non_bool_integral T>
inline constexpr T lcm(T m, T n) noexcept
{
  if (m == 0 || n == 0)
  {
    return T(0);
  }
  using U = ::std::make_unsigned_t<T>;
  U const um{::fast_io::details::algorithms::integral_magnitude(m)};
  U const un{::fast_io::details::algorithms::integral_magnitude(n)};
  U const divisor{::fast_io::details::algorithms::gcd_unsigned(um, un)};
  return static_cast<T>((um / divisor) * un);
}

} // namespace fast_io::algorithms
