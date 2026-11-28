#pragma once

namespace fast_io::details::algorithms
{

// This follows the decision order of libc++'s `std::ranges::swap`:
//   1. an unqualified `swap` found by ADL when an operand is a class or enum,
//   2. element-wise swap for lvalue arrays of equal extent,
//   3. the move-based exchange for lvalues of the same movable type.
// Unlike a set of overloads, one function template selects the branch with
// `if constexpr`; the branch is recorded in `swap_kind_v` so the `throws`
// condition is the one belonging to the branch that is actually taken.

enum class swap_kind
{
  none,
  adl,
  array,
  exchange
};

namespace swap_detail
{

template<typename T>
concept class_or_enum =
  ::std::is_class_v<::std::remove_cvref_t<T>> ||
  ::std::is_union_v<::std::remove_cvref_t<T>> ||
  ::std::is_enum_v<::std::remove_cvref_t<T>>;

template<typename T, typename U>
consteval bool array_shape()
{
  if constexpr (::std::is_lvalue_reference_v<T> &&
                ::std::is_lvalue_reference_v<U>)
  {
    using array_t = ::std::remove_reference_t<T>;
    using array_u = ::std::remove_reference_t<U>;
    if constexpr (::std::is_array_v<array_t> && ::std::is_array_v<array_u>)
    {
      return ::std::extent_v<array_t> == ::std::extent_v<array_u>;
    }
    else
    {
      return false;
    }
  }
  else
  {
    return false;
  }
}

template<typename T, typename U>
consteval bool exchangeable()
{
  using value_t = ::std::remove_cvref_t<T>;
  using value_u = ::std::remove_cvref_t<U>;
  return ::std::is_lvalue_reference_v<T> &&
         ::std::is_lvalue_reference_v<U> &&
         ::std::same_as<value_t, value_u> &&
         ::std::move_constructible<value_t> &&
         ::std::assignable_from<value_t &, value_t>;
}

} // namespace swap_detail

// The zero-argument `swap` makes an unqualified lookup consider ADL
// candidates only. libc++ uses a deleted template for the same purpose.
namespace swap_adl
{
template<typename T>
void swap(T &, T &) = delete;

template<typename T, typename U>
concept swappable = requires(T &&t, U &&u) {
  swap(static_cast<T &&>(t), static_cast<U &&>(u));
};

template<typename T, typename U>
concept nothrow_swappable = requires(T &&t, U &&u) {
  { swap(static_cast<T &&>(t), static_cast<U &&>(u)) } noexcept;
};

template<typename T, typename U>
inline constexpr void adl_swap(T &&t, U &&u)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::swap_adl::nothrow_swappable<T, U>)
{
  swap(static_cast<T &&>(t), static_cast<U &&>(u));
}
} // namespace swap_adl

template<typename T, typename U>
consteval bool swap_adl_viable()
{
  if constexpr (::fast_io::details::algorithms::swap_detail::class_or_enum<T> ||
                ::fast_io::details::algorithms::swap_detail::class_or_enum<U>)
  {
    return ::fast_io::details::algorithms::swap_adl::swappable<T, U>;
  }
  else
  {
    return false;
  }
}

template<typename T, typename U>
consteval swap_kind swap_kind_of()
{
  if constexpr (::fast_io::details::algorithms::swap_adl_viable<T, U>())
  {
    return ::fast_io::details::algorithms::swap_kind::adl;
  }
  else if constexpr (::fast_io::details::algorithms::swap_detail::array_shape<T, U>())
  {
    return ::fast_io::details::algorithms::swap_kind::array;
  }
  else if constexpr (::fast_io::details::algorithms::swap_detail::exchangeable<T, U>())
  {
    return ::fast_io::details::algorithms::swap_kind::exchange;
  }
  else
  {
    return ::fast_io::details::algorithms::swap_kind::none;
  }
}

template<typename T, typename U>
inline constexpr swap_kind swap_kind_v{
  ::fast_io::details::algorithms::swap_kind_of<T, U>()};

template<typename T, typename U>
consteval bool swap_nothrow()
{
  constexpr ::fast_io::details::algorithms::swap_kind kind{
    ::fast_io::details::algorithms::swap_kind_v<T, U>};
  if constexpr (kind == ::fast_io::details::algorithms::swap_kind::adl)
  {
    return ::fast_io::details::algorithms::swap_adl::nothrow_swappable<T, U>;
  }
  else if constexpr (kind == ::fast_io::details::algorithms::swap_kind::array)
  {
    using array_t = ::std::remove_reference_t<T>;
    using array_u = ::std::remove_reference_t<U>;
    return ::fast_io::details::algorithms::swap_nothrow<
      ::std::remove_extent_t<array_t> &,
      ::std::remove_extent_t<array_u> &>();
  }
  else if constexpr (kind == ::fast_io::details::algorithms::swap_kind::exchange)
  {
    return ::fast_io::details::algorithms::move_no_throw<
      ::std::remove_cvref_t<T>>;
  }
  else
  {
    return false;
  }
}

// One-argument condition used by algorithm families that exchange a pair of
// values of a known element type.
template<typename T>
inline constexpr bool swap_no_throw_v{
  ::fast_io::details::algorithms::swap_nothrow<T &, T &>()};

} // namespace fast_io::details::algorithms

namespace fast_io::algorithms
{

// Exchange two values, preferring the type's own ADL swap, then element-wise
// swapping for arrays, then the move-based exchange. Rvalues are rejected for
// the move-based branch, as in `std::ranges::swap`.
template<typename T, typename U>
  requires (::fast_io::details::algorithms::swap_kind_v<T, U> !=
            ::fast_io::details::algorithms::swap_kind::none)
inline constexpr void swap(T &&a, U &&b)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::swap_nothrow<T, U>())
{
  FAST_IO_ALGORITHMS_CUSTOMIZE(
    swap_define, a, b);
  constexpr ::fast_io::details::algorithms::swap_kind kind{
    ::fast_io::details::algorithms::swap_kind_v<T, U>};
  if constexpr (kind == ::fast_io::details::algorithms::swap_kind::adl)
  {
    ::fast_io::details::algorithms::swap_adl::adl_swap(
      static_cast<T &&>(a), static_cast<U &&>(b));
  }
  else if constexpr (kind == ::fast_io::details::algorithms::swap_kind::array)
  {
    constexpr ::std::size_t extent{
      ::std::extent_v<::std::remove_reference_t<T>>};
    for (::std::size_t i{}; i != extent; ++i)
    {
      ::fast_io::algorithms::swap(a[i], b[i]);
    }
  }
  else
  {
    ::std::remove_cvref_t<T> temp{::std::move(a)};
    a = ::std::move(b);
    b = ::std::move(temp);
  }
}

// Exchange the values referred to by two iterators through `swap` above, so
// the element type's own swap is preferred here as well.
template<::std::indirectly_readable Iter1, ::std::indirectly_readable Iter2>
  requires (::fast_io::details::algorithms::swap_kind_v<
              ::std::iter_reference_t<Iter1>,
              ::std::iter_reference_t<Iter2>> !=
            ::fast_io::details::algorithms::swap_kind::none)
FAST_IO_UNSAFE_API
inline constexpr void iter_swap(Iter1 a, Iter2 b)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::swap_nothrow<
      ::std::iter_reference_t<Iter1>, ::std::iter_reference_t<Iter2>>())
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(a), decltype(b)>),
    iter_swap_define, a, b);
  ::fast_io::algorithms::swap(*a, *b);
}

} // namespace fast_io::algorithms
