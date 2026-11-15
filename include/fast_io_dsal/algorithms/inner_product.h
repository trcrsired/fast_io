#pragma once

namespace fast_io::details::algorithms
{

template<typename Iter1, typename Iter2, typename T, typename Op1,
  typename Op2>
concept inner_product_iter_no_throw =
  ::fast_io::details::algorithms::iter_no_throw<Iter1> &&
  ::fast_io::details::algorithms::iter_no_throw<Iter2> &&
  ::std::is_nothrow_move_constructible_v<T> &&
  requires(Iter1 &first1, Iter2 &first2, T &acc, Op1 &op1, Op2 &op2)
{
  { acc = op1.compute(acc, op2.compute(*first1, *first2)) } noexcept;
};

template<typename Iter1, typename Snt1, typename Iter2, typename Snt2,
  typename T, typename Op1, typename Op2>
concept inner_product_iters_no_throw =
  ::fast_io::details::algorithms::sentinel_no_throw<Iter1, Snt1> &&
  ::fast_io::details::algorithms::sentinel_no_throw<Iter2, Snt2> &&
  ::fast_io::details::algorithms::inner_product_iter_no_throw<
    Iter1, Iter2, T, Op1, Op2>;

template<typename R1, typename R2, typename T, typename Op1, typename Op2>
concept inner_product_ranges_no_throw =
  ::std::ranges::input_range<R1> &&
  ::std::ranges::input_range<R2> &&
  ::std::is_nothrow_move_constructible_v<T> &&
  ::std::is_nothrow_move_constructible_v<Op1> &&
  ::std::is_nothrow_move_constructible_v<Op2> &&
  ::fast_io::details::algorithms::inner_product_iter_no_throw<
    ::std::ranges::iterator_t<R1>, ::std::ranges::iterator_t<R2>,
    T, Op1, Op2>;

template<typename R1, typename R2, typename T, typename Op1, typename Op2>
concept inner_product_index_range_no_throw =
  ::std::ranges::random_access_range<R1> &&
  ::std::ranges::random_access_range<R2> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R1> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R2> &&
  ::std::is_nothrow_move_constructible_v<T> &&
  ::std::is_nothrow_move_constructible_v<Op1> &&
  ::std::is_nothrow_move_constructible_v<Op2> &&
  ::fast_io::details::algorithms::inner_product_iter_no_throw<
    ::fast_io::details::algorithms::algo_index_iterator<R1>,
    ::fast_io::details::algorithms::algo_index_iterator<R2>,
    T, Op1, Op2>;

template<typename Iter1, typename Snt1, typename Iter2, typename Snt2,
  typename T, typename Op1, typename Op2>
inline constexpr T inner_product_iters_impl(
  Iter1 first1, Snt1 last1, Iter2 first2, Snt2 last2, T init,
  Op1 &op1, Op2 &op2)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::inner_product_iters_no_throw<
      Iter1, Snt1, Iter2, Snt2, T, Op1, Op2>)
{
  for (; first1 != last1 && first2 != last2; ++first1, ++first2)
  {
    init = op1.compute(init, op2.compute(*first1, *first2));
  }
  return init;
}

} // namespace fast_io::details::algorithms

namespace fast_io::algorithms
{

// Generalized inner product: `init = op1(init, op2(*first1, *first2))`.
//
// Unlike the standard form only the first range is bounded by an end iterator;
// both ends are passed here, so the scan stops at whichever range ends first
// and never reads past either range.
template<::std::input_iterator Iter1, ::std::sentinel_for<Iter1> Snt1,
  ::std::input_iterator Iter2, ::std::sentinel_for<Iter2> Snt2,
  typename T, typename Op1 = ::fast_io::algorithms::field_ops::add,
  typename Op2 = ::fast_io::algorithms::field_ops::multiply>
inline constexpr T inner_product_iters(
  Iter1 first1, Snt1 last1, Iter2 first2, Snt2 last2, T init,
  Op1 op1 = Op1{}, Op2 op2 = Op2{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !(::fast_io::details::algorithms::inner_product_iters_no_throw<
        Iter1, Snt1, Iter2, Snt2, T, Op1, Op2> &&
      ::fast_io::details::algorithms::operation_pass_no_throw<Op1> &&
      ::fast_io::details::algorithms::operation_pass_no_throw<Op2>))
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<
      decltype(first1), decltype(first2)>),
    inner_product_iters_define, first1, last1, first2, last2, init, op1, op2);
  return ::fast_io::details::algorithms::inner_product_iters_impl(
    first1, last1, first2, last2, init, op1, op2);
}

template<::std::ranges::input_range R1, ::std::ranges::input_range R2,
  typename T, typename Op1 = ::fast_io::algorithms::field_ops::add,
  typename Op2 = ::fast_io::algorithms::field_ops::multiply>
inline constexpr T inner_product(
  R1 &&r1, R2 &&r2, T init, Op1 op1 = Op1{}, Op2 op2 = Op2{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !(::fast_io::details::algorithms::inner_product_ranges_no_throw<
        R1, R2, T, Op1, Op2> &&
      ::fast_io::details::algorithms::operation_pass_no_throw<Op1> &&
      ::fast_io::details::algorithms::operation_pass_no_throw<Op2>))
{
  return ::fast_io::algorithms::inner_product_iters(
    ::std::ranges::begin(r1), ::std::ranges::end(r1),
    ::std::ranges::begin(r2), ::std::ranges::end(r2), init, op1, op2);
}

template<::std::ranges::random_access_range R1,
  ::std::ranges::random_access_range R2, typename T,
  typename Op1 = ::fast_io::algorithms::field_ops::add,
  typename Op2 = ::fast_io::algorithms::field_ops::multiply>
inline constexpr T inner_product_index_range(
  R1 &&r1, ::std::size_t firstidx1, ::std::size_t lastidx1,
  R2 &&r2, ::std::size_t firstidx2, ::std::size_t lastidx2,
  T init, Op1 op1 = Op1{}, Op2 op2 = Op2{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !(::fast_io::details::algorithms::inner_product_index_range_no_throw<
        R1, R2, T, Op1, Op2> &&
      ::fast_io::details::algorithms::operation_pass_no_throw<Op1> &&
      ::fast_io::details::algorithms::operation_pass_no_throw<Op2>))
{
  auto [first1, last1] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R1>(r1), firstidx1, lastidx1);
  auto [first2, last2] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R2>(r2), firstidx2, lastidx2);
  return ::fast_io::algorithms::inner_product_iters(
    first1, last1, first2, last2, init, op1, op2);
}

} // namespace fast_io::algorithms
