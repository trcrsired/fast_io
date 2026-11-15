#pragma once

namespace fast_io::details::algorithms
{

template<typename Iter, typename Snt, typename Cmp>
concept nth_element_iters_no_throw =
  ::fast_io::details::algorithms::sentinel_no_throw<Iter, Snt> &&
  ::std::random_access_iterator<Iter> &&
  ::fast_io::details::algorithms::sort_iters_no_throw<Iter, Cmp> &&
  ::fast_io::details::algorithms::comparator_pass_no_throw<Cmp>;

template<typename R, typename Cmp>
concept nth_element_ranges_no_throw =
  ::std::ranges::random_access_range<R> &&
  ::fast_io::details::algorithms::nth_element_iters_no_throw<
    ::std::ranges::iterator_t<R>, ::std::ranges::sentinel_t<R>, Cmp>;

template<typename R, typename Cmp>
concept nth_element_index_range_no_throw =
  ::std::ranges::random_access_range<R> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R> &&
  ::fast_io::details::algorithms::nth_element_iters_no_throw<
    ::fast_io::details::algorithms::algo_index_iterator<R>,
    ::fast_io::details::algorithms::algo_index_iterator<R>, Cmp>;

// Partition `[first, first + len)` around the pivot at `first`. With `equal`
// false, elements strictly less than the pivot end up in
// `[first, first + result)`; with `equal` true the `<=` elements do. The pivot
// is left at `first` and the rest is `>=` (or `>`) the pivot. The caller swaps
// the pivot into place.
template<bool equal, typename Iter, typename Cmp>
inline constexpr ::std::iter_difference_t<Iter> nth_element_partition(
  Iter first, ::std::iter_difference_t<Iter> len, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::sort_ops_iter_no_throw<Iter, Cmp>)
{
  using value_type = ::std::iter_value_t<Iter>;
  auto const *const pivot_ptr{::std::addressof(*first)};
  if consteval
  {
    return ::fast_io::details::algorithms::sort_partition_lomuto<equal>(
      first, len, pivot_ptr, cmp);
  }
  else
  {
    if constexpr (sizeof(value_type) <= 96)
    {
      return ::fast_io::details::algorithms::sort_partition_lomuto<equal>(
        first, len, pivot_ptr, cmp);
    }
    else
    {
      return ::fast_io::details::algorithms::sort_partition_hoare<equal>(
        first, len, pivot_ptr, cmp);
    }
  }
}

template<typename Iter, typename Snt, typename Cmp>
inline constexpr void nth_element_iters_impl(
  Iter first, Iter nth, Snt last, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::nth_element_iters_no_throw<
      Iter, Snt, Cmp>)
{
  using difference_type = ::std::iter_difference_t<Iter>;
  Iter last_iter{first};
  for (; last_iter != last; ++last_iter)
  {
  }
  difference_type length{last_iter - first};
  if (length < 2)
  {
    return;
  }
  // A non-decreasing range already satisfies the postcondition. The scan stops
  // at the first inversion, so random input only pays a couple of
  // comparisons and sorted input becomes linear.
  bool non_decreasing{true};
  Iter previous{first};
  for (Iter it{first + 1}; it != last_iter; ++it)
  {
    if (0 > cmp.compare(*it, *previous))
    {
      non_decreasing = false;
      break;
    }
    previous = it;
  }
  if (non_decreasing)
  {
    return;
  }
  difference_type depth{
    2 * ::fast_io::details::algorithms::sort_floor_log2(
          static_cast<::std::size_t>(length))};
  while (length > 1)
  {
    if (depth <= 0)
    {
      ::fast_io::algorithms::sort_iters(first, last_iter, cmp);
      return;
    }
    --depth;
    Iter pivot{};
    if (length < 64)
    {
      pivot = ::fast_io::details::algorithms::sort_median3(
        first, first + length / 2, last_iter - 1, cmp);
    }
    else
    {
      pivot = ::fast_io::details::algorithms::sort_choose_pivot(
        first, length, cmp);
    }
    if (pivot != first)
    {
      ::fast_io::algorithms::iter_swap(first, pivot);
    }
    difference_type const num_lt{
      ::fast_io::details::algorithms::nth_element_partition<false>(
        first, length, cmp)};
    Iter const middle{first + num_lt};
    if (middle != first)
    {
      ::fast_io::algorithms::iter_swap(first, middle);
    }
    if (nth == middle)
    {
      return;
    }
    if (nth < middle)
    {
      last_iter = middle;
      length = num_lt;
      continue;
    }
    // The pivot is the smallest element of the range. Partition once more
    // with the equal-element predicate so all elements equal to the pivot are
    // grouped directly after it; every one of them is already in its final
    // position. This keeps duplicate-heavy inputs linear instead of peeling
    // one equal element per partition.
    if (num_lt == 0)
    {
      difference_type const right_length{last_iter - middle};
      difference_type const num_eq{
        ::fast_io::details::algorithms::nth_element_partition<true>(
          middle, right_length, cmp)};
      Iter const equal_end{middle + 1 + num_eq};
      if (nth < equal_end)
      {
        return;
      }
      first = equal_end;
      length = last_iter - first;
      continue;
    }
    first = middle + 1;
    length = last_iter - first;
  }
}

} // namespace fast_io::details::algorithms

namespace fast_io::algorithms
{

// Rearrange `[first, last)` so that the element at `nth` is the one that would
// be there after a full sort, with smaller elements before it and larger ones
// after it.
template<::std::random_access_iterator Iter, ::std::sentinel_for<Iter> Snt,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
  requires ::fast_io::details::algorithms::three_way_comparator<
    Cmp, ::std::iter_value_t<Iter>>
inline constexpr void nth_element_iters(
  Iter first, Iter nth, Snt last, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::nth_element_iters_no_throw<
      Iter, Snt, Cmp>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first)>),
    nth_element_iters_define, first, nth, last, cmp);
  ::fast_io::details::algorithms::nth_element_iters_impl(
    first, nth, last, cmp);
}

template<::std::ranges::random_access_range R,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr void nth_element(
  R &&r, ::std::ranges::iterator_t<R> nth, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::nth_element_ranges_no_throw<R, Cmp>)
{
  ::fast_io::algorithms::nth_element_iters(
    ::std::ranges::begin(r), nth, ::std::ranges::end(r), cmp);
}

template<::std::ranges::random_access_range R,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr void nth_element_index_range(
  R &&r, ::std::size_t firstidx, ::std::size_t nthidx,
  ::std::size_t lastidx, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::nth_element_index_range_no_throw<R, Cmp>)
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R>(r), firstidx, lastidx);
  if (nthidx < firstidx || lastidx < nthidx) [[unlikely]]
  {
    ::fast_io::containers::details::violation_report_on_invalid_argument<
      ::fast_io::details::algorithms::range_throws_on_violations<R>>();
  }
  ::fast_io::algorithms::nth_element_iters(
    first,
    first + static_cast<::std::iter_difference_t<
      ::fast_io::details::algorithms::algo_index_iterator<R>>>(
      nthidx - firstidx),
    last, cmp);
}

} // namespace fast_io::algorithms
