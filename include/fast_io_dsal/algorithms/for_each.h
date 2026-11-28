#pragma once

namespace fast_io::details::algorithms
{

template<typename Iter, typename F>
concept for_each_iter_no_throw =
  ::fast_io::details::algorithms::iter_no_throw<Iter> &&
  requires(Iter& first, F& f)
{
  { f(*first) } noexcept;
};

template<typename Iter, typename Snt, typename F>
concept for_each_iters_no_throw =
  ::fast_io::details::algorithms::sentinel_no_throw<Iter, Snt> &&
  ::fast_io::details::algorithms::for_each_iter_no_throw<Iter, F>;

template<typename R, typename F>
concept ranges_no_throw =
  ::std::ranges::range<R> &&
  ::fast_io::details::algorithms::iter_no_throw<::std::ranges::iterator_t<R>> &&
  ::std::is_nothrow_move_constructible_v<F> &&
  ::fast_io::details::algorithms::for_each_iter_no_throw<
    ::std::ranges::iterator_t<R>, F>;

template<typename R, typename F>
concept for_each_index_range_no_throw =
  ::std::ranges::random_access_range<R> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R> &&
  ::std::is_nothrow_move_constructible_v<F> &&
  ::fast_io::details::algorithms::for_each_iter_no_throw<
    ::fast_io::details::algorithms::algo_index_iterator<R>, F>;

template<typename Iter, typename F>
concept for_each_n_iters_no_throw =
  ::fast_io::details::algorithms::iter_no_throw<Iter> &&
  ::fast_io::details::algorithms::for_each_iter_no_throw<Iter, F>;

template<typename R, typename F>
concept for_each_n_ranges_no_throw =
  ::std::ranges::range<R> &&
  ::std::is_nothrow_move_constructible_v<F> &&
  ::fast_io::details::algorithms::for_each_iter_no_throw<
    ::std::ranges::iterator_t<R>, F>;

template<::std::input_iterator Iter, ::std::sentinel_for<Iter> Snt, typename F>
FAST_IO_UNSAFE_API
inline constexpr void for_each_iters_impl(Iter first, Snt last, F& f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::details::algorithms::for_each_iters_no_throw<Iter, Snt, F>)
{
  if constexpr(::std::contiguous_iterator<Iter> && !::std::is_pointer_v<Iter> &&
    ::std::contiguous_iterator<Snt>)
  {
    ::fast_io::details::algorithms::for_each_iters_impl(
      ::std::to_address(first), ::std::to_address(last), f);
  }
  else if constexpr(::std::contiguous_iterator<Iter> && !::std::is_pointer_v<Iter> &&
    !::std::contiguous_iterator<Snt> && ::std::sized_sentinel_for<Snt, Iter>)
  {
    auto const count{last - first};
    auto const ptr{::std::to_address(first)};
    ::fast_io::details::algorithms::for_each_iters_impl(ptr, ptr + count, f);
  }
  else if constexpr(requires{
    for_each_iters_define(first, last, f);
  })
  {
    for_each_iters_define(first, last, f);
  }
  else
  {
    for(;first!=last;++first)
    {
      f(*first);
    }
  }
}

template<typename Iter, typename F>
FAST_IO_UNSAFE_API
inline constexpr Iter for_each_n_iters_impl(
  Iter first, ::std::size_t count, F &f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::for_each_n_iters_no_throw<Iter, F>)
{
  for (; count != 0; --count, ++first)
  {
    f(*first);
  }
  return first;
}

}

namespace fast_io::algorithms
{
template<::std::input_iterator Iter, ::std::sentinel_for<Iter> Snt, typename F>
FAST_IO_UNSAFE_API
inline constexpr void for_each_iters(Iter first, Snt last, F f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::details::algorithms::for_each_iters_no_throw<Iter, Snt, F>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first)>),
    for_each_iters_define, first, last, f);
  ::fast_io::details::algorithms::for_each_iters_impl(first, last, f);
}

template<::std::ranges::range R, typename F>
inline constexpr void for_each(R&& r, F f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::details::algorithms::ranges_no_throw<R, F>)
{
  ::fast_io::algorithms::for_each_iters(::std::ranges::begin(r), ::std::ranges::end(r), ::std::move(f));
}

template<::std::ranges::random_access_range R, typename F>
inline constexpr void for_each_index_range(R&& r, ::std::size_t firstidx, ::std::size_t lastidx, F f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::details::algorithms::for_each_index_range_no_throw<R, F>)
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(::std::forward<R>(r), firstidx, lastidx);
  ::fast_io::algorithms::for_each_iters(first, last, ::std::move(f));
}

// Invoke `f` on the first `count` elements starting at `first` and return the
// iterator one past the last visited element.
template<::std::input_iterator Iter, typename F>
FAST_IO_UNSAFE_API
inline constexpr Iter for_each_n_iters(Iter first, ::std::size_t count, F f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::for_each_n_iters_no_throw<Iter, F>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first)>),
    for_each_n_iters_define, first, count, f);
  return ::fast_io::details::algorithms::for_each_n_iters_impl(
    first, count, f);
}

template<::std::ranges::range R, typename F>
FAST_IO_UNSAFE_API
inline constexpr ::std::ranges::iterator_t<R> for_each_n(
  R &&r, ::std::size_t count, F f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::for_each_n_ranges_no_throw<R, F>)
{
  return ::fast_io::algorithms::for_each_n_iters(
    ::std::ranges::begin(r), count, ::std::move(f));
}

}
