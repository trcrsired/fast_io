#pragma once

namespace fast_io::details::algorithms
{

template<typename Cmp, typename T, typename U>
concept binary_search_compare_no_throw =
  ::fast_io::details::algorithms::comparator_pass_no_throw<Cmp> &&
  requires(Cmp cmp, T const &a, U const &b)
{
  { 0 > cmp.compare(a, b) } noexcept;
  { 0 > cmp.compare(b, a) } noexcept;
};

template<typename Iter, typename Snt, typename T, typename Cmp>
concept lower_bound_iters_no_throw =
  ::fast_io::details::algorithms::sentinel_no_throw<Iter, Snt> &&
  ::fast_io::details::algorithms::binary_search_compare_no_throw<
    Cmp, ::std::iter_reference_t<Iter>, T>;

template<typename R, typename T, typename Cmp>
concept lower_bound_ranges_no_throw =
  ::std::ranges::forward_range<R> &&
  ::std::is_nothrow_move_constructible_v<Cmp> &&
  ::fast_io::details::algorithms::lower_bound_iters_no_throw<
    ::std::ranges::iterator_t<R>, ::std::ranges::sentinel_t<R>, T, Cmp>;

template<typename R, typename T, typename Cmp>
concept lower_bound_index_range_no_throw =
  ::std::ranges::random_access_range<R> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R> &&
  ::fast_io::details::algorithms::lower_bound_iters_no_throw<
    ::fast_io::details::algorithms::algo_index_iterator<R>,
    ::fast_io::details::algorithms::algo_index_iterator<R>, T, Cmp>;

template<typename Iter, typename Snt, typename T, typename Cmp>
inline constexpr Iter lower_bound_iters_impl(
  Iter first, Snt last, T const &value, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::lower_bound_iters_no_throw<
      Iter, Snt, T, Cmp>)
{
  using difference_type = ::std::iter_difference_t<Iter>;
  difference_type length{};
  for (Iter it{first}; it != last; ++it)
  {
    ++length;
  }
  while (length > 0)
  {
    difference_type half{length / 2};
    Iter it{first};
    for (difference_type i{}; i != half; ++i)
    {
      ++it;
    }
    if (0 > cmp.compare(*it, value))
    {
      first = ++it;
      length -= half + 1;
    }
    else
    {
      length = half;
    }
  }
  return first;
}

template<typename Iter, typename Snt, typename T, typename Cmp>
inline constexpr Iter upper_bound_iters_impl(
  Iter first, Snt last, T const &value, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::lower_bound_iters_no_throw<
      Iter, Snt, T, Cmp>)
{
  using difference_type = ::std::iter_difference_t<Iter>;
  difference_type length{};
  for (Iter it{first}; it != last; ++it)
  {
    ++length;
  }
  while (length > 0)
  {
    difference_type half{length / 2};
    Iter it{first};
    for (difference_type i{}; i != half; ++i)
    {
      ++it;
    }
    if (!(0 > cmp.compare(value, *it)))
    {
      first = ++it;
      length -= half + 1;
    }
    else
    {
      length = half;
    }
  }
  return first;
}

} // namespace fast_io::details::algorithms

namespace fast_io::algorithms
{

template<::std::forward_iterator Iter, ::std::sentinel_for<Iter> Snt,
  typename T, typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr Iter lower_bound_iters(
  Iter first, Snt last, T const &value, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::lower_bound_iters_no_throw<
      Iter, Snt, T, Cmp>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first)>),
    lower_bound_iters_define, first, last, value, cmp);
  return ::fast_io::details::algorithms::lower_bound_iters_impl(
    first, last, value, cmp);
}

template<::std::ranges::forward_range R, typename T,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr ::std::ranges::iterator_t<R> lower_bound(
  R &&r, T const &value, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::lower_bound_ranges_no_throw<
      R, T, Cmp>)
{
  return ::fast_io::algorithms::lower_bound_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), value, cmp);
}

template<::std::ranges::random_access_range R, typename T,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr ::std::size_t lower_bound_index_range(
  R &&r, ::std::size_t firstidx, ::std::size_t lastidx,
  T const &value, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::lower_bound_index_range_no_throw<
      R, T, Cmp>)
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R>(r), firstidx, lastidx);
  auto const result{::fast_io::algorithms::lower_bound_iters(
    first, last, value, cmp)};
  return firstidx + static_cast<::std::size_t>(result - first);
}

template<::std::ranges::random_access_range R, typename T,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr ::std::size_t lower_bound_index_of(
  R &&r, T const &value, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::lower_bound_ranges_no_throw<
      R, T, Cmp>)
{
  auto const first{::std::ranges::begin(r)};
  auto const result{::fast_io::algorithms::lower_bound(
    ::std::forward<R>(r), value, cmp)};
  return static_cast<::std::size_t>(result - first);
}

template<::std::forward_iterator Iter, ::std::sentinel_for<Iter> Snt,
  typename T, typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr Iter upper_bound_iters(
  Iter first, Snt last, T const &value, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::lower_bound_iters_no_throw<
      Iter, Snt, T, Cmp>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first)>),
    upper_bound_iters_define, first, last, value, cmp);
  return ::fast_io::details::algorithms::upper_bound_iters_impl(
    first, last, value, cmp);
}

template<::std::ranges::forward_range R, typename T,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr ::std::ranges::iterator_t<R> upper_bound(
  R &&r, T const &value, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::lower_bound_ranges_no_throw<
      R, T, Cmp>)
{
  return ::fast_io::algorithms::upper_bound_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), value, cmp);
}

template<::std::ranges::random_access_range R, typename T,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr ::std::size_t upper_bound_index_range(
  R &&r, ::std::size_t firstidx, ::std::size_t lastidx,
  T const &value, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::lower_bound_index_range_no_throw<
      R, T, Cmp>)
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R>(r), firstidx, lastidx);
  auto const result{::fast_io::algorithms::upper_bound_iters(
    first, last, value, cmp)};
  return firstidx + static_cast<::std::size_t>(result - first);
}

template<::std::ranges::random_access_range R, typename T,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr ::std::size_t upper_bound_index_of(
  R &&r, T const &value, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::lower_bound_ranges_no_throw<
      R, T, Cmp>)
{
  auto const first{::std::ranges::begin(r)};
  auto const result{::fast_io::algorithms::upper_bound(
    ::std::forward<R>(r), value, cmp)};
  return static_cast<::std::size_t>(result - first);
}

template<::std::forward_iterator Iter, ::std::sentinel_for<Iter> Snt,
  typename T, typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr ::std::pair<Iter, Iter> equal_range_iters(
  Iter first, Snt last, T const &value, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::lower_bound_iters_no_throw<
      Iter, Snt, T, Cmp>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first)>),
    equal_range_iters_define, first, last, value, cmp);
  auto const lower{::fast_io::details::algorithms::lower_bound_iters_impl(
    first, last, value, cmp)};
  return {
    lower,
    ::fast_io::details::algorithms::upper_bound_iters_impl(
      lower, last, value, cmp)};
}

template<::std::ranges::forward_range R, typename T,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr ::std::pair<
  ::std::ranges::iterator_t<R>, ::std::ranges::iterator_t<R>>
equal_range(R &&r, T const &value, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::lower_bound_ranges_no_throw<
      R, T, Cmp>)
{
  return ::fast_io::algorithms::equal_range_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), value, cmp);
}

template<::std::ranges::random_access_range R, typename T,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr ::std::pair<::std::size_t, ::std::size_t>
equal_range_index_range(
  R &&r, ::std::size_t firstidx, ::std::size_t lastidx,
  T const &value, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::lower_bound_index_range_no_throw<
      R, T, Cmp>)
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R>(r), firstidx, lastidx);
  auto const result{::fast_io::algorithms::equal_range_iters(
    first, last, value, cmp)};
  return {
    firstidx + static_cast<::std::size_t>(result.first - first),
    firstidx + static_cast<::std::size_t>(result.second - first)};
}

template<::std::ranges::random_access_range R, typename T,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr ::std::pair<::std::size_t, ::std::size_t>
equal_range_index_of(R &&r, T const &value, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::lower_bound_ranges_no_throw<
      R, T, Cmp>)
{
  auto const first{::std::ranges::begin(r)};
  auto const result{::fast_io::algorithms::equal_range(
    ::std::forward<R>(r), value, cmp)};
  return {
    static_cast<::std::size_t>(result.first - first),
    static_cast<::std::size_t>(result.second - first)};
}

template<::std::forward_iterator Iter, ::std::sentinel_for<Iter> Snt,
  typename T, typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr bool binary_search_iters(
  Iter first, Snt last, T const &value, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::lower_bound_iters_no_throw<
      Iter, Snt, T, Cmp>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first)>),
    binary_search_iters_define, first, last, value, cmp);
  auto const found{
    ::fast_io::details::algorithms::lower_bound_iters_impl(
      first, last, value, cmp)};
  return found != last && !(0 > cmp.compare(value, *found));
}

template<::std::ranges::forward_range R, typename T,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr bool binary_search(
  R &&r, T const &value, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::lower_bound_ranges_no_throw<
      R, T, Cmp>)
{
  return ::fast_io::algorithms::binary_search_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), value, cmp);
}

template<::std::ranges::random_access_range R, typename T,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr bool binary_search_index_range(
  R &&r, ::std::size_t firstidx, ::std::size_t lastidx,
  T const &value, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::lower_bound_index_range_no_throw<
      R, T, Cmp>)
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R>(r), firstidx, lastidx);
  return ::fast_io::algorithms::binary_search_iters(
    first, last, value, cmp);
}

} // namespace fast_io::algorithms
