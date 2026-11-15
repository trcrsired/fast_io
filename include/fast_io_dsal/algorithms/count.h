#pragma once

namespace fast_io::details::algorithms
{

template<typename Iter, typename T>
concept count_iter_no_throw =
  ::fast_io::details::algorithms::iter_no_throw<Iter> &&
  requires(Iter &it, T const &value)
{
  { *it == value } noexcept;
};

template<typename Iter, typename Snt, typename T>
concept count_iters_no_throw =
  ::fast_io::details::algorithms::sentinel_no_throw<Iter, Snt> &&
  ::fast_io::details::algorithms::count_iter_no_throw<Iter, T>;

template<typename R, typename T>
concept count_ranges_no_throw =
  ::std::ranges::input_range<R> &&
  ::fast_io::details::algorithms::count_iter_no_throw<
    ::std::ranges::iterator_t<R>, T>;

template<typename R, typename T>
concept count_index_range_no_throw =
  ::std::ranges::random_access_range<R> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R> &&
  ::fast_io::details::algorithms::count_iter_no_throw<
    ::fast_io::details::algorithms::algo_index_iterator<R>, T>;

template<typename Iter, typename F>
concept count_if_iter_no_throw =
  ::fast_io::details::algorithms::iter_no_throw<Iter> &&
  requires(Iter &it, F &f)
{
  { static_cast<bool>(f(*it)) } noexcept;
};

template<typename Iter, typename Snt, typename F>
concept count_if_iters_no_throw =
  ::fast_io::details::algorithms::sentinel_no_throw<Iter, Snt> &&
  ::fast_io::details::algorithms::count_if_iter_no_throw<Iter, F>;

template<typename R, typename F>
concept count_if_ranges_no_throw =
  ::std::ranges::input_range<R> &&
  ::std::is_nothrow_move_constructible_v<F> &&
  ::fast_io::details::algorithms::count_if_iter_no_throw<
    ::std::ranges::iterator_t<R>, F>;

template<typename R, typename F>
concept count_if_index_range_no_throw =
  ::std::ranges::random_access_range<R> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R> &&
  ::std::is_nothrow_move_constructible_v<F> &&
  ::fast_io::details::algorithms::count_if_iter_no_throw<
    ::fast_io::details::algorithms::algo_index_iterator<R>, F>;

template<typename Iter, typename Snt, typename T>
inline constexpr ::std::iter_difference_t<Iter> count_iters_impl(
  Iter first, Snt last, T const &value)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::count_iters_no_throw<Iter, Snt, T>)
{
  ::std::iter_difference_t<Iter> result{};
  for (; first != last; ++first)
  {
    if (*first == value)
    {
      ++result;
    }
  }
  return result;
}

template<typename Iter, typename Snt, typename F>
inline constexpr ::std::iter_difference_t<Iter> count_if_iters_impl(
  Iter first, Snt last, F &f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::count_if_iters_no_throw<Iter, Snt, F>)
{
  ::std::iter_difference_t<Iter> result{};
  for (; first != last; ++first)
  {
    if (static_cast<bool>(f(*first)))
    {
      ++result;
    }
  }
  return result;
}

} // namespace fast_io::details::algorithms

namespace fast_io::algorithms
{

// Count elements equal to `value`.
template<::std::input_iterator Iter, ::std::sentinel_for<Iter> Snt, typename T>
inline constexpr ::std::iter_difference_t<Iter> count_iters(
  Iter first, Snt last, T const &value)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::count_iters_no_throw<Iter, Snt, T>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first)>),
    count_iters_define, first, last, value);
  return ::fast_io::details::algorithms::count_iters_impl(
    first, last, value);
}

template<::std::ranges::input_range R, typename T>
inline constexpr ::std::ranges::range_difference_t<R> count(
  R &&r, T const &value)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::count_ranges_no_throw<R, T>)
{
  return ::fast_io::algorithms::count_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), value);
}

template<::std::ranges::random_access_range R, typename T>
inline constexpr ::std::size_t count_index_range(
  R &&r, ::std::size_t firstidx, ::std::size_t lastidx, T const &value)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::count_index_range_no_throw<R, T>)
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R>(r), firstidx, lastidx);
  return static_cast<::std::size_t>(
    ::fast_io::algorithms::count_iters(first, last, value));
}

// Count elements for which `f` returns true.
template<::std::input_iterator Iter, ::std::sentinel_for<Iter> Snt,
  typename F>
inline constexpr ::std::iter_difference_t<Iter> count_if_iters(
  Iter first, Snt last, F f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::count_if_iters_no_throw<Iter, Snt, F>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first)>),
    count_if_iters_define, first, last, f);
  return ::fast_io::details::algorithms::count_if_iters_impl(
    first, last, f);
}

template<::std::ranges::input_range R, typename F>
inline constexpr ::std::ranges::range_difference_t<R> count_if(R &&r, F f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::count_if_ranges_no_throw<R, F>)
{
  return ::fast_io::algorithms::count_if_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), ::std::move(f));
}

template<::std::ranges::random_access_range R, typename F>
inline constexpr ::std::size_t count_if_index_range(
  R &&r, ::std::size_t firstidx, ::std::size_t lastidx, F f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::count_if_index_range_no_throw<R, F>)
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R>(r), firstidx, lastidx);
  return static_cast<::std::size_t>(
    ::fast_io::algorithms::count_if_iters(
      first, last, ::std::move(f)));
}

} // namespace fast_io::algorithms
