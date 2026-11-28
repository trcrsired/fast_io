#pragma once

namespace fast_io::details::algorithms
{

template<typename Iter1, typename Snt1, typename Iter2, typename Snt2,
  typename Cmp>
concept is_permutation_iters_no_throw =
  ::fast_io::details::algorithms::sentinel_no_throw<Iter1, Snt1> &&
  ::fast_io::details::algorithms::sentinel_no_throw<Iter2, Snt2> &&
  ::std::forward_iterator<Iter1> &&
  ::std::forward_iterator<Iter2> &&
  ::fast_io::details::algorithms::comparator_pass_no_throw<Cmp> &&
  ::fast_io::details::algorithms::equal_call_no_throw_v<
    Cmp, ::std::iter_reference_t<Iter1>, ::std::iter_reference_t<Iter1>> &&
  ::fast_io::details::algorithms::equal_call_no_throw_v<
    Cmp, ::std::iter_reference_t<Iter1>, ::std::iter_reference_t<Iter2>>;

template<typename R1, typename R2, typename Cmp>
concept is_permutation_ranges_no_throw =
  ::std::ranges::forward_range<R1> &&
  ::std::ranges::forward_range<R2> &&
  ::fast_io::details::algorithms::is_permutation_iters_no_throw<
    ::std::ranges::iterator_t<R1>, ::std::ranges::sentinel_t<R1>,
    ::std::ranges::iterator_t<R2>, ::std::ranges::sentinel_t<R2>, Cmp>;

template<typename R1, typename R2, typename Cmp>
concept is_permutation_index_range_no_throw =
  ::std::ranges::random_access_range<R1> &&
  ::std::ranges::random_access_range<R2> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R1> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R2> &&
  ::fast_io::details::algorithms::is_permutation_iters_no_throw<
    ::fast_io::details::algorithms::algo_index_iterator<R1>,
    ::fast_io::details::algorithms::algo_index_iterator<R1>,
    ::fast_io::details::algorithms::algo_index_iterator<R2>,
    ::fast_io::details::algorithms::algo_index_iterator<R2>, Cmp>;

template<typename Iter, typename Snt, typename Cmp>
concept next_permutation_iters_no_throw =
  ::fast_io::details::algorithms::sentinel_no_throw<Iter, Snt> &&
  ::std::bidirectional_iterator<Iter> &&
  ::fast_io::details::algorithms::comparator_pass_no_throw<Cmp> &&
  ::fast_io::details::algorithms::reverse_iters_no_throw<Iter, Iter> &&
  ::fast_io::details::algorithms::swap_nothrow<
    ::std::iter_reference_t<Iter>, ::std::iter_reference_t<Iter>>() &&
  ::fast_io::details::algorithms::three_way_comparator_for<
    Cmp, ::std::iter_reference_t<Iter>>;

template<typename R, typename Cmp>
concept next_permutation_ranges_no_throw =
  ::std::ranges::bidirectional_range<R> &&
  ::fast_io::details::algorithms::next_permutation_iters_no_throw<
    ::std::ranges::iterator_t<R>, ::std::ranges::sentinel_t<R>, Cmp>;

template<typename R, typename Cmp>
concept next_permutation_index_range_no_throw =
  ::std::ranges::random_access_range<R> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R> &&
  ::fast_io::details::algorithms::next_permutation_iters_no_throw<
    ::fast_io::details::algorithms::algo_index_iterator<R>,
    ::fast_io::details::algorithms::algo_index_iterator<R>, Cmp>;

template<typename Iter1, typename Snt1, typename Iter2, typename Snt2,
  typename Cmp>
FAST_IO_UNSAFE_API
inline constexpr bool is_permutation_iters_impl(
  Iter1 first1, Snt1 last1, Iter2 first2, Snt2 last2, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::is_permutation_iters_no_throw<
      Iter1, Snt1, Iter2, Snt2, Cmp>)
{
  Iter1 const begin1{first1};
  Iter2 const begin2{first2};
  while (first1 != last1 && first2 != last2)
  {
    ++first1;
    ++first2;
  }
  if (first1 != last1 || first2 != last2)
  {
    return false;
  }
  for (Iter1 it{begin1}; it != last1; ++it)
  {
    bool seen{};
    for (Iter1 prev{begin1}; prev != it; ++prev)
    {
      if (::fast_io::details::algorithms::equal(cmp, *prev, *it))
      {
        seen = true;
        break;
      }
    }
    if (seen)
    {
      continue;
    }
    ::std::size_t count1{};
    for (Iter1 scan{begin1}; scan != last1; ++scan)
    {
      if (::fast_io::details::algorithms::equal(cmp, *scan, *it))
      {
        ++count1;
      }
    }
    ::std::size_t count2{};
    for (Iter2 scan{begin2}; scan != last2; ++scan)
    {
      if (::fast_io::details::algorithms::equal(cmp, *scan, *it))
      {
        ++count2;
      }
    }
    if (count1 != count2)
    {
      return false;
    }
  }
  return true;
}

template<typename Iter, typename Snt, typename Cmp>
FAST_IO_UNSAFE_API
inline constexpr bool next_permutation_iters_impl(
  Iter first, Snt last, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::next_permutation_iters_no_throw<
      Iter, Snt, Cmp>)
{
  Iter end{first};
  for (; end != last; ++end)
  {
  }
  if (first == end)
  {
    return false;
  }
  Iter i{end};
  --i;
  for (;;)
  {
    Iter i1{i};
    if (i == first)
    {
      ::fast_io::algorithms::reverse_iters(first, end);
      return false;
    }
    --i;
    if (0 > cmp.compare(*i, *i1))
    {
      Iter i2{end};
      do
      {
        --i2;
      } while (!(0 > cmp.compare(*i, *i2)));
      ::fast_io::algorithms::iter_swap(i, i2);
      ::fast_io::algorithms::reverse_iters(i1, end);
      return true;
    }
  }
}

template<typename Iter, typename Snt, typename Cmp>
FAST_IO_UNSAFE_API
inline constexpr bool prev_permutation_iters_impl(
  Iter first, Snt last, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::next_permutation_iters_no_throw<
      Iter, Snt, Cmp>)
{
  Iter end{first};
  for (; end != last; ++end)
  {
  }
  if (first == end)
  {
    return false;
  }
  Iter i{end};
  --i;
  for (;;)
  {
    Iter i1{i};
    if (i == first)
    {
      ::fast_io::algorithms::reverse_iters(first, end);
      return false;
    }
    --i;
    if (0 > cmp.compare(*i1, *i))
    {
      Iter i2{end};
      do
      {
        --i2;
      } while (!(0 > cmp.compare(*i2, *i)));
      ::fast_io::algorithms::iter_swap(i, i2);
      ::fast_io::algorithms::reverse_iters(i1, end);
      return true;
    }
  }
}

} // namespace fast_io::details::algorithms

namespace fast_io::algorithms
{

// Check whether two ranges are permutations of each other.
template<::std::forward_iterator Iter1, ::std::sentinel_for<Iter1> Snt1,
  ::std::forward_iterator Iter2, ::std::sentinel_for<Iter2> Snt2,
  typename Cmp = ::fast_io::algorithms::equal_to>
  requires ::fast_io::details::algorithms::equality_comparator<
    Cmp, ::std::iter_value_t<Iter1>, ::std::iter_value_t<Iter2>>
FAST_IO_UNSAFE_API
inline constexpr bool is_permutation_iters(
  Iter1 first1, Snt1 last1, Iter2 first2, Snt2 last2, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::is_permutation_iters_no_throw<
      Iter1, Snt1, Iter2, Snt2, Cmp>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first1), decltype(first2)>),
    is_permutation_iters_define, first1, last1, first2, last2, cmp);
  return ::fast_io::details::algorithms::is_permutation_iters_impl(
    first1, last1, first2, last2, cmp);
}

template<::std::ranges::forward_range R1,
  ::std::ranges::forward_range R2,
  typename Cmp = ::fast_io::algorithms::equal_to>
inline constexpr bool is_permutation(
  R1 &&r1, R2 &&r2, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::is_permutation_ranges_no_throw<
      R1, R2, Cmp>)
{
  return ::fast_io::algorithms::is_permutation_iters(
    ::std::ranges::begin(r1), ::std::ranges::end(r1),
    ::std::ranges::begin(r2), ::std::ranges::end(r2), cmp);
}

template<::std::ranges::random_access_range R1,
  ::std::ranges::random_access_range R2,
  typename Cmp = ::fast_io::algorithms::equal_to>
inline constexpr bool is_permutation_index_range(
  R1 &&r1, ::std::size_t firstidx1, ::std::size_t lastidx1,
  R2 &&r2, ::std::size_t firstidx2, ::std::size_t lastidx2,
  Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::is_permutation_index_range_no_throw<
      R1, R2, Cmp>)
{
  auto [first1, last1] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R1>(r1), firstidx1, lastidx1);
  auto [first2, last2] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R2>(r2), firstidx2, lastidx2);
  return ::fast_io::algorithms::is_permutation_iters(
    first1, last1, first2, last2, cmp);
}

// Check whether `[first1, last1)` is a permutation of a prefix of
// `[first2, last2)`.
template<::std::forward_iterator Iter1, ::std::sentinel_for<Iter1> Snt1,
  ::std::forward_iterator Iter2, ::std::sentinel_for<Iter2> Snt2,
  typename Cmp = ::fast_io::algorithms::equal_to>
  requires ::fast_io::details::algorithms::equality_comparator<
    Cmp, ::std::iter_value_t<Iter1>, ::std::iter_value_t<Iter2>>
FAST_IO_UNSAFE_API
inline constexpr bool is_permutation_prefix_iters(
  Iter1 first1, Snt1 last1, Iter2 first2, Snt2 last2, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::is_permutation_iters_no_throw<
      Iter1, Snt1, Iter2, Snt2, Cmp>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first1), decltype(first2)>),
    is_permutation_prefix_iters_define, first1, last1, first2, last2, cmp);
  Iter2 prefix_last{first2};
  for (Iter1 it{first1}; it != last1; ++it)
  {
    if (prefix_last == last2)
    {
      return false;
    }
    ++prefix_last;
  }
  return ::fast_io::details::algorithms::is_permutation_iters_impl(
    first1, last1, first2, prefix_last, cmp);
}

template<::std::ranges::forward_range R1,
  ::std::ranges::forward_range R2,
  typename Cmp = ::fast_io::algorithms::equal_to>
inline constexpr bool is_permutation_prefix(
  R1 &&r1, R2 &&r2, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::is_permutation_ranges_no_throw<
      R1, R2, Cmp>)
{
  return ::fast_io::algorithms::is_permutation_prefix_iters(
    ::std::ranges::begin(r1), ::std::ranges::end(r1),
    ::std::ranges::begin(r2), ::std::ranges::end(r2), cmp);
}

// Rearrange into the next lexicographically greater permutation.
template<::std::bidirectional_iterator Iter, ::std::sentinel_for<Iter> Snt,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
  requires ::fast_io::details::algorithms::three_way_comparator<
    Cmp, ::std::iter_value_t<Iter>>
FAST_IO_UNSAFE_API
inline constexpr bool next_permutation_iters(
  Iter first, Snt last, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::next_permutation_iters_no_throw<
      Iter, Snt, Cmp>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first)>),
    next_permutation_iters_define, first, last, cmp);
  return ::fast_io::details::algorithms::next_permutation_iters_impl(
    first, last, cmp);
}

template<::std::ranges::bidirectional_range R,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr bool next_permutation(R &&r, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::next_permutation_ranges_no_throw<
      R, Cmp>)
{
  return ::fast_io::algorithms::next_permutation_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), cmp);
}

template<::std::ranges::random_access_range R,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr bool next_permutation_index_range(
  R &&r, ::std::size_t firstidx, ::std::size_t lastidx, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::next_permutation_index_range_no_throw<
      R, Cmp>)
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R>(r), firstidx, lastidx);
  return ::fast_io::algorithms::next_permutation_iters(
    first, last, cmp);
}

// Rearrange into the next lexicographically smaller permutation.
template<::std::bidirectional_iterator Iter, ::std::sentinel_for<Iter> Snt,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
  requires ::fast_io::details::algorithms::three_way_comparator<
    Cmp, ::std::iter_value_t<Iter>>
FAST_IO_UNSAFE_API
inline constexpr bool prev_permutation_iters(
  Iter first, Snt last, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::next_permutation_iters_no_throw<
      Iter, Snt, Cmp>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first)>),
    prev_permutation_iters_define, first, last, cmp);
  return ::fast_io::details::algorithms::prev_permutation_iters_impl(
    first, last, cmp);
}

template<::std::ranges::bidirectional_range R,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr bool prev_permutation(R &&r, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::next_permutation_ranges_no_throw<
      R, Cmp>)
{
  return ::fast_io::algorithms::prev_permutation_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), cmp);
}

template<::std::ranges::random_access_range R,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr bool prev_permutation_index_range(
  R &&r, ::std::size_t firstidx, ::std::size_t lastidx, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::next_permutation_index_range_no_throw<
      R, Cmp>)
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R>(r), firstidx, lastidx);
  return ::fast_io::algorithms::prev_permutation_iters(
    first, last, cmp);
}

} // namespace fast_io::algorithms
