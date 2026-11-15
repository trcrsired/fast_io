#pragma once

// Unstable in-place sort, ported from ipnsort by Lukas Bergdoll and Orson
// Peters (sort-research-rs).
//
// Components:
//   - insertion sort for very small inputs
//   - ascending/descending run detection
//   - introsort-style quicksort with a recursion depth limit
//   - glidesort-style adaptive pseudomedian pivot selection
//   - branchless cyclic Lomuto partition for small/medium elements
//   - branchy cyclic Hoare partition for large elements
//   - ancestor pivot tracking for low-cardinality inputs
//   - branchless heapsort fallback
//   - sorting networks + bidirectional merge small sort for small elements
//   - merge-based small sort for medium trivially copyable elements
//
// Elements must be movable. The fast small sorts are used for trivially
// copyable elements only; everything else falls back to insertion sort for
// small subranges.

namespace fast_io::details::algorithms
{

template<typename Cmp>
concept sort_comparator_pass_no_throw =
  ::std::is_nothrow_copy_constructible_v<Cmp> &&
  ::std::is_nothrow_move_constructible_v<Cmp> &&
  ::std::is_nothrow_default_constructible_v<Cmp>;

template<typename Cmp, typename T>
concept sort_compare_no_throw =
  requires(Cmp cmp, T const &a, T const &b)
{
  { 0 > cmp.compare(a, b) } noexcept;
  { 0 > cmp.compare(b, a) } noexcept;
};

// Comparators may provide `equal(a, b)` as a cheaper equality test. When it is
// absent, equality falls back to `compare(a, b) == 0`.
template<typename Cmp, typename T>
inline constexpr bool sort_equal_no_throw_v = []() constexpr
{
  if constexpr (requires(Cmp cmp, T const &a, T const &b) { cmp.equal(a, b); })
  {
    return ::fast_io::details::algorithms::sort_comparator_pass_no_throw<Cmp> &&
           requires(Cmp cmp, T const &a, T const &b)
    {
      { cmp.equal(a, b) } noexcept;
    };
  }
  else
  {
    return ::fast_io::details::algorithms::sort_comparator_pass_no_throw<Cmp> &&
           requires(Cmp cmp, T const &a, T const &b)
    {
      { cmp.compare(a, b) == 0 } noexcept;
    };
  }
}();

template<typename Cmp, typename T>
inline constexpr bool sort_elements_equal(Cmp cmp, T const &a, T const &b)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::sort_equal_no_throw_v<Cmp, T>)
{
  if constexpr (requires { cmp.equal(a, b); })
  {
    return cmp.equal(a, b);
  }
  else
  {
    return cmp.compare(a, b) == 0;
  }
}

template<typename Cmp, typename T>
concept sort_ops_no_throw =
  ::fast_io::details::algorithms::sort_comparator_pass_no_throw<Cmp> &&
  ::fast_io::details::algorithms::sort_compare_no_throw<Cmp, T> &&
  ::fast_io::details::algorithms::move_no_throw<T> &&
  ::fast_io::details::algorithms::swap_no_throw_v<T>;

template<typename Cmp, typename T>
concept sort_copy_ops_no_throw =
  ::fast_io::details::algorithms::sort_comparator_pass_no_throw<Cmp> &&
  ::fast_io::details::algorithms::sort_compare_no_throw<Cmp, T> &&
  ::std::is_nothrow_copy_constructible_v<T> &&
  ::std::is_nothrow_copy_assignable_v<T> &&
  ::std::is_nothrow_destructible_v<T>;

template<typename Iter, typename Cmp>
concept sort_ops_iter_no_throw =
  ::fast_io::details::algorithms::iter_no_throw<Iter> &&
  ::fast_io::details::algorithms::sort_ops_no_throw<
    Cmp, ::std::iter_value_t<Iter>>;

template<typename Iter, typename Cmp>
concept sort_copy_ops_iter_no_throw =
  ::fast_io::details::algorithms::iter_no_throw<Iter> &&
  ::fast_io::details::algorithms::sort_copy_ops_no_throw<
    Cmp, ::std::iter_value_t<Iter>>;

template<typename Iter, typename Cmp>
inline constexpr bool sort_equal_iter_no_throw_v{
  ::fast_io::details::algorithms::iter_no_throw<Iter> &&
  ::fast_io::details::algorithms::sort_equal_no_throw_v<
    Cmp, ::std::iter_value_t<Iter>>};

template<typename Iter, typename Cmp>
concept sort_iters_no_throw =
  ::std::random_access_iterator<Iter> &&
  ::fast_io::details::algorithms::decrement_iter_no_throw<Iter> &&
  ::fast_io::details::algorithms::sort_compare_no_throw<
    Cmp, ::std::iter_value_t<Iter>> &&
  ::fast_io::details::algorithms::sort_equal_no_throw_v<
    Cmp, ::std::iter_value_t<Iter>> &&
  ::fast_io::details::algorithms::move_no_throw<
    ::std::iter_value_t<Iter>> &&
  ::fast_io::details::algorithms::swap_no_throw_v<
    ::std::iter_value_t<Iter>>;

template<typename R, typename Cmp>
concept sort_index_range_no_throw =
  ::std::ranges::random_access_range<R> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R> &&
  ::fast_io::details::algorithms::sort_iters_no_throw<
    ::fast_io::details::algorithms::algo_index_iterator<R>, Cmp> &&
  ::fast_io::details::algorithms::sort_comparator_pass_no_throw<Cmp>;

template<typename T>
inline constexpr bool sort_network_small_v{
  ::std::is_trivially_copyable_v<T> &&
  ::std::is_nothrow_copy_constructible_v<T> &&
  ::std::is_nothrow_copy_assignable_v<T> &&
  sizeof(T) <= sizeof(::std::uint64_t) &&
  sizeof(T) * 32 <= 4096};

template<typename T>
inline constexpr bool sort_general_small_v{
  !::fast_io::details::algorithms::sort_network_small_v<T> &&
  ::std::is_trivially_copyable_v<T> &&
  ::std::is_nothrow_copy_constructible_v<T> &&
  ::std::is_nothrow_copy_assignable_v<T> &&
  sizeof(T) * 48 <= 4096};

template<typename T>
inline constexpr ::std::ptrdiff_t sort_small_sort_threshold_v{
  (::fast_io::details::algorithms::sort_network_small_v<T> ||
   ::fast_io::details::algorithms::sort_general_small_v<T>)
    ? 32
    : 16};

template<typename Cmp, typename T>
inline constexpr bool sort_small_ops_no_throw = []() constexpr
{
  if constexpr (::fast_io::details::algorithms::sort_network_small_v<T> ||
                ::fast_io::details::algorithms::sort_general_small_v<T>)
  {
    return ::fast_io::details::algorithms::sort_comparator_pass_no_throw<Cmp> &&
           ::fast_io::details::algorithms::sort_compare_no_throw<Cmp, T> &&
           ::std::is_nothrow_copy_constructible_v<T> &&
           ::std::is_nothrow_copy_assignable_v<T> &&
           ::std::is_nothrow_destructible_v<T>;
  }
  else
  {
    return ::fast_io::details::algorithms::sort_ops_no_throw<Cmp, T>;
  }
}();

// Insert the element at `tail` into the already sorted range
// [`begin`, `tail`), using the hole technique.
template<typename Iter, typename Cmp>
inline constexpr void sort_insert_tail(
  Iter begin, Iter tail, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::sort_ops_iter_no_throw<Iter, Cmp>)
{
  using value_type = ::std::iter_value_t<Iter>;
  Iter sift{tail - 1};
  if (0 <= cmp.compare(*tail, *sift))
  {
    return;
  }
  value_type temp{::std::move(*tail)};
  Iter gap{tail};
  for (;;)
  {
    *gap = ::std::move(*sift);
    gap = sift;
    if (sift == begin)
    {
      break;
    }
    --sift;
    if (0 <= cmp.compare(temp, *sift))
    {
      break;
    }
  }
  *gap = ::std::move(temp);
}

// Sort `v[0, len)` assuming `v[0, offset)` is already sorted.
template<typename Iter, typename Cmp>
inline constexpr void sort_insertion_sort_shift_left(
  Iter v, ::std::iter_difference_t<Iter> len,
  ::std::iter_difference_t<Iter> offset, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::sort_ops_iter_no_throw<Iter, Cmp>)
{
  using difference_type = ::std::iter_difference_t<Iter>;
  using value_type = ::std::iter_value_t<Iter>;
  for (difference_type tail{offset}; tail < len; ++tail)
  {
    Iter tail_it{v + tail};
    Iter sift{tail_it - 1};
    if (0 <= cmp.compare(*tail_it, *sift))
    {
      continue;
    }
    value_type temp{::std::move(*tail_it)};
    Iter gap{tail_it};
    for (;;)
    {
      *gap = ::std::move(*sift);
      gap = sift;
      if (sift == v)
      {
        break;
      }
      --sift;
      if (0 <= cmp.compare(temp, *sift))
      {
        break;
      }
    }
    *gap = ::std::move(temp);
  }
}

template<typename Iter, typename Cmp>
inline constexpr void sort_swap_if_less(Iter a, Iter b, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::sort_copy_ops_iter_no_throw<Iter, Cmp>)
{
  using value_type = ::std::iter_value_t<Iter>;
  static_assert(::std::is_trivially_copyable_v<value_type>);
  value_type temp_a{*a};
  value_type temp_b{*b};
  bool const swap{0 > cmp.compare(temp_b, temp_a)};
  *a = swap ? temp_b : temp_a;
  *b = swap ? temp_a : temp_b;
}

// Optimal sorting network for 9 elements.
template<typename Iter, typename Cmp>
inline constexpr void sort9_optimal(Iter v, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::sort_copy_ops_iter_no_throw<Iter, Cmp>)
{
  ::fast_io::details::algorithms::sort_swap_if_less(v + 0, v + 3, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 1, v + 7, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 2, v + 5, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 4, v + 8, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 0, v + 7, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 2, v + 4, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 3, v + 8, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 5, v + 6, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 0, v + 2, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 1, v + 3, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 4, v + 5, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 7, v + 8, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 1, v + 4, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 3, v + 6, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 5, v + 7, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 0, v + 1, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 2, v + 4, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 3, v + 5, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 6, v + 8, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 2, v + 3, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 4, v + 5, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 6, v + 7, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 1, v + 2, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 3, v + 4, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 5, v + 6, cmp);
}

// Optimal sorting network for 13 elements.
template<typename Iter, typename Cmp>
inline constexpr void sort13_optimal(Iter v, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::sort_copy_ops_iter_no_throw<Iter, Cmp>)
{
  ::fast_io::details::algorithms::sort_swap_if_less(v + 0, v + 12, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 1, v + 10, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 2, v + 9, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 3, v + 7, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 5, v + 11, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 6, v + 8, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 1, v + 6, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 2, v + 3, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 4, v + 11, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 7, v + 9, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 8, v + 10, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 0, v + 4, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 1, v + 2, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 3, v + 6, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 7, v + 8, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 9, v + 10, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 11, v + 12, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 4, v + 6, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 5, v + 9, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 8, v + 11, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 10, v + 12, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 0, v + 5, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 3, v + 8, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 4, v + 7, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 6, v + 11, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 9, v + 10, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 0, v + 1, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 2, v + 5, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 6, v + 9, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 7, v + 8, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 10, v + 11, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 1, v + 3, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 2, v + 4, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 5, v + 6, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 9, v + 10, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 1, v + 2, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 3, v + 4, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 5, v + 7, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 6, v + 8, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 2, v + 3, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 4, v + 5, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 6, v + 7, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 8, v + 9, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 3, v + 4, cmp);
  ::fast_io::details::algorithms::sort_swap_if_less(v + 5, v + 6, cmp);
}

// Merge two adjacent sorted runs into uninitialized memory. Each source
// element is read exactly once and each destination element is constructed
// exactly once. The order invariant is checked at the end; a broken strict
// weak ordering traps.
template<typename T, typename Cmp>
inline constexpr void sort_bidirectional_merge_construct(
  T *src, ::std::ptrdiff_t len, T *dest, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !(::fast_io::details::algorithms::sort_comparator_pass_no_throw<Cmp> &&
      ::fast_io::details::algorithms::sort_compare_no_throw<Cmp, T> &&
      ::std::is_nothrow_copy_constructible_v<T>))
{
  if (len < 2)
  {
    if (len == 1)
    {
      ::new (static_cast<void *>(dest)) T(src[0]);
    }
    return;
  }
  ::std::ptrdiff_t const len_div_2{len / 2};
  ::std::ptrdiff_t left{0};
  ::std::ptrdiff_t right{len_div_2};
  ::std::ptrdiff_t dst{0};
  ::std::ptrdiff_t left_rev{len_div_2 - 1};
  ::std::ptrdiff_t right_rev{len - 1};
  ::std::ptrdiff_t dst_rev{len - 1};
  for (::std::ptrdiff_t i{0}; i != len_div_2; ++i)
  {
    bool const take_left{0 <= cmp.compare(src[right], src[left])};
    if (take_left)
    {
      ::new (static_cast<void *>(dest + dst)) T(src[left]);
      ++left;
    }
    else
    {
      ::new (static_cast<void *>(dest + dst)) T(src[right]);
      ++right;
    }
    ++dst;

    bool const take_right_rev{0 <= cmp.compare(src[right_rev], src[left_rev])};
    if (take_right_rev)
    {
      ::new (static_cast<void *>(dest + dst_rev)) T(src[right_rev]);
      --right_rev;
    }
    else
    {
      ::new (static_cast<void *>(dest + dst_rev)) T(src[left_rev]);
      --left_rev;
    }
    --dst_rev;
  }
  ::std::ptrdiff_t const left_end{left_rev + 1};
  ::std::ptrdiff_t const right_end{right_rev + 1};
  if (len % 2 != 0)
  {
    if (left < left_end)
    {
      ::new (static_cast<void *>(dest + dst)) T(src[left]);
      ++left;
    }
    else
    {
      ::new (static_cast<void *>(dest + dst)) T(src[right]);
      ++right;
    }
  }
  if (left != left_end || right != right_end) [[unlikely]]
  {
    __builtin_trap();
  }
}

// Same merge, into already initialized elements.
template<typename T, typename Cmp>
inline constexpr void sort_bidirectional_merge_assign(
  T *src, ::std::ptrdiff_t len, T *dest, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !(::fast_io::details::algorithms::sort_comparator_pass_no_throw<Cmp> &&
      ::fast_io::details::algorithms::sort_compare_no_throw<Cmp, T> &&
      ::std::is_nothrow_copy_assignable_v<T>))
{
  if (len < 2)
  {
    if (len == 1)
    {
      dest[0] = src[0];
    }
    return;
  }
  ::std::ptrdiff_t const len_div_2{len / 2};
  ::std::ptrdiff_t left{0};
  ::std::ptrdiff_t right{len_div_2};
  ::std::ptrdiff_t dst{0};
  ::std::ptrdiff_t left_rev{len_div_2 - 1};
  ::std::ptrdiff_t right_rev{len - 1};
  ::std::ptrdiff_t dst_rev{len - 1};
  for (::std::ptrdiff_t i{0}; i != len_div_2; ++i)
  {
    bool const take_left{0 <= cmp.compare(src[right], src[left])};
    if (take_left)
    {
      dest[dst] = src[left];
      ++left;
    }
    else
    {
      dest[dst] = src[right];
      ++right;
    }
    ++dst;

    bool const take_right_rev{0 <= cmp.compare(src[right_rev], src[left_rev])};
    if (take_right_rev)
    {
      dest[dst_rev] = src[right_rev];
      --right_rev;
    }
    else
    {
      dest[dst_rev] = src[left_rev];
      --left_rev;
    }
    --dst_rev;
  }
  ::std::ptrdiff_t const left_end{left_rev + 1};
  ::std::ptrdiff_t const right_end{right_rev + 1};
  if (len % 2 != 0)
  {
    if (left < left_end)
    {
      dest[dst] = src[left];
      ++left;
    }
    else
    {
      dest[dst] = src[right];
      ++right;
    }
  }
  if (left != left_end || right != right_end) [[unlikely]]
  {
    __builtin_trap();
  }
}

// Stable sort of 4 elements into uninitialized `dst`.
template<typename T, typename Cmp>
inline constexpr void sort4_stable(T *v, T *dst, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::sort_copy_ops_no_throw<Cmp, T>)
{
  bool const c1{0 > cmp.compare(v[1], v[0])};
  bool const c2{0 > cmp.compare(v[3], v[2])};
  ::std::ptrdiff_t const a{c1 ? 1 : 0};
  ::std::ptrdiff_t const b{c1 ? 0 : 1};
  ::std::ptrdiff_t const c{2 + (c2 ? 1 : 0)};
  ::std::ptrdiff_t const d{2 + (c2 ? 0 : 1)};
  bool const c3{0 > cmp.compare(v[c], v[a])};
  bool const c4{0 > cmp.compare(v[d], v[b])};
  ::std::ptrdiff_t const min{c3 ? c : a};
  ::std::ptrdiff_t const max{c4 ? b : d};
  ::std::ptrdiff_t const unknown_left{c3 ? a : (c4 ? c : b)};
  ::std::ptrdiff_t const unknown_right{c4 ? d : (c3 ? b : c)};
  bool const c5{0 > cmp.compare(v[unknown_right], v[unknown_left])};
  ::std::ptrdiff_t const lo{c5 ? unknown_right : unknown_left};
  ::std::ptrdiff_t const hi{c5 ? unknown_left : unknown_right};
  ::new (static_cast<void *>(dst + 0)) T(v[min]);
  ::new (static_cast<void *>(dst + 1)) T(v[lo]);
  ::new (static_cast<void *>(dst + 2)) T(v[hi]);
  ::new (static_cast<void *>(dst + 3)) T(v[max]);
}

// Stable sort of 8 elements into uninitialized `dst`, using `temp` for 8
// intermediate elements.
template<typename T, typename Cmp>
inline constexpr void sort8_stable(
  T *v, T *dst, T *temp, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::sort_copy_ops_no_throw<Cmp, T>)
{
  ::fast_io::details::algorithms::sort4_stable(v, temp, cmp);
  ::fast_io::details::algorithms::sort4_stable(v + 4, temp + 4, cmp);
  ::fast_io::details::algorithms::sort_bidirectional_merge_construct(
    temp, 8, dst, cmp);
}

template<typename Iter, typename Cmp>
inline constexpr void sort_small_sort_fallback(
  Iter v, ::std::iter_difference_t<Iter> len, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::sort_ops_iter_no_throw<Iter, Cmp>)
{
  if (len >= 2)
  {
    ::fast_io::details::algorithms::sort_insertion_sort_shift_left(v, len, 1, cmp);
  }
}

template<typename T, typename Cmp>
inline constexpr void sort_small_sort_general(
  T *v, ::std::ptrdiff_t len, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::sort_copy_ops_no_throw<Cmp, T>)
{
  if (len < 2)
  {
    return;
  }
  static_assert(::fast_io::details::algorithms::sort_general_small_v<T>);
  alignas(T) ::std::byte scratch_storage[sizeof(T) * 48];
  T *s{reinterpret_cast<T *>(scratch_storage)};
  ::std::ptrdiff_t const len_div_2{len / 2};
  ::std::ptrdiff_t presorted_len;
  if constexpr (sizeof(T) <= 16)
  {
    if (len >= 16)
    {
      ::fast_io::details::algorithms::sort8_stable(v, s, s + len, cmp);
      ::fast_io::details::algorithms::sort8_stable(
        v + len_div_2, s + len_div_2, s + len + 8, cmp);
      presorted_len = 8;
    }
    else if (len >= 8)
    {
      ::fast_io::details::algorithms::sort4_stable(v, s, cmp);
      ::fast_io::details::algorithms::sort4_stable(v + len_div_2, s + len_div_2, cmp);
      presorted_len = 4;
    }
    else
    {
      ::new (static_cast<void *>(s)) T(v[0]);
      ::new (static_cast<void *>(s + len_div_2)) T(v[len_div_2]);
      presorted_len = 1;
    }
  }
  else
  {
    if (len >= 8)
    {
      ::fast_io::details::algorithms::sort4_stable(v, s, cmp);
      ::fast_io::details::algorithms::sort4_stable(v + len_div_2, s + len_div_2, cmp);
      presorted_len = 4;
    }
    else
    {
      ::new (static_cast<void *>(s)) T(v[0]);
      ::new (static_cast<void *>(s + len_div_2)) T(v[len_div_2]);
      presorted_len = 1;
    }
  }
  for (::std::ptrdiff_t k{0}; k != 2; ++k)
  {
    ::std::ptrdiff_t const offset{k != 0 ? len_div_2 : 0};
    ::std::ptrdiff_t const desired_len{
      k != 0 ? len - len_div_2 : len_div_2};
    for (::std::ptrdiff_t i{presorted_len}; i != desired_len; ++i)
    {
      ::new (static_cast<void *>(s + offset + i)) T(v[offset + i]);
      ::fast_io::details::algorithms::sort_insert_tail(
        s + offset, s + offset + i, cmp);
    }
  }
  ::fast_io::details::algorithms::sort_bidirectional_merge_assign(s, len, v, cmp);
}

template<typename T, typename Cmp>
inline constexpr void sort_small_sort_network(
  T *v, ::std::ptrdiff_t len, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::sort_copy_ops_no_throw<Cmp, T>)
{
  if (len < 2)
  {
    return;
  }
  static_assert(::fast_io::details::algorithms::sort_network_small_v<T>);
  ::std::ptrdiff_t const len_div_2{len / 2};
  bool const no_merge{len < 18};
  ::std::ptrdiff_t region_start{0};
  ::std::ptrdiff_t region_len{no_merge ? len : len_div_2};
  for (;;)
  {
    ::std::ptrdiff_t presorted_len;
    if (region_len >= 13)
    {
      ::fast_io::details::algorithms::sort13_optimal(v + region_start, cmp);
      presorted_len = 13;
    }
    else if (region_len >= 9)
    {
      ::fast_io::details::algorithms::sort9_optimal(v + region_start, cmp);
      presorted_len = 9;
    }
    else
    {
      presorted_len = 1;
    }
    ::fast_io::details::algorithms::sort_insertion_sort_shift_left(
      v + region_start, region_len, presorted_len, cmp);
    if (no_merge)
    {
      return;
    }
    if (region_start != 0)
    {
      break;
    }
    region_start = len_div_2;
    region_len = len - len_div_2;
  }
  alignas(T) ::std::byte scratch_storage[sizeof(T) * 32];
  T *s{reinterpret_cast<T *>(scratch_storage)};
  ::fast_io::details::algorithms::sort_bidirectional_merge_construct(v, len, s, cmp);
  for (::std::ptrdiff_t i{0}; i != len; ++i)
  {
    v[i] = s[i];
  }
}

template<typename Iter, typename Cmp>
inline constexpr void sort_small_sort(
  Iter v, ::std::iter_difference_t<Iter> len, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !(::fast_io::details::algorithms::iter_no_throw<Iter> &&
      ::fast_io::details::algorithms::sort_ops_iter_no_throw<Iter, Cmp> &&
      ::fast_io::details::algorithms::sort_small_ops_no_throw<
        Cmp, ::std::iter_value_t<Iter>>))
{
  using value_type = ::std::iter_value_t<Iter>;
  if consteval
  {
    // The fast small sorts need uninitialized scratch storage, which cannot
    // be obtained in a constant expression. Constant evaluation is not
    // performance sensitive, so use the insertion-sort fallback there.
    ::fast_io::details::algorithms::sort_small_sort_fallback(v, len, cmp);
  }
  else
  {
    if constexpr (::std::contiguous_iterator<Iter>)
    {
      value_type *const base{::std::to_address(v)};
      if constexpr (::fast_io::details::algorithms::sort_network_small_v<value_type>)
      {
        ::fast_io::details::algorithms::sort_small_sort_network(base, len, cmp);
      }
      else if constexpr (::fast_io::details::algorithms::sort_general_small_v<value_type>)
      {
        ::fast_io::details::algorithms::sort_small_sort_general(base, len, cmp);
      }
      else
      {
        ::fast_io::details::algorithms::sort_small_sort_fallback(v, len, cmp);
      }
    }
    else
    {
      ::fast_io::details::algorithms::sort_small_sort_fallback(v, len, cmp);
    }
  }
}

template<typename Iter, typename Cmp>
inline constexpr void sort_heapsort_sift_down(
  Iter first, ::std::iter_difference_t<Iter> n,
  ::std::iter_difference_t<Iter> node, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::sort_ops_iter_no_throw<Iter, Cmp>)
{
  using difference_type = ::std::iter_difference_t<Iter>;
  Iter node_it{first + node};
  for (;;)
  {
    difference_type child{static_cast<difference_type>((node << 1) + 1)};
    if (child >= n)
    {
      break;
    }
    Iter child_it{first + child};
    if (child + 1 < n)
    {
      Iter sibling{child_it + 1};
      if (0 > cmp.compare(*child_it, *sibling))
      {
        child_it = sibling;
      }
    }
    if (0 <= cmp.compare(*node_it, *child_it))
    {
      break;
    }
    ::fast_io::algorithms::swap(*node_it, *child_it);
    node = child;
    node_it = child_it;
  }
}

// Branchless-heapsort style fallback, interleaving heap construction and
// extraction. Used when the quicksort depth limit is exhausted.
template<typename Iter, typename Cmp>
inline constexpr void sort_heapsort(
  Iter first, ::std::iter_difference_t<Iter> len, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::sort_ops_iter_no_throw<Iter, Cmp>)
{
  using difference_type = ::std::iter_difference_t<Iter>;
  for (difference_type i{static_cast<difference_type>(len + len / 2)}; i != 0;)
  {
    --i;
    difference_type sift_idx;
    if (i >= len)
    {
      sift_idx = static_cast<difference_type>(i - len);
    }
    else
    {
      if (i != 0)
      {
        ::fast_io::algorithms::swap(*first, *(first + i));
      }
      sift_idx = 0;
    }
    difference_type const sub_n{static_cast<difference_type>(i < len ? i : len)};
    ::fast_io::details::algorithms::sort_heapsort_sift_down(
      first, sub_n, sift_idx, cmp);
  }
}

template<typename Iter, typename Cmp>
inline constexpr Iter sort_median3(Iter a, Iter b, Iter c, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::sort_ops_iter_no_throw<Iter, Cmp>)
{
  bool const x{0 > cmp.compare(*a, *b)};
  bool const y{0 > cmp.compare(*a, *c)};
  if (x == y)
  {
    bool const z{0 > cmp.compare(*b, *c)};
    return (z != x) ? c : b;
  }
  return a;
}

template<typename Iter, typename Cmp>
inline constexpr Iter sort_median3_rec(
  Iter a, Iter b, Iter c,
  ::std::iter_difference_t<Iter> n, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::sort_ops_iter_no_throw<Iter, Cmp>)
{
  using difference_type = ::std::iter_difference_t<Iter>;
  if (n * 8 >= 64)
  {
    difference_type const n8{static_cast<difference_type>(n / 8)};
    a = ::fast_io::details::algorithms::sort_median3_rec(
      a, a + n8 * 4, a + n8 * 7, n8, cmp);
    b = ::fast_io::details::algorithms::sort_median3_rec(
      b, b + n8 * 4, b + n8 * 7, n8, cmp);
    c = ::fast_io::details::algorithms::sort_median3_rec(
      c, c + n8 * 4, c + n8 * 7, n8, cmp);
  }
  return ::fast_io::details::algorithms::sort_median3(a, b, c, cmp);
}

// Adaptive pseudomedian pivot selection from glidesort. Samples an
// approximation of the median of roughly sqrt(n) elements.
template<typename Iter, typename Cmp>
inline constexpr Iter sort_choose_pivot(
  Iter first, ::std::iter_difference_t<Iter> len, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::sort_ops_iter_no_throw<Iter, Cmp>)
{
  using difference_type = ::std::iter_difference_t<Iter>;
  if (len < 8) [[unlikely]]
  {
    __builtin_trap();
  }
  difference_type const len_div_8{static_cast<difference_type>(len / 8)};
  Iter const a{first};
  Iter const b{first + len_div_8 * 4};
  Iter const c{first + len_div_8 * 7};
  if (len < 64)
  {
    return ::fast_io::details::algorithms::sort_median3(a, b, c, cmp);
  }
  return ::fast_io::details::algorithms::sort_median3_rec(
    a, b, c, len_div_8, cmp);
}

// Branchless cyclic Lomuto partition for small and medium elements.
// `equal` selects the equal-element partition: elements <= pivot go left.
template<bool equal, typename Iter, typename Cmp>
inline constexpr ::std::iter_difference_t<Iter> sort_partition_lomuto(
  Iter first, ::std::iter_difference_t<Iter> len,
  ::std::iter_value_t<Iter> const *__restrict pivot_ptr, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::sort_ops_iter_no_throw<Iter, Cmp>)
{
  using difference_type = ::std::iter_difference_t<Iter>;
  using value_type = ::std::iter_value_t<Iter>;
  difference_type const m{static_cast<difference_type>(len - 1)};
  Iter const base{first + 1};
  value_type gap_value{::std::move(*base)};
  Iter gap_it{base};
  difference_type num_lt{0};
  Iter right_it{base + 1};
  // ipnsort unrolls the partition loop by two for element sizes up to 16
  // bytes. The unrolled form gives the out-of-order engine two independent
  // comparison chains per iteration.
  constexpr difference_type unroll_len{(sizeof(value_type) <= 16) ? 2 : 1};
  Iter const unroll_end_it{
    base + static_cast<difference_type>(m - (unroll_len - 1))};
  while (right_it < unroll_end_it)
  {
    for (difference_type k{}; k != unroll_len; ++k)
    {
      bool right_is_lt;
      if constexpr (equal)
      {
        right_is_lt = 0 <= cmp.compare(*pivot_ptr, *right_it);
      }
      else
      {
        right_is_lt = 0 > cmp.compare(*right_it, *pivot_ptr);
      }
      Iter const left_it{base + num_lt};
      if constexpr (::fast_io::details::algorithms::copy_trivial_v<value_type>)
      {
        *gap_it = *left_it;
        *left_it = *right_it;
      }
      else
      {
        if (left_it != gap_it)
        {
          *gap_it = ::std::move(*left_it);
        }
        *left_it = ::std::move(*right_it);
      }
      gap_it = right_it;
      num_lt += right_is_lt ? 1 : 0;
      ++right_it;
    }
  }
  Iter const end_it{base + m};
  while (right_it < end_it)
  {
    bool right_is_lt;
    if constexpr (equal)
    {
      right_is_lt = 0 <= cmp.compare(*pivot_ptr, *right_it);
    }
    else
    {
      right_is_lt = 0 > cmp.compare(*right_it, *pivot_ptr);
    }
    Iter const left_it{base + num_lt};
    if constexpr (::fast_io::details::algorithms::copy_trivial_v<value_type>)
    {
      *gap_it = *left_it;
      *left_it = *right_it;
    }
    else
    {
      if (left_it != gap_it)
      {
        *gap_it = ::std::move(*left_it);
      }
      *left_it = ::std::move(*right_it);
    }
    gap_it = right_it;
    num_lt += right_is_lt ? 1 : 0;
    ++right_it;
  }
  bool final_is_lt;
  if constexpr (equal)
  {
    final_is_lt = 0 <= cmp.compare(*pivot_ptr, gap_value);
  }
  else
  {
    final_is_lt = 0 > cmp.compare(gap_value, *pivot_ptr);
  }
  Iter const left_it{base + num_lt};
  if constexpr (::fast_io::details::algorithms::copy_trivial_v<value_type>)
  {
    *gap_it = *left_it;
    *left_it = gap_value;
  }
  else
  {
    if (left_it != gap_it)
    {
      *gap_it = ::std::move(*left_it);
    }
    *left_it = ::std::move(gap_value);
  }
  num_lt += final_is_lt ? 1 : 0;
  return num_lt;
}

template<typename T>
struct sort_gap
{
  alignas(T) ::std::byte storage[sizeof(T)];
  T *pos{};
  bool active{};
  inline constexpr ~sort_gap() noexcept
  {
    if (active)
    {
      ptr()->~T();
    }
  }
  inline constexpr T *ptr() noexcept
  {
    return reinterpret_cast<T *>(storage);
  }
  inline constexpr void emplace(T &&value)
    noexcept(::std::is_nothrow_move_constructible_v<T>)
  {
    ::new (static_cast<void *>(ptr())) T(::std::move(value));
    active = true;
  }
};

// Branchy cyclic Hoare partition for large elements. Fewer element moves
// than pairwise swapping, which matters when copies are expensive.
template<bool equal, typename Iter, typename Cmp>
inline constexpr ::std::iter_difference_t<Iter> sort_partition_hoare(
  Iter first, ::std::iter_difference_t<Iter> len,
  ::std::iter_value_t<Iter> const *__restrict pivot_ptr, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::sort_ops_iter_no_throw<Iter, Cmp>)
{
  using difference_type = ::std::iter_difference_t<Iter>;
  using value_type = ::std::iter_value_t<Iter>;
  difference_type const m{static_cast<difference_type>(len - 1)};
  Iter const base{first + 1};
  Iter left_it{base};
  Iter right_it{base + m};
  ::fast_io::details::algorithms::sort_gap<value_type> gap;
  for (;;)
  {
    while (left_it < right_it)
    {
      bool left_is_lt;
      if constexpr (equal)
      {
        left_is_lt = 0 <= cmp.compare(*pivot_ptr, *left_it);
      }
      else
      {
        left_is_lt = 0 > cmp.compare(*left_it, *pivot_ptr);
      }
      if (!left_is_lt)
      {
        break;
      }
      ++left_it;
    }
    for (;;)
    {
      --right_it;
      if (left_it >= right_it)
      {
        break;
      }
      bool right_is_lt;
      if constexpr (equal)
      {
        right_is_lt = 0 <= cmp.compare(*pivot_ptr, *right_it);
      }
      else
      {
        right_is_lt = 0 > cmp.compare(*right_it, *pivot_ptr);
      }
      if (right_is_lt)
      {
        break;
      }
    }
    if (left_it >= right_it)
    {
      break;
    }
    if (!gap.active)
    {
      gap.emplace(::std::move(*left_it));
    }
    else if (::std::addressof(*left_it) != gap.pos)
    {
      *gap.pos = ::std::move(*left_it);
    }
    gap.pos = ::std::addressof(*right_it);
    *left_it = ::std::move(*right_it);
    ++left_it;
  }
  if (gap.active)
  {
    *gap.pos = ::std::move(*gap.ptr());
    gap.ptr()->~value_type();
    gap.active = false;
  }
  return static_cast<difference_type>(left_it - base);
}

template<bool equal, typename Iter, typename Cmp>
inline constexpr ::std::iter_difference_t<Iter> sort_partition(
  Iter first, ::std::iter_difference_t<Iter> len, Iter pivot, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::sort_ops_iter_no_throw<Iter, Cmp>)
{
  using difference_type = ::std::iter_difference_t<Iter>;
  using value_type = ::std::iter_value_t<Iter>;
  if (len < 2)
  {
    return 0;
  }
  if (pivot >= first + len) [[unlikely]]
  {
    __builtin_trap();
  }
  if (pivot != first)
  {
    ::fast_io::algorithms::swap(*first, *pivot);
  }
  difference_type num_lt;
  auto const *const pivot_ptr{::std::addressof(*first)};
  if consteval
  {
    // The Hoare partition keeps one element in a byte buffer and therefore
    // cannot run in a constant expression. Lomuto has no such storage.
    num_lt = ::fast_io::details::algorithms::sort_partition_lomuto<equal>(
      first, len, pivot_ptr, cmp);
  }
  else
  {
    if constexpr (sizeof(value_type) <= 96)
    {
      num_lt = ::fast_io::details::algorithms::sort_partition_lomuto<equal>(
        first, len, pivot_ptr, cmp);
    }
    else
    {
      num_lt = ::fast_io::details::algorithms::sort_partition_hoare<equal>(
        first, len, pivot_ptr, cmp);
    }
  }
  if (num_lt != 0)
  {
    ::fast_io::algorithms::swap(*first, *(first + num_lt));
  }
  return num_lt;
}

template<typename Iter, typename Cmp>
inline constexpr void sort_quicksort(
  Iter first, ::std::iter_difference_t<Iter> len,
  Iter ancestor, bool has_ancestor,
  ::std::iter_difference_t<Iter> limit, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !(::fast_io::details::algorithms::sort_ops_iter_no_throw<Iter, Cmp> &&
      ::fast_io::details::algorithms::sort_equal_iter_no_throw_v<Iter, Cmp>))
{
  using difference_type = ::std::iter_difference_t<Iter>;
  using value_type = ::std::iter_value_t<Iter>;
  for (;;)
  {
    if (len <= ::fast_io::details::algorithms::sort_small_sort_threshold_v<value_type>)
    {
      ::fast_io::details::algorithms::sort_small_sort(first, len, cmp);
      return;
    }
    if (limit == 0)
    {
      ::fast_io::details::algorithms::sort_heapsort(first, len, cmp);
      return;
    }
    --limit;
    Iter const pivot{
      ::fast_io::details::algorithms::sort_choose_pivot(first, len, cmp)};
    if (has_ancestor &&
        ::fast_io::details::algorithms::sort_elements_equal(
          cmp, *ancestor, *pivot))
    {
      difference_type const num_lt{
        ::fast_io::details::algorithms::sort_partition<true>(
          first, len, pivot, cmp)};
      first += static_cast<difference_type>(num_lt + 1);
      len -= static_cast<difference_type>(num_lt + 1);
      has_ancestor = false;
      continue;
    }
    difference_type const num_lt{
      ::fast_io::details::algorithms::sort_partition<false>(
        first, len, pivot, cmp)};
    ::fast_io::details::algorithms::sort_quicksort(
      first, num_lt, ancestor, has_ancestor, limit, cmp);
    first += static_cast<difference_type>(num_lt + 1);
    len -= static_cast<difference_type>(num_lt + 1);
    ancestor = first - 1;
    has_ancestor = true;
  }
}

struct sort_run
{
  ::std::ptrdiff_t len;
  bool reversed;
};

template<typename Iter, typename Cmp>
inline constexpr ::fast_io::details::algorithms::sort_run sort_find_existing_run(
  Iter first, ::std::iter_difference_t<Iter> len, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::sort_ops_iter_no_throw<Iter, Cmp>)
{
  using difference_type = ::std::iter_difference_t<Iter>;
  if (len < 2)
  {
    return {static_cast<::std::ptrdiff_t>(len), false};
  }
  using value_type = ::std::iter_value_t<Iter>;
  Iter const last{first + len};
  Iter run_it{first + 2};
  bool const descending{0 > cmp.compare(*(run_it - 1), *first)};
  if (descending)
  {
    if constexpr (sizeof(value_type) > 8)
    {
      while (run_it + 1 < last)
      {
        if (0 <= cmp.compare(*run_it, *(run_it - 1)))
        {
          break;
        }
        ++run_it;
        if (0 <= cmp.compare(*run_it, *(run_it - 1)))
        {
          break;
        }
        ++run_it;
      }
      if (run_it < last && 0 > cmp.compare(*run_it, *(run_it - 1)))
      {
        ++run_it;
      }
    }
    else
    {
      while (run_it < last && 0 > cmp.compare(*run_it, *(run_it - 1)))
      {
        ++run_it;
      }
    }
  }
  else
  {
    if constexpr (sizeof(value_type) > 8)
    {
      while (run_it + 1 < last)
      {
        if (0 > cmp.compare(*run_it, *(run_it - 1)))
        {
          break;
        }
        ++run_it;
        if (0 > cmp.compare(*run_it, *(run_it - 1)))
        {
          break;
        }
        ++run_it;
      }
      if (run_it < last && 0 <= cmp.compare(*run_it, *(run_it - 1)))
      {
        ++run_it;
      }
    }
    else
    {
      while (run_it < last && 0 <= cmp.compare(*run_it, *(run_it - 1)))
      {
        ++run_it;
      }
    }
  }
  return {static_cast<::std::ptrdiff_t>(run_it - first), descending};
}

inline constexpr ::std::ptrdiff_t sort_floor_log2(::std::size_t n) noexcept
{
  // std::bit_width(n) is 1 + floor(log2(n)) for n != 0.
  return n == 0 ? 0
                : static_cast<::std::ptrdiff_t>(::std::bit_width(n) - 1);
}

template<::std::random_access_iterator Iter, typename Cmp>
inline constexpr void sort_iters_impl(Iter first, Iter last, Cmp cmp)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !(::fast_io::details::algorithms::sort_iters_no_throw<Iter, Cmp> &&
      ::fast_io::details::algorithms::sort_comparator_pass_no_throw<Cmp>))
{
  using difference_type = ::std::iter_difference_t<Iter>;
  if constexpr (::std::contiguous_iterator<Iter> && !::std::is_pointer_v<Iter>)
  {
    ::fast_io::details::algorithms::sort_iters_impl(
      ::std::to_address(first), ::std::to_address(last), cmp);
    return;
  }
  else
  {
    difference_type const len{static_cast<difference_type>(last - first)};
    if (len < 2)
    {
      return;
    }
    constexpr difference_type max_len_always_insertion_sort{20};
    if (len <= max_len_always_insertion_sort)
    {
      ::fast_io::details::algorithms::sort_insertion_sort_shift_left(
        first, len, 1, cmp);
      return;
    }
    auto const run{
      ::fast_io::details::algorithms::sort_find_existing_run(first, len, cmp)};
    if (run.len == static_cast<::std::ptrdiff_t>(len))
    {
      if (run.reversed)
      {
        if constexpr (::std::is_pointer_v<Iter>)
        {
          ::fast_io::details::algorithms::reverse_ptr(first, len);
        }
        else
        {
          ::fast_io::details::algorithms::reverse_iters_impl(
            first, first + len);
        }
      }
      return;
    }
    difference_type const limit{
      static_cast<difference_type>(
        2 * ::fast_io::details::algorithms::sort_floor_log2(
              static_cast<::std::size_t>(len)))};
    ::fast_io::details::algorithms::sort_quicksort(
      first, len, first, false, limit, cmp);
  }
}

} // namespace fast_io::details::algorithms

namespace fast_io::algorithms
{

// Unstable, in-place, O(N log N) worst-case sort.
template<::std::random_access_iterator Iter,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr void sort_iters(Iter first, Iter last, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !(::fast_io::details::algorithms::sort_iters_no_throw<Iter, Cmp> &&
      ::fast_io::details::algorithms::sort_comparator_pass_no_throw<Cmp>))
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first)>),
    sort_iters_define, first, last, cmp);
  ::fast_io::details::algorithms::sort_iters_impl(first, last, cmp);
}

template<::std::ranges::random_access_range R,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr void sort(R &&r, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !(::fast_io::details::algorithms::sort_iters_no_throw<
        ::std::ranges::iterator_t<R>, Cmp> &&
      ::fast_io::details::algorithms::sort_comparator_pass_no_throw<Cmp>))
{
  ::fast_io::algorithms::sort_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), cmp);
}

template<::std::ranges::random_access_range R,
  typename Cmp = ::fast_io::algorithms::compare_three_way>
inline constexpr void sort_index_range(
  R &&r, ::std::size_t firstidx, ::std::size_t lastidx, Cmp cmp = Cmp{})
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::sort_index_range_no_throw<R, Cmp>)
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R>(r), firstidx, lastidx);
  ::fast_io::algorithms::sort_iters(first, last, cmp);
}

} // namespace fast_io::algorithms
