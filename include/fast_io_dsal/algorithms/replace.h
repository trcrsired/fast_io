#pragma once

namespace fast_io::details::algorithms
{

template<typename Iter, typename T>
concept replace_iter_no_throw =
  ::fast_io::details::algorithms::iter_no_throw<Iter> &&
  requires(Iter &it, T const &old_value, T const &new_value)
{
  { *it == old_value } noexcept;
  { *it = new_value } noexcept;
};

template<typename Iter, typename Snt, typename T>
concept replace_iters_no_throw =
  ::fast_io::details::algorithms::sentinel_no_throw<Iter, Snt> &&
  ::fast_io::details::algorithms::replace_iter_no_throw<Iter, T>;

template<typename R, typename T>
concept replace_ranges_no_throw =
  ::std::ranges::range<R> &&
  ::fast_io::details::algorithms::replace_iter_no_throw<
    ::std::ranges::iterator_t<R>, T>;

template<typename R, typename T>
concept replace_index_range_no_throw =
  ::std::ranges::random_access_range<R> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R> &&
  ::fast_io::details::algorithms::replace_iter_no_throw<
    ::fast_io::details::algorithms::algo_index_iterator<R>, T>;

template<typename Iter, typename F, typename T>
concept replace_if_iter_no_throw =
  ::fast_io::details::algorithms::iter_no_throw<Iter> &&
  requires(Iter &it, F &f, T const &new_value)
{
  { static_cast<bool>(f(*it)) } noexcept;
  { *it = new_value } noexcept;
};

template<typename Iter, typename Snt, typename F, typename T>
concept replace_if_iters_no_throw =
  ::fast_io::details::algorithms::sentinel_no_throw<Iter, Snt> &&
  ::fast_io::details::algorithms::replace_if_iter_no_throw<Iter, F, T>;

template<typename R, typename F, typename T>
concept replace_if_ranges_no_throw =
  ::std::ranges::range<R> &&
  ::std::is_nothrow_move_constructible_v<F> &&
  ::fast_io::details::algorithms::replace_if_iter_no_throw<
    ::std::ranges::iterator_t<R>, F, T>;

template<typename R, typename F, typename T>
concept replace_if_index_range_no_throw =
  ::std::ranges::random_access_range<R> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R> &&
  ::std::is_nothrow_move_constructible_v<F> &&
  ::fast_io::details::algorithms::replace_if_iter_no_throw<
    ::fast_io::details::algorithms::algo_index_iterator<R>, F, T>;

template<typename Iter, typename Snt, typename OIter, typename T>
concept replace_copy_iters_no_throw =
  ::fast_io::details::algorithms::sentinel_no_throw<Iter, Snt> &&
  ::fast_io::details::algorithms::output_iter_no_throw<
    OIter, ::std::iter_value_t<Iter>> &&
  requires(Iter &it, OIter &dest, T const &old_value, T const &new_value)
{
  { *it == old_value } noexcept;
  { *dest = new_value } noexcept;
  { *dest = *it } noexcept;
};

template<typename R, typename OIter, typename T>
concept replace_copy_ranges_no_throw =
  ::std::ranges::input_range<R> &&
  ::fast_io::details::algorithms::replace_copy_iters_no_throw<
    ::std::ranges::iterator_t<R>, ::std::ranges::sentinel_t<R>,
    OIter, T>;

template<typename Iter, typename Snt, typename OIter, typename F, typename T>
concept replace_copy_if_iters_no_throw =
  ::fast_io::details::algorithms::sentinel_no_throw<Iter, Snt> &&
  ::fast_io::details::algorithms::output_iter_no_throw<
    OIter, ::std::iter_value_t<Iter>> &&
  requires(Iter &it, OIter &dest, F &f, T const &new_value)
{
  { static_cast<bool>(f(*it)) } noexcept;
  { *dest = new_value } noexcept;
  { *dest = *it } noexcept;
};

template<typename R, typename OIter, typename F, typename T>
concept replace_copy_if_ranges_no_throw =
  ::std::ranges::input_range<R> &&
  ::std::is_nothrow_move_constructible_v<F> &&
  ::fast_io::details::algorithms::replace_copy_if_iters_no_throw<
    ::std::ranges::iterator_t<R>, ::std::ranges::sentinel_t<R>,
    OIter, F, T>;

template<typename Iter, typename Snt, typename T>
inline constexpr void replace_iters_impl(
  Iter first, Snt last, T const &old_value, T const &new_value)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::replace_iters_no_throw<Iter, Snt, T>)
{
  for (; first != last; ++first)
  {
    if (*first == old_value)
    {
      *first = new_value;
    }
  }
}

template<typename Iter, typename Snt, typename F, typename T>
inline constexpr void replace_if_iters_impl(
  Iter first, Snt last, F &f, T const &new_value)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::replace_if_iters_no_throw<
      Iter, Snt, F, T>)
{
  for (; first != last; ++first)
  {
    if (static_cast<bool>(f(*first)))
    {
      *first = new_value;
    }
  }
}

template<typename Iter, typename Snt, typename OIter, typename T>
inline constexpr OIter replace_copy_iters_impl(
  Iter first, Snt last, OIter dest, T const &old_value,
  T const &new_value)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::replace_copy_iters_no_throw<
      Iter, Snt, OIter, T>)
{
  for (; first != last; ++first, ++dest)
  {
    if (*first == old_value)
    {
      *dest = new_value;
    }
    else
    {
      *dest = *first;
    }
  }
  return dest;
}

template<typename Iter, typename Snt, typename OIter, typename F, typename T>
inline constexpr OIter replace_copy_if_iters_impl(
  Iter first, Snt last, OIter dest, F &f, T const &new_value)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::replace_copy_if_iters_no_throw<
      Iter, Snt, OIter, F, T>)
{
  for (; first != last; ++first, ++dest)
  {
    if (static_cast<bool>(f(*first)))
    {
      *dest = new_value;
    }
    else
    {
      *dest = *first;
    }
  }
  return dest;
}

} // namespace fast_io::details::algorithms

namespace fast_io::algorithms
{

template<::std::input_iterator Iter, ::std::sentinel_for<Iter> Snt,
  typename T>
inline constexpr void replace_iters(
  Iter first, Snt last, T const &old_value, T const &new_value)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::replace_iters_no_throw<Iter, Snt, T>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first)>),
    replace_iters_define, first, last, old_value, new_value);
  ::fast_io::details::algorithms::replace_iters_impl(
    first, last, old_value, new_value);
}

template<::std::ranges::range R, typename T>
inline constexpr void replace(
  R &&r, T const &old_value, T const &new_value)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::replace_ranges_no_throw<R, T>)
{
  ::fast_io::algorithms::replace_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), old_value,
    new_value);
}

template<::std::ranges::random_access_range R, typename T>
inline constexpr void replace_index_range(
  R &&r, ::std::size_t firstidx, ::std::size_t lastidx,
  T const &old_value, T const &new_value)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::replace_index_range_no_throw<R, T>)
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R>(r), firstidx, lastidx);
  ::fast_io::algorithms::replace_iters(
    first, last, old_value, new_value);
}

template<::std::input_iterator Iter, ::std::sentinel_for<Iter> Snt,
  typename F, typename T>
inline constexpr void replace_if_iters(
  Iter first, Snt last, F f, T const &new_value)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::replace_if_iters_no_throw<
      Iter, Snt, F, T>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first)>),
    replace_if_iters_define, first, last, f, new_value);
  ::fast_io::details::algorithms::replace_if_iters_impl(
    first, last, f, new_value);
}

template<::std::ranges::range R, typename F, typename T>
inline constexpr void replace_if(R &&r, F f, T const &new_value)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::replace_if_ranges_no_throw<R, F, T>)
{
  ::fast_io::algorithms::replace_if_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), ::std::move(f),
    new_value);
}

template<::std::ranges::random_access_range R, typename F, typename T>
inline constexpr void replace_if_index_range(
  R &&r, ::std::size_t firstidx, ::std::size_t lastidx, F f,
  T const &new_value)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::replace_if_index_range_no_throw<
      R, F, T>)
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R>(r), firstidx, lastidx);
  ::fast_io::algorithms::replace_if_iters(
    first, last, ::std::move(f), new_value);
}

template<::std::input_iterator Iter, ::std::sentinel_for<Iter> Snt,
  typename OIter, typename T>
inline constexpr OIter replace_copy_iters(
  Iter first, Snt last, OIter dest, T const &old_value,
  T const &new_value)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::replace_copy_iters_no_throw<
      Iter, Snt, OIter, T>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first), decltype(dest)>),
    replace_copy_iters_define, first, last, dest, old_value, new_value);
  return ::fast_io::details::algorithms::replace_copy_iters_impl(
    first, last, dest, old_value, new_value);
}

template<::std::ranges::input_range R, typename OIter, typename T>
inline constexpr OIter replace_copy(
  R &&r, OIter dest, T const &old_value, T const &new_value)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::replace_copy_ranges_no_throw<
      R, OIter, T>)
{
  return ::fast_io::algorithms::replace_copy_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), dest, old_value,
    new_value);
}

template<::std::input_iterator Iter, ::std::sentinel_for<Iter> Snt,
  typename OIter, typename F, typename T>
inline constexpr OIter replace_copy_if_iters(
  Iter first, Snt last, OIter dest, F f, T const &new_value)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::replace_copy_if_iters_no_throw<
      Iter, Snt, OIter, F, T>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first), decltype(dest)>),
    replace_copy_if_iters_define, first, last, dest, f, new_value);
  return ::fast_io::details::algorithms::replace_copy_if_iters_impl(
    first, last, dest, f, new_value);
}

template<::std::ranges::input_range R, typename OIter, typename F, typename T>
inline constexpr OIter replace_copy_if(
  R &&r, OIter dest, F f, T const &new_value)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::replace_copy_if_ranges_no_throw<
      R, OIter, F, T>)
{
  return ::fast_io::algorithms::replace_copy_if_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), dest, ::std::move(f),
    new_value);
}

} // namespace fast_io::algorithms
