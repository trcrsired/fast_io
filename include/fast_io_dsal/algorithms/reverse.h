#pragma once

namespace fast_io::details::algorithms
{

template<typename T>
inline constexpr bool reverse_block_v{
  ::fast_io::details::algorithms::copy_trivial_v<T> &&
  sizeof(T) <= 16 &&
  !::fast_io::details::algorithms::swap_adl::swappable<T &, T &>};

template<typename T>
inline constexpr void reverse_ptr(T *first, ::std::ptrdiff_t len)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !(::fast_io::details::algorithms::move_no_throw<T> &&
      ::fast_io::details::algorithms::swap_no_throw_v<T>))
{
  if (len < 2)
  {
    return;
  }
  ::std::ptrdiff_t i{0};
  ::std::ptrdiff_t j{static_cast<::std::ptrdiff_t>(len - 1)};
  if constexpr (::fast_io::details::algorithms::reverse_block_v<T>)
  {
    // Block reversal lets the compiler use vector loads/stores and element
    // shuffles. The scalar swap loop below does not get vectorized.
    while (j - i >= 7)
    {
      T a0{::std::move(first[i + 0])};
      T a1{::std::move(first[i + 1])};
      T a2{::std::move(first[i + 2])};
      T a3{::std::move(first[i + 3])};
      T b0{::std::move(first[j - 0])};
      T b1{::std::move(first[j - 1])};
      T b2{::std::move(first[j - 2])};
      T b3{::std::move(first[j - 3])};
      first[i + 0] = ::std::move(b0);
      first[i + 1] = ::std::move(b1);
      first[i + 2] = ::std::move(b2);
      first[i + 3] = ::std::move(b3);
      first[j - 0] = ::std::move(a0);
      first[j - 1] = ::std::move(a1);
      first[j - 2] = ::std::move(a2);
      first[j - 3] = ::std::move(a3);
      i += 4;
      j -= 4;
    }
  }
  while (i < j)
  {
    ::fast_io::algorithms::swap(first[i], first[j]);
    ++i;
    --j;
  }
}

template<typename Iter>
concept reverse_iter_no_throw =
  ::fast_io::details::algorithms::decrement_iter_no_throw<Iter> &&
  ::fast_io::details::algorithms::move_no_throw<::std::iter_value_t<Iter>> &&
  ::fast_io::details::algorithms::swap_no_throw_v<::std::iter_value_t<Iter>>;

template<typename Iter, typename Snt>
concept reverse_iters_no_throw =
  ::std::bidirectional_iterator<Iter> &&
  ::std::sentinel_for<Snt, Iter> &&
  ::fast_io::details::algorithms::reverse_iter_no_throw<Iter> &&
  (::std::bidirectional_iterator<Snt>
     ? ::fast_io::details::algorithms::reverse_iter_no_throw<Snt>
     : ::std::sized_sentinel_for<Snt, Iter>);

template<typename R>
concept reverse_index_range_no_throw =
  ::std::ranges::random_access_range<R> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R> &&
  ::fast_io::details::algorithms::reverse_iters_no_throw<
    ::fast_io::details::algorithms::algo_index_iterator<R>,
    ::fast_io::details::algorithms::algo_index_iterator<R>>;

template<typename Iter, typename Snt>
inline constexpr void reverse_iters_impl(Iter first, Snt last)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::reverse_iters_no_throw<Iter, Snt>)
{
  if constexpr (::std::contiguous_iterator<Iter> &&
                (::std::same_as<Iter, Snt> ||
                 ::std::sized_sentinel_for<Snt, Iter>))
  {
    ::fast_io::details::algorithms::reverse_ptr(
      ::std::to_address(first),
      static_cast<::std::ptrdiff_t>(last - first));
  }
  else if constexpr (::std::bidirectional_iterator<Snt>)
  {
    if (first == last)
    {
      return;
    }
    --last;
    while (first != last)
    {
      ::fast_io::algorithms::swap(*first, *last);
      ++first;
      if (first == last)
      {
        break;
      }
      --last;
    }
  }
  else
  {
    // The sentinel is not an iterator, but it is sized, so walk to the last
    // element once and reverse the resulting iterator pair.
    auto count{last - first};
    Iter last_iter{first};
    for (; count > 0; --count)
    {
      ++last_iter;
    }
    ::fast_io::details::algorithms::reverse_iters_impl(first, last_iter);
  }
}


template<typename Iter, typename Snt, typename OIter>
concept reverse_copy_iters_no_throw =
  ::fast_io::details::algorithms::sentinel_no_throw<Iter, Snt> &&
  ::fast_io::details::algorithms::output_iter_no_throw<
    OIter, ::std::iter_value_t<Iter>> &&
  requires(Iter &it, OIter &dest)
{
  { *dest = *it } noexcept;
};

template<typename Iter, typename Snt, typename OIter>
inline constexpr OIter reverse_copy_iters_impl(
  Iter first, Snt last, OIter dest)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::reverse_copy_iters_no_throw<
      Iter, Snt, OIter>)
{
  Iter last_iter{first};
  for (; last_iter != last; ++last_iter)
  {
  }
  while (last_iter != first)
  {
    --last_iter;
    *dest = *last_iter;
    ++dest;
  }
  return dest;
}

} // namespace fast_io::details::algorithms

namespace fast_io::algorithms
{

template<::std::bidirectional_iterator Iter, ::std::sentinel_for<Iter> Snt>
inline constexpr void reverse_iters(Iter first, Snt last)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::reverse_iters_no_throw<Iter, Snt>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first)>),
    reverse_iters_define, first, last);
  ::fast_io::details::algorithms::reverse_iters_impl(first, last);
}

template<::std::ranges::bidirectional_range R>
inline constexpr void reverse(R &&r)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::reverse_iters_no_throw<
      ::std::ranges::iterator_t<R>, ::std::ranges::sentinel_t<R>>)
{
  ::fast_io::algorithms::reverse_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r));
}

template<::std::ranges::random_access_range R>
inline constexpr void reverse_index_range(
  R &&r, ::std::size_t firstidx, ::std::size_t lastidx)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::reverse_index_range_no_throw<R>)
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R>(r), firstidx, lastidx);
  ::fast_io::algorithms::reverse_iters(first, last);
}


// Copy `[first, last)` to `dest` in reverse order.
template<::std::bidirectional_iterator Iter, ::std::sentinel_for<Iter> Snt,
  typename OIter>
inline constexpr OIter reverse_copy_iters(Iter first, Snt last, OIter dest)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::reverse_copy_iters_no_throw<
      Iter, Snt, OIter>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first), decltype(dest)>),
    reverse_copy_iters_define, first, last, dest);
  return ::fast_io::details::algorithms::reverse_copy_iters_impl(
    first, last, dest);
}

template<::std::ranges::bidirectional_range R, typename OIter>
inline constexpr OIter reverse_copy(R &&r, OIter dest)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::reverse_copy_iters_no_throw<
      ::std::ranges::iterator_t<R>, ::std::ranges::sentinel_t<R>,
      OIter>)
{
  return ::fast_io::algorithms::reverse_copy_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), dest);
}

} // namespace fast_io::algorithms
