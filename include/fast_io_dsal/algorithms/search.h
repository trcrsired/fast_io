#pragma once

namespace fast_io::details::algorithms
{

template<typename Cmp, typename T, typename U>
concept search_compare_no_throw =
  ::fast_io::details::algorithms::equal_call_no_throw_v<Cmp, T, U>;

template<typename Iter, typename Snt, typename Cmp>
concept adjacent_find_iters_no_throw =
  ::fast_io::details::algorithms::sentinel_no_throw<Iter, Snt> &&
  ::fast_io::details::algorithms::comparator_pass_no_throw<Cmp> &&
  ::fast_io::details::algorithms::search_compare_no_throw<
    Cmp, ::std::iter_reference_t<Iter>, ::std::iter_reference_t<Iter>>;

template<typename R, typename Cmp>
concept adjacent_find_ranges_no_throw =
  ::std::ranges::forward_range<R> &&
  ::std::is_nothrow_move_constructible_v<Cmp> &&
  ::fast_io::details::algorithms::adjacent_find_iters_no_throw<
    ::std::ranges::iterator_t<R>, ::std::ranges::sentinel_t<R>, Cmp>;

template<typename R, typename Cmp>
concept adjacent_find_index_range_no_throw =
  ::std::ranges::random_access_range<R> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R> &&
  ::fast_io::details::algorithms::adjacent_find_iters_no_throw<
    ::fast_io::details::algorithms::algo_index_iterator<R>,
    ::fast_io::details::algorithms::algo_index_iterator<R>, Cmp>;

template<typename Iter1, typename Snt1, typename Iter2, typename Snt2,
  typename Cmp>
concept search_iters_no_throw =
  ::fast_io::details::algorithms::sentinel_no_throw<Iter1, Snt1> &&
  ::fast_io::details::algorithms::sentinel_no_throw<Iter2, Snt2> &&
  ::fast_io::details::algorithms::comparator_pass_no_throw<Cmp> &&
  ::fast_io::details::algorithms::search_compare_no_throw<
    Cmp, ::std::iter_reference_t<Iter1>, ::std::iter_reference_t<Iter2>>;

template<typename R1, typename R2, typename Cmp>
concept search_ranges_no_throw =
  ::std::ranges::forward_range<R1> &&
  ::std::ranges::forward_range<R2> &&
  ::std::is_nothrow_move_constructible_v<Cmp> &&
  ::fast_io::details::algorithms::search_iters_no_throw<
    ::std::ranges::iterator_t<R1>, ::std::ranges::sentinel_t<R1>,
    ::std::ranges::iterator_t<R2>, ::std::ranges::sentinel_t<R2>, Cmp>;

template<typename R1, typename R2, typename Cmp>
concept search_index_range_no_throw =
  ::std::ranges::random_access_range<R1> &&
  ::std::ranges::random_access_range<R2> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R1> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R2> &&
  ::fast_io::details::algorithms::search_iters_no_throw<
    ::fast_io::details::algorithms::algo_index_iterator<R1>,
    ::fast_io::details::algorithms::algo_index_iterator<R1>,
    ::fast_io::details::algorithms::algo_index_iterator<R2>,
    ::fast_io::details::algorithms::algo_index_iterator<R2>, Cmp>;

template<typename Iter, typename Snt, typename T, typename Cmp>
concept search_n_iters_no_throw =
  ::fast_io::details::algorithms::sentinel_no_throw<Iter, Snt> &&
  ::fast_io::details::algorithms::comparator_pass_no_throw<Cmp> &&
  ::fast_io::details::algorithms::search_compare_no_throw<
    Cmp, ::std::iter_reference_t<Iter>, T>;

template<typename R, typename T, typename Cmp>
concept search_n_ranges_no_throw =
  ::std::ranges::forward_range<R> &&
  ::std::is_nothrow_move_constructible_v<Cmp> &&
  ::fast_io::details::algorithms::search_n_iters_no_throw<
    ::std::ranges::iterator_t<R>, ::std::ranges::sentinel_t<R>, T, Cmp>;

template<typename R, typename T, typename Cmp>
concept search_n_index_range_no_throw =
  ::std::ranges::random_access_range<R> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R> &&
  ::fast_io::details::algorithms::search_n_iters_no_throw<
    ::fast_io::details::algorithms::algo_index_iterator<R>,
    ::fast_io::details::algorithms::algo_index_iterator<R>, T, Cmp>;

template<typename Iter, typename Snt, typename Cmp>
inline constexpr Iter adjacent_find_iters_impl(
  Iter first, Snt last, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::adjacent_find_iters_no_throw<
      Iter, Snt, Cmp>)
{
  Iter current{first};
  while (current != last)
  {
    Iter next{current};
    ++next;
    if (next == last)
    {
      return next;
    }
    if (::fast_io::details::algorithms::equal(cmp, *current, *next))
    {
      return current;
    }
    ++current;
  }
  return current;
}

template<typename Iter1, typename Snt1, typename Iter2, typename Snt2,
  typename Cmp>
inline constexpr Iter1 find_end_iters_impl(
  Iter1 first1, Snt1 last1, Iter2 first2, Snt2 last2, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::search_iters_no_throw<
      Iter1, Snt1, Iter2, Snt2, Cmp>)
{
  if (first2 == last2)
  {
    for (; first1 != last1; ++first1)
    {
    }
    return first1;
  }
  Iter1 result{first1};
  bool found{};
  while (first1 != last1)
  {
    Iter1 it1{first1};
    Iter2 it2{first2};
    while (it1 != last1 && it2 != last2 &&
      ::fast_io::details::algorithms::equal(cmp, *it1, *it2))
    {
      ++it1;
      ++it2;
    }
    if (it2 == last2)
    {
      result = first1;
      found = true;
    }
    if (it1 == last1)
    {
      return found ? result : it1;
    }
    ++first1;
  }
  return found ? result : first1;
}

template<typename Iter1, typename Snt1, typename Iter2, typename Snt2,
  typename Cmp>
inline constexpr Iter1 find_first_of_iters_impl(
  Iter1 first1, Snt1 last1, Iter2 first2, Snt2 last2, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::search_iters_no_throw<
      Iter1, Snt1, Iter2, Snt2, Cmp>)
{
  Iter1 it1{first1};
  for (; it1 != last1; ++it1)
  {
    for (Iter2 it2{first2}; it2 != last2; ++it2)
    {
      if (::fast_io::details::algorithms::equal(cmp, *it1, *it2))
      {
        return it1;
      }
    }
  }
  return it1;
}

template<typename Iter1, typename Snt1, typename Iter2, typename Snt2,
  typename Cmp>
inline constexpr Iter1 search_iters_impl(
  Iter1 first1, Snt1 last1, Iter2 first2, Snt2 last2, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::search_iters_no_throw<
      Iter1, Snt1, Iter2, Snt2, Cmp>)
{
  if (first2 == last2)
  {
    return first1;
  }
  while (first1 != last1)
  {
    Iter1 it1{first1};
    Iter2 it2{first2};
    while (it1 != last1 && it2 != last2 &&
      ::fast_io::details::algorithms::equal(cmp, *it1, *it2))
    {
      ++it1;
      ++it2;
    }
    if (it2 == last2)
    {
      return first1;
    }
    if (it1 == last1)
    {
      return it1;
    }
    ++first1;
  }
  return first1;
}

template<typename Iter, typename Snt, typename T, typename Cmp>
inline constexpr Iter search_n_iters_impl(
  Iter first, Snt last, ::std::size_t count, T const &value, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::search_n_iters_no_throw<
      Iter, Snt, T, Cmp>)
{
  if (count == 0)
  {
    return first;
  }
  Iter current{first};
  while (current != last)
  {
    if (!::fast_io::details::algorithms::equal(cmp, *current, value))
    {
      ++current;
      continue;
    }
    Iter it{current};
    ::std::size_t remaining{count};
    while (remaining != 0 && it != last &&
      ::fast_io::details::algorithms::equal(cmp, *it, value))
    {
      ++it;
      --remaining;
    }
    if (remaining == 0)
    {
      return current;
    }
    while (current != it)
    {
      ++current;
    }
  }
  return current;
}

} // namespace fast_io::details::algorithms

namespace fast_io::algorithms
{

// Find the first pair of adjacent elements that compare equal.
template<::std::forward_iterator Iter, ::std::sentinel_for<Iter> Snt,
  typename Cmp = ::fast_io::algorithms::equal_to>
inline constexpr Iter adjacent_find_iters(
  Iter first, Snt last, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::adjacent_find_iters_no_throw<
      Iter, Snt, Cmp>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first)>),
    adjacent_find_iters_define, first, last, cmp);
  return ::fast_io::details::algorithms::adjacent_find_iters_impl(
    first, last, cmp);
}

template<::std::ranges::forward_range R,
  typename Cmp = ::fast_io::algorithms::equal_to>
inline constexpr ::std::ranges::iterator_t<R> adjacent_find(
  R &&r, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::adjacent_find_ranges_no_throw<R, Cmp>)
{
  return ::fast_io::algorithms::adjacent_find_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), cmp);
}

template<::std::ranges::random_access_range R,
  typename Cmp = ::fast_io::algorithms::equal_to>
inline constexpr ::std::size_t adjacent_find_index_range(
  R &&r, ::std::size_t firstidx, ::std::size_t lastidx, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::adjacent_find_index_range_no_throw<R, Cmp>)
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R>(r), firstidx, lastidx);
  auto const found{::fast_io::algorithms::adjacent_find_iters(
    first, last, cmp)};
  return firstidx + static_cast<::std::size_t>(found - first);
}

template<::std::ranges::random_access_range R,
  typename Cmp = ::fast_io::algorithms::equal_to>
inline constexpr ::std::size_t adjacent_find_index_of(
  R &&r, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::adjacent_find_ranges_no_throw<R, Cmp>)
{
  auto const first{::std::ranges::begin(r)};
  auto const found{::fast_io::algorithms::adjacent_find(
    ::std::forward<R>(r), cmp)};
  return static_cast<::std::size_t>(found - first);
}

// Find the last occurrence of `[first2, last2)` in `[first1, last1)`.
template<::std::forward_iterator Iter1, ::std::sentinel_for<Iter1> Snt1,
  ::std::forward_iterator Iter2, ::std::sentinel_for<Iter2> Snt2,
  typename Cmp = ::fast_io::algorithms::equal_to>
inline constexpr Iter1 find_end_iters(
  Iter1 first1, Snt1 last1, Iter2 first2, Snt2 last2, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::search_iters_no_throw<
      Iter1, Snt1, Iter2, Snt2, Cmp>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first1), decltype(first2)>),
    find_end_iters_define, first1, last1, first2, last2, cmp);
  return ::fast_io::details::algorithms::find_end_iters_impl(
    first1, last1, first2, last2, cmp);
}

template<::std::ranges::forward_range R1, ::std::ranges::forward_range R2,
  typename Cmp = ::fast_io::algorithms::equal_to>
inline constexpr ::std::ranges::iterator_t<R1> find_end(
  R1 &&r1, R2 &&r2, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::search_ranges_no_throw<R1, R2, Cmp>)
{
  return ::fast_io::algorithms::find_end_iters(
    ::std::ranges::begin(r1), ::std::ranges::end(r1),
    ::std::ranges::begin(r2), ::std::ranges::end(r2), cmp);
}

template<::std::ranges::random_access_range R1,
  ::std::ranges::random_access_range R2,
  typename Cmp = ::fast_io::algorithms::equal_to>
inline constexpr ::std::size_t find_end_index_range(
  R1 &&r1, ::std::size_t firstidx1, ::std::size_t lastidx1,
  R2 &&r2, ::std::size_t firstidx2, ::std::size_t lastidx2,
  Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::search_index_range_no_throw<R1, R2, Cmp>)
{
  auto [first1, last1] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R1>(r1), firstidx1, lastidx1);
  auto [first2, last2] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R2>(r2), firstidx2, lastidx2);
  auto const found{::fast_io::algorithms::find_end_iters(
    first1, last1, first2, last2, cmp)};
  return firstidx1 + static_cast<::std::size_t>(found - first1);
}

template<::std::ranges::random_access_range R1,
  ::std::ranges::random_access_range R2,
  typename Cmp = ::fast_io::algorithms::equal_to>
inline constexpr ::std::size_t find_end_index_of(
  R1 &&r1, R2 &&r2, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::search_ranges_no_throw<R1, R2, Cmp>)
{
  auto const first{::std::ranges::begin(r1)};
  auto const found{::fast_io::algorithms::find_end(
    ::std::forward<R1>(r1), ::std::forward<R2>(r2), cmp)};
  return static_cast<::std::size_t>(found - first);
}

// Find the first element of `[first1, last1)` that also appears in
// `[first2, last2)`.
template<::std::forward_iterator Iter1, ::std::sentinel_for<Iter1> Snt1,
  ::std::forward_iterator Iter2, ::std::sentinel_for<Iter2> Snt2,
  typename Cmp = ::fast_io::algorithms::equal_to>
inline constexpr Iter1 find_first_of_iters(
  Iter1 first1, Snt1 last1, Iter2 first2, Snt2 last2, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::search_iters_no_throw<
      Iter1, Snt1, Iter2, Snt2, Cmp>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first1), decltype(first2)>),
    find_first_of_iters_define, first1, last1, first2, last2, cmp);
  return ::fast_io::details::algorithms::find_first_of_iters_impl(
    first1, last1, first2, last2, cmp);
}

template<::std::ranges::forward_range R1, ::std::ranges::forward_range R2,
  typename Cmp = ::fast_io::algorithms::equal_to>
inline constexpr ::std::ranges::iterator_t<R1> find_first_of(
  R1 &&r1, R2 &&r2, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::search_ranges_no_throw<R1, R2, Cmp>)
{
  return ::fast_io::algorithms::find_first_of_iters(
    ::std::ranges::begin(r1), ::std::ranges::end(r1),
    ::std::ranges::begin(r2), ::std::ranges::end(r2), cmp);
}

template<::std::ranges::random_access_range R1,
  ::std::ranges::random_access_range R2,
  typename Cmp = ::fast_io::algorithms::equal_to>
inline constexpr ::std::size_t find_first_of_index_range(
  R1 &&r1, ::std::size_t firstidx1, ::std::size_t lastidx1,
  R2 &&r2, ::std::size_t firstidx2, ::std::size_t lastidx2,
  Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::search_index_range_no_throw<R1, R2, Cmp>)
{
  auto [first1, last1] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R1>(r1), firstidx1, lastidx1);
  auto [first2, last2] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R2>(r2), firstidx2, lastidx2);
  auto const found{::fast_io::algorithms::find_first_of_iters(
    first1, last1, first2, last2, cmp)};
  return firstidx1 + static_cast<::std::size_t>(found - first1);
}

template<::std::ranges::random_access_range R1,
  ::std::ranges::random_access_range R2,
  typename Cmp = ::fast_io::algorithms::equal_to>
inline constexpr ::std::size_t find_first_of_index_of(
  R1 &&r1, R2 &&r2, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::search_ranges_no_throw<R1, R2, Cmp>)
{
  auto const first{::std::ranges::begin(r1)};
  auto const found{::fast_io::algorithms::find_first_of(
    ::std::forward<R1>(r1), ::std::forward<R2>(r2), cmp)};
  return static_cast<::std::size_t>(found - first);
}

// Find the first occurrence of `[first2, last2)` in `[first1, last1)`.
template<::std::forward_iterator Iter1, ::std::sentinel_for<Iter1> Snt1,
  ::std::forward_iterator Iter2, ::std::sentinel_for<Iter2> Snt2,
  typename Cmp = ::fast_io::algorithms::equal_to>
inline constexpr Iter1 search_iters(
  Iter1 first1, Snt1 last1, Iter2 first2, Snt2 last2, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::search_iters_no_throw<
      Iter1, Snt1, Iter2, Snt2, Cmp>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first1), decltype(first2)>),
    search_iters_define, first1, last1, first2, last2, cmp);
  return ::fast_io::details::algorithms::search_iters_impl(
    first1, last1, first2, last2, cmp);
}

template<::std::ranges::forward_range R1, ::std::ranges::forward_range R2,
  typename Cmp = ::fast_io::algorithms::equal_to>
inline constexpr ::std::ranges::iterator_t<R1> search(
  R1 &&r1, R2 &&r2, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::search_ranges_no_throw<R1, R2, Cmp>)
{
  return ::fast_io::algorithms::search_iters(
    ::std::ranges::begin(r1), ::std::ranges::end(r1),
    ::std::ranges::begin(r2), ::std::ranges::end(r2), cmp);
}

template<::std::ranges::random_access_range R1,
  ::std::ranges::random_access_range R2,
  typename Cmp = ::fast_io::algorithms::equal_to>
inline constexpr ::std::size_t search_index_range(
  R1 &&r1, ::std::size_t firstidx1, ::std::size_t lastidx1,
  R2 &&r2, ::std::size_t firstidx2, ::std::size_t lastidx2,
  Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::search_index_range_no_throw<R1, R2, Cmp>)
{
  auto [first1, last1] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R1>(r1), firstidx1, lastidx1);
  auto [first2, last2] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R2>(r2), firstidx2, lastidx2);
  auto const found{::fast_io::algorithms::search_iters(
    first1, last1, first2, last2, cmp)};
  return firstidx1 + static_cast<::std::size_t>(found - first1);
}

template<::std::ranges::random_access_range R1,
  ::std::ranges::random_access_range R2,
  typename Cmp = ::fast_io::algorithms::equal_to>
inline constexpr ::std::size_t search_index_of(
  R1 &&r1, R2 &&r2, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::search_ranges_no_throw<R1, R2, Cmp>)
{
  auto const first{::std::ranges::begin(r1)};
  auto const found{::fast_io::algorithms::search(
    ::std::forward<R1>(r1), ::std::forward<R2>(r2), cmp)};
  return static_cast<::std::size_t>(found - first);
}

// Find `count` consecutive elements equal to `value`.
template<::std::forward_iterator Iter, ::std::sentinel_for<Iter> Snt,
  typename T, typename Cmp = ::fast_io::algorithms::equal_to>
inline constexpr Iter search_n_iters(
  Iter first, Snt last, ::std::size_t count, T const &value,
  Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::search_n_iters_no_throw<
      Iter, Snt, T, Cmp>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first)>),
    search_n_iters_define, first, last, count, value, cmp);
  return ::fast_io::details::algorithms::search_n_iters_impl(
    first, last, count, value, cmp);
}

template<::std::ranges::forward_range R, typename T,
  typename Cmp = ::fast_io::algorithms::equal_to>
inline constexpr ::std::ranges::iterator_t<R> search_n(
  R &&r, ::std::size_t count, T const &value, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::search_n_ranges_no_throw<R, T, Cmp>)
{
  return ::fast_io::algorithms::search_n_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), count, value, cmp);
}

template<::std::ranges::random_access_range R, typename T,
  typename Cmp = ::fast_io::algorithms::equal_to>
inline constexpr ::std::size_t search_n_index_range(
  R &&r, ::std::size_t firstidx, ::std::size_t lastidx,
  ::std::size_t count, T const &value, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::search_n_index_range_no_throw<R, T, Cmp>)
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R>(r), firstidx, lastidx);
  auto const found{::fast_io::algorithms::search_n_iters(
    first, last, count, value, cmp)};
  return firstidx + static_cast<::std::size_t>(found - first);
}

template<::std::ranges::random_access_range R, typename T,
  typename Cmp = ::fast_io::algorithms::equal_to>
inline constexpr ::std::size_t search_n_index_of(
  R &&r, ::std::size_t count, T const &value, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::search_n_ranges_no_throw<R, T, Cmp>)
{
  auto const first{::std::ranges::begin(r)};
  auto const found{::fast_io::algorithms::search_n(
    ::std::forward<R>(r), count, value, cmp)};
  return static_cast<::std::size_t>(found - first);
}

} // namespace fast_io::algorithms
