#pragma once

namespace fast_io::details::algorithms
{

template<typename Iter, typename Snt>
concept shift_left_iters_no_throw =
  ::fast_io::details::algorithms::sentinel_no_throw<Iter, Snt> &&
  requires(Iter &dest, Iter &src)
{
  { *dest = ::std::move(*src) } noexcept;
};

template<typename Iter, typename Snt>
concept shift_right_iters_no_throw =
  ::fast_io::details::algorithms::sentinel_no_throw<Iter, Snt> &&
  ::fast_io::details::algorithms::decrement_iter_no_throw<Iter> &&
  requires(Iter &dest, Iter &src)
{
  { *dest = ::std::move(*src) } noexcept;
};

template<typename R>
concept shift_left_ranges_no_throw =
  ::std::ranges::forward_range<R> &&
  ::fast_io::details::algorithms::shift_left_iters_no_throw<
    ::std::ranges::iterator_t<R>, ::std::ranges::sentinel_t<R>>;

template<typename R>
concept shift_right_ranges_no_throw =
  ::std::ranges::bidirectional_range<R> &&
  ::fast_io::details::algorithms::shift_right_iters_no_throw<
    ::std::ranges::iterator_t<R>, ::std::ranges::sentinel_t<R>>;

template<typename R>
concept shift_left_index_range_no_throw =
  ::std::ranges::random_access_range<R> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R> &&
  ::fast_io::details::algorithms::shift_left_iters_no_throw<
    ::fast_io::details::algorithms::algo_index_iterator<R>,
    ::fast_io::details::algorithms::algo_index_iterator<R>>;

template<typename R>
concept shift_right_index_range_no_throw =
  ::std::ranges::random_access_range<R> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R> &&
  ::fast_io::details::algorithms::shift_right_iters_no_throw<
    ::fast_io::details::algorithms::algo_index_iterator<R>,
    ::fast_io::details::algorithms::algo_index_iterator<R>>;

template<typename Iter, typename Snt>
inline constexpr Iter shift_left_iters_impl(
  Iter first, Snt last, ::std::iter_difference_t<Iter> n)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::shift_left_iters_no_throw<
      Iter, Snt>)
{
  using difference_type = ::std::iter_difference_t<Iter>;
  if (n <= 0)
  {
    Iter last_iter{first};
    for (; last_iter != last; ++last_iter)
    {
    }
    return last_iter;
  }
  Iter src{first};
  for (difference_type i{}; i != n; ++i)
  {
    if (src == last)
    {
      return first;
    }
    ++src;
  }
  Iter dest{first};
  for (; src != last; ++src, ++dest)
  {
    *dest = ::std::move(*src);
  }
  return dest;
}

template<typename Iter, typename Snt>
inline constexpr Iter shift_left_iters(
  Iter first, Snt last, ::std::iter_difference_t<Iter> n)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::shift_left_iters_no_throw<
      Iter, Snt>)
{
  return ::fast_io::details::algorithms::shift_left_iters_impl(
    first, last, n);
}

template<typename Iter, typename Snt>
inline constexpr Iter shift_right_iters_impl(
  Iter first, Snt last, ::std::iter_difference_t<Iter> n)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::shift_right_iters_no_throw<
      Iter, Snt>)
{
  using difference_type = ::std::iter_difference_t<Iter>;
  if (n <= 0)
  {
    return first;
  }
  Iter last_iter{first};
  difference_type length{};
  for (; last_iter != last; ++last_iter, ++length)
  {
  }
  if (n >= length)
  {
    return last_iter;
  }
  Iter new_first{first};
  for (difference_type i{}; i != n; ++i)
  {
    ++new_first;
  }
  Iter src{last_iter};
  for (difference_type i{}; i != n; ++i)
  {
    --src;
  }
  Iter dest{last_iter};
  while (src != first)
  {
    --src;
    --dest;
    *dest = ::std::move(*src);
  }
  return new_first;
}

} // namespace fast_io::details::algorithms

namespace fast_io::algorithms
{

template<::std::forward_iterator Iter, ::std::sentinel_for<Iter> Snt>
inline constexpr Iter shift_left_iters(
  Iter first, Snt last, ::std::iter_difference_t<Iter> n)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::shift_left_iters_no_throw<
      Iter, Snt>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first)>),
    shift_left_iters_define, first, last, n);
  return ::fast_io::details::algorithms::shift_left_iters_impl(
    first, last, n);
}

template<::std::ranges::forward_range R>
inline constexpr ::std::ranges::iterator_t<R> shift_left(
  R &&r, ::std::ranges::range_difference_t<R> n)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::shift_left_ranges_no_throw<R>)
{
  return ::fast_io::algorithms::shift_left_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), n);
}

template<::std::ranges::random_access_range R>
inline constexpr ::std::size_t shift_left_index_range(
  R &&r, ::std::size_t firstidx, ::std::size_t lastidx,
  ::std::size_t n)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::shift_left_index_range_no_throw<R>)
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R>(r), firstidx, lastidx);
  auto const result{::fast_io::algorithms::shift_left_iters(
    first, last,
    static_cast<::std::iter_difference_t<
      ::fast_io::details::algorithms::algo_index_iterator<R>>>(n))};
  return firstidx + static_cast<::std::size_t>(result - first);
}

template<::std::ranges::random_access_range R>
inline constexpr ::std::size_t shift_left_index_of(
  R &&r, ::std::ranges::range_difference_t<R> n)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::shift_left_ranges_no_throw<R>)
{
  auto const first{::std::ranges::begin(r)};
  auto const result{::fast_io::algorithms::shift_left(
    ::std::forward<R>(r), n)};
  return static_cast<::std::size_t>(result - first);
}

template<::std::bidirectional_iterator Iter, ::std::sentinel_for<Iter> Snt>
inline constexpr Iter shift_right_iters(
  Iter first, Snt last, ::std::iter_difference_t<Iter> n)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::shift_right_iters_no_throw<
      Iter, Snt>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first)>),
    shift_right_iters_define, first, last, n);
  return ::fast_io::details::algorithms::shift_right_iters_impl(
    first, last, n);
}

template<::std::ranges::bidirectional_range R>
inline constexpr ::std::ranges::iterator_t<R> shift_right(
  R &&r, ::std::ranges::range_difference_t<R> n)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::shift_right_ranges_no_throw<R>)
{
  return ::fast_io::algorithms::shift_right_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), n);
}

template<::std::ranges::random_access_range R>
inline constexpr ::std::size_t shift_right_index_range(
  R &&r, ::std::size_t firstidx, ::std::size_t lastidx,
  ::std::size_t n)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::shift_right_index_range_no_throw<R>)
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R>(r), firstidx, lastidx);
  auto const result{::fast_io::algorithms::shift_right_iters(
    first, last,
    static_cast<::std::iter_difference_t<
      ::fast_io::details::algorithms::algo_index_iterator<R>>>(n))};
  return firstidx + static_cast<::std::size_t>(result - first);
}

template<::std::ranges::random_access_range R>
inline constexpr ::std::size_t shift_right_index_of(
  R &&r, ::std::ranges::range_difference_t<R> n)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::shift_right_ranges_no_throw<R>)
{
  auto const first{::std::ranges::begin(r)};
  auto const result{::fast_io::algorithms::shift_right(
    ::std::forward<R>(r), n)};
  return static_cast<::std::size_t>(result - first);
}

} // namespace fast_io::algorithms
