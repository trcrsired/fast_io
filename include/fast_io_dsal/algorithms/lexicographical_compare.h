#pragma once

namespace fast_io::details::algorithms
{

template<typename Cmp>
concept lexicographical_compare_pass_no_throw =
  ::std::is_nothrow_copy_constructible_v<Cmp> &&
  ::std::is_nothrow_move_constructible_v<Cmp> &&
  ::std::is_nothrow_default_constructible_v<Cmp>;

template<typename Cmp, typename T, typename U>
concept lexicographical_compare_call_no_throw =
  ::fast_io::details::algorithms::three_way_compare_no_throw<Cmp, T, U>;

template<typename Iter1, typename Snt1, typename Iter2, typename Snt2,
  typename Cmp>
concept lexicographical_compare_iters_no_throw =
  ::fast_io::details::algorithms::sentinel_no_throw<Iter1, Snt1> &&
  ::fast_io::details::algorithms::sentinel_no_throw<Iter2, Snt2> &&
  ::fast_io::details::algorithms::lexicographical_compare_pass_no_throw<Cmp> &&
  ::fast_io::details::algorithms::lexicographical_compare_call_no_throw<
    Cmp, ::std::iter_reference_t<Iter1>, ::std::iter_reference_t<Iter2>>;

template<typename R1, typename R2, typename Cmp>
concept lexicographical_compare_ranges_no_throw =
  ::std::ranges::input_range<R1> &&
  ::std::ranges::input_range<R2> &&
  ::fast_io::details::algorithms::lexicographical_compare_iters_no_throw<
    ::std::ranges::iterator_t<R1>, ::std::ranges::sentinel_t<R1>,
    ::std::ranges::iterator_t<R2>, ::std::ranges::sentinel_t<R2>, Cmp>;

template<typename R1, typename R2, typename Cmp>
concept lexicographical_compare_index_range_no_throw =
  ::std::ranges::random_access_range<R1> &&
  ::std::ranges::random_access_range<R2> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R1> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R2> &&
  ::fast_io::details::algorithms::lexicographical_compare_iters_no_throw<
    ::fast_io::details::algorithms::algo_index_iterator<R1>,
    ::fast_io::details::algorithms::algo_index_iterator<R1>,
    ::fast_io::details::algorithms::algo_index_iterator<R2>,
    ::fast_io::details::algorithms::algo_index_iterator<R2>, Cmp>;

template<typename Iter1, typename Snt1, typename Iter2, typename Snt2,
  typename Cmp>
FAST_IO_UNSAFE_API
inline constexpr bool lexicographical_compare_iters_impl(
  Iter1 first1, Snt1 last1, Iter2 first2, Snt2 last2, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::lexicographical_compare_iters_no_throw<
      Iter1, Snt1, Iter2, Snt2, Cmp>)
{
  while (first1 != last1 && first2 != last2)
  {
    auto const result{cmp.compare(*first1, *first2)};
    if (0 > result)
    {
      return true;
    }
    if (0 < result)
    {
      return false;
    }
    ++first1;
    ++first2;
  }
  return first1 == last1 && first2 != last2;
}

template<typename Iter1, typename Snt1, typename Iter2, typename Snt2,
  typename Cmp>
FAST_IO_UNSAFE_API
inline constexpr auto lexicographical_compare_three_way_iters_impl(
  Iter1 first1, Snt1 last1, Iter2 first2, Snt2 last2, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::lexicographical_compare_iters_no_throw<
      Iter1, Snt1, Iter2, Snt2, Cmp>)
  -> decltype(cmp.compare(*first1, *first2))
{
  using result_type = decltype(cmp.compare(*first1, *first2));
  static_assert(
    ::std::same_as<result_type, ::std::strong_ordering> ||
    ::std::same_as<result_type, ::std::weak_ordering> ||
    ::std::same_as<result_type, ::std::partial_ordering>);
  while (first1 != last1 && first2 != last2)
  {
    result_type const result{cmp.compare(*first1, *first2)};
    if (result != 0)
    {
      return result;
    }
    ++first1;
    ++first2;
  }
  if (first1 == last1)
  {
    return first2 == last2 ? ::std::strong_ordering::equal
                           : ::std::strong_ordering::less;
  }
  return ::std::strong_ordering::greater;
}

} // namespace fast_io::details::algorithms

namespace fast_io::algorithms
{

// Lexicographical comparison using a three-way comparator.
template<::std::input_iterator Iter1, ::std::sentinel_for<Iter1> Snt1,
  ::std::input_iterator Iter2, ::std::sentinel_for<Iter2> Snt2,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
  requires ::fast_io::details::algorithms::three_way_compare<
    Cmp, ::std::iter_value_t<Iter1>, ::std::iter_value_t<Iter2>>
FAST_IO_UNSAFE_API
inline constexpr bool lexicographical_compare_iters(
  Iter1 first1, Snt1 last1, Iter2 first2, Snt2 last2, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::lexicographical_compare_iters_no_throw<
      Iter1, Snt1, Iter2, Snt2, Cmp>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first1), decltype(first2)>),
    lexicographical_compare_iters_define, first1, last1, first2, last2, cmp);
  return ::fast_io::details::algorithms::lexicographical_compare_iters_impl(
    first1, last1, first2, last2, cmp);
}

template<::std::ranges::input_range R1, ::std::ranges::input_range R2,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr bool lexicographical_compare(
  R1 &&r1, R2 &&r2, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::lexicographical_compare_ranges_no_throw<
      R1, R2, Cmp>)
{
  return ::fast_io::algorithms::lexicographical_compare_iters(
    ::std::ranges::begin(r1), ::std::ranges::end(r1),
    ::std::ranges::begin(r2), ::std::ranges::end(r2), cmp);
}

template<::std::ranges::random_access_range R1,
  ::std::ranges::random_access_range R2,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr bool lexicographical_compare_index_range(
  R1 &&r1, ::std::size_t firstidx1, ::std::size_t lastidx1,
  R2 &&r2, ::std::size_t firstidx2, ::std::size_t lastidx2,
  Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::lexicographical_compare_index_range_no_throw<
      R1, R2, Cmp>)
{
  auto [first1, last1] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R1>(r1), firstidx1, lastidx1);
  auto [first2, last2] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R2>(r2), firstidx2, lastidx2);
  return ::fast_io::algorithms::lexicographical_compare_iters(
    first1, last1, first2, last2, cmp);
}

// Three-way lexicographical comparison. The result is the comparison category
// produced by the comparator.
template<::std::input_iterator Iter1, ::std::sentinel_for<Iter1> Snt1,
  ::std::input_iterator Iter2, ::std::sentinel_for<Iter2> Snt2,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
  requires ::fast_io::details::algorithms::three_way_compare<
    Cmp, ::std::iter_value_t<Iter1>, ::std::iter_value_t<Iter2>>
FAST_IO_UNSAFE_API
inline constexpr auto lexicographical_compare_three_way_iters(
  Iter1 first1, Snt1 last1, Iter2 first2, Snt2 last2, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::lexicographical_compare_iters_no_throw<
      Iter1, Snt1, Iter2, Snt2, Cmp>)
  -> decltype(cmp.compare(*first1, *first2))
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first1), decltype(first2)>),
    lexicographical_compare_three_way_iters_define, first1, last1, first2, last2, cmp);
  return ::fast_io::details::algorithms::
    lexicographical_compare_three_way_iters_impl(
      first1, last1, first2, last2, cmp);
}

template<::std::ranges::input_range R1, ::std::ranges::input_range R2,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr auto lexicographical_compare_three_way(
  R1 &&r1, R2 &&r2, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::lexicographical_compare_ranges_no_throw<
      R1, R2, Cmp>)
  -> decltype(cmp.compare(
    *::std::ranges::begin(r1), *::std::ranges::begin(r2)))
{
  return ::fast_io::algorithms::lexicographical_compare_three_way_iters(
    ::std::ranges::begin(r1), ::std::ranges::end(r1),
    ::std::ranges::begin(r2), ::std::ranges::end(r2), cmp);
}

template<::std::ranges::random_access_range R1,
  ::std::ranges::random_access_range R2,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr auto lexicographical_compare_three_way_index_range(
  R1 &&r1, ::std::size_t firstidx1, ::std::size_t lastidx1,
  R2 &&r2, ::std::size_t firstidx2, ::std::size_t lastidx2,
  Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::lexicographical_compare_index_range_no_throw<
      R1, R2, Cmp>)
  -> decltype(cmp.compare(
    *::std::ranges::begin(r1), *::std::ranges::begin(r2)))
{
  auto [first1, last1] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R1>(r1), firstidx1, lastidx1);
  auto [first2, last2] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R2>(r2), firstidx2, lastidx2);
  return ::fast_io::algorithms::lexicographical_compare_three_way_iters(
    first1, last1, first2, last2, cmp);
}

} // namespace fast_io::algorithms
