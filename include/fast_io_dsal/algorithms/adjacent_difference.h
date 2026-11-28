#pragma once

namespace fast_io::details::algorithms
{

template<typename Iter, typename OIter, typename Op>
concept adjacent_difference_iter_no_throw =
  ::fast_io::details::algorithms::iter_no_throw<Iter> &&
  ::fast_io::details::algorithms::output_iter_no_throw<
    OIter, ::std::iter_value_t<Iter>> &&
  ::std::is_nothrow_constructible_v<
    ::std::iter_value_t<Iter>, ::std::iter_reference_t<Iter>> &&
  ::std::is_nothrow_move_assignable_v<::std::iter_value_t<Iter>> &&
  requires(Iter &first, OIter &dest,
    ::std::iter_value_t<Iter> &previous, Op &op)
{
  { *dest = previous } noexcept;
  { *dest = op.compute(*first, previous) } noexcept;
};

template<typename Iter, typename Snt, typename OIter, typename Op>
concept adjacent_difference_iters_no_throw =
  ::fast_io::details::algorithms::sentinel_no_throw<Iter, Snt> &&
  ::fast_io::details::algorithms::adjacent_difference_iter_no_throw<
    Iter, OIter, Op>;

template<typename R, typename OIter, typename Op>
concept adjacent_difference_ranges_no_throw =
  ::std::ranges::input_range<R> &&
  ::std::is_nothrow_move_constructible_v<Op> &&
  ::fast_io::details::algorithms::adjacent_difference_iter_no_throw<
    ::std::ranges::iterator_t<R>, OIter, Op>;

template<typename R, typename OIter, typename Op>
concept adjacent_difference_index_range_no_throw =
  ::std::ranges::random_access_range<R> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R> &&
  ::std::is_nothrow_move_constructible_v<Op> &&
  ::fast_io::details::algorithms::adjacent_difference_iter_no_throw<
    ::fast_io::details::algorithms::algo_index_iterator<R>, OIter, Op>;

template<typename Iter, typename Snt, typename OIter, typename Op>
FAST_IO_UNSAFE_API
inline constexpr OIter adjacent_difference_iters_impl(
  Iter first, Snt last, OIter dest, Op &op)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::adjacent_difference_iters_no_throw<
      Iter, Snt, OIter, Op>)
{
  if (first == last)
  {
    return dest;
  }
  ::std::iter_value_t<Iter> previous{*first};
  *dest = previous;
  for (++first; first != last; ++first)
  {
    ::std::iter_value_t<Iter> current{*first};
    ++dest;
    *dest = op.compute(current, previous);
    previous = current;
  }
  return ++dest;
}

} // namespace fast_io::details::algorithms

namespace fast_io::algorithms
{

// Write `*first` to `*dest` and `op.compute(current, previous)` for every later
// element. `dest` may equal `first`; the previous value is cached before the
// write, so the in-place form is well-defined.
template<::std::input_iterator Iter, ::std::sentinel_for<Iter> Snt,
  typename OIter, typename Op = ::fast_io::algorithms::field_ops::minus>
FAST_IO_UNSAFE_API
inline constexpr OIter adjacent_difference_iters(
  Iter first, Snt last, OIter dest, Op op = Op{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !(::fast_io::details::algorithms::adjacent_difference_iters_no_throw<
        Iter, Snt, OIter, Op> &&
      ::fast_io::details::algorithms::operation_pass_no_throw<Op>))
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<
      decltype(first), decltype(dest)>),
    adjacent_difference_iters_define, first, last, dest, op);
  return ::fast_io::details::algorithms::adjacent_difference_iters_impl(
    first, last, dest, op);
}

template<::std::ranges::input_range R, typename OIter,
  typename Op = ::fast_io::algorithms::field_ops::minus>
FAST_IO_UNSAFE_API
inline constexpr OIter adjacent_difference(
  R &&r, OIter dest, Op op = Op{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !(::fast_io::details::algorithms::adjacent_difference_ranges_no_throw<
        R, OIter, Op> &&
      ::fast_io::details::algorithms::operation_pass_no_throw<Op>))
{
  return ::fast_io::algorithms::adjacent_difference_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), dest, op);
}

template<::std::ranges::random_access_range R, typename OIter,
  typename Op = ::fast_io::algorithms::field_ops::minus>
FAST_IO_UNSAFE_API
inline constexpr OIter adjacent_difference_index_range(
  R &&r, ::std::size_t firstidx, ::std::size_t lastidx, OIter dest,
  Op op = Op{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !(::fast_io::details::algorithms::adjacent_difference_index_range_no_throw<
        R, OIter, Op> &&
      ::fast_io::details::algorithms::operation_pass_no_throw<Op>))
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R>(r), firstidx, lastidx);
  return ::fast_io::algorithms::adjacent_difference_iters(
    first, last, dest, op);
}

} // namespace fast_io::algorithms
