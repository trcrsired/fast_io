#pragma once

namespace fast_io::details::algorithms
{

template<typename Iter, typename F>
concept all_of_iter_no_throw =
  ::fast_io::details::algorithms::iter_no_throw<Iter> &&
  requires(Iter &it, F &f)
{
  { static_cast<bool>(f(*it)) } noexcept;
};

template<typename Iter, typename Snt, typename F>
concept all_of_iters_no_throw =
  ::fast_io::details::algorithms::sentinel_no_throw<Iter, Snt> &&
  ::fast_io::details::algorithms::all_of_iter_no_throw<Iter, F>;

template<typename R, typename F>
concept all_of_ranges_no_throw =
  ::std::ranges::input_range<R> &&
  ::std::is_nothrow_move_constructible_v<F> &&
  ::fast_io::details::algorithms::all_of_iter_no_throw<
    ::std::ranges::iterator_t<R>, F>;

template<typename R, typename F>
concept all_of_index_range_no_throw =
  ::std::ranges::random_access_range<R> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R> &&
  ::std::is_nothrow_move_constructible_v<F> &&
  ::fast_io::details::algorithms::all_of_iter_no_throw<
    ::fast_io::details::algorithms::algo_index_iterator<R>, F>;

template<typename Iter, typename Snt, typename F>
inline constexpr bool all_of_iters_impl(Iter first, Snt last, F &f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::all_of_iters_no_throw<Iter, Snt, F>)
{
  for (; first != last; ++first)
  {
    if (!static_cast<bool>(f(*first)))
    {
      return false;
    }
  }
  return true;
}

template<typename Iter, typename Snt, typename F>
inline constexpr bool any_of_iters_impl(Iter first, Snt last, F &f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::all_of_iters_no_throw<Iter, Snt, F>)
{
  for (; first != last; ++first)
  {
    if (static_cast<bool>(f(*first)))
    {
      return true;
    }
  }
  return false;
}

template<typename Iter, typename Snt, typename F>
inline constexpr bool none_of_iters_impl(Iter first, Snt last, F &f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::all_of_iters_no_throw<Iter, Snt, F>)
{
  for (; first != last; ++first)
  {
    if (static_cast<bool>(f(*first)))
    {
      return false;
    }
  }
  return true;
}

} // namespace fast_io::details::algorithms

namespace fast_io::algorithms
{

// True when `f` returns true for every element of `[first, last)`.
template<::std::input_iterator Iter, ::std::sentinel_for<Iter> Snt, typename F>
inline constexpr bool all_of_iters(Iter first, Snt last, F f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::all_of_iters_no_throw<Iter, Snt, F>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first)>),
    all_of_iters_define, first, last, f);
  return ::fast_io::details::algorithms::all_of_iters_impl(first, last, f);
}

template<::std::ranges::input_range R, typename F>
inline constexpr bool all_of(R &&r, F f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::all_of_ranges_no_throw<R, F>)
{
  return ::fast_io::algorithms::all_of_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), ::std::move(f));
}

template<::std::ranges::random_access_range R, typename F>
inline constexpr bool all_of_index_range(
  R &&r, ::std::size_t firstidx, ::std::size_t lastidx, F f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::all_of_index_range_no_throw<R, F>)
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R>(r), firstidx, lastidx);
  return ::fast_io::algorithms::all_of_iters(
    first, last, ::std::move(f));
}

// True when `f` returns true for at least one element of `[first, last)`.
template<::std::input_iterator Iter, ::std::sentinel_for<Iter> Snt, typename F>
inline constexpr bool any_of_iters(Iter first, Snt last, F f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::all_of_iters_no_throw<Iter, Snt, F>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first)>),
    any_of_iters_define, first, last, f);
  return ::fast_io::details::algorithms::any_of_iters_impl(first, last, f);
}

template<::std::ranges::input_range R, typename F>
inline constexpr bool any_of(R &&r, F f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::all_of_ranges_no_throw<R, F>)
{
  return ::fast_io::algorithms::any_of_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), ::std::move(f));
}

template<::std::ranges::random_access_range R, typename F>
inline constexpr bool any_of_index_range(
  R &&r, ::std::size_t firstidx, ::std::size_t lastidx, F f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::all_of_index_range_no_throw<R, F>)
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R>(r), firstidx, lastidx);
  return ::fast_io::algorithms::any_of_iters(
    first, last, ::std::move(f));
}

// True when `f` returns false for every element of `[first, last)`.
template<::std::input_iterator Iter, ::std::sentinel_for<Iter> Snt, typename F>
inline constexpr bool none_of_iters(Iter first, Snt last, F f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::all_of_iters_no_throw<Iter, Snt, F>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first)>),
    none_of_iters_define, first, last, f);
  return ::fast_io::details::algorithms::none_of_iters_impl(first, last, f);
}

template<::std::ranges::input_range R, typename F>
inline constexpr bool none_of(R &&r, F f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::all_of_ranges_no_throw<R, F>)
{
  return ::fast_io::algorithms::none_of_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), ::std::move(f));
}

template<::std::ranges::random_access_range R, typename F>
inline constexpr bool none_of_index_range(
  R &&r, ::std::size_t firstidx, ::std::size_t lastidx, F f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::all_of_index_range_no_throw<R, F>)
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R>(r), firstidx, lastidx);
  return ::fast_io::algorithms::none_of_iters(
    first, last, ::std::move(f));
}

} // namespace fast_io::algorithms
