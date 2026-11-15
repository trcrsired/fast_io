#pragma once

namespace fast_io::details::algorithms
{

template<typename Iter, typename T, typename Op>
concept reduce_iter_no_throw =
  ::fast_io::details::algorithms::iter_no_throw<Iter> &&
  ::std::is_nothrow_move_constructible_v<T> &&
  requires(Iter &first, T &acc, Op &op)
{
  { acc = op.compute(acc, *first) } noexcept;
};

template<typename Iter, typename Snt, typename T, typename Op>
concept reduce_iters_no_throw =
  ::fast_io::details::algorithms::sentinel_no_throw<Iter, Snt> &&
  ::fast_io::details::algorithms::reduce_iter_no_throw<Iter, T, Op>;

template<typename R, typename T, typename Op>
concept reduce_ranges_no_throw =
  ::std::ranges::input_range<R> &&
  ::std::is_nothrow_move_constructible_v<T> &&
  ::std::is_nothrow_move_constructible_v<Op> &&
  ::fast_io::details::algorithms::reduce_iter_no_throw<
    ::std::ranges::iterator_t<R>, T, Op>;

template<typename R, typename T, typename Op>
concept reduce_index_range_no_throw =
  ::std::ranges::random_access_range<R> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R> &&
  ::std::is_nothrow_move_constructible_v<T> &&
  ::std::is_nothrow_move_constructible_v<Op> &&
  ::fast_io::details::algorithms::reduce_iter_no_throw<
    ::fast_io::details::algorithms::algo_index_iterator<R>, T, Op>;

template<typename Iter, typename Snt, typename T, typename Op>
inline constexpr T reduce_iters_impl(Iter first, Snt last, T init, Op &op)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::reduce_iters_no_throw<
      Iter, Snt, T, Op>)
{
  for (; first != last; ++first)
  {
    init = op.compute(init, *first);
  }
  return init;
}

} // namespace fast_io::details::algorithms

namespace fast_io::algorithms
{

// Left fold over `[first, last)`, starting from `init`.
//
// `op` is a named object with a `compute(acc, element)` member, never a
// closure. The implementation folds strictly left to right, which is a valid
// grouping for the relaxed `reduce` contract, so `reduce` and `accumulate`
// currently produce identical results.
template<::std::input_iterator Iter, ::std::sentinel_for<Iter> Snt,
  typename T, typename Op = ::fast_io::algorithms::field_ops::add>
inline constexpr T reduce_iters(
  Iter first, Snt last, T init, Op op = Op{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !(::fast_io::details::algorithms::reduce_iters_no_throw<
        Iter, Snt, T, Op> &&
      ::fast_io::details::algorithms::operation_pass_no_throw<Op>))
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first)>),
    reduce_iters_define, first, last, init, op);
  return ::fast_io::details::algorithms::reduce_iters_impl(
    first, last, init, op);
}

template<::std::ranges::input_range R, typename T,
  typename Op = ::fast_io::algorithms::field_ops::add>
inline constexpr T reduce(R &&r, T init, Op op = Op{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !(::fast_io::details::algorithms::reduce_ranges_no_throw<R, T, Op> &&
      ::fast_io::details::algorithms::operation_pass_no_throw<Op>))
{
  return ::fast_io::algorithms::reduce_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), init, op);
}

template<::std::ranges::random_access_range R, typename T,
  typename Op = ::fast_io::algorithms::field_ops::add>
inline constexpr T reduce_index_range(
  R &&r, ::std::size_t firstidx, ::std::size_t lastidx, T init,
  Op op = Op{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !(::fast_io::details::algorithms::reduce_index_range_no_throw<R, T, Op> &&
      ::fast_io::details::algorithms::operation_pass_no_throw<Op>))
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R>(r), firstidx, lastidx);
  return ::fast_io::algorithms::reduce_iters(first, last, init, op);
}

// Strict left fold, same signature and same implementation as `reduce`.
template<::std::input_iterator Iter, ::std::sentinel_for<Iter> Snt,
  typename T, typename Op = ::fast_io::algorithms::field_ops::add>
inline constexpr T accumulate_iters(
  Iter first, Snt last, T init, Op op = Op{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !(::fast_io::details::algorithms::reduce_iters_no_throw<
        Iter, Snt, T, Op> &&
      ::fast_io::details::algorithms::operation_pass_no_throw<Op>))
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first)>),
    accumulate_iters_define, first, last, init, op);
  return ::fast_io::details::algorithms::reduce_iters_impl(
    first, last, init, op);
}

template<::std::ranges::input_range R, typename T,
  typename Op = ::fast_io::algorithms::field_ops::add>
inline constexpr T accumulate(R &&r, T init, Op op = Op{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !(::fast_io::details::algorithms::reduce_ranges_no_throw<R, T, Op> &&
      ::fast_io::details::algorithms::operation_pass_no_throw<Op>))
{
  return ::fast_io::algorithms::accumulate_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), init, op);
}

template<::std::ranges::random_access_range R, typename T,
  typename Op = ::fast_io::algorithms::field_ops::add>
inline constexpr T accumulate_index_range(
  R &&r, ::std::size_t firstidx, ::std::size_t lastidx, T init,
  Op op = Op{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !(::fast_io::details::algorithms::reduce_index_range_no_throw<R, T, Op> &&
      ::fast_io::details::algorithms::operation_pass_no_throw<Op>))
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R>(r), firstidx, lastidx);
  return ::fast_io::algorithms::accumulate_iters(first, last, init, op);
}

} // namespace fast_io::algorithms
