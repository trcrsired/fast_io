#pragma once

namespace fast_io::details::algorithms
{

// A byte-wide element can be filled with memset: every target byte has the same
// value, which is exactly what assigning a one-byte source produces.
template<typename T, typename U>
inline constexpr bool fill_byte_memset{
  !::std::is_volatile_v<T> &&
  ::std::is_assignable_v<T &, U const &> &&
  (::std::is_integral_v<::std::remove_cv_t<T>> ||
   ::std::same_as<::std::remove_cv_t<T>, ::std::byte>) &&
  (::std::is_integral_v<::std::remove_cv_t<U>> ||
   ::std::same_as<::std::remove_cv_t<U>, ::std::byte>) &&
  sizeof(T) == 1 && sizeof(U) == 1};

template<typename Iter, typename T>
concept fill_iter_no_throw =
  ::fast_io::details::algorithms::iter_no_throw<Iter> &&
  requires(Iter &first, T const &value) {
    { *first = value } noexcept;
  };

template<typename Iter, typename Snt, typename T>
concept fill_iters_no_throw =
  ::fast_io::details::algorithms::sentinel_no_throw<Iter, Snt> &&
  ::fast_io::details::algorithms::fill_iter_no_throw<Iter, T>;

template<typename R, typename T>
concept fill_ranges_no_throw =
  ::std::ranges::range<R> &&
  ::fast_io::details::algorithms::fill_iter_no_throw<
    ::std::ranges::iterator_t<R>, T>;

template<typename R, typename T>
concept fill_index_range_no_throw =
  ::std::ranges::random_access_range<R> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R> &&
  ::fast_io::details::algorithms::fill_iter_no_throw<
    ::fast_io::details::algorithms::algo_index_iterator<R>, T>;

template<typename Iter, typename Snt, typename T>
concept uninitialized_fill_iters_no_throw =
  ::fast_io::details::algorithms::sentinel_no_throw<Iter, Snt> &&
  ::std::is_nothrow_constructible_v<::std::iter_value_t<Iter>, T const &> &&
  ::std::is_nothrow_destructible_v<::std::iter_value_t<Iter>>;

template<typename Iter, typename T>
concept uninitialized_fill_n_iters_no_throw =
  ::fast_io::details::algorithms::iter_no_throw<Iter> &&
  ::std::is_nothrow_constructible_v<::std::iter_value_t<Iter>, T const &> &&
  ::std::is_nothrow_destructible_v<::std::iter_value_t<Iter>>;

template<typename R, typename T>
concept uninitialized_fill_ranges_no_throw =
  ::std::ranges::range<R> &&
  ::std::is_nothrow_constructible_v<
    ::std::ranges::range_value_t<R>, T const &> &&
  ::std::is_nothrow_destructible_v<::std::ranges::range_value_t<R>>;

template<typename R, typename T>
concept uninitialized_fill_index_range_no_throw =
  ::std::ranges::random_access_range<R> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R> &&
  ::fast_io::details::algorithms::uninitialized_fill_iters_no_throw<
    ::fast_io::details::algorithms::algo_index_iterator<R>,
    ::fast_io::details::algorithms::algo_index_iterator<R>, T>;

template<typename T, typename U>
inline constexpr T *fill_pointer(T *first, T *last, U const &value)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::fill_byte_memset<T, U> &&
    !noexcept(*first = value))
{
  if constexpr (::fast_io::details::algorithms::fill_byte_memset<T, U>)
  {
    auto const count{static_cast<::std::size_t>(last - first)};
    FAST_IO_IF_NOT_CONSTEVAL
    {
      if (count != 0)
      {
        ::fast_io::freestanding::my_memset(
          static_cast<void *>(first),
          static_cast<int>(static_cast<unsigned char>(value)), count);
      }
      return last;
    }
    for (::std::size_t i{}; i != count; ++i)
    {
      first[i] = value;
    }
  }
  else
  {
    for (; first != last; ++first)
    {
      *first = value;
    }
  }
  return last;
}

template<typename T, typename U>
inline constexpr T *fill_n_pointer(T *first, ::std::size_t count, U const &value)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::fill_byte_memset<T, U> &&
    !noexcept(*first = value))
{
  if constexpr (::fast_io::details::algorithms::fill_byte_memset<T, U>)
  {
    FAST_IO_IF_NOT_CONSTEVAL
    {
      if (count != 0)
      {
        ::fast_io::freestanding::my_memset(
          static_cast<void *>(first),
          static_cast<int>(static_cast<unsigned char>(value)), count);
      }
      return first + count;
    }
    for (::std::size_t i{}; i != count; ++i)
    {
      first[i] = value;
    }
    return first + count;
  }
  else
  {
    for (::std::size_t i{}; i != count; ++i, ++first)
    {
      *first = value;
    }
    return first;
  }
}

template<typename T, typename U>
inline constexpr T *uninitialized_fill_pointer(T *first, T *last, U const &value)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::std::is_nothrow_constructible_v<T, U const &> ||
    !::std::is_nothrow_destructible_v<T>)
{
  auto const count{static_cast<::std::size_t>(last - first)};
  if constexpr (::fast_io::details::algorithms::fill_byte_memset<T, U> &&
    ::std::is_trivially_copy_constructible_v<T>)
  {
    FAST_IO_IF_NOT_CONSTEVAL
    {
      if (count != 0)
      {
        ::fast_io::freestanding::my_memset(
          static_cast<void *>(first),
          static_cast<int>(static_cast<unsigned char>(value)), count);
      }
      return last;
    }
    for (::std::size_t i{}; i != count; ++i)
    {
      ::new (static_cast<void *>(first + i)) T(value);
    }
  }
  else
  {
    ::fast_io::details::algorithms::uninitialized_guard<T *, T> guard{
      first, first};
    for (::std::size_t i{}; i != count; ++i)
    {
      ::new (static_cast<void *>(first + i)) T(value);
      ++guard.current;
    }
    guard.current = guard.first;
  }
  return last;
}

template<typename T, typename U>
inline constexpr T *uninitialized_fill_n_pointer(
  T *first, ::std::size_t count, U const &value)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::std::is_nothrow_constructible_v<T, U const &> ||
    !::std::is_nothrow_destructible_v<T>)
{
  if constexpr (::fast_io::details::algorithms::fill_byte_memset<T, U> &&
    ::std::is_trivially_copy_constructible_v<T>)
  {
    FAST_IO_IF_NOT_CONSTEVAL
    {
      if (count != 0)
      {
        ::fast_io::freestanding::my_memset(
          static_cast<void *>(first),
          static_cast<int>(static_cast<unsigned char>(value)), count);
      }
      return first + count;
    }
    for (::std::size_t i{}; i != count; ++i)
    {
      ::new (static_cast<void *>(first + i)) T(value);
    }
  }
  else
  {
    ::fast_io::details::algorithms::uninitialized_guard<T *, T> guard{
      first, first};
    for (::std::size_t i{}; i != count; ++i)
    {
      ::new (static_cast<void *>(first + i)) T(value);
      ++guard.current;
    }
    guard.current = guard.first;
  }
  return first + count;
}

template<typename Iter, typename Snt, typename T>
inline constexpr Iter fill_iters_impl(Iter first, Snt last, T const &value)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::fill_iters_no_throw<Iter, Snt, T>)
{
  if constexpr (::std::is_pointer_v<Iter> && ::std::same_as<Iter, Snt>)
  {
    return ::fast_io::details::algorithms::fill_pointer(first, last, value);
  }
  else if constexpr (::std::contiguous_iterator<Iter> &&
    !::std::is_pointer_v<Iter> && ::std::contiguous_iterator<Snt>)
  {
    auto const first_pointer{::std::to_address(first)};
    auto const last_pointer{::std::to_address(last)};
    ::fast_io::details::algorithms::fill_pointer(
      first_pointer, last_pointer, value);
    return first + (last_pointer - first_pointer);
  }
  else if constexpr (::std::contiguous_iterator<Iter> &&
    !::std::is_pointer_v<Iter> && !::std::contiguous_iterator<Snt> &&
    ::std::sized_sentinel_for<Snt, Iter>)
  {
    auto const count{last - first};
    auto const first_pointer{::std::to_address(first)};
    ::fast_io::details::algorithms::fill_pointer(
      first_pointer, first_pointer + count, value);
    return first + count;
  }
  else
  {
    for (; first != last; ++first)
    {
      *first = value;
    }
    return first;
  }
}

template<typename Iter, typename T>
inline constexpr Iter fill_n_iters_impl(Iter first, ::std::size_t count, T const &value)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::fill_iter_no_throw<Iter, T>)
{
  if constexpr (::std::is_pointer_v<Iter>)
  {
    return ::fast_io::details::algorithms::fill_n_pointer(first, count, value);
  }
  else if constexpr (::std::contiguous_iterator<Iter> &&
    requires(Iter &it, ::std::iter_difference_t<Iter> difference) {
      { ::std::to_address(it) } noexcept;
      { it + difference } noexcept;
    })
  {
    auto const first_pointer{::std::to_address(first)};
    ::fast_io::details::algorithms::fill_n_pointer(
      first_pointer, count, value);
    return first + static_cast<::std::iter_difference_t<Iter>>(count);
  }
  else
  {
    for (::std::size_t i{}; i != count; ++i, ++first)
    {
      *first = value;
    }
    return first;
  }
}

template<typename Iter, typename Snt, typename T>
inline constexpr Iter uninitialized_fill_iters_impl(
  Iter first, Snt last, T const &value)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::uninitialized_fill_iters_no_throw<
      Iter, Snt, T>)
{
  if constexpr (::std::is_pointer_v<Iter> && ::std::same_as<Iter, Snt>)
  {
    return ::fast_io::details::algorithms::uninitialized_fill_pointer(
      first, last, value);
  }
  else if constexpr (::std::contiguous_iterator<Iter> &&
    !::std::is_pointer_v<Iter> && ::std::contiguous_iterator<Snt>)
  {
    auto const first_pointer{::std::to_address(first)};
    auto const last_pointer{::std::to_address(last)};
    ::fast_io::details::algorithms::uninitialized_fill_pointer(
      first_pointer, last_pointer, value);
    return first + (last_pointer - first_pointer);
  }
  else if constexpr (::std::contiguous_iterator<Iter> &&
    !::std::is_pointer_v<Iter> && !::std::contiguous_iterator<Snt> &&
    ::std::sized_sentinel_for<Snt, Iter>)
  {
    auto const count{last - first};
    auto const first_pointer{::std::to_address(first)};
    ::fast_io::details::algorithms::uninitialized_fill_pointer(
      first_pointer, first_pointer + count, value);
    return first + count;
  }
  else
  {
    using value_type = ::std::iter_value_t<Iter>;
    ::fast_io::details::algorithms::uninitialized_guard<Iter, value_type> guard{
      first, first};
    for (; first != last; ++first)
    {
      ::new (static_cast<void *>(__builtin_addressof(*first))) value_type(
        value);
      ++guard.current;
    }
    guard.current = guard.first;
    return first;
  }
}

template<typename Iter, typename T>
inline constexpr Iter uninitialized_fill_n_iters_impl(
  Iter first, ::std::size_t count, T const &value)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::uninitialized_fill_n_iters_no_throw<
      Iter, T>)
{
  if constexpr (::std::is_pointer_v<Iter>)
  {
    return ::fast_io::details::algorithms::uninitialized_fill_n_pointer(
      first, count, value);
  }
  else if constexpr (::std::contiguous_iterator<Iter> &&
    requires(Iter &it, ::std::iter_difference_t<Iter> difference) {
      { ::std::to_address(it) } noexcept;
      { it + difference } noexcept;
    })
  {
    auto const first_pointer{::std::to_address(first)};
    ::fast_io::details::algorithms::uninitialized_fill_n_pointer(
      first_pointer, count, value);
    return first + static_cast<::std::iter_difference_t<Iter>>(count);
  }
  else
  {
    using value_type = ::std::iter_value_t<Iter>;
    ::fast_io::details::algorithms::uninitialized_guard<Iter, value_type> guard{
      first, first};
    for (::std::size_t i{}; i != count; ++i, ++first)
    {
      ::new (static_cast<void *>(__builtin_addressof(*first))) value_type(
        value);
      ++guard.current;
    }
    guard.current = guard.first;
    return first;
  }
}

} // namespace fast_io::details::algorithms

namespace fast_io::algorithms
{

template<::std::input_iterator Iter, ::std::sentinel_for<Iter> Snt, typename T>
inline constexpr Iter fill_iters(Iter first, Snt last, T const &value)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::fill_iters_no_throw<Iter, Snt, T>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first)>),
    fill_iters_define, first, last, value);
  return ::fast_io::details::algorithms::fill_iters_impl(first, last, value);
}

template<::std::ranges::range R, typename T>
inline constexpr ::std::ranges::iterator_t<R> fill(R &&r, T const &value)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::fill_ranges_no_throw<R, T>)
{
  return ::fast_io::algorithms::fill_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), value);
}

template<::std::input_iterator Iter, typename T>
inline constexpr Iter fill_n_iters(Iter first, ::std::size_t count, T const &value)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::fill_iter_no_throw<Iter, T>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first)>),
    fill_n_iters_define, first, count, value);
  return ::fast_io::details::algorithms::fill_n_iters_impl(first, count, value);
}

template<::std::ranges::input_range R, typename T>
inline constexpr ::std::ranges::iterator_t<R> fill_n(
  R &&r, ::std::size_t count, T const &value)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::fill_iter_no_throw<
      ::std::ranges::iterator_t<R>, T>)
{
  return ::fast_io::algorithms::fill_n_iters(
    ::std::ranges::begin(r), count, value);
}

template<::std::ranges::random_access_range R, typename T>
inline constexpr void fill_index_range(
  R &&r, ::std::size_t firstidx, ::std::size_t lastidx, T const &value)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::fill_index_range_no_throw<R, T>)
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R>(r), firstidx, lastidx);
  (void)::fast_io::algorithms::fill_iters(first, last, value);
}

template<::std::input_iterator Iter, ::std::sentinel_for<Iter> Snt, typename T>
inline constexpr Iter uninitialized_fill_iters(
  Iter first, Snt last, T const &value)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::uninitialized_fill_iters_no_throw<
      Iter, Snt, T>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first)>),
    uninitialized_fill_iters_define, first, last, value);
  return ::fast_io::details::algorithms::uninitialized_fill_iters_impl(
    first, last, value);
}

template<::std::ranges::range R, typename T>
inline constexpr ::std::ranges::iterator_t<R> uninitialized_fill(
  R &&r, T const &value)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::uninitialized_fill_ranges_no_throw<R, T>)
{
  return ::fast_io::algorithms::uninitialized_fill_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), value);
}

template<::std::input_iterator Iter, typename T>
inline constexpr Iter uninitialized_fill_n_iters(
  Iter first, ::std::size_t count, T const &value)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::uninitialized_fill_n_iters_no_throw<
      Iter, T>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first)>),
    uninitialized_fill_n_iters_define, first, count, value);
  return ::fast_io::details::algorithms::uninitialized_fill_n_iters_impl(
    first, count, value);
}

template<::std::ranges::input_range R, typename T>
inline constexpr ::std::ranges::iterator_t<R> uninitialized_fill_n(
  R &&r, ::std::size_t count, T const &value)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::uninitialized_fill_n_iters_no_throw<
      ::std::ranges::iterator_t<R>, T>)
{
  return ::fast_io::algorithms::uninitialized_fill_n_iters(
    ::std::ranges::begin(r), count, value);
}

template<::std::ranges::random_access_range R, typename T>
inline constexpr void uninitialized_fill_index_range(
  R &&r, ::std::size_t firstidx, ::std::size_t lastidx, T const &value)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::uninitialized_fill_index_range_no_throw<R, T>)
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R>(r), firstidx, lastidx);
  (void)::fast_io::algorithms::uninitialized_fill_iters(first, last, value);
}

} // namespace fast_io::algorithms
