#pragma once

namespace fast_io::details::algorithms
{

template<typename Iter, typename G>
concept generate_iter_no_throw =
  ::fast_io::details::algorithms::iter_no_throw<Iter> &&
  requires(Iter &it, G &g)
{
  { *it = g() } noexcept;
};

template<typename Iter, typename Snt, typename G>
concept generate_iters_no_throw =
  ::fast_io::details::algorithms::sentinel_no_throw<Iter, Snt> &&
  ::fast_io::details::algorithms::generate_iter_no_throw<Iter, G>;

template<typename R, typename G>
concept generate_ranges_no_throw =
  ::std::ranges::range<R> &&
  ::std::is_nothrow_move_constructible_v<G> &&
  ::fast_io::details::algorithms::generate_iter_no_throw<
    ::std::ranges::iterator_t<R>, G>;

template<typename R, typename G>
concept generate_index_range_no_throw =
  ::std::ranges::random_access_range<R> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R> &&
  ::std::is_nothrow_move_constructible_v<G> &&
  ::fast_io::details::algorithms::generate_iter_no_throw<
    ::fast_io::details::algorithms::algo_index_iterator<R>, G>;

template<typename Iter, typename Snt, typename G>
inline constexpr void generate_iters_impl(Iter first, Snt last, G &g)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::generate_iters_no_throw<
      Iter, Snt, G>)
{
  for (; first != last; ++first)
  {
    *first = g();
  }
}

template<typename Iter, typename G>
inline constexpr Iter generate_n_iters_impl(
  Iter first, ::std::size_t count, G &g)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::generate_iter_no_throw<Iter, G>)
{
  for (; count != 0; --count, ++first)
  {
    *first = g();
  }
  return first;
}

} // namespace fast_io::details::algorithms

namespace fast_io::algorithms
{

// Assign the result of `g()` to every element of `[first, last)`.
template<::std::input_iterator Iter, ::std::sentinel_for<Iter> Snt,
  typename G>
inline constexpr void generate_iters(Iter first, Snt last, G g)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::generate_iters_no_throw<
      Iter, Snt, G>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first)>),
    generate_iters_define, first, last, g);
  ::fast_io::details::algorithms::generate_iters_impl(first, last, g);
}

template<::std::ranges::range R, typename G>
inline constexpr void generate(R &&r, G g)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::generate_ranges_no_throw<R, G>)
{
  ::fast_io::algorithms::generate_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), ::std::move(g));
}

template<::std::ranges::random_access_range R, typename G>
inline constexpr void generate_index_range(
  R &&r, ::std::size_t firstidx, ::std::size_t lastidx, G g)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::generate_index_range_no_throw<R, G>)
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R>(r), firstidx, lastidx);
  ::fast_io::algorithms::generate_iters(first, last, ::std::move(g));
}

// Assign the result of `g()` to the first `count` elements.
template<typename Iter, typename G>
  requires ::std::input_or_output_iterator<Iter>
inline constexpr Iter generate_n_iters(Iter first, ::std::size_t count, G g)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::generate_iter_no_throw<Iter, G>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first)>),
    generate_n_iters_define, first, count, g);
  return ::fast_io::details::algorithms::generate_n_iters_impl(
    first, count, g);
}

template<::std::ranges::range R, typename G>
inline constexpr ::std::ranges::iterator_t<R> generate_n(
  R &&r, ::std::size_t count, G g)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::generate_iter_no_throw<
      ::std::ranges::iterator_t<R>, G>)
{
  return ::fast_io::algorithms::generate_n_iters(
    ::std::ranges::begin(r), count, ::std::move(g));
}

} // namespace fast_io::algorithms
