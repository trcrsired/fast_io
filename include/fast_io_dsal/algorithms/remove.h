#pragma once

namespace fast_io::details::algorithms
{

template<typename Iter, typename T>
concept remove_iter_no_throw =
  ::fast_io::details::algorithms::iter_no_throw<Iter> &&
  requires(Iter &it, T const &value)
{
  { *it == value } noexcept;
};

template<typename Iter, typename Snt, typename T>
concept remove_iters_no_throw =
  ::fast_io::details::algorithms::sentinel_no_throw<Iter, Snt> &&
  ::fast_io::details::algorithms::remove_iter_no_throw<Iter, T> &&
  requires(Iter &dest, Iter &src)
{
  { *dest = ::std::move(*src) } noexcept;
};

template<typename R, typename T>
concept remove_ranges_no_throw =
  ::std::ranges::range<R> &&
  ::fast_io::details::algorithms::remove_iters_no_throw<
    ::std::ranges::iterator_t<R>, ::std::ranges::sentinel_t<R>, T>;

template<typename R, typename T>
concept remove_index_range_no_throw =
  ::std::ranges::random_access_range<R> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R> &&
  ::fast_io::details::algorithms::remove_iters_no_throw<
    ::fast_io::details::algorithms::algo_index_iterator<R>,
    ::fast_io::details::algorithms::algo_index_iterator<R>, T>;

template<typename Iter, typename F>
concept remove_if_iter_no_throw =
  ::fast_io::details::algorithms::iter_no_throw<Iter> &&
  requires(Iter &it, F &f)
{
  { static_cast<bool>(f(*it)) } noexcept;
};

template<typename Iter, typename Snt, typename F>
concept remove_if_iters_no_throw =
  ::fast_io::details::algorithms::sentinel_no_throw<Iter, Snt> &&
  ::fast_io::details::algorithms::remove_if_iter_no_throw<Iter, F> &&
  requires(Iter &dest, Iter &src)
{
  { *dest = ::std::move(*src) } noexcept;
};

template<typename R, typename F>
concept remove_if_ranges_no_throw =
  ::std::ranges::range<R> &&
  ::std::is_nothrow_move_constructible_v<F> &&
  ::fast_io::details::algorithms::remove_if_iters_no_throw<
    ::std::ranges::iterator_t<R>, ::std::ranges::sentinel_t<R>, F>;

template<typename R, typename F>
concept remove_if_index_range_no_throw =
  ::std::ranges::random_access_range<R> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R> &&
  ::std::is_nothrow_move_constructible_v<F> &&
  ::fast_io::details::algorithms::remove_if_iters_no_throw<
    ::fast_io::details::algorithms::algo_index_iterator<R>,
    ::fast_io::details::algorithms::algo_index_iterator<R>, F>;

template<typename T, typename V>
FAST_IO_UNSAFE_API
inline constexpr T *remove_ptr(T *first, T *last, V const &value)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::remove_iters_no_throw<T *, T *, V>)
{
  first = ::fast_io::details::algorithms::find_ptr(first, last, value);
  if (first == last)
  {
    return first;
  }
  T *dest{first};
  for (T *it{first}; ++it != last;)
  {
    if (*it == value)
    {
      continue;
    }
    *dest = ::std::move(*it);
    ++dest;
  }
  return dest;
}

template<typename T, typename F>
FAST_IO_UNSAFE_API
inline constexpr T *remove_if_ptr(T *first, T *last, F &f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::remove_if_iters_no_throw<T *, T *, F>)
{
  for (; first != last; ++first)
  {
    if (f(*first))
    {
      break;
    }
  }
  if (first == last)
  {
    return first;
  }
  T *dest{first};
  for (T *it{first}; ++it != last;)
  {
    if (f(*it))
    {
      continue;
    }
    *dest = ::std::move(*it);
    ++dest;
  }
  return dest;
}

template<typename Iter, typename Snt, typename T>
FAST_IO_UNSAFE_API
inline constexpr Iter remove_iters_impl(Iter first, Snt last, T const &value)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::remove_iters_no_throw<Iter, Snt, T>)
{
  if constexpr (::std::is_pointer_v<Iter> && ::std::same_as<Iter, Snt>)
  {
    return ::fast_io::details::algorithms::remove_ptr(first, last, value);
  }
  else if constexpr (::std::contiguous_iterator<Iter> &&
    !::std::is_pointer_v<Iter> && ::std::contiguous_iterator<Snt>)
  {
    auto const first_pointer{::std::to_address(first)};
    auto const result_pointer{::fast_io::details::algorithms::remove_ptr(
      first_pointer, ::std::to_address(last), value)};
    return first + static_cast<::std::iter_difference_t<Iter>>(
      result_pointer - first_pointer);
  }
  else if constexpr (::std::contiguous_iterator<Iter> &&
    !::std::is_pointer_v<Iter> && !::std::contiguous_iterator<Snt> &&
    ::std::sized_sentinel_for<Snt, Iter>)
  {
    auto const count{last - first};
    auto const first_pointer{::std::to_address(first)};
    auto const result_pointer{::fast_io::details::algorithms::remove_ptr(
      first_pointer, first_pointer + count, value)};
    return first + static_cast<::std::iter_difference_t<Iter>>(
      result_pointer - first_pointer);
  }
  else
  {
    Iter result{::fast_io::algorithms::find_iters(first, last, value)};
    if (result == last)
    {
      return result;
    }
    for (Iter it{result}; ++it != last;)
    {
      if (*it == value)
      {
        continue;
      }
      *result = ::std::move(*it);
      ++result;
    }
    return result;
  }
}

template<typename Iter, typename Snt, typename F>
FAST_IO_UNSAFE_API
inline constexpr Iter remove_if_iters_impl(Iter first, Snt last, F &f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::remove_if_iters_no_throw<Iter, Snt, F>)
{
  if constexpr (::std::is_pointer_v<Iter> && ::std::same_as<Iter, Snt>)
  {
    return ::fast_io::details::algorithms::remove_if_ptr(first, last, f);
  }
  else if constexpr (::std::contiguous_iterator<Iter> &&
    !::std::is_pointer_v<Iter> && ::std::contiguous_iterator<Snt>)
  {
    auto const first_pointer{::std::to_address(first)};
    auto const result_pointer{::fast_io::details::algorithms::remove_if_ptr(
      first_pointer, ::std::to_address(last), f)};
    return first + static_cast<::std::iter_difference_t<Iter>>(
      result_pointer - first_pointer);
  }
  else if constexpr (::std::contiguous_iterator<Iter> &&
    !::std::is_pointer_v<Iter> && !::std::contiguous_iterator<Snt> &&
    ::std::sized_sentinel_for<Snt, Iter>)
  {
    auto const count{last - first};
    auto const first_pointer{::std::to_address(first)};
    auto const result_pointer{::fast_io::details::algorithms::remove_if_ptr(
      first_pointer, first_pointer + count, f)};
    return first + static_cast<::std::iter_difference_t<Iter>>(
      result_pointer - first_pointer);
  }
  else
  {
    Iter result{first};
    for (; result != last; ++result)
    {
      if (f(*result))
      {
        break;
      }
    }
    if (result == last)
    {
      return result;
    }
    for (Iter it{result}; ++it != last;)
    {
      if (f(*it))
      {
        continue;
      }
      *result = ::std::move(*it);
      ++result;
    }
    return result;
  }
}

} // namespace fast_io::details::algorithms

namespace fast_io::algorithms
{

// Remove every element equal to `value`, preserving the relative order of the
// remaining elements. Returns the new logical end.
template<::std::forward_iterator Iter, ::std::sentinel_for<Iter> Snt,
  typename T>
FAST_IO_UNSAFE_API
inline constexpr Iter remove_iters(Iter first, Snt last, T const &value)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::remove_iters_no_throw<Iter, Snt, T>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first)>),
    remove_iters_define, first, last, value);
  return ::fast_io::details::algorithms::remove_iters_impl(first, last, value);
}

template<::std::ranges::forward_range R, typename T>
FAST_IO_UNSAFE_API
inline constexpr ::std::ranges::iterator_t<R> remove(R &&r, T const &value)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::remove_ranges_no_throw<R, T>)
{
  return ::fast_io::algorithms::remove_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), value);
}

// Remove `[firstidx, lastidx)` elements equal to `value`. Returns the index of
// the new logical end.
template<::std::ranges::random_access_range R, typename T>
inline constexpr ::std::size_t remove_index_range(
  R &&r, ::std::size_t firstidx, ::std::size_t lastidx, T const &value)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::remove_index_range_no_throw<R, T>)
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R>(r), firstidx, lastidx);
  auto const result{::fast_io::algorithms::remove_iters(first, last, value)};
  return firstidx + static_cast<::std::size_t>(result - first);
}

template<::std::ranges::random_access_range R, typename T>
inline constexpr ::std::size_t remove_index_of(R &&r, T const &value)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::remove_ranges_no_throw<R, T>)
{
  auto const first{::std::ranges::begin(r)};
  auto const result{::fast_io::algorithms::remove(
    ::std::forward<R>(r), value)};
  return static_cast<::std::size_t>(result - first);
}

// Remove every element for which `f` returns true, preserving the relative
// order of the remaining elements. Returns the new logical end.
template<::std::forward_iterator Iter, ::std::sentinel_for<Iter> Snt,
  typename F>
FAST_IO_UNSAFE_API
inline constexpr Iter remove_if_iters(Iter first, Snt last, F f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::remove_if_iters_no_throw<Iter, Snt, F>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first)>),
    remove_if_iters_define, first, last, f);
  return ::fast_io::details::algorithms::remove_if_iters_impl(first, last, f);
}

template<::std::ranges::forward_range R, typename F>
FAST_IO_UNSAFE_API
inline constexpr ::std::ranges::iterator_t<R> remove_if(R &&r, F f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::remove_if_ranges_no_throw<R, F>)
{
  return ::fast_io::algorithms::remove_if_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), ::std::move(f));
}

// Remove `[firstidx, lastidx)` elements for which `f` returns true. Returns
// the index of the new logical end.
template<::std::ranges::random_access_range R, typename F>
inline constexpr ::std::size_t remove_if_index_range(
  R &&r, ::std::size_t firstidx, ::std::size_t lastidx, F f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::remove_if_index_range_no_throw<R, F>)
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R>(r), firstidx, lastidx);
  auto const result{::fast_io::algorithms::remove_if_iters(
    first, last, ::std::move(f))};
  return firstidx + static_cast<::std::size_t>(result - first);
}

template<::std::ranges::random_access_range R, typename F>
inline constexpr ::std::size_t remove_if_index_of(R &&r, F f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::remove_if_ranges_no_throw<R, F>)
{
  auto const first{::std::ranges::begin(r)};
  auto const result{::fast_io::algorithms::remove_if(
    ::std::forward<R>(r), ::std::move(f))};
  return static_cast<::std::size_t>(result - first);
}

} // namespace fast_io::algorithms
