#pragma once

namespace fast_io::details::algorithms
{

template<typename Iter1, typename Snt1, typename Iter2, typename Snt2,
  typename Cmp>
concept set_iters_no_throw =
  ::fast_io::details::algorithms::sentinel_no_throw<Iter1, Snt1> &&
  ::fast_io::details::algorithms::sentinel_no_throw<Iter2, Snt2> &&
  ::fast_io::details::algorithms::comparator_pass_no_throw<Cmp> &&
  requires(Cmp cmp, Iter1 &it1, Iter2 &it2)
{
  { 0 > cmp.compare(*it1, *it2) } noexcept;
  { 0 > cmp.compare(*it2, *it1) } noexcept;
};

template<typename Iter1, typename Snt1, typename Iter2, typename Snt2,
  typename OIter, typename Cmp>
concept set_output_iters_no_throw =
  ::fast_io::details::algorithms::set_iters_no_throw<
    Iter1, Snt1, Iter2, Snt2, Cmp> &&
  ::fast_io::details::algorithms::output_iter_no_throw<
    OIter, ::std::iter_value_t<Iter1>> &&
  requires(Iter1 &it1, Iter2 &it2, OIter &dest)
{
  { *dest = *it1 } noexcept;
  { *dest = *it2 } noexcept;
};

template<typename R1, typename R2, typename Cmp>
concept set_ranges_no_throw =
  ::std::ranges::input_range<R1> &&
  ::std::ranges::input_range<R2> &&
  ::std::is_nothrow_move_constructible_v<Cmp> &&
  ::fast_io::details::algorithms::set_iters_no_throw<
    ::std::ranges::iterator_t<R1>, ::std::ranges::sentinel_t<R1>,
    ::std::ranges::iterator_t<R2>, ::std::ranges::sentinel_t<R2>, Cmp>;

template<typename R1, typename R2, typename OIter, typename Cmp>
concept set_output_ranges_no_throw =
  ::std::ranges::input_range<R1> &&
  ::std::ranges::input_range<R2> &&
  ::std::is_nothrow_move_constructible_v<Cmp> &&
  ::fast_io::details::algorithms::set_output_iters_no_throw<
    ::std::ranges::iterator_t<R1>, ::std::ranges::sentinel_t<R1>,
    ::std::ranges::iterator_t<R2>, ::std::ranges::sentinel_t<R2>,
    OIter, Cmp>;

template<typename R1, typename R2, typename Cmp>
concept set_index_range_no_throw =
  ::std::ranges::random_access_range<R1> &&
  ::std::ranges::random_access_range<R2> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R1> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R2> &&
  ::fast_io::details::algorithms::set_iters_no_throw<
    ::fast_io::details::algorithms::algo_index_iterator<R1>,
    ::fast_io::details::algorithms::algo_index_iterator<R1>,
    ::fast_io::details::algorithms::algo_index_iterator<R2>,
    ::fast_io::details::algorithms::algo_index_iterator<R2>, Cmp>;

template<typename R1, typename R2, typename OIter, typename Cmp>
concept set_output_index_range_no_throw =
  ::std::ranges::random_access_range<R1> &&
  ::std::ranges::random_access_range<R2> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R1> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R2> &&
  ::fast_io::details::algorithms::set_output_iters_no_throw<
    ::fast_io::details::algorithms::algo_index_iterator<R1>,
    ::fast_io::details::algorithms::algo_index_iterator<R1>,
    ::fast_io::details::algorithms::algo_index_iterator<R2>,
    ::fast_io::details::algorithms::algo_index_iterator<R2>,
    OIter, Cmp>;

template<typename Iter1, typename Snt1, typename Iter2, typename Snt2,
  typename Cmp>
inline constexpr bool includes_iters_impl(
  Iter1 first1, Snt1 last1, Iter2 first2, Snt2 last2, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::set_iters_no_throw<
      Iter1, Snt1, Iter2, Snt2, Cmp>)
{
  while (first2 != last2)
  {
    if (first1 == last1 || 0 > cmp.compare(*first2, *first1))
    {
      return false;
    }
    if (!(0 > cmp.compare(*first1, *first2)))
    {
      ++first2;
    }
    ++first1;
  }
  return true;
}

template<typename Iter1, typename Snt1, typename Iter2, typename Snt2,
  typename OIter, typename Cmp>
inline constexpr OIter set_difference_iters_impl(
  Iter1 first1, Snt1 last1, Iter2 first2, Snt2 last2, OIter dest, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::set_output_iters_no_throw<
      Iter1, Snt1, Iter2, Snt2, OIter, Cmp>)
{
  while (first1 != last1)
  {
    if (first2 == last2)
    {
      break;
    }
    if (0 > cmp.compare(*first1, *first2))
    {
      *dest = *first1;
      ++dest;
      ++first1;
    }
    else if (0 > cmp.compare(*first2, *first1))
    {
      ++first2;
    }
    else
    {
      ++first1;
      ++first2;
    }
  }
  for (; first1 != last1; ++first1, ++dest)
  {
    *dest = *first1;
  }
  return dest;
}

template<typename Iter1, typename Snt1, typename Iter2, typename Snt2,
  typename OIter, typename Cmp>
inline constexpr OIter set_intersection_iters_impl(
  Iter1 first1, Snt1 last1, Iter2 first2, Snt2 last2, OIter dest, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::set_output_iters_no_throw<
      Iter1, Snt1, Iter2, Snt2, OIter, Cmp>)
{
  while (first1 != last1 && first2 != last2)
  {
    if (0 > cmp.compare(*first1, *first2))
    {
      ++first1;
    }
    else if (0 > cmp.compare(*first2, *first1))
    {
      ++first2;
    }
    else
    {
      *dest = *first1;
      ++dest;
      ++first1;
      ++first2;
    }
  }
  return dest;
}

template<typename Iter1, typename Snt1, typename Iter2, typename Snt2,
  typename OIter, typename Cmp>
inline constexpr OIter set_symmetric_difference_iters_impl(
  Iter1 first1, Snt1 last1, Iter2 first2, Snt2 last2, OIter dest, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::set_output_iters_no_throw<
      Iter1, Snt1, Iter2, Snt2, OIter, Cmp>)
{
  while (first1 != last1 && first2 != last2)
  {
    if (0 > cmp.compare(*first1, *first2))
    {
      *dest = *first1;
      ++dest;
      ++first1;
    }
    else if (0 > cmp.compare(*first2, *first1))
    {
      *dest = *first2;
      ++dest;
      ++first2;
    }
    else
    {
      ++first1;
      ++first2;
    }
  }
  for (; first1 != last1; ++first1, ++dest)
  {
    *dest = *first1;
  }
  for (; first2 != last2; ++first2, ++dest)
  {
    *dest = *first2;
  }
  return dest;
}

template<typename Iter1, typename Snt1, typename Iter2, typename Snt2,
  typename OIter, typename Cmp>
inline constexpr OIter set_union_iters_impl(
  Iter1 first1, Snt1 last1, Iter2 first2, Snt2 last2, OIter dest, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::set_output_iters_no_throw<
      Iter1, Snt1, Iter2, Snt2, OIter, Cmp>)
{
  while (first1 != last1 && first2 != last2)
  {
    if (0 > cmp.compare(*first1, *first2))
    {
      *dest = *first1;
      ++dest;
      ++first1;
    }
    else if (0 > cmp.compare(*first2, *first1))
    {
      *dest = *first2;
      ++dest;
      ++first2;
    }
    else
    {
      *dest = *first1;
      ++dest;
      ++first1;
      ++first2;
    }
  }
  for (; first1 != last1; ++first1, ++dest)
  {
    *dest = *first1;
  }
  for (; first2 != last2; ++first2, ++dest)
  {
    *dest = *first2;
  }
  return dest;
}

} // namespace fast_io::details::algorithms

namespace fast_io::algorithms
{

template<::std::input_iterator Iter1, ::std::sentinel_for<Iter1> Snt1,
  ::std::input_iterator Iter2, ::std::sentinel_for<Iter2> Snt2,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr bool includes_iters(
  Iter1 first1, Snt1 last1, Iter2 first2, Snt2 last2, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::set_iters_no_throw<
      Iter1, Snt1, Iter2, Snt2, Cmp>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<Iter1, Iter2>),
    includes_iters_define, first1, last1, first2, last2, cmp);
  return ::fast_io::details::algorithms::includes_iters_impl(
    first1, last1, first2, last2, cmp);
}

template<::std::ranges::input_range R1, ::std::ranges::input_range R2,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr bool includes(R1 &&r1, R2 &&r2, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::set_ranges_no_throw<R1, R2, Cmp>)
{
  return ::fast_io::algorithms::includes_iters(
    ::std::ranges::begin(r1), ::std::ranges::end(r1),
    ::std::ranges::begin(r2), ::std::ranges::end(r2), cmp);
}

template<::std::ranges::random_access_range R1,
  ::std::ranges::random_access_range R2,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr bool includes_index_range(
  R1 &&r1, ::std::size_t firstidx1, ::std::size_t lastidx1,
  R2 &&r2, ::std::size_t firstidx2, ::std::size_t lastidx2,
  Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::set_index_range_no_throw<R1, R2, Cmp>)
{
  auto [first1, last1] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R1>(r1), firstidx1, lastidx1);
  auto [first2, last2] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R2>(r2), firstidx2, lastidx2);
  return ::fast_io::algorithms::includes_iters(
    first1, last1, first2, last2, cmp);
}

#define FAST_IO_ALGORITHMS_SET_OUTPUT_ENTRY(name)                            \
  template<::std::input_iterator Iter1, ::std::sentinel_for<Iter1> Snt1,     \
    ::std::input_iterator Iter2, ::std::sentinel_for<Iter2> Snt2,            \
    typename OIter, typename Cmp = ::fast_io::algorithms::compare_three_way> \
  inline constexpr OIter name##_iters(                                       \
    Iter1 first1, Snt1 last1, Iter2 first2, Snt2 last2, OIter dest,          \
    Cmp cmp = Cmp{})                                                         \
    FAST_IO_HERBCEPTIONS_THROWS_IF(                                          \
      !::fast_io::details::algorithms::set_output_iters_no_throw<            \
        Iter1, Snt1, Iter2, Snt2, OIter, Cmp>)                              \
  {                                                                          \
    FAST_IO_ALGORITHMS_CUSTOMIZE_IF(                                         \
      (!::fast_io::details::algorithms::contiguous_iters<Iter1, Iter2,       \
        OIter>),                                                             \
      name##_iters_define, first1, last1, first2, last2, dest, cmp);         \
    return ::fast_io::details::algorithms::name##_iters_impl(                \
      first1, last1, first2, last2, dest, cmp);                              \
  }                                                                          \
  template<::std::ranges::input_range R1, ::std::ranges::input_range R2,     \
    typename OIter, typename Cmp = ::fast_io::algorithms::compare_three_way> \
  inline constexpr OIter name(                                               \
    R1 &&r1, R2 &&r2, OIter dest, Cmp cmp = Cmp{})                           \
    FAST_IO_HERBCEPTIONS_THROWS_IF(                                          \
      !::fast_io::details::algorithms::set_output_ranges_no_throw<           \
        R1, R2, OIter, Cmp>)                                                 \
  {                                                                          \
    return ::fast_io::algorithms::name##_iters(                              \
      ::std::ranges::begin(r1), ::std::ranges::end(r1),                      \
      ::std::ranges::begin(r2), ::std::ranges::end(r2), dest, cmp);          \
  }                                                                          \
  template<::std::ranges::random_access_range R1,                            \
    ::std::ranges::random_access_range R2, typename OIter,                   \
    typename Cmp = ::fast_io::algorithms::compare_three_way>                 \
  inline constexpr OIter name##_index_range(                                 \
    R1 &&r1, ::std::size_t firstidx1, ::std::size_t lastidx1,                \
    R2 &&r2, ::std::size_t firstidx2, ::std::size_t lastidx2,                \
    OIter dest, Cmp cmp = Cmp{})                                             \
    FAST_IO_HERBCEPTIONS_THROWS_IF(                                          \
      !::fast_io::details::algorithms::set_output_index_range_no_throw<      \
        R1, R2, OIter, Cmp>)                                                 \
  {                                                                          \
    auto [first1, last1] =                                                   \
      ::fast_io::details::algorithms::algo_index_to_iters(                   \
        ::std::forward<R1>(r1), firstidx1, lastidx1);                        \
    auto [first2, last2] =                                                   \
      ::fast_io::details::algorithms::algo_index_to_iters(                   \
        ::std::forward<R2>(r2), firstidx2, lastidx2);                        \
    return ::fast_io::algorithms::name##_iters(                              \
      first1, last1, first2, last2, dest, cmp);                              \
  }

FAST_IO_ALGORITHMS_SET_OUTPUT_ENTRY(set_difference)
FAST_IO_ALGORITHMS_SET_OUTPUT_ENTRY(set_intersection)
FAST_IO_ALGORITHMS_SET_OUTPUT_ENTRY(set_symmetric_difference)
FAST_IO_ALGORITHMS_SET_OUTPUT_ENTRY(set_union)

#undef FAST_IO_ALGORITHMS_SET_OUTPUT_ENTRY

} // namespace fast_io::algorithms
