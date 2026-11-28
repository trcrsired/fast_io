#pragma once

namespace fast_io::details::algorithms
{

template<typename Iter, typename F>
concept partition_iter_no_throw =
  ::fast_io::details::algorithms::iter_no_throw<Iter> &&
  requires(Iter &it, F &f)
{
  { static_cast<bool>(f(*it)) } noexcept;
};

template<typename Iter, typename Snt, typename F>
concept partition_iters_no_throw =
  ::fast_io::details::algorithms::sentinel_no_throw<Iter, Snt> &&
  ::fast_io::details::algorithms::partition_iter_no_throw<Iter, F>;

template<typename R, typename F>
concept partition_ranges_no_throw =
  ::std::ranges::forward_range<R> &&
  ::std::is_nothrow_move_constructible_v<F> &&
  ::fast_io::details::algorithms::partition_iter_no_throw<
    ::std::ranges::iterator_t<R>, F>;

template<typename R, typename F>
concept partition_index_range_no_throw =
  ::std::ranges::random_access_range<R> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R> &&
  ::std::is_nothrow_move_constructible_v<F> &&
  ::fast_io::details::algorithms::partition_iter_no_throw<
    ::fast_io::details::algorithms::algo_index_iterator<R>, F>;

template<typename Iter, typename Snt, typename F>
concept partition_swap_iters_no_throw =
  ::fast_io::details::algorithms::partition_iters_no_throw<Iter, Snt, F> &&
  ::fast_io::details::algorithms::swap_nothrow<
    ::std::iter_reference_t<Iter>, ::std::iter_reference_t<Iter>>();

template<typename Iter, typename Snt, typename OIter1, typename OIter2,
  typename F>
concept partition_copy_iters_no_throw =
  ::fast_io::details::algorithms::partition_iters_no_throw<Iter, Snt, F> &&
  ::fast_io::details::algorithms::output_iter_no_throw<
    OIter1, ::std::iter_value_t<Iter>> &&
  ::fast_io::details::algorithms::output_iter_no_throw<
    OIter2, ::std::iter_value_t<Iter>> &&
  requires(Iter &it, OIter1 &dest1, OIter2 &dest2)
{
  { *dest1 = *it } noexcept;
  { *dest2 = *it } noexcept;
};

template<typename R, typename OIter1, typename OIter2, typename F>
concept partition_copy_ranges_no_throw =
  ::std::ranges::forward_range<R> &&
  ::std::is_nothrow_move_constructible_v<F> &&
  ::fast_io::details::algorithms::partition_copy_iters_no_throw<
    ::std::ranges::iterator_t<R>, ::std::ranges::sentinel_t<R>,
    OIter1, OIter2, F>;

template<typename Iter, typename Snt, typename F>
concept stable_partition_iters_no_throw =
  ::fast_io::details::algorithms::partition_swap_iters_no_throw<Iter, Snt, F> &&
  ::fast_io::details::algorithms::rotate_iters_no_throw<Iter, Iter>;

template<typename R, typename F>
concept stable_partition_ranges_no_throw =
  ::std::ranges::forward_range<R> &&
  ::std::is_nothrow_move_constructible_v<F> &&
  ::fast_io::details::algorithms::stable_partition_iters_no_throw<
    ::std::ranges::iterator_t<R>, ::std::ranges::sentinel_t<R>, F>;

template<typename R, typename F>
concept stable_partition_index_range_no_throw =
  ::std::ranges::random_access_range<R> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R> &&
  ::std::is_nothrow_move_constructible_v<F> &&
  ::fast_io::details::algorithms::stable_partition_iters_no_throw<
    ::fast_io::details::algorithms::algo_index_iterator<R>,
    ::fast_io::details::algorithms::algo_index_iterator<R>, F>;

template<typename Iter, typename Snt, typename F>
FAST_IO_UNSAFE_API
inline constexpr bool is_partitioned_iters_impl(
  Iter first, Snt last, F &f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::partition_iters_no_throw<
      Iter, Snt, F>)
{
  for (; first != last; ++first)
  {
    if (!static_cast<bool>(f(*first)))
    {
      ++first;
      break;
    }
  }
  for (; first != last; ++first)
  {
    if (static_cast<bool>(f(*first)))
    {
      return false;
    }
  }
  return true;
}

template<typename Iter, typename Snt, typename F>
FAST_IO_UNSAFE_API
inline constexpr Iter partition_iters_impl(Iter first, Snt last, F &f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::partition_swap_iters_no_throw<
      Iter, Snt, F>)
{
  for (;; ++first)
  {
    if (first == last)
    {
      return first;
    }
    if (!static_cast<bool>(f(*first)))
    {
      break;
    }
  }
  for (Iter it{first}; ++it != last;)
  {
    if (static_cast<bool>(f(*it)))
    {
      ::fast_io::algorithms::iter_swap(first, it);
      ++first;
    }
  }
  return first;
}

template<typename Iter, typename Snt, typename OIter1, typename OIter2,
  typename F>
FAST_IO_UNSAFE_API
inline constexpr ::std::pair<OIter1, OIter2> partition_copy_iters_impl(
  Iter first, Snt last, OIter1 dest_true, OIter2 dest_false, F &f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::partition_copy_iters_no_throw<
      Iter, Snt, OIter1, OIter2, F>)
{
  for (; first != last; ++first)
  {
    if (static_cast<bool>(f(*first)))
    {
      *dest_true = *first;
      ++dest_true;
    }
    else
    {
      *dest_false = *first;
      ++dest_false;
    }
  }
  return {dest_true, dest_false};
}

template<typename Iter, typename Snt, typename F>
FAST_IO_UNSAFE_API
inline constexpr Iter stable_partition_iters_impl(
  Iter first, Snt last, F &f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::stable_partition_iters_no_throw<
      Iter, Snt, F>)
{
  using difference_type = ::std::iter_difference_t<Iter>;
  difference_type length{};
  for (Iter it{first}; it != last; ++it)
  {
    ++length;
  }
  if (length == 0)
  {
    return first;
  }
  if (length == 1)
  {
    Iter next{first};
    ++next;
    return static_cast<bool>(f(*first)) ? next : first;
  }
  difference_type half{length / 2};
  Iter middle{first};
  for (difference_type i{}; i != half; ++i)
  {
    ++middle;
  }
  Iter left{::fast_io::details::algorithms::stable_partition_iters_impl(
    first, middle, f)};
  Iter right{::fast_io::details::algorithms::stable_partition_iters_impl(
    middle, last, f)};
  if (left == middle)
  {
    return right;
  }
  return ::fast_io::algorithms::rotate_iters(left, middle, right);
}

template<typename Iter, typename Snt, typename F>
FAST_IO_UNSAFE_API
inline constexpr Iter partition_point_iters_impl(
  Iter first, Snt last, F &f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::partition_iters_no_throw<
      Iter, Snt, F>)
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
    if (static_cast<bool>(f(*it)))
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

template<::std::input_iterator Iter, ::std::sentinel_for<Iter> Snt,
  typename F>
FAST_IO_UNSAFE_API
inline constexpr bool is_partitioned_iters(Iter first, Snt last, F f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::partition_iters_no_throw<
      Iter, Snt, F>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first)>),
    is_partitioned_iters_define, first, last, f);
  return ::fast_io::details::algorithms::is_partitioned_iters_impl(
    first, last, f);
}

template<::std::ranges::forward_range R, typename F>
inline constexpr bool is_partitioned(R &&r, F f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::partition_ranges_no_throw<R, F>)
{
  return ::fast_io::algorithms::is_partitioned_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), ::std::move(f));
}

template<::std::ranges::random_access_range R, typename F>
inline constexpr bool is_partitioned_index_range(
  R &&r, ::std::size_t firstidx, ::std::size_t lastidx, F f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::partition_index_range_no_throw<R, F>)
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R>(r), firstidx, lastidx);
  return ::fast_io::algorithms::is_partitioned_iters(
    first, last, ::std::move(f));
}

template<::std::forward_iterator Iter, ::std::sentinel_for<Iter> Snt,
  typename F>
FAST_IO_UNSAFE_API
inline constexpr Iter partition_iters(Iter first, Snt last, F f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::partition_swap_iters_no_throw<
      Iter, Snt, F>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first)>),
    partition_iters_define, first, last, f);
  return ::fast_io::details::algorithms::partition_iters_impl(
    first, last, f);
}

template<::std::ranges::forward_range R, typename F>
inline constexpr ::std::ranges::iterator_t<R> partition(R &&r, F f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::partition_swap_iters_no_throw<
      ::std::ranges::iterator_t<R>, ::std::ranges::sentinel_t<R>, F>)
{
  return ::fast_io::algorithms::partition_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), ::std::move(f));
}

template<::std::ranges::random_access_range R, typename F>
inline constexpr ::std::size_t partition_index_range(
  R &&r, ::std::size_t firstidx, ::std::size_t lastidx, F f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::partition_index_range_no_throw<R, F>)
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R>(r), firstidx, lastidx);
  auto const result{::fast_io::algorithms::partition_iters(
    first, last, ::std::move(f))};
  return firstidx + static_cast<::std::size_t>(result - first);
}

template<::std::ranges::random_access_range R, typename F>
inline constexpr ::std::size_t partition_index_of(R &&r, F f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::partition_swap_iters_no_throw<
      ::std::ranges::iterator_t<R>, ::std::ranges::sentinel_t<R>, F>)
{
  auto const first{::std::ranges::begin(r)};
  auto const result{::fast_io::algorithms::partition(
    ::std::forward<R>(r), ::std::move(f))};
  return static_cast<::std::size_t>(result - first);
}

template<::std::forward_iterator Iter, ::std::sentinel_for<Iter> Snt,
  typename OIter1, typename OIter2, typename F>
FAST_IO_UNSAFE_API
inline constexpr ::std::pair<OIter1, OIter2> partition_copy_iters(
  Iter first, Snt last, OIter1 dest_true, OIter2 dest_false, F f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::partition_copy_iters_no_throw<
      Iter, Snt, OIter1, OIter2, F>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first), decltype(dest_true), decltype(dest_false)>),
    partition_copy_iters_define, first, last, dest_true, dest_false, f);
  return ::fast_io::details::algorithms::partition_copy_iters_impl(
    first, last, dest_true, dest_false, f);
}

template<::std::ranges::forward_range R, typename OIter1, typename OIter2,
  typename F>
FAST_IO_UNSAFE_API
inline constexpr ::std::pair<OIter1, OIter2> partition_copy(
  R &&r, OIter1 dest_true, OIter2 dest_false, F f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::partition_copy_ranges_no_throw<
      R, OIter1, OIter2, F>)
{
  return ::fast_io::algorithms::partition_copy_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), dest_true,
    dest_false, ::std::move(f));
}

template<::std::forward_iterator Iter, ::std::sentinel_for<Iter> Snt,
  typename F>
FAST_IO_UNSAFE_API
inline constexpr Iter stable_partition_iters(Iter first, Snt last, F f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::stable_partition_iters_no_throw<
      Iter, Snt, F>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first)>),
    stable_partition_iters_define, first, last, f);
  return ::fast_io::details::algorithms::stable_partition_iters_impl(
    first, last, f);
}

template<::std::ranges::forward_range R, typename F>
inline constexpr ::std::ranges::iterator_t<R> stable_partition(
  R &&r, F f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::stable_partition_ranges_no_throw<
      R, F>)
{
  return ::fast_io::algorithms::stable_partition_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), ::std::move(f));
}

template<::std::ranges::random_access_range R, typename F>
inline constexpr ::std::size_t stable_partition_index_range(
  R &&r, ::std::size_t firstidx, ::std::size_t lastidx, F f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::stable_partition_index_range_no_throw<R, F>)
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R>(r), firstidx, lastidx);
  auto const result{::fast_io::algorithms::stable_partition_iters(
    first, last, ::std::move(f))};
  return firstidx + static_cast<::std::size_t>(result - first);
}

template<::std::ranges::random_access_range R, typename F>
inline constexpr ::std::size_t stable_partition_index_of(R &&r, F f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::stable_partition_ranges_no_throw<R, F>)
{
  auto const first{::std::ranges::begin(r)};
  auto const result{::fast_io::algorithms::stable_partition(
    ::std::forward<R>(r), ::std::move(f))};
  return static_cast<::std::size_t>(result - first);
}

template<::std::forward_iterator Iter, ::std::sentinel_for<Iter> Snt,
  typename F>
FAST_IO_UNSAFE_API
inline constexpr Iter partition_point_iters(Iter first, Snt last, F f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::partition_iters_no_throw<
      Iter, Snt, F>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first)>),
    partition_point_iters_define, first, last, f);
  return ::fast_io::details::algorithms::partition_point_iters_impl(
    first, last, f);
}

template<::std::ranges::forward_range R, typename F>
inline constexpr ::std::ranges::iterator_t<R> partition_point(
  R &&r, F f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::partition_ranges_no_throw<R, F>)
{
  return ::fast_io::algorithms::partition_point_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), ::std::move(f));
}

template<::std::ranges::random_access_range R, typename F>
inline constexpr ::std::size_t partition_point_index_range(
  R &&r, ::std::size_t firstidx, ::std::size_t lastidx, F f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::partition_index_range_no_throw<R, F>)
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R>(r), firstidx, lastidx);
  auto const result{::fast_io::algorithms::partition_point_iters(
    first, last, ::std::move(f))};
  return firstidx + static_cast<::std::size_t>(result - first);
}

template<::std::ranges::random_access_range R, typename F>
inline constexpr ::std::size_t partition_point_index_of(R &&r, F f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::partition_ranges_no_throw<R, F>)
{
  auto const first{::std::ranges::begin(r)};
  auto const result{::fast_io::algorithms::partition_point(
    ::std::forward<R>(r), ::std::move(f))};
  return static_cast<::std::size_t>(result - first);
}

} // namespace fast_io::algorithms
