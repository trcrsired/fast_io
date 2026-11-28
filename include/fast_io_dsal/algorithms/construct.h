#pragma once

// Construction of objects in uninitialized storage:
//   - uninitialized_default_construct: default-initialization (`T t;`)
//   - uninitialized_value_construct: value-initialization (`T()`)
//
// The uninitialized copy/move/fill operations live with their families in
// `copy.h`, `relocate.h` and `fill.h`.

namespace fast_io::details::algorithms
{

template<typename Iter, typename Snt>
concept uninitialized_construct_iters_no_throw =
  ::fast_io::details::algorithms::sentinel_no_throw<Iter, Snt> &&
  ::std::is_nothrow_default_constructible_v<::std::iter_value_t<Iter>> &&
  ::std::is_nothrow_destructible_v<::std::iter_value_t<Iter>>;

template<typename Iter>
concept uninitialized_construct_n_iters_no_throw =
  ::fast_io::details::algorithms::iter_no_throw<Iter> &&
  ::std::is_nothrow_default_constructible_v<::std::iter_value_t<Iter>> &&
  ::std::is_nothrow_destructible_v<::std::iter_value_t<Iter>>;

template<typename R>
concept uninitialized_construct_ranges_no_throw =
  ::std::ranges::range<R> &&
  ::std::is_nothrow_default_constructible_v<::std::ranges::range_value_t<R>> &&
  ::std::is_nothrow_destructible_v<::std::ranges::range_value_t<R>>;

template<typename R>
concept uninitialized_construct_index_range_no_throw =
  ::std::ranges::random_access_range<R> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R> &&
  ::fast_io::details::algorithms::uninitialized_construct_iters_no_throw<
    ::fast_io::details::algorithms::algo_index_iterator<R>,
    ::fast_io::details::algorithms::algo_index_iterator<R>>;

// Default-initialization with a trivial constructor leaves the values
// indeterminate, so writing zeros is a permitted outcome and turns the whole
// range into one memset. Value-initialization of a trivially default
// constructible type zero-initializes, so the same fast path is exact there.
template<typename T>
inline constexpr bool uninitialized_construct_zero_v{
  !::std::is_volatile_v<T> &&
  ::std::is_trivially_default_constructible_v<T>};

template<typename T>
FAST_IO_UNSAFE_API
inline constexpr T *uninitialized_default_construct_pointer(T *first, T *last)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::std::is_nothrow_default_constructible_v<T> ||
    !::std::is_nothrow_destructible_v<T>)
{
  auto const count{static_cast<::std::size_t>(last - first)};
  if constexpr (::fast_io::details::algorithms::uninitialized_construct_zero_v<T>)
  {
    FAST_IO_IF_NOT_CONSTEVAL
    {
      if (count != 0)
      {
        ::fast_io::freestanding::my_memset(
          static_cast<void *>(first), 0, count * sizeof(T));
      }
      return last;
    }
    for (::std::size_t i{}; i != count; ++i)
    {
      ::new (static_cast<void *>(first + i)) T;
    }
  }
  else
  {
    ::fast_io::details::algorithms::uninitialized_guard<T *, T> guard{
      first, first};
    for (::std::size_t i{}; i != count; ++i)
    {
      ::new (static_cast<void *>(first + i)) T;
      ++guard.current;
    }
    guard.current = guard.first;
  }
  return last;
}

template<typename T>
FAST_IO_UNSAFE_API
inline constexpr T *uninitialized_default_construct_n_pointer(
  T *first, ::std::size_t count)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::std::is_nothrow_default_constructible_v<T> ||
    !::std::is_nothrow_destructible_v<T>)
{
  if constexpr (::fast_io::details::algorithms::uninitialized_construct_zero_v<T>)
  {
    FAST_IO_IF_NOT_CONSTEVAL
    {
      if (count != 0)
      {
        ::fast_io::freestanding::my_memset(
          static_cast<void *>(first), 0, count * sizeof(T));
      }
      return first + count;
    }
    for (::std::size_t i{}; i != count; ++i)
    {
      ::new (static_cast<void *>(first + i)) T;
    }
  }
  else
  {
    ::fast_io::details::algorithms::uninitialized_guard<T *, T> guard{
      first, first};
    for (::std::size_t i{}; i != count; ++i)
    {
      ::new (static_cast<void *>(first + i)) T;
      ++guard.current;
    }
    guard.current = guard.first;
  }
  return first + count;
}

template<typename T>
FAST_IO_UNSAFE_API
inline constexpr T *uninitialized_value_construct_pointer(T *first, T *last)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::std::is_nothrow_default_constructible_v<T> ||
    !::std::is_nothrow_destructible_v<T>)
{
  auto const count{static_cast<::std::size_t>(last - first)};
  if constexpr (::fast_io::details::algorithms::uninitialized_construct_zero_v<T>)
  {
    FAST_IO_IF_NOT_CONSTEVAL
    {
      if (count != 0)
      {
        ::fast_io::freestanding::my_memset(
          static_cast<void *>(first), 0, count * sizeof(T));
      }
      return last;
    }
    for (::std::size_t i{}; i != count; ++i)
    {
      ::new (static_cast<void *>(first + i)) T();
    }
  }
  else
  {
    ::fast_io::details::algorithms::uninitialized_guard<T *, T> guard{
      first, first};
    for (::std::size_t i{}; i != count; ++i)
    {
      ::new (static_cast<void *>(first + i)) T();
      ++guard.current;
    }
    guard.current = guard.first;
  }
  return last;
}

template<typename T>
FAST_IO_UNSAFE_API
inline constexpr T *uninitialized_value_construct_n_pointer(
  T *first, ::std::size_t count)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::std::is_nothrow_default_constructible_v<T> ||
    !::std::is_nothrow_destructible_v<T>)
{
  if constexpr (::fast_io::details::algorithms::uninitialized_construct_zero_v<T>)
  {
    FAST_IO_IF_NOT_CONSTEVAL
    {
      if (count != 0)
      {
        ::fast_io::freestanding::my_memset(
          static_cast<void *>(first), 0, count * sizeof(T));
      }
      return first + count;
    }
    for (::std::size_t i{}; i != count; ++i)
    {
      ::new (static_cast<void *>(first + i)) T();
    }
  }
  else
  {
    ::fast_io::details::algorithms::uninitialized_guard<T *, T> guard{
      first, first};
    for (::std::size_t i{}; i != count; ++i)
    {
      ::new (static_cast<void *>(first + i)) T();
      ++guard.current;
    }
    guard.current = guard.first;
  }
  return first + count;
}

template<typename Iter, typename Snt>
FAST_IO_UNSAFE_API
inline constexpr Iter uninitialized_default_construct_iters_impl(
  Iter first, Snt last)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::uninitialized_construct_iters_no_throw<
      Iter, Snt>)
{
  if constexpr (::std::is_pointer_v<Iter> && ::std::same_as<Iter, Snt>)
  {
    return ::fast_io::details::algorithms::uninitialized_default_construct_pointer(
      first, last);
  }
  else if constexpr (::std::contiguous_iterator<Iter> &&
    !::std::is_pointer_v<Iter> && ::std::contiguous_iterator<Snt>)
  {
    auto const first_pointer{::std::to_address(first)};
    auto const last_pointer{::std::to_address(last)};
    ::fast_io::details::algorithms::uninitialized_default_construct_pointer(
      first_pointer, last_pointer);
    return first + (last_pointer - first_pointer);
  }
  else if constexpr (::std::contiguous_iterator<Iter> &&
    !::std::is_pointer_v<Iter> && !::std::contiguous_iterator<Snt> &&
    ::std::sized_sentinel_for<Snt, Iter>)
  {
    auto const count{last - first};
    auto const first_pointer{::std::to_address(first)};
    ::fast_io::details::algorithms::uninitialized_default_construct_pointer(
      first_pointer, first_pointer + count);
    return first + count;
  }
  else
  {
    using value_type = ::std::iter_value_t<Iter>;
    ::fast_io::details::algorithms::uninitialized_guard<Iter, value_type> guard{
      first, first};
    for (; first != last; ++first)
    {
      ::new (static_cast<void *>(__builtin_addressof(*first))) value_type;
      ++guard.current;
    }
    guard.current = guard.first;
    return first;
  }
}

template<typename Iter>
FAST_IO_UNSAFE_API
inline constexpr Iter uninitialized_default_construct_n_iters_impl(
  Iter first, ::std::size_t count)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::uninitialized_construct_n_iters_no_throw<
      Iter>)
{
  if constexpr (::std::is_pointer_v<Iter>)
  {
    return ::fast_io::details::algorithms::uninitialized_default_construct_n_pointer(
      first, count);
  }
  else if constexpr (::std::contiguous_iterator<Iter> &&
    requires(Iter &it, ::std::iter_difference_t<Iter> difference) {
      { ::std::to_address(it) } noexcept;
      { it + difference } noexcept;
    })
  {
    auto const first_pointer{::std::to_address(first)};
    ::fast_io::details::algorithms::uninitialized_default_construct_n_pointer(
      first_pointer, count);
    return first + static_cast<::std::iter_difference_t<Iter>>(count);
  }
  else
  {
    using value_type = ::std::iter_value_t<Iter>;
    ::fast_io::details::algorithms::uninitialized_guard<Iter, value_type> guard{
      first, first};
    for (::std::size_t i{}; i != count; ++i, ++first)
    {
      ::new (static_cast<void *>(__builtin_addressof(*first))) value_type;
      ++guard.current;
    }
    guard.current = guard.first;
    return first;
  }
}

template<typename Iter, typename Snt>
FAST_IO_UNSAFE_API
inline constexpr Iter uninitialized_value_construct_iters_impl(
  Iter first, Snt last)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::uninitialized_construct_iters_no_throw<
      Iter, Snt>)
{
  if constexpr (::std::is_pointer_v<Iter> && ::std::same_as<Iter, Snt>)
  {
    return ::fast_io::details::algorithms::uninitialized_value_construct_pointer(
      first, last);
  }
  else if constexpr (::std::contiguous_iterator<Iter> &&
    !::std::is_pointer_v<Iter> && ::std::contiguous_iterator<Snt>)
  {
    auto const first_pointer{::std::to_address(first)};
    auto const last_pointer{::std::to_address(last)};
    ::fast_io::details::algorithms::uninitialized_value_construct_pointer(
      first_pointer, last_pointer);
    return first + (last_pointer - first_pointer);
  }
  else if constexpr (::std::contiguous_iterator<Iter> &&
    !::std::is_pointer_v<Iter> && !::std::contiguous_iterator<Snt> &&
    ::std::sized_sentinel_for<Snt, Iter>)
  {
    auto const count{last - first};
    auto const first_pointer{::std::to_address(first)};
    ::fast_io::details::algorithms::uninitialized_value_construct_pointer(
      first_pointer, first_pointer + count);
    return first + count;
  }
  else
  {
    using value_type = ::std::iter_value_t<Iter>;
    ::fast_io::details::algorithms::uninitialized_guard<Iter, value_type> guard{
      first, first};
    for (; first != last; ++first)
    {
      ::new (static_cast<void *>(__builtin_addressof(*first))) value_type();
      ++guard.current;
    }
    guard.current = guard.first;
    return first;
  }
}

template<typename Iter>
FAST_IO_UNSAFE_API
inline constexpr Iter uninitialized_value_construct_n_iters_impl(
  Iter first, ::std::size_t count)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::uninitialized_construct_n_iters_no_throw<
      Iter>)
{
  if constexpr (::std::is_pointer_v<Iter>)
  {
    return ::fast_io::details::algorithms::uninitialized_value_construct_n_pointer(
      first, count);
  }
  else if constexpr (::std::contiguous_iterator<Iter> &&
    requires(Iter &it, ::std::iter_difference_t<Iter> difference) {
      { ::std::to_address(it) } noexcept;
      { it + difference } noexcept;
    })
  {
    auto const first_pointer{::std::to_address(first)};
    ::fast_io::details::algorithms::uninitialized_value_construct_n_pointer(
      first_pointer, count);
    return first + static_cast<::std::iter_difference_t<Iter>>(count);
  }
  else
  {
    using value_type = ::std::iter_value_t<Iter>;
    ::fast_io::details::algorithms::uninitialized_guard<Iter, value_type> guard{
      first, first};
    for (::std::size_t i{}; i != count; ++i, ++first)
    {
      ::new (static_cast<void *>(__builtin_addressof(*first))) value_type();
      ++guard.current;
    }
    guard.current = guard.first;
    return first;
  }
}

} // namespace fast_io::details::algorithms

namespace fast_io::algorithms
{

template<::std::input_iterator Iter, ::std::sentinel_for<Iter> Snt>
FAST_IO_UNSAFE_API
inline constexpr Iter uninitialized_default_construct_iters(Iter first, Snt last)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::uninitialized_construct_iters_no_throw<
      Iter, Snt>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first)>),
    uninitialized_default_construct_iters_define, first, last);
  return ::fast_io::details::algorithms::
    uninitialized_default_construct_iters_impl(first, last);
}

template<::std::ranges::range R>
inline constexpr ::std::ranges::iterator_t<R> uninitialized_default_construct(
  R &&r)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::uninitialized_construct_ranges_no_throw<R>)
{
  return ::fast_io::algorithms::uninitialized_default_construct_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r));
}

template<::std::input_iterator Iter>
FAST_IO_UNSAFE_API
inline constexpr Iter uninitialized_default_construct_n_iters(
  Iter first, ::std::size_t count)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::uninitialized_construct_n_iters_no_throw<
      Iter>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first)>),
    uninitialized_default_construct_n_iters_define, first, count);
  return ::fast_io::details::algorithms::
    uninitialized_default_construct_n_iters_impl(first, count);
}

template<::std::ranges::input_range R>
inline constexpr ::std::ranges::iterator_t<R> uninitialized_default_construct_n(
  R &&r, ::std::size_t count)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::uninitialized_construct_n_iters_no_throw<
      ::std::ranges::iterator_t<R>>)
{
  return ::fast_io::algorithms::uninitialized_default_construct_n_iters(
    ::std::ranges::begin(r), count);
}

template<::std::input_iterator Iter, ::std::sentinel_for<Iter> Snt>
FAST_IO_UNSAFE_API
inline constexpr Iter uninitialized_value_construct_iters(Iter first, Snt last)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::uninitialized_construct_iters_no_throw<
      Iter, Snt>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first)>),
    uninitialized_value_construct_iters_define, first, last);
  return ::fast_io::details::algorithms::
    uninitialized_value_construct_iters_impl(first, last);
}

template<::std::ranges::range R>
inline constexpr ::std::ranges::iterator_t<R> uninitialized_value_construct(
  R &&r)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::uninitialized_construct_ranges_no_throw<R>)
{
  return ::fast_io::algorithms::uninitialized_value_construct_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r));
}

template<::std::input_iterator Iter>
FAST_IO_UNSAFE_API
inline constexpr Iter uninitialized_value_construct_n_iters(
  Iter first, ::std::size_t count)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::uninitialized_construct_n_iters_no_throw<
      Iter>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first)>),
    uninitialized_value_construct_n_iters_define, first, count);
  return ::fast_io::details::algorithms::
    uninitialized_value_construct_n_iters_impl(first, count);
}

template<::std::ranges::input_range R>
inline constexpr ::std::ranges::iterator_t<R> uninitialized_value_construct_n(
  R &&r, ::std::size_t count)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::uninitialized_construct_n_iters_no_throw<
      ::std::ranges::iterator_t<R>>)
{
  return ::fast_io::algorithms::uninitialized_value_construct_n_iters(
    ::std::ranges::begin(r), count);
}

template<::std::ranges::random_access_range R>
inline constexpr void uninitialized_default_construct_index_range(
  R &&r, ::std::size_t firstidx, ::std::size_t lastidx)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::uninitialized_construct_index_range_no_throw<R>)
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R>(r), firstidx, lastidx);
  (void)::fast_io::algorithms::uninitialized_default_construct_iters(
    first, last);
}

template<::std::ranges::random_access_range R>
inline constexpr void uninitialized_value_construct_index_range(
  R &&r, ::std::size_t firstidx, ::std::size_t lastidx)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::uninitialized_construct_index_range_no_throw<R>)
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R>(r), firstidx, lastidx);
  (void)::fast_io::algorithms::uninitialized_value_construct_iters(
    first, last);
}

} // namespace fast_io::algorithms
