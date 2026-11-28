#pragma once

namespace fast_io::details::algorithms
{

template<typename Iter1, typename Snt1, typename Iter2>
concept swap_ranges_iters_no_throw =
  ::fast_io::details::algorithms::sentinel_no_throw<Iter1, Snt1> &&
  ::fast_io::details::algorithms::iter_no_throw<Iter2> &&
  ::fast_io::details::algorithms::swap_nothrow<
    ::std::iter_reference_t<Iter1>, ::std::iter_reference_t<Iter2>>();

template<typename R1, typename Iter2>
concept swap_ranges_ranges_no_throw =
  ::std::ranges::range<R1> &&
  ::fast_io::details::algorithms::swap_ranges_iters_no_throw<
    ::std::ranges::iterator_t<R1>, ::std::ranges::sentinel_t<R1>, Iter2>;

template<typename R1, typename Iter2>
concept swap_ranges_index_range_no_throw =
  ::std::ranges::random_access_range<R1> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R1> &&
  ::fast_io::details::algorithms::swap_ranges_iters_no_throw<
    ::fast_io::details::algorithms::algo_index_iterator<R1>,
    ::fast_io::details::algorithms::algo_index_iterator<R1>, Iter2>;

template<typename Iter1, typename Snt1, typename Iter2>
FAST_IO_UNSAFE_API
inline constexpr Iter2 swap_ranges_iters_impl(
  Iter1 first1, Snt1 last1, Iter2 first2)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::swap_ranges_iters_no_throw<
      Iter1, Snt1, Iter2>)
{
  for (; first1 != last1; ++first1, ++first2)
  {
    ::fast_io::algorithms::iter_swap(first1, first2);
  }
  return first2;
}

} // namespace fast_io::details::algorithms

namespace fast_io::algorithms
{

// Swap corresponding elements of two ranges.
template<::std::input_iterator Iter1, ::std::sentinel_for<Iter1> Snt1,
  ::std::input_iterator Iter2>
FAST_IO_UNSAFE_API
inline constexpr Iter2 swap_ranges_iters(
  Iter1 first1, Snt1 last1, Iter2 first2)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::swap_ranges_iters_no_throw<
      Iter1, Snt1, Iter2>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first1), decltype(first2)>),
    swap_ranges_iters_define, first1, last1, first2);
  return ::fast_io::details::algorithms::swap_ranges_iters_impl(
    first1, last1, first2);
}

template<::std::ranges::range R1, ::std::input_iterator Iter2>
FAST_IO_UNSAFE_API
inline constexpr Iter2 swap_ranges(R1 &&r1, Iter2 first2)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::swap_ranges_ranges_no_throw<
      R1, Iter2>)
{
  return ::fast_io::algorithms::swap_ranges_iters(
    ::std::ranges::begin(r1), ::std::ranges::end(r1), first2);
}

template<::std::ranges::random_access_range R1, ::std::input_iterator Iter2>
FAST_IO_UNSAFE_API
inline constexpr Iter2 swap_ranges_index_range(
  R1 &&r1, ::std::size_t firstidx, ::std::size_t lastidx, Iter2 first2)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::swap_ranges_index_range_no_throw<
      R1, Iter2>)
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R1>(r1), firstidx, lastidx);
  return ::fast_io::algorithms::swap_ranges_iters(first, last, first2);
}

} // namespace fast_io::algorithms
