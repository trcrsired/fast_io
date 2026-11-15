#pragma once

namespace fast_io::details::algorithms
{

template<typename Iter, typename T>
concept iota_iter_no_throw =
  ::fast_io::details::algorithms::iter_no_throw<Iter> &&
  ::std::is_nothrow_move_constructible_v<T> &&
  requires(Iter &first, T &value)
{
  { *first = value } noexcept;
  { ++value } noexcept;
};

template<typename Iter, typename Snt, typename T>
concept iota_iters_no_throw =
  ::fast_io::details::algorithms::sentinel_no_throw<Iter, Snt> &&
  ::fast_io::details::algorithms::iota_iter_no_throw<Iter, T>;

template<typename R, typename T>
concept iota_ranges_no_throw =
  ::std::ranges::range<R> &&
  ::fast_io::details::algorithms::iota_iter_no_throw<
    ::std::ranges::iterator_t<R>, T>;

template<typename R, typename T>
concept iota_index_range_no_throw =
  ::std::ranges::random_access_range<R> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R> &&
  ::fast_io::details::algorithms::iota_iter_no_throw<
    ::fast_io::details::algorithms::algo_index_iterator<R>, T>;

template<typename Iter, typename Snt, typename T>
inline constexpr T iota_iters_impl(Iter first, Snt last, T value)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::iota_iters_no_throw<Iter, Snt, T>)
{
  if constexpr (::std::contiguous_iterator<Iter> && !::std::is_pointer_v<Iter> &&
    ::std::contiguous_iterator<Snt>)
  {
    return ::fast_io::details::algorithms::iota_iters_impl(
      ::std::to_address(first), ::std::to_address(last), value);
  }
  else if constexpr (::std::contiguous_iterator<Iter> && !::std::is_pointer_v<Iter> &&
    !::std::contiguous_iterator<Snt> && ::std::sized_sentinel_for<Snt, Iter>)
  {
    auto const count{last - first};
    auto const ptr{::std::to_address(first)};
    return ::fast_io::details::algorithms::iota_iters_impl(
      ptr, ptr + count, value);
  }
  else
  {
    for (; first != last; ++first)
    {
      *first = value;
      ++value;
    }
    return value;
  }
}

} // namespace fast_io::details::algorithms

namespace fast_io::algorithms
{

// Write `value`, `++value`, ... into `[first, last)` and return the value after
// the last increment. An empty range returns `value` unchanged.
template<::std::input_iterator Iter, ::std::sentinel_for<Iter> Snt, typename T>
inline constexpr T iota_iters(Iter first, Snt last, T value)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::iota_iters_no_throw<Iter, Snt, T>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first)>),
    iota_iters_define, first, last, value);
  return ::fast_io::details::algorithms::iota_iters_impl(first, last, value);
}

// Write consecutive values over the whole range.
template<::std::ranges::range R, typename T>
inline constexpr T iota(R &&r, T value)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::iota_ranges_no_throw<R, T>)
{
  return ::fast_io::algorithms::iota_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), value);
}

// Write consecutive values over the checked subrange `[firstidx, lastidx)`.
template<::std::ranges::random_access_range R, typename T>
inline constexpr T iota_index_range(
  R &&r, ::std::size_t firstidx, ::std::size_t lastidx, T value)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::iota_index_range_no_throw<R, T>)
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R>(r), firstidx, lastidx);
  return ::fast_io::algorithms::iota_iters(first, last, value);
}

} // namespace fast_io::algorithms
