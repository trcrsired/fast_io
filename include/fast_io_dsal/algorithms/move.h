#pragma once

namespace fast_io::details::algorithms
{

template<typename Iter, typename Snt, typename OIter>
concept move_iters_no_throw =
  ::fast_io::details::algorithms::sentinel_no_throw<Iter, Snt> &&
  ::fast_io::details::algorithms::output_iter_no_throw<
    OIter, ::std::iter_value_t<Iter>> &&
  requires(Iter &first, OIter &dest)
{
  { *dest = ::std::move(*first) } noexcept;
};

template<typename Iter, typename Snt, typename OIter>
concept move_backward_iters_no_throw =
  ::fast_io::details::algorithms::sentinel_no_throw<Iter, Snt> &&
  ::fast_io::details::algorithms::decrement_iter_no_throw<Iter> &&
  ::fast_io::details::algorithms::decrement_iter_no_throw<OIter> &&
  ::fast_io::details::algorithms::output_iter_no_throw<
    OIter, ::std::iter_value_t<Iter>> &&
  requires(Iter &last, OIter &d_last)
{
  { *d_last = ::std::move(*last) } noexcept;
};

template<typename Iter, typename Snt, typename OIter>
inline constexpr OIter move_iters_impl(Iter first, Snt last, OIter dest)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::move_iters_no_throw<
      Iter, Snt, OIter>)
{
  for (; first != last; ++first, ++dest)
  {
    *dest = ::std::move(*first);
  }
  return dest;
}

template<typename Iter, typename Snt, typename OIter>
inline constexpr OIter move_backward_iters_impl(
  Iter first, Snt last, OIter d_last)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::move_backward_iters_no_throw<
      Iter, Snt, OIter>)
{
  Iter last_iter{first};
  for (; last_iter != last; ++last_iter)
  {
  }
  while (last_iter != first)
  {
    --last_iter;
    --d_last;
    *d_last = ::std::move(*last_iter);
  }
  return d_last;
}

} // namespace fast_io::details::algorithms

namespace fast_io::algorithms
{

// Move elements from `[first, last)` to `dest`, which must not overlap the
// source in a way that would clobber unread elements.
template<::std::input_iterator Iter, ::std::sentinel_for<Iter> Snt,
  typename OIter>
inline constexpr OIter move_iters(Iter first, Snt last, OIter dest)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::move_iters_no_throw<
      Iter, Snt, OIter>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first), decltype(dest)>),
    move_iters_define, first, last, dest);
  return ::fast_io::details::algorithms::move_iters_impl(
    first, last, dest);
}

template<::std::ranges::input_range R, typename OIter>
inline constexpr OIter move(R &&r, OIter dest)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::move_iters_no_throw<
      ::std::ranges::iterator_t<R>, ::std::ranges::sentinel_t<R>,
      OIter>)
{
  return ::fast_io::algorithms::move_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), dest);
}

// Move elements backwards into a destination whose end is `d_last`.
template<::std::bidirectional_iterator Iter, ::std::sentinel_for<Iter> Snt,
  ::std::bidirectional_iterator OIter>
inline constexpr OIter move_backward_iters(
  Iter first, Snt last, OIter d_last)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::move_backward_iters_no_throw<
      Iter, Snt, OIter>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first), decltype(d_last)>),
    move_backward_iters_define, first, last, d_last);
  return ::fast_io::details::algorithms::move_backward_iters_impl(
    first, last, d_last);
}

template<::std::ranges::bidirectional_range R,
  ::std::bidirectional_iterator OIter>
inline constexpr OIter move_backward(R &&r, OIter d_last)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::move_backward_iters_no_throw<
      ::std::ranges::iterator_t<R>, ::std::ranges::sentinel_t<R>,
      OIter>)
{
  return ::fast_io::algorithms::move_backward_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), d_last);
}

} // namespace fast_io::algorithms
