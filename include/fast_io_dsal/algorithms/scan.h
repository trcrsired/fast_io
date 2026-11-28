#pragma once

namespace fast_io::details::algorithms
{

// Inclusive scan has the same contract as partial_sum: the first element seeds
// the fold and is written unchanged.
template<typename Iter, typename OIter, typename Op>
concept inclusive_scan_iter_no_throw =
  ::fast_io::details::algorithms::partial_sum_iter_no_throw<Iter, OIter, Op>;

template<typename Iter, typename Snt, typename OIter, typename Op>
concept inclusive_scan_iters_no_throw =
  ::fast_io::details::algorithms::partial_sum_iters_no_throw<
    Iter, Snt, OIter, Op>;

template<typename R, typename OIter, typename Op>
concept inclusive_scan_ranges_no_throw =
  ::fast_io::details::algorithms::partial_sum_ranges_no_throw<R, OIter, Op>;

template<typename R, typename OIter, typename Op>
concept inclusive_scan_index_range_no_throw =
  ::fast_io::details::algorithms::partial_sum_index_range_no_throw<
    R, OIter, Op>;

template<typename Iter, typename OIter, typename T, typename Op>
concept exclusive_scan_iter_no_throw =
  ::fast_io::details::algorithms::iter_no_throw<Iter> &&
  ::fast_io::details::algorithms::output_iter_no_throw<OIter, T> &&
  ::std::is_nothrow_move_constructible_v<T> &&
  ::std::is_nothrow_move_assignable_v<T> &&
  ::std::is_nothrow_constructible_v<T, decltype(
    ::std::declval<Op &>().compute(
      ::std::declval<T const &>(), *::std::declval<Iter &>()))> &&
  requires(Iter &first, OIter &dest, T &acc, Op &op)
{
  { *dest = ::std::move(acc) } noexcept;
  { acc = op.compute(acc, *first) } noexcept;
};

template<typename Iter, typename Snt, typename OIter, typename T, typename Op>
concept exclusive_scan_iters_no_throw =
  ::fast_io::details::algorithms::sentinel_no_throw<Iter, Snt> &&
  ::fast_io::details::algorithms::exclusive_scan_iter_no_throw<
    Iter, OIter, T, Op>;

template<typename R, typename OIter, typename T, typename Op>
concept exclusive_scan_ranges_no_throw =
  ::std::ranges::input_range<R> &&
  ::std::is_nothrow_move_constructible_v<T> &&
  ::std::is_nothrow_move_constructible_v<Op> &&
  ::fast_io::details::algorithms::exclusive_scan_iter_no_throw<
    ::std::ranges::iterator_t<R>, OIter, T, Op>;

template<typename R, typename OIter, typename T, typename Op>
concept exclusive_scan_index_range_no_throw =
  ::std::ranges::random_access_range<R> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R> &&
  ::std::is_nothrow_move_constructible_v<T> &&
  ::std::is_nothrow_move_constructible_v<Op> &&
  ::fast_io::details::algorithms::exclusive_scan_iter_no_throw<
    ::fast_io::details::algorithms::algo_index_iterator<R>, OIter, T, Op>;

template<typename Iter, typename Snt, typename OIter, typename T, typename Op>
FAST_IO_UNSAFE_API
inline constexpr OIter exclusive_scan_iters_impl(
  Iter first, Snt last, OIter dest, T init, Op &op)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::exclusive_scan_iters_no_throw<
      Iter, Snt, OIter, T, Op>)
{
  if (first == last)
  {
    return dest;
  }
  T next{op.compute(init, *first)};
  while (true)
  {
    *dest = ::std::move(init);
    ++dest;
    ++first;
    if (first == last)
    {
      break;
    }
    init = ::std::move(next);
    next = op.compute(init, *first);
  }
  return dest;
}

} // namespace fast_io::details::algorithms

namespace fast_io::algorithms
{

// Inclusive prefix fold: each output element is the fold of the input prefix
// ending at that element. The first element seeds the fold.
template<::std::input_iterator Iter, ::std::sentinel_for<Iter> Snt,
  typename OIter, typename Op = ::fast_io::algorithms::field_ops::add>
FAST_IO_UNSAFE_API
inline constexpr OIter inclusive_scan_iters(
  Iter first, Snt last, OIter dest, Op op = Op{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !(::fast_io::details::algorithms::inclusive_scan_iters_no_throw<
        Iter, Snt, OIter, Op> &&
      ::fast_io::details::algorithms::operation_pass_no_throw<Op>))
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<
      decltype(first), decltype(dest)>),
    inclusive_scan_iters_define, first, last, dest, op);
  return ::fast_io::details::algorithms::partial_sum_iters_impl(
    first, last, dest, op);
}

template<::std::ranges::input_range R, typename OIter,
  typename Op = ::fast_io::algorithms::field_ops::add>
FAST_IO_UNSAFE_API
inline constexpr OIter inclusive_scan(R &&r, OIter dest, Op op = Op{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !(::fast_io::details::algorithms::inclusive_scan_ranges_no_throw<
        R, OIter, Op> &&
      ::fast_io::details::algorithms::operation_pass_no_throw<Op>))
{
  return ::fast_io::algorithms::inclusive_scan_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), dest, op);
}

template<::std::ranges::random_access_range R, typename OIter,
  typename Op = ::fast_io::algorithms::field_ops::add>
FAST_IO_UNSAFE_API
inline constexpr OIter inclusive_scan_index_range(
  R &&r, ::std::size_t firstidx, ::std::size_t lastidx, OIter dest,
  Op op = Op{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !(::fast_io::details::algorithms::inclusive_scan_index_range_no_throw<
        R, OIter, Op> &&
      ::fast_io::details::algorithms::operation_pass_no_throw<Op>))
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R>(r), firstidx, lastidx);
  return ::fast_io::algorithms::inclusive_scan_iters(first, last, dest, op);
}

// Exclusive prefix fold: the first output is `init`, and each later output is
// the fold of `init` and the preceding input elements. The output range must
// not overlap the input range.
template<::std::input_iterator Iter, ::std::sentinel_for<Iter> Snt,
  typename OIter, typename T,
  typename Op = ::fast_io::algorithms::field_ops::add>
FAST_IO_UNSAFE_API
inline constexpr OIter exclusive_scan_iters(
  Iter first, Snt last, OIter dest, T init, Op op = Op{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !(::fast_io::details::algorithms::exclusive_scan_iters_no_throw<
        Iter, Snt, OIter, T, Op> &&
      ::fast_io::details::algorithms::operation_pass_no_throw<Op>))
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<
      decltype(first), decltype(dest)>),
    exclusive_scan_iters_define, first, last, dest, init, op);
  return ::fast_io::details::algorithms::exclusive_scan_iters_impl(
    first, last, dest, init, op);
}

template<::std::ranges::input_range R, typename OIter, typename T,
  typename Op = ::fast_io::algorithms::field_ops::add>
FAST_IO_UNSAFE_API
inline constexpr OIter exclusive_scan(
  R &&r, OIter dest, T init, Op op = Op{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !(::fast_io::details::algorithms::exclusive_scan_ranges_no_throw<
        R, OIter, T, Op> &&
      ::fast_io::details::algorithms::operation_pass_no_throw<Op>))
{
  return ::fast_io::algorithms::exclusive_scan_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), dest, init, op);
}

template<::std::ranges::random_access_range R, typename OIter, typename T,
  typename Op = ::fast_io::algorithms::field_ops::add>
FAST_IO_UNSAFE_API
inline constexpr OIter exclusive_scan_index_range(
  R &&r, ::std::size_t firstidx, ::std::size_t lastidx, OIter dest,
  T init, Op op = Op{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !(::fast_io::details::algorithms::exclusive_scan_index_range_no_throw<
        R, OIter, T, Op> &&
      ::fast_io::details::algorithms::operation_pass_no_throw<Op>))
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R>(r), firstidx, lastidx);
  return ::fast_io::algorithms::exclusive_scan_iters(
    first, last, dest, init, op);
}

} // namespace fast_io::algorithms
