#pragma once

namespace fast_io::details::algorithms
{

template<typename Iter, typename Snt, typename OIter, typename F>
concept transform_iters_no_throw =
  ::fast_io::details::algorithms::sentinel_no_throw<Iter, Snt> &&
  ::fast_io::details::algorithms::output_iter_no_throw<
    OIter, ::std::iter_value_t<Iter>> &&
  requires(Iter &it, OIter &dest, F &f)
{
  { *dest = f(*it) } noexcept;
};

template<typename R, typename OIter, typename F>
concept transform_ranges_no_throw =
  ::std::ranges::input_range<R> &&
  ::std::is_nothrow_move_constructible_v<F> &&
  ::fast_io::details::algorithms::transform_iters_no_throw<
    ::std::ranges::iterator_t<R>, ::std::ranges::sentinel_t<R>,
    OIter, F>;

template<typename R, typename OIter, typename F>
concept transform_index_range_no_throw =
  ::std::ranges::random_access_range<R> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R> &&
  ::std::is_nothrow_move_constructible_v<F> &&
  ::fast_io::details::algorithms::transform_iters_no_throw<
    ::fast_io::details::algorithms::algo_index_iterator<R>,
    ::fast_io::details::algorithms::algo_index_iterator<R>, OIter, F>;

template<typename Iter1, typename Snt1, typename Iter2, typename Snt2,
  typename OIter, typename F>
concept transform_binary_iters_no_throw =
  ::fast_io::details::algorithms::sentinel_no_throw<Iter1, Snt1> &&
  ::fast_io::details::algorithms::sentinel_no_throw<Iter2, Snt2> &&
  ::fast_io::details::algorithms::output_iter_no_throw<
    OIter, ::std::iter_value_t<Iter1>> &&
  requires(Iter1 &it1, Iter2 &it2, OIter &dest, F &f)
{
  { *dest = f(*it1, *it2) } noexcept;
};

template<typename R1, typename R2, typename OIter, typename F>
concept transform_binary_ranges_no_throw =
  ::std::ranges::input_range<R1> &&
  ::std::ranges::input_range<R2> &&
  ::std::is_nothrow_move_constructible_v<F> &&
  ::fast_io::details::algorithms::transform_binary_iters_no_throw<
    ::std::ranges::iterator_t<R1>, ::std::ranges::sentinel_t<R1>,
    ::std::ranges::iterator_t<R2>, ::std::ranges::sentinel_t<R2>,
    OIter, F>;

template<typename R1, typename R2, typename OIter, typename F>
concept transform_binary_index_range_no_throw =
  ::std::ranges::random_access_range<R1> &&
  ::std::ranges::random_access_range<R2> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R1> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R2> &&
  ::std::is_nothrow_move_constructible_v<F> &&
  ::fast_io::details::algorithms::transform_binary_iters_no_throw<
    ::fast_io::details::algorithms::algo_index_iterator<R1>,
    ::fast_io::details::algorithms::algo_index_iterator<R1>,
    ::fast_io::details::algorithms::algo_index_iterator<R2>,
    ::fast_io::details::algorithms::algo_index_iterator<R2>, OIter, F>;

template<typename Iter, typename Snt, typename OIter, typename F>
inline constexpr OIter transform_iters_impl(
  Iter first, Snt last, OIter dest, F &f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::transform_iters_no_throw<
      Iter, Snt, OIter, F>)
{
  for (; first != last; ++first, ++dest)
  {
    *dest = f(*first);
  }
  return dest;
}

template<typename Iter1, typename Snt1, typename Iter2, typename Snt2,
  typename OIter, typename F>
inline constexpr OIter transform_binary_iters_impl(
  Iter1 first1, Snt1 last1, Iter2 first2, Snt2 last2, OIter dest, F &f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::transform_binary_iters_no_throw<
      Iter1, Snt1, Iter2, Snt2, OIter, F>)
{
  for (; first1 != last1 && first2 != last2; ++first1, ++first2, ++dest)
  {
    *dest = f(*first1, *first2);
  }
  return dest;
}

} // namespace fast_io::details::algorithms

namespace fast_io::algorithms
{

// Apply a unary function to every element of `[first, last)`.
template<::std::input_iterator Iter, ::std::sentinel_for<Iter> Snt,
  typename OIter, typename F>
inline constexpr OIter transform_iters(
  Iter first, Snt last, OIter dest, F f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::transform_iters_no_throw<
      Iter, Snt, OIter, F>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first), decltype(dest)>),
    transform_iters_define, first, last, dest, f);
  return ::fast_io::details::algorithms::transform_iters_impl(
    first, last, dest, f);
}

template<::std::ranges::input_range R, typename OIter, typename F>
inline constexpr OIter transform(R &&r, OIter dest, F f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::transform_ranges_no_throw<
      R, OIter, F>)
{
  return ::fast_io::algorithms::transform_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), dest,
    ::std::move(f));
}

template<::std::ranges::random_access_range R, typename OIter, typename F>
inline constexpr OIter transform_index_range(
  R &&r, ::std::size_t firstidx, ::std::size_t lastidx, OIter dest, F f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::transform_index_range_no_throw<
      R, OIter, F>)
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R>(r), firstidx, lastidx);
  return ::fast_io::algorithms::transform_iters(
    first, last, dest, ::std::move(f));
}

// Apply a binary function to corresponding elements of two ranges.
template<::std::input_iterator Iter1, ::std::sentinel_for<Iter1> Snt1,
  ::std::input_iterator Iter2, ::std::sentinel_for<Iter2> Snt2,
  typename OIter, typename F>
inline constexpr OIter transform_binary_iters(
  Iter1 first1, Snt1 last1, Iter2 first2, Snt2 last2, OIter dest, F f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::transform_binary_iters_no_throw<
      Iter1, Snt1, Iter2, Snt2, OIter, F>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first1), decltype(first2), decltype(dest)>),
    transform_binary_iters_define, first1, last1, first2, last2, dest, f);
  return ::fast_io::details::algorithms::transform_binary_iters_impl(
    first1, last1, first2, last2, dest, f);
}

template<::std::ranges::input_range R1, ::std::ranges::input_range R2,
  typename OIter, typename F>
inline constexpr OIter transform_binary(
  R1 &&r1, R2 &&r2, OIter dest, F f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::transform_binary_ranges_no_throw<
      R1, R2, OIter, F>)
{
  return ::fast_io::algorithms::transform_binary_iters(
    ::std::ranges::begin(r1), ::std::ranges::end(r1),
    ::std::ranges::begin(r2), ::std::ranges::end(r2), dest,
    ::std::move(f));
}

template<::std::ranges::random_access_range R1,
  ::std::ranges::random_access_range R2, typename OIter, typename F>
inline constexpr OIter transform_binary_index_range(
  R1 &&r1, ::std::size_t firstidx1, ::std::size_t lastidx1,
  R2 &&r2, ::std::size_t firstidx2, ::std::size_t lastidx2,
  OIter dest, F f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::transform_binary_index_range_no_throw<
      R1, R2, OIter, F>)
{
  auto [first1, last1] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R1>(r1), firstidx1, lastidx1);
  auto [first2, last2] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R2>(r2), firstidx2, lastidx2);
  return ::fast_io::algorithms::transform_binary_iters(
    first1, last1, first2, last2, dest, ::std::move(f));
}

} // namespace fast_io::algorithms
