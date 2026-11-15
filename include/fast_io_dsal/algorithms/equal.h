#pragma once

namespace fast_io::details::algorithms
{

template<typename Iter1, typename Snt1, typename Iter2, typename Snt2,
  typename Cmp>
concept equal_iters_no_throw =
  ::fast_io::details::algorithms::sentinel_no_throw<Iter1, Snt1> &&
  ::fast_io::details::algorithms::sentinel_no_throw<Iter2, Snt2> &&
  ::fast_io::details::algorithms::comparator_pass_no_throw<Cmp> &&
  ::fast_io::details::algorithms::equal_call_no_throw_v<
    Cmp, ::std::iter_reference_t<Iter1>, ::std::iter_reference_t<Iter2>>;

template<typename R1, typename R2, typename Cmp>
concept equal_ranges_no_throw =
  ::std::ranges::input_range<R1> &&
  ::std::ranges::input_range<R2> &&
  ::std::is_nothrow_move_constructible_v<Cmp> &&
  ::fast_io::details::algorithms::equal_iters_no_throw<
    ::std::ranges::iterator_t<R1>, ::std::ranges::sentinel_t<R1>,
    ::std::ranges::iterator_t<R2>, ::std::ranges::sentinel_t<R2>, Cmp>;

template<typename R1, typename R2, typename Cmp>
concept equal_index_range_no_throw =
  ::std::ranges::random_access_range<R1> &&
  ::std::ranges::random_access_range<R2> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R1> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R2> &&
  ::fast_io::details::algorithms::equal_iters_no_throw<
    ::fast_io::details::algorithms::algo_index_iterator<R1>,
    ::fast_io::details::algorithms::algo_index_iterator<R1>,
    ::fast_io::details::algorithms::algo_index_iterator<R2>,
    ::fast_io::details::algorithms::algo_index_iterator<R2>, Cmp>;

template<typename Iter1, typename Snt1, typename Iter2, typename Snt2,
  typename Cmp>
inline constexpr bool equal_iters_impl(
  Iter1 first1, Snt1 last1, Iter2 first2, Snt2 last2, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::equal_iters_no_throw<
      Iter1, Snt1, Iter2, Snt2, Cmp>)
{
  while (first1 != last1 && first2 != last2)
  {
    if (!::fast_io::details::algorithms::equal(cmp, *first1, *first2))
    {
      return false;
    }
    ++first1;
    ++first2;
  }
  return first1 == last1 && first2 == last2;
}

} // namespace fast_io::details::algorithms

namespace fast_io::algorithms
{

// Check whether two ranges contain equal elements in the same order.
template<::std::input_iterator Iter1, ::std::sentinel_for<Iter1> Snt1,
  ::std::input_iterator Iter2, ::std::sentinel_for<Iter2> Snt2,
  typename Cmp = ::fast_io::algorithms::equal_to>
inline constexpr bool equal_iters(
  Iter1 first1, Snt1 last1, Iter2 first2, Snt2 last2, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::equal_iters_no_throw<
      Iter1, Snt1, Iter2, Snt2, Cmp>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first1), decltype(first2)>),
    equal_iters_define, first1, last1, first2, last2, cmp);
  return ::fast_io::details::algorithms::equal_iters_impl(
    first1, last1, first2, last2, cmp);
}

template<::std::ranges::input_range R1, ::std::ranges::input_range R2,
  typename Cmp = ::fast_io::algorithms::equal_to>
inline constexpr bool equal(R1 &&r1, R2 &&r2, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::equal_ranges_no_throw<R1, R2, Cmp>)
{
  return ::fast_io::algorithms::equal_iters(
    ::std::ranges::begin(r1), ::std::ranges::end(r1),
    ::std::ranges::begin(r2), ::std::ranges::end(r2), cmp);
}

template<::std::ranges::random_access_range R1,
  ::std::ranges::random_access_range R2,
  typename Cmp = ::fast_io::algorithms::equal_to>
inline constexpr bool equal_index_range(
  R1 &&r1, ::std::size_t firstidx1, ::std::size_t lastidx1,
  R2 &&r2, ::std::size_t firstidx2, ::std::size_t lastidx2,
  Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::equal_index_range_no_throw<R1, R2, Cmp>)
{
  auto [first1, last1] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R1>(r1), firstidx1, lastidx1);
  auto [first2, last2] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R2>(r2), firstidx2, lastidx2);
  return ::fast_io::algorithms::equal_iters(
    first1, last1, first2, last2, cmp);
}

} // namespace fast_io::algorithms
