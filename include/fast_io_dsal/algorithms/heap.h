#pragma once

namespace fast_io::details::algorithms
{

template<typename T>
inline constexpr bool heap_move_no_throw{
  ::std::is_nothrow_move_constructible_v<T> &&
  ::std::is_nothrow_move_assignable_v<T>};

template<typename Cmp, typename T>
concept heap_compare_no_throw =
  ::fast_io::details::algorithms::three_way_comparator_for<Cmp, T>;

template<typename Cmp>
concept heap_comparator_pass_no_throw =
  ::std::is_nothrow_copy_constructible_v<Cmp> &&
  ::std::is_nothrow_move_constructible_v<Cmp> &&
  ::std::is_nothrow_default_constructible_v<Cmp>;

template<typename Iter, typename Cmp>
concept is_heap_iters_no_throw =
  ::fast_io::details::algorithms::iter_no_throw<Iter> &&
  ::fast_io::details::algorithms::heap_compare_no_throw<
    Cmp, ::std::iter_value_t<Iter>>;

template<typename Iter, typename Cmp>
concept heap_iters_no_throw =
  ::fast_io::details::algorithms::is_heap_iters_no_throw<Iter, Cmp> &&
  ::fast_io::details::algorithms::heap_move_no_throw<::std::iter_value_t<Iter>>;

template<typename R, typename Cmp>
concept is_heap_index_range_no_throw =
  ::std::ranges::random_access_range<R> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R> &&
  ::fast_io::details::algorithms::is_heap_iters_no_throw<
    ::fast_io::details::algorithms::algo_index_iterator<R>, Cmp> &&
  ::fast_io::details::algorithms::heap_comparator_pass_no_throw<Cmp>;

template<typename R, typename Cmp>
concept heap_index_range_no_throw =
  ::std::ranges::random_access_range<R> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R> &&
  ::fast_io::details::algorithms::heap_iters_no_throw<
    ::fast_io::details::algorithms::algo_index_iterator<R>, Cmp> &&
  ::fast_io::details::algorithms::heap_comparator_pass_no_throw<Cmp>;

// Move `value` down from `idx` while the heap property is violated. The hole
// at `idx` is filled with `value` at the end, so the element is only
// move-assigned once per level and never swapped.
template<typename Iter, typename Cmp>
inline constexpr void heap_sift_down(
  Iter first, ::std::iter_difference_t<Iter> idx,
  ::std::iter_difference_t<Iter> n, ::std::iter_value_t<Iter> value, Cmp &cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !(::fast_io::details::algorithms::heap_compare_no_throw<
        Cmp, ::std::iter_value_t<Iter>> &&
      ::fast_io::details::algorithms::heap_move_no_throw<
        ::std::iter_value_t<Iter>>))
{
  using difference_type = ::std::iter_difference_t<Iter>;
  for (;;)
  {
    difference_type child{static_cast<difference_type>((idx << 1) + 1)};
    if (child >= n)
    {
      break;
    }
    if (child + 1 < n && cmp.compare(first[child], first[child + 1]) < 0)
    {
      ++child;
    }
    if (!(cmp.compare(value, first[child]) < 0))
    {
      break;
    }
    first[idx] = ::std::move(first[child]);
    idx = child;
  }
  first[idx] = ::std::move(value);
}

// Move `value` up from `idx` while it is greater than its parent.
template<typename Iter, typename Cmp>
inline constexpr void heap_sift_up(
  Iter first, ::std::iter_difference_t<Iter> idx,
  ::std::iter_value_t<Iter> value, Cmp &cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !(::fast_io::details::algorithms::heap_compare_no_throw<
        Cmp, ::std::iter_value_t<Iter>> &&
      ::fast_io::details::algorithms::heap_move_no_throw<
        ::std::iter_value_t<Iter>>))
{
  using difference_type = ::std::iter_difference_t<Iter>;
  while (idx != 0)
  {
    difference_type const parent{static_cast<difference_type>((idx - 1) >> 1)};
    if (!(cmp.compare(first[parent], value) < 0))
    {
      break;
    }
    first[idx] = ::std::move(first[parent]);
    idx = parent;
  }
  first[idx] = ::std::move(value);
}

template<typename Iter, typename Cmp>
inline constexpr Iter is_heap_until_iters_impl(Iter first, Iter last, Cmp &cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::is_heap_iters_no_throw<Iter, Cmp>)
{
  if constexpr (::std::contiguous_iterator<Iter> && !::std::is_pointer_v<Iter>)
  {
    auto const first_pointer{::std::to_address(first)};
    auto const found{::fast_io::details::algorithms::is_heap_until_iters_impl(
      first_pointer, ::std::to_address(last), cmp)};
    return first + static_cast<::std::iter_difference_t<Iter>>(found - first_pointer);
  }
  else
  {
    using difference_type = ::std::iter_difference_t<Iter>;
    auto const n{last - first};
    for (difference_type idx{1}; idx < n; ++idx)
    {
      if (cmp.compare(first[static_cast<difference_type>((idx - 1) >> 1)], first[idx]) < 0)
      {
        return first + idx;
      }
    }
    return last;
  }
}

template<typename Iter, typename Cmp>
inline constexpr void make_heap_iters_impl(Iter first, Iter last, Cmp &cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::heap_iters_no_throw<Iter, Cmp>)
{
  if constexpr (::std::contiguous_iterator<Iter> && !::std::is_pointer_v<Iter>)
  {
    ::fast_io::details::algorithms::make_heap_iters_impl(
      ::std::to_address(first), ::std::to_address(last), cmp);
  }
  else
  {
    using difference_type = ::std::iter_difference_t<Iter>;
    auto const n{last - first};
    if (n < 2)
    {
      return;
    }
    for (difference_type i{static_cast<difference_type>((n - 2) >> 1)};; --i)
    {
      auto value{::std::move(first[i])};
      ::fast_io::details::algorithms::heap_sift_down(
        first, i, n, ::std::move(value), cmp);
      if (i == 0)
      {
        break;
      }
    }
  }
}

template<typename Iter, typename Cmp>
inline constexpr void push_heap_iters_impl(Iter first, Iter last, Cmp &cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::heap_iters_no_throw<Iter, Cmp>)
{
  if constexpr (::std::contiguous_iterator<Iter> && !::std::is_pointer_v<Iter>)
  {
    ::fast_io::details::algorithms::push_heap_iters_impl(
      ::std::to_address(first), ::std::to_address(last), cmp);
  }
  else
  {
    auto const n{last - first};
    if (n < 2)
    {
      return;
    }
    auto value{::std::move(first[n - 1])};
    ::fast_io::details::algorithms::heap_sift_up(
      first, n - 1, ::std::move(value), cmp);
  }
}

template<typename Iter, typename Cmp>
inline constexpr void pop_heap_iters_impl(Iter first, Iter last, Cmp &cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::heap_iters_no_throw<Iter, Cmp>)
{
  if constexpr (::std::contiguous_iterator<Iter> && !::std::is_pointer_v<Iter>)
  {
    ::fast_io::details::algorithms::pop_heap_iters_impl(
      ::std::to_address(first), ::std::to_address(last), cmp);
  }
  else
  {
    auto n{last - first};
    if (n < 2)
    {
      return;
    }
    --n;
    auto temp{::std::move(first[0])};
    first[0] = ::std::move(first[n]);
    first[n] = ::std::move(temp);
    auto value{::std::move(first[0])};
    ::fast_io::details::algorithms::heap_sift_down(
      first, 0, n, ::std::move(value), cmp);
  }
}

template<typename Iter, typename Cmp>
inline constexpr void sort_heap_iters_impl(Iter first, Iter last, Cmp &cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::heap_iters_no_throw<Iter, Cmp>)
{
  if constexpr (::std::contiguous_iterator<Iter> && !::std::is_pointer_v<Iter>)
  {
    ::fast_io::details::algorithms::sort_heap_iters_impl(
      ::std::to_address(first), ::std::to_address(last), cmp);
  }
  else
  {
    auto n{last - first};
    while (n > 1)
    {
      --n;
      auto temp{::std::move(first[0])};
      first[0] = ::std::move(first[n]);
      first[n] = ::std::move(temp);
      auto value{::std::move(first[0])};
      ::fast_io::details::algorithms::heap_sift_down(
        first, 0, n, ::std::move(value), cmp);
    }
  }
}

} // namespace fast_io::details::algorithms

namespace fast_io::algorithms
{

template<::std::random_access_iterator Iter,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
  requires ::fast_io::details::algorithms::three_way_comparator<
    Cmp, ::std::iter_value_t<Iter>>
inline constexpr Iter is_heap_until_iters(Iter first, Iter last, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !(::fast_io::details::algorithms::is_heap_iters_no_throw<Iter, Cmp> &&
      ::fast_io::details::algorithms::heap_comparator_pass_no_throw<Cmp>))
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first)>),
    is_heap_until_iters_define, first, last, cmp);
  return ::fast_io::details::algorithms::is_heap_until_iters_impl(first, last, cmp);
}

template<::std::ranges::random_access_range R,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr ::std::ranges::iterator_t<R> is_heap_until(R &&r, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !(::fast_io::details::algorithms::is_heap_iters_no_throw<
        ::std::ranges::iterator_t<R>, Cmp> &&
      ::fast_io::details::algorithms::heap_comparator_pass_no_throw<Cmp>))
{
  return ::fast_io::algorithms::is_heap_until_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), cmp);
}

template<::std::random_access_iterator Iter,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
  requires ::fast_io::details::algorithms::three_way_comparator<
    Cmp, ::std::iter_value_t<Iter>>
inline constexpr bool is_heap_iters(Iter first, Iter last, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !(::fast_io::details::algorithms::is_heap_iters_no_throw<Iter, Cmp> &&
      ::fast_io::details::algorithms::heap_comparator_pass_no_throw<Cmp>))
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first)>),
    is_heap_iters_define, first, last, cmp);
  return ::fast_io::details::algorithms::is_heap_until_iters_impl(first, last, cmp) == last;
}

template<::std::ranges::random_access_range R,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr bool is_heap(R &&r, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !(::fast_io::details::algorithms::is_heap_iters_no_throw<
        ::std::ranges::iterator_t<R>, Cmp> &&
      ::fast_io::details::algorithms::heap_comparator_pass_no_throw<Cmp>))
{
  return ::fast_io::algorithms::is_heap_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), cmp);
}

template<::std::random_access_iterator Iter,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
  requires ::fast_io::details::algorithms::three_way_comparator<
    Cmp, ::std::iter_value_t<Iter>>
inline constexpr void make_heap_iters(Iter first, Iter last, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !(::fast_io::details::algorithms::heap_iters_no_throw<Iter, Cmp> &&
      ::fast_io::details::algorithms::heap_comparator_pass_no_throw<Cmp>))
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first)>),
    make_heap_iters_define, first, last, cmp);
  ::fast_io::details::algorithms::make_heap_iters_impl(first, last, cmp);
}

template<::std::ranges::random_access_range R,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr void make_heap(R &&r, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !(::fast_io::details::algorithms::heap_iters_no_throw<
        ::std::ranges::iterator_t<R>, Cmp> &&
      ::fast_io::details::algorithms::heap_comparator_pass_no_throw<Cmp>))
{
  ::fast_io::algorithms::make_heap_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), cmp);
}

template<::std::random_access_iterator Iter,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
  requires ::fast_io::details::algorithms::three_way_comparator<
    Cmp, ::std::iter_value_t<Iter>>
inline constexpr void push_heap_iters(Iter first, Iter last, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !(::fast_io::details::algorithms::heap_iters_no_throw<Iter, Cmp> &&
      ::fast_io::details::algorithms::heap_comparator_pass_no_throw<Cmp>))
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first)>),
    push_heap_iters_define, first, last, cmp);
  ::fast_io::details::algorithms::push_heap_iters_impl(first, last, cmp);
}

template<::std::ranges::random_access_range R,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr void push_heap(R &&r, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !(::fast_io::details::algorithms::heap_iters_no_throw<
        ::std::ranges::iterator_t<R>, Cmp> &&
      ::fast_io::details::algorithms::heap_comparator_pass_no_throw<Cmp>))
{
  ::fast_io::algorithms::push_heap_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), cmp);
}

template<::std::random_access_iterator Iter,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
  requires ::fast_io::details::algorithms::three_way_comparator<
    Cmp, ::std::iter_value_t<Iter>>
inline constexpr void pop_heap_iters(Iter first, Iter last, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !(::fast_io::details::algorithms::heap_iters_no_throw<Iter, Cmp> &&
      ::fast_io::details::algorithms::heap_comparator_pass_no_throw<Cmp>))
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first)>),
    pop_heap_iters_define, first, last, cmp);
  ::fast_io::details::algorithms::pop_heap_iters_impl(first, last, cmp);
}

template<::std::ranges::random_access_range R,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr void pop_heap(R &&r, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !(::fast_io::details::algorithms::heap_iters_no_throw<
        ::std::ranges::iterator_t<R>, Cmp> &&
      ::fast_io::details::algorithms::heap_comparator_pass_no_throw<Cmp>))
{
  ::fast_io::algorithms::pop_heap_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), cmp);
}

template<::std::random_access_iterator Iter,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
  requires ::fast_io::details::algorithms::three_way_comparator<
    Cmp, ::std::iter_value_t<Iter>>
inline constexpr void sort_heap_iters(Iter first, Iter last, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !(::fast_io::details::algorithms::heap_iters_no_throw<Iter, Cmp> &&
      ::fast_io::details::algorithms::heap_comparator_pass_no_throw<Cmp>))
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first)>),
    sort_heap_iters_define, first, last, cmp);
  ::fast_io::details::algorithms::sort_heap_iters_impl(first, last, cmp);
}

template<::std::ranges::random_access_range R,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr void sort_heap(R &&r, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !(::fast_io::details::algorithms::heap_iters_no_throw<
        ::std::ranges::iterator_t<R>, Cmp> &&
      ::fast_io::details::algorithms::heap_comparator_pass_no_throw<Cmp>))
{
  ::fast_io::algorithms::sort_heap_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), cmp);
}

template<::std::ranges::random_access_range R,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr ::std::size_t is_heap_until_index_range(
  R &&r, ::std::size_t firstidx, ::std::size_t lastidx, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::is_heap_index_range_no_throw<R, Cmp>)
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R>(r), firstidx, lastidx);
  auto const found{
    ::fast_io::algorithms::is_heap_until_iters(first, last, cmp)};
  return firstidx + static_cast<::std::size_t>(found - first);
}

template<::std::ranges::random_access_range R,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr ::std::size_t is_heap_until_index_of(
  R &&r, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !(::fast_io::details::algorithms::is_heap_iters_no_throw<
        ::std::ranges::iterator_t<R>, Cmp> &&
      ::fast_io::details::algorithms::heap_comparator_pass_no_throw<Cmp>))
{
  auto const first{::std::ranges::begin(r)};
  auto const found{::fast_io::algorithms::is_heap_until(
    ::std::forward<R>(r), cmp)};
  return static_cast<::std::size_t>(found - first);
}

template<::std::ranges::random_access_range R,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr bool is_heap_index_range(
  R &&r, ::std::size_t firstidx, ::std::size_t lastidx, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::is_heap_index_range_no_throw<R, Cmp>)
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R>(r), firstidx, lastidx);
  return ::fast_io::algorithms::is_heap_iters(first, last, cmp);
}

template<::std::ranges::random_access_range R,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr void make_heap_index_range(
  R &&r, ::std::size_t firstidx, ::std::size_t lastidx, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::heap_index_range_no_throw<R, Cmp>)
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R>(r), firstidx, lastidx);
  ::fast_io::algorithms::make_heap_iters(first, last, cmp);
}

template<::std::ranges::random_access_range R,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr void push_heap_index_range(
  R &&r, ::std::size_t firstidx, ::std::size_t lastidx, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::heap_index_range_no_throw<R, Cmp>)
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R>(r), firstidx, lastidx);
  ::fast_io::algorithms::push_heap_iters(first, last, cmp);
}

template<::std::ranges::random_access_range R,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr void pop_heap_index_range(
  R &&r, ::std::size_t firstidx, ::std::size_t lastidx, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::heap_index_range_no_throw<R, Cmp>)
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R>(r), firstidx, lastidx);
  ::fast_io::algorithms::pop_heap_iters(first, last, cmp);
}

template<::std::ranges::random_access_range R,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr void sort_heap_index_range(
  R &&r, ::std::size_t firstidx, ::std::size_t lastidx, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::heap_index_range_no_throw<R, Cmp>)
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R>(r), firstidx, lastidx);
  ::fast_io::algorithms::sort_heap_iters(first, last, cmp);
}

} // namespace fast_io::algorithms
