#pragma once

// Destruction of objects in initialized storage:
//   - destroy_at
//   - destroy_iters / destroy
//   - destroy_n_iters / destroy_n
//
// Trivially destructible types are a strict no-op: no destructor call and no
// instantiated loop body.

namespace fast_io::details::algorithms
{

template<typename Iter, typename Snt>
concept destroy_iters_no_throw =
  ::fast_io::details::algorithms::sentinel_no_throw<Iter, Snt> &&
  ::std::is_nothrow_destructible_v<::std::iter_value_t<Iter>>;

template<typename Iter>
concept destroy_n_iters_no_throw =
  ::fast_io::details::algorithms::iter_no_throw<Iter> &&
  ::std::is_nothrow_destructible_v<::std::iter_value_t<Iter>> &&
  (!::std::is_trivially_destructible_v<::std::iter_value_t<Iter>> ||
   ::fast_io::details::algorithms::advance_iters_no_throw<Iter>);

template<typename R>
concept destroy_ranges_no_throw =
  ::std::ranges::range<R> &&
  ::std::is_nothrow_destructible_v<::std::ranges::range_value_t<R>>;

template<typename R>
concept destroy_index_range_no_throw =
  ::std::ranges::random_access_range<R> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R> &&
  ::fast_io::details::algorithms::destroy_iters_no_throw<
    ::fast_io::details::algorithms::algo_index_iterator<R>,
    ::fast_io::details::algorithms::algo_index_iterator<R>>;

template<typename T>
FAST_IO_UNSAFE_API
inline constexpr void destroy_at_impl(T *location)
  FAST_IO_HERBCEPTIONS_THROWS_IF(!::std::is_nothrow_destructible_v<T>)
{
  if constexpr (::std::is_trivially_destructible_v<T>)
  {
    (void)location;
  }
  else if constexpr (::std::is_array_v<T>)
  {
    for (auto &element : *location)
    {
      ::fast_io::details::algorithms::destroy_at_impl(
        __builtin_addressof(element));
    }
  }
  else
  {
    location->~T();
  }
}

template<typename Iter, typename Snt>
FAST_IO_UNSAFE_API
inline constexpr Iter destroy_iters_impl(Iter first, Snt last)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::destroy_iters_no_throw<Iter, Snt>)
{
  using value_type = ::std::iter_value_t<Iter>;
  if constexpr (::std::is_trivially_destructible_v<value_type>)
  {
    if constexpr (::std::same_as<Iter, Snt>)
    {
      (void)first;
      return last;
    }
    else if constexpr (::std::random_access_iterator<Iter> &&
      ::std::sized_sentinel_for<Snt, Iter>)
    {
      first += last - first;
      return first;
    }
    else
    {
      for (; first != last; ++first)
      {
      }
      return first;
    }
  }
  else
  {
    for (; first != last; ++first)
    {
      ::fast_io::details::algorithms::destroy_at_impl(
        __builtin_addressof(*first));
    }
    return first;
  }
}

template<typename Iter>
FAST_IO_UNSAFE_API
inline constexpr Iter destroy_n_iters_impl(Iter first, ::std::size_t count)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::destroy_n_iters_no_throw<Iter>)
{
  using value_type = ::std::iter_value_t<Iter>;
  if constexpr (::std::is_trivially_destructible_v<value_type>)
  {
    ::fast_io::details::algorithms::advance_iters_impl(
      first, static_cast<::std::iter_difference_t<Iter>>(count));
    return first;
  }
  else
  {
    for (::std::size_t i{}; i != count; ++i, ++first)
    {
      ::fast_io::details::algorithms::destroy_at_impl(
        __builtin_addressof(*first));
    }
    return first;
  }
}

} // namespace fast_io::details::algorithms

namespace fast_io::algorithms
{

template<typename T>
FAST_IO_UNSAFE_API
inline constexpr void destroy_at(T *location)
  FAST_IO_HERBCEPTIONS_THROWS_IF(!::std::is_nothrow_destructible_v<T>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE(
    destroy_at_define, location);
  ::fast_io::details::algorithms::destroy_at_impl(location);
}

template<::std::input_iterator Iter, ::std::sentinel_for<Iter> Snt>
FAST_IO_UNSAFE_API
inline constexpr Iter destroy_iters(Iter first, Snt last)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::destroy_iters_no_throw<Iter, Snt>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first)>),
    destroy_iters_define, first, last);
  return ::fast_io::details::algorithms::destroy_iters_impl(first, last);
}

template<::std::ranges::range R>
FAST_IO_UNSAFE_API
inline constexpr ::std::ranges::iterator_t<R> destroy(R &&r)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::destroy_ranges_no_throw<R>)
{
  return ::fast_io::algorithms::destroy_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r));
}

template<::std::input_iterator Iter>
FAST_IO_UNSAFE_API
inline constexpr Iter destroy_n_iters(Iter first, ::std::size_t count)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::destroy_n_iters_no_throw<Iter>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first)>),
    destroy_n_iters_define, first, count);
  return ::fast_io::details::algorithms::destroy_n_iters_impl(first, count);
}

template<::std::ranges::input_range R>
FAST_IO_UNSAFE_API
inline constexpr ::std::ranges::iterator_t<R> destroy_n(
  R &&r, ::std::size_t count)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::destroy_n_iters_no_throw<
      ::std::ranges::iterator_t<R>>)
{
  return ::fast_io::algorithms::destroy_n_iters(
    ::std::ranges::begin(r), count);
}

template<::std::ranges::random_access_range R>
inline constexpr void destroy_index_range(
  R &&r, ::std::size_t firstidx, ::std::size_t lastidx)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::destroy_index_range_no_throw<R>)
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R>(r), firstidx, lastidx);
  (void)::fast_io::algorithms::destroy_iters(first, last);
}

} // namespace fast_io::algorithms
