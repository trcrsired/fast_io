#pragma once

namespace fast_io::details::algorithms
{

template<typename Iter, typename Cmp>
concept is_sorted_iters_no_throw =
  ::fast_io::details::algorithms::iter_no_throw<Iter> &&
  ::fast_io::details::algorithms::comparator_pass_no_throw<Cmp> &&
  ::fast_io::details::algorithms::three_way_comparator_for<
    Cmp, ::std::iter_reference_t<Iter>>;

template<typename Iter, typename Snt, typename Cmp>
concept is_sorted_until_iters_no_throw =
  ::fast_io::details::algorithms::sentinel_no_throw<Iter, Snt> &&
  ::fast_io::details::algorithms::is_sorted_iters_no_throw<Iter, Cmp>;

template<typename R, typename Cmp>
concept is_sorted_ranges_no_throw =
  ::std::ranges::forward_range<R> &&
  ::std::is_nothrow_move_constructible_v<Cmp> &&
  ::fast_io::details::algorithms::is_sorted_until_iters_no_throw<
    ::std::ranges::iterator_t<R>, ::std::ranges::sentinel_t<R>, Cmp>;

template<typename R, typename Cmp>
concept is_sorted_index_range_no_throw =
  ::std::ranges::random_access_range<R> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R> &&
  ::fast_io::details::algorithms::is_sorted_until_iters_no_throw<
    ::fast_io::details::algorithms::algo_index_iterator<R>,
    ::fast_io::details::algorithms::algo_index_iterator<R>, Cmp>;

template<typename Iter, typename Snt, typename Cmp>
FAST_IO_UNSAFE_API
inline constexpr Iter is_sorted_until_iters_impl(
  Iter first, Snt last, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::is_sorted_until_iters_no_throw<
      Iter, Snt, Cmp>)
{
  if (first == last)
  {
    return first;
  }
  Iter next{first};
  ++next;
  while (next != last)
  {
    if (0 > cmp.compare(*next, *first))
    {
      return next;
    }
    ++first;
    ++next;
  }
  return next;
}

} // namespace fast_io::details::algorithms

namespace fast_io::algorithms
{

template<::std::forward_iterator Iter, ::std::sentinel_for<Iter> Snt,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
  requires ::fast_io::details::algorithms::three_way_comparator<
    Cmp, ::std::iter_value_t<Iter>>
FAST_IO_UNSAFE_API
inline constexpr Iter is_sorted_until_iters(
  Iter first, Snt last, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::is_sorted_until_iters_no_throw<
      Iter, Snt, Cmp>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first)>),
    is_sorted_until_iters_define, first, last, cmp);
  return ::fast_io::details::algorithms::is_sorted_until_iters_impl(
    first, last, cmp);
}

template<::std::ranges::forward_range R,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr ::std::ranges::iterator_t<R> is_sorted_until(
  R &&r, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::is_sorted_ranges_no_throw<R, Cmp>)
{
  return ::fast_io::algorithms::is_sorted_until_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), cmp);
}

template<::std::ranges::random_access_range R,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr ::std::size_t is_sorted_until_index_range(
  R &&r, ::std::size_t firstidx, ::std::size_t lastidx,
  Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::is_sorted_index_range_no_throw<R, Cmp>)
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R>(r), firstidx, lastidx);
  auto const result{::fast_io::algorithms::is_sorted_until_iters(
    first, last, cmp)};
  return firstidx + static_cast<::std::size_t>(result - first);
}

template<::std::ranges::random_access_range R,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr ::std::size_t is_sorted_until_index_of(
  R &&r, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::is_sorted_ranges_no_throw<R, Cmp>)
{
  auto const first{::std::ranges::begin(r)};
  auto const result{::fast_io::algorithms::is_sorted_until(
    ::std::forward<R>(r), cmp)};
  return static_cast<::std::size_t>(result - first);
}

template<::std::forward_iterator Iter, ::std::sentinel_for<Iter> Snt,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
  requires ::fast_io::details::algorithms::three_way_comparator<
    Cmp, ::std::iter_value_t<Iter>>
FAST_IO_UNSAFE_API
inline constexpr bool is_sorted_iters(
  Iter first, Snt last, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::is_sorted_until_iters_no_throw<
      Iter, Snt, Cmp>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first)>),
    is_sorted_iters_define, first, last, cmp);
  auto const result{
    ::fast_io::details::algorithms::is_sorted_until_iters_impl(
      first, last, cmp)};
  return result == last;
}

template<::std::ranges::forward_range R,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr bool is_sorted(R &&r, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::is_sorted_ranges_no_throw<R, Cmp>)
{
  return ::fast_io::algorithms::is_sorted_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), cmp);
}

template<::std::ranges::random_access_range R,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr bool is_sorted_index_range(
  R &&r, ::std::size_t firstidx, ::std::size_t lastidx,
  Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::is_sorted_index_range_no_throw<R, Cmp>)
{
  return ::fast_io::algorithms::is_sorted_until_index_range(
    ::std::forward<R>(r), firstidx, lastidx, cmp) == lastidx;
}

} // namespace fast_io::algorithms
