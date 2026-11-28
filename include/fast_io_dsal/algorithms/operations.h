#pragma once

namespace fast_io::algorithms::field_ops
{

// Field operations are named objects with a `compute` member, mirroring the
// `compare` member of comparators. `operator()` is deliberately not used: a
// closure has no `compute` member, so a lambda cannot be passed where an
// operation is expected.
//
// The objects are not class templates. `compute` deduces both operands, so one
// object works for every element and accumulator type. This is the difference
// from `std::plus<T>` and friends, whose class template parameter fixes the
// operand type.

struct add
{
  template<typename T, typename U>
  inline constexpr auto compute(T const &t, U const &u) const
    FAST_IO_HERBCEPTIONS_THROWS_IF(!noexcept(t + u))
  {
    return t + u;
  }
};

struct minus
{
  template<typename T, typename U>
  inline constexpr auto compute(T const &t, U const &u) const
    FAST_IO_HERBCEPTIONS_THROWS_IF(!noexcept(t - u))
  {
    return t - u;
  }
};

struct multiply
{
  template<typename T, typename U>
  FAST_IO_UNSAFE_API
  inline constexpr auto compute(T const &t, U const &u) const
    FAST_IO_HERBCEPTIONS_THROWS_IF(!noexcept(t * u))
  {
    return t * u;
  }
};

struct divide
{
  template<typename T, typename U>
  inline constexpr auto compute(T const &t, U const &u) const
    FAST_IO_HERBCEPTIONS_THROWS_IF(!noexcept(t / u))
  {
    return t / u;
  }
};

struct modulo
{
  template<typename T, typename U>
  inline constexpr auto compute(T const &t, U const &u) const
    FAST_IO_HERBCEPTIONS_THROWS_IF(!noexcept(t % u))
  {
    return t % u;
  }
};

struct negate
{
  template<typename T>
  inline constexpr auto compute(T const &t) const
    FAST_IO_HERBCEPTIONS_THROWS_IF(!noexcept(-t))
  {
    return -t;
  }
};

struct bit_and
{
  template<typename T, typename U>
  inline constexpr auto compute(T const &t, U const &u) const
    FAST_IO_HERBCEPTIONS_THROWS_IF(!noexcept(t & u))
  {
    return t & u;
  }
};

struct bit_or
{
  template<typename T, typename U>
  inline constexpr auto compute(T const &t, U const &u) const
    FAST_IO_HERBCEPTIONS_THROWS_IF(!noexcept(t | u))
  {
    return t | u;
  }
};

struct bit_xor
{
  template<typename T, typename U>
  inline constexpr auto compute(T const &t, U const &u) const
    FAST_IO_HERBCEPTIONS_THROWS_IF(!noexcept(t ^ u))
  {
    return t ^ u;
  }
};

struct bit_not
{
  template<typename T>
  inline constexpr auto compute(T const &t) const
    FAST_IO_HERBCEPTIONS_THROWS_IF(!noexcept(~t))
  {
    return ~t;
  }
};

struct logical_and
{
  template<typename T, typename U>
  inline constexpr bool compute(T const &t, U const &u) const
    FAST_IO_HERBCEPTIONS_THROWS_IF(!noexcept(static_cast<bool>(t) && static_cast<bool>(u)))
  {
    return static_cast<bool>(t) && static_cast<bool>(u);
  }
};

struct logical_or
{
  template<typename T, typename U>
  inline constexpr bool compute(T const &t, U const &u) const
    FAST_IO_HERBCEPTIONS_THROWS_IF(!noexcept(static_cast<bool>(t) || static_cast<bool>(u)))
  {
    return static_cast<bool>(t) || static_cast<bool>(u);
  }
};

struct logical_not
{
  template<typename T>
  inline constexpr bool compute(T const &t) const
    FAST_IO_HERBCEPTIONS_THROWS_IF(!noexcept(static_cast<bool>(t)))
  {
    return !static_cast<bool>(t);
  }
};

// Value-returning minimum/maximum. The scalar `min` / `max` helpers are not
// provided because they return references; these objects return a value and
// are meant for folds such as `reduce`.
struct minimum
{
  template<typename T, typename U>
  inline constexpr auto compute(T const &t, U const &u) const
    FAST_IO_HERBCEPTIONS_THROWS_IF(!noexcept(t < u ? t : u))
  {
    return t < u ? t : u;
  }
};

struct maximum
{
  template<typename T, typename U>
  inline constexpr auto compute(T const &t, U const &u) const
    FAST_IO_HERBCEPTIONS_THROWS_IF(!noexcept(t < u ? u : t))
  {
    return t < u ? u : t;
  }
};

} // namespace fast_io::algorithms::field_ops
