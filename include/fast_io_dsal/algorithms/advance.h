#pragma once

// Iterator advance, mirroring `std::ranges::advance(it, n)`.
//
// Advance is in place: the algorithm does not own the iterator it changes.
// Random-access iterators are advanced with `it += n`; other iterators walk
// one step at a time. A negative count is supported for bidirectional
// iterators and is a contract violation otherwise.

namespace fast_io::details::algorithms
{

template<typename Iter>
concept advance_iters_no_throw =
  ::fast_io::details::algorithms::iter_no_throw<Iter> &&
  (::std::random_access_iterator<Iter> ||
   !::std::bidirectional_iterator<Iter> ||
   ::fast_io::details::algorithms::decrement_iter_no_throw<Iter>);

template<typename Iter>
inline constexpr void advance_iters_impl(
  Iter &it, ::std::iter_difference_t<Iter> n)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::advance_iters_no_throw<Iter>)
{
  if constexpr (::std::random_access_iterator<Iter>)
  {
    it += n;
  }
  else if (n >= 0)
  {
    for (; n != 0; --n)
    {
      ++it;
    }
  }
  else if constexpr (::std::bidirectional_iterator<Iter>)
  {
    for (; n != 0; ++n)
    {
      --it;
    }
  }
  else
  {
    __builtin_trap();
  }
}

} // namespace fast_io::details::algorithms

namespace fast_io::algorithms
{

template<::std::input_iterator Iter>
inline constexpr void advance_iters(
  Iter &it, ::std::iter_difference_t<Iter> n)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::advance_iters_no_throw<Iter>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(it)>),
    advance_iters_define, it, n);
  ::fast_io::details::algorithms::advance_iters_impl(it, n);
}

} // namespace fast_io::algorithms
