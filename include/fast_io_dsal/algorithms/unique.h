#pragma once

namespace fast_io::details::algorithms
{

template<typename Iter, typename Snt, typename Cmp>
concept unique_iters_no_throw =
  ::fast_io::details::algorithms::sentinel_no_throw<Iter, Snt> &&
  ::fast_io::details::algorithms::comparator_pass_no_throw<Cmp> &&
  ::fast_io::details::algorithms::equal_call_no_throw_v<
    Cmp, ::std::iter_reference_t<Iter>, ::std::iter_reference_t<Iter>> &&
  requires(Iter &dest, Iter &src)
{
  { *dest = ::std::move(*src) } noexcept;
};

template<typename R, typename Cmp>
concept unique_ranges_no_throw =
  ::std::ranges::forward_range<R> &&
  ::std::is_nothrow_move_constructible_v<Cmp> &&
  ::fast_io::details::algorithms::unique_iters_no_throw<
    ::std::ranges::iterator_t<R>, ::std::ranges::sentinel_t<R>, Cmp>;

template<typename R, typename Cmp>
concept unique_index_range_no_throw =
  ::std::ranges::random_access_range<R> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R> &&
  ::fast_io::details::algorithms::unique_iters_no_throw<
    ::fast_io::details::algorithms::algo_index_iterator<R>,
    ::fast_io::details::algorithms::algo_index_iterator<R>, Cmp>;

template<typename Iter, typename Snt, typename OIter, typename Cmp>
concept unique_copy_iters_no_throw =
  ::fast_io::details::algorithms::sentinel_no_throw<Iter, Snt> &&
  ::fast_io::details::algorithms::output_iter_no_throw<
    OIter, ::std::iter_value_t<Iter>> &&
  ::fast_io::details::algorithms::comparator_pass_no_throw<Cmp> &&
  ::fast_io::details::algorithms::equal_call_no_throw_v<
    Cmp, ::std::iter_reference_t<Iter>, ::std::iter_reference_t<Iter>> &&
  requires(Iter &src, OIter &dest)
{
  { *dest = *src } noexcept;
};

template<typename R, typename OIter, typename Cmp>
concept unique_copy_ranges_no_throw =
  ::std::ranges::forward_range<R> &&
  ::std::is_nothrow_move_constructible_v<Cmp> &&
  ::fast_io::details::algorithms::unique_copy_iters_no_throw<
    ::std::ranges::iterator_t<R>, ::std::ranges::sentinel_t<R>,
    OIter, Cmp>;

template<typename Iter, typename Snt, typename Cmp>
FAST_IO_UNSAFE_API
inline constexpr Iter unique_iters_impl(
  Iter first, Snt last, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::unique_iters_no_throw<
      Iter, Snt, Cmp>)
{
  if (first == last)
  {
    return first;
  }
  Iter result{first};
  Iter current{first};
  ++current;
  while (current != last)
  {
    if (!::fast_io::details::algorithms::equal(cmp, *result, *current))
    {
      ++result;
      if (result != current)
      {
        *result = ::std::move(*current);
      }
    }
    ++current;
  }
  ++result;
  return result;
}

template<typename Iter, typename Snt, typename OIter, typename Cmp>
FAST_IO_UNSAFE_API
inline constexpr OIter unique_copy_iters_impl(
  Iter first, Snt last, OIter dest, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::unique_copy_iters_no_throw<
      Iter, Snt, OIter, Cmp>)
{
  if (first == last)
  {
    return dest;
  }
  Iter previous{first};
  *dest = *previous;
  ++dest;
  ++first;
  for (; first != last; ++first)
  {
    if (!::fast_io::details::algorithms::equal(cmp, *previous, *first))
    {
      *dest = *first;
      ++dest;
      previous = first;
    }
  }
  return dest;
}

} // namespace fast_io::details::algorithms

namespace fast_io::algorithms
{

// Remove consecutive equal elements. Returns the new logical end.
template<::std::forward_iterator Iter, ::std::sentinel_for<Iter> Snt,
  typename Cmp = ::fast_io::algorithms::equal_to>
  requires ::fast_io::details::algorithms::equality_comparator<
    Cmp, ::std::iter_value_t<Iter>>
FAST_IO_UNSAFE_API
inline constexpr Iter unique_iters(
  Iter first, Snt last, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::unique_iters_no_throw<
      Iter, Snt, Cmp>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first)>),
    unique_iters_define, first, last, cmp);
  return ::fast_io::details::algorithms::unique_iters_impl(
    first, last, cmp);
}

template<::std::ranges::forward_range R,
  typename Cmp = ::fast_io::algorithms::equal_to>
inline constexpr ::std::ranges::iterator_t<R> unique(
  R &&r, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::unique_ranges_no_throw<R, Cmp>)
{
  return ::fast_io::algorithms::unique_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), cmp);
}

template<::std::ranges::random_access_range R,
  typename Cmp = ::fast_io::algorithms::equal_to>
inline constexpr ::std::size_t unique_index_range(
  R &&r, ::std::size_t firstidx, ::std::size_t lastidx,
  Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::unique_index_range_no_throw<R, Cmp>)
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R>(r), firstidx, lastidx);
  auto const result{::fast_io::algorithms::unique_iters(
    first, last, cmp)};
  return firstidx + static_cast<::std::size_t>(result - first);
}

template<::std::ranges::random_access_range R,
  typename Cmp = ::fast_io::algorithms::equal_to>
inline constexpr ::std::size_t unique_index_of(R &&r, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::unique_ranges_no_throw<R, Cmp>)
{
  auto const first{::std::ranges::begin(r)};
  auto const result{::fast_io::algorithms::unique(
    ::std::forward<R>(r), cmp)};
  return static_cast<::std::size_t>(result - first);
}

// Copy the first element of each run of equal elements.
template<::std::forward_iterator Iter, ::std::sentinel_for<Iter> Snt,
  typename OIter, typename Cmp = ::fast_io::algorithms::equal_to>
  requires ::fast_io::details::algorithms::equality_comparator<
    Cmp, ::std::iter_value_t<Iter>>
FAST_IO_UNSAFE_API
inline constexpr OIter unique_copy_iters(
  Iter first, Snt last, OIter dest, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::unique_copy_iters_no_throw<
      Iter, Snt, OIter, Cmp>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first), decltype(dest)>),
    unique_copy_iters_define, first, last, dest, cmp);
  return ::fast_io::details::algorithms::unique_copy_iters_impl(
    first, last, dest, cmp);
}

template<::std::ranges::forward_range R, typename OIter,
  typename Cmp = ::fast_io::algorithms::equal_to>
FAST_IO_UNSAFE_API
inline constexpr OIter unique_copy(
  R &&r, OIter dest, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::unique_copy_ranges_no_throw<
      R, OIter, Cmp>)
{
  return ::fast_io::algorithms::unique_copy_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), dest, cmp);
}

} // namespace fast_io::algorithms
