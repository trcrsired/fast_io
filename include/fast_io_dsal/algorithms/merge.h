#pragma once

namespace fast_io::details::algorithms
{

template<typename Iter1, typename Snt1, typename Iter2, typename Snt2,
  typename OIter, typename Cmp>
concept merge_iters_no_throw =
  ::fast_io::details::algorithms::sentinel_no_throw<Iter1, Snt1> &&
  ::fast_io::details::algorithms::sentinel_no_throw<Iter2, Snt2> &&
  ::fast_io::details::algorithms::output_iter_no_throw<
    OIter, ::std::iter_value_t<Iter1>> &&
  ::fast_io::details::algorithms::comparator_pass_no_throw<Cmp> &&
  requires(Cmp cmp, Iter1 &it1, Iter2 &it2, OIter &dest)
{
  { cmp.compare(*it1, *it2) } noexcept
    -> ::fast_io::details::algorithms::three_way_result;
  { *dest = *it1 } noexcept;
  { *dest = *it2 } noexcept;
};

template<typename R1, typename R2, typename OIter, typename Cmp>
concept merge_ranges_no_throw =
  ::std::ranges::input_range<R1> &&
  ::std::ranges::input_range<R2> &&
  ::std::is_nothrow_move_constructible_v<Cmp> &&
  ::fast_io::details::algorithms::merge_iters_no_throw<
    ::std::ranges::iterator_t<R1>, ::std::ranges::sentinel_t<R1>,
    ::std::ranges::iterator_t<R2>, ::std::ranges::sentinel_t<R2>,
    OIter, Cmp>;

template<typename Iter1, typename Snt1, typename Iter2, typename Snt2,
  typename OIter, typename Cmp>
inline constexpr OIter merge_iters_impl(
  Iter1 first1, Snt1 last1, Iter2 first2, Snt2 last2, OIter dest, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::merge_iters_no_throw<
      Iter1, Snt1, Iter2, Snt2, OIter, Cmp>)
{
  while (first1 != last1 && first2 != last2)
  {
    if (0 > cmp.compare(*first2, *first1))
    {
      *dest = *first2;
      ++first2;
    }
    else
    {
      *dest = *first1;
      ++first1;
    }
    ++dest;
  }
  for (; first1 != last1; ++first1, ++dest)
  {
    *dest = *first1;
  }
  for (; first2 != last2; ++first2, ++dest)
  {
    *dest = *first2;
  }
  return dest;
}

template<typename Iter, typename Snt, typename Cmp>
concept inplace_merge_iters_no_throw =
  ::fast_io::details::algorithms::sentinel_no_throw<Iter, Snt> &&
  ::fast_io::details::algorithms::comparator_pass_no_throw<Cmp> &&
  ::fast_io::details::algorithms::three_way_comparator_for<
    Cmp, ::std::iter_reference_t<Iter>> &&
  ::fast_io::details::algorithms::rotate_iters_no_throw<Iter, Iter>;

template<typename R, typename Cmp>
concept inplace_merge_ranges_no_throw =
  ::std::ranges::forward_range<R> &&
  ::std::is_nothrow_move_constructible_v<Cmp> &&
  ::fast_io::details::algorithms::inplace_merge_iters_no_throw<
    ::std::ranges::iterator_t<R>, ::std::ranges::sentinel_t<R>, Cmp>;

template<typename R, typename Cmp>
concept inplace_merge_index_range_no_throw =
  ::std::ranges::random_access_range<R> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R> &&
  ::fast_io::details::algorithms::inplace_merge_iters_no_throw<
    ::fast_io::details::algorithms::algo_index_iterator<R>,
    ::fast_io::details::algorithms::algo_index_iterator<R>, Cmp>;

template<typename Iter, typename Snt, typename Cmp>
inline constexpr void inplace_merge_iters_impl(
  Iter first, Iter middle, Snt last, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::inplace_merge_iters_no_throw<
      Iter, Snt, Cmp>)
{
  while (first != middle && middle != last)
  {
    if (!(0 > cmp.compare(*middle, *first)))
    {
      ++first;
      continue;
    }
    Iter next{middle};
    ++next;
    ::fast_io::algorithms::rotate_iters(first, middle, next);
    ++first;
    middle = next;
  }
}

} // namespace fast_io::details::algorithms

namespace fast_io::algorithms
{

// Merge two sorted ranges into `dest`.
template<::std::input_iterator Iter1, ::std::sentinel_for<Iter1> Snt1,
  ::std::input_iterator Iter2, ::std::sentinel_for<Iter2> Snt2,
  typename OIter, typename Cmp = ::fast_io::algorithms::compare_three_way>
  requires ::fast_io::details::algorithms::three_way_compare<
    Cmp, ::std::iter_value_t<Iter1>, ::std::iter_value_t<Iter2>>
inline constexpr OIter merge_iters(
  Iter1 first1, Snt1 last1, Iter2 first2, Snt2 last2, OIter dest,
  Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::merge_iters_no_throw<
      Iter1, Snt1, Iter2, Snt2, OIter, Cmp>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first1), decltype(first2), decltype(dest)>),
    merge_iters_define, first1, last1, first2, last2, dest, cmp);
  return ::fast_io::details::algorithms::merge_iters_impl(
    first1, last1, first2, last2, dest, cmp);
}

template<::std::ranges::input_range R1, ::std::ranges::input_range R2,
  typename OIter, typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr OIter merge(
  R1 &&r1, R2 &&r2, OIter dest, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::merge_ranges_no_throw<
      R1, R2, OIter, Cmp>)
{
  return ::fast_io::algorithms::merge_iters(
    ::std::ranges::begin(r1), ::std::ranges::end(r1),
    ::std::ranges::begin(r2), ::std::ranges::end(r2), dest, cmp);
}

// Merge two adjacent sorted subranges in place.
template<::std::forward_iterator Iter, ::std::sentinel_for<Iter> Snt,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
  requires ::fast_io::details::algorithms::three_way_compare<
    Cmp, ::std::iter_value_t<Iter>>
inline constexpr void inplace_merge_iters(
  Iter first, Iter middle, Snt last, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::inplace_merge_iters_no_throw<
      Iter, Snt, Cmp>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first)>),
    inplace_merge_iters_define, first, middle, last, cmp);
  ::fast_io::details::algorithms::inplace_merge_iters_impl(
    first, middle, last, cmp);
}

template<::std::ranges::forward_range R,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr void inplace_merge(
  R &&r, ::std::ranges::iterator_t<R> middle, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::inplace_merge_ranges_no_throw<
      R, Cmp>)
{
  ::fast_io::algorithms::inplace_merge_iters(
    ::std::ranges::begin(r), middle, ::std::ranges::end(r), cmp);
}

template<::std::ranges::random_access_range R,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr void inplace_merge_index_range(
  R &&r, ::std::size_t firstidx, ::std::size_t middleidx,
  ::std::size_t lastidx, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::inplace_merge_index_range_no_throw<R, Cmp>)
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R>(r), firstidx, lastidx);
  if (middleidx < firstidx || lastidx < middleidx) [[unlikely]]
  {
    ::fast_io::containers::details::violation_report_on_invalid_argument<
      ::fast_io::details::algorithms::range_throws_on_violations<R>>();
  }
  ::fast_io::algorithms::inplace_merge_iters(
    first,
    first + static_cast<::std::iter_difference_t<
      ::fast_io::details::algorithms::algo_index_iterator<R>>>(
      middleidx - firstidx),
    last, cmp);
}

} // namespace fast_io::algorithms
