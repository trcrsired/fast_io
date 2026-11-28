#pragma once

namespace fast_io::details::algorithms
{

// A rotation of a contiguous range whose element type is trivially copyable
// can be performed with byte moves. The threshold matches the stack buffer
// below: when the smaller side fits, copy that side out and slide the other.
inline constexpr ::std::size_t rotate_buffer_bytes{
  512 * sizeof(::std::size_t)};

// A type with its own ADL swap must be rotated through that swap, even when
// its representation is trivially copyable, so byte-wise paths exclude it.
template<typename T>
inline constexpr bool rotate_byte_copyable{
  ::fast_io::details::algorithms::copy_trivial_v<T> &&
  !::fast_io::details::algorithms::swap_adl::swappable<T &, T &>};

template<typename T>
FAST_IO_UNSAFE_API
inline constexpr void rotate_ptr_memmove(T *first, T *middle, T *last)
  FAST_IO_HERBCEPTIONS_THROWS_IF(false)
{
  ::std::size_t const left{static_cast<::std::size_t>(middle - first)};
  ::std::size_t const right{static_cast<::std::size_t>(last - middle)};
  alignas(T) ::std::byte buffer[::fast_io::details::algorithms::rotate_buffer_bytes];
  T *const saved{reinterpret_cast<T *>(buffer)};
  if (left <= right)
  {
    // Save the left side, slide the right side over it, then drop the saved
    // elements into the hole at the end.
    ::fast_io::freestanding::my_memcpy(saved, first, left * sizeof(T));
    ::fast_io::freestanding::my_memmove(first, middle, right * sizeof(T));
    ::fast_io::freestanding::my_memcpy(first + right, saved, left * sizeof(T));
  }
  else
  {
    // Save the right side, slide the left side over it, then drop the saved
    // elements into the hole at the front.
    ::fast_io::freestanding::my_memcpy(saved, middle, right * sizeof(T));
    ::fast_io::freestanding::my_memmove(first + right, first, left * sizeof(T));
    ::fast_io::freestanding::my_memcpy(first, saved, right * sizeof(T));
  }
}

// Juggling rotation: every element is moved at most twice, which beats a
// three-move swap for large element types. `gcd(left + right, right)` cycles
// are walked, so the number of extra rounds is the gcd.
template<typename T>
FAST_IO_UNSAFE_API
inline constexpr void rotate_ptr_gcd(T *first, T *middle, T *last)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::move_no_throw<T>)
{
  ::std::size_t const left{static_cast<::std::size_t>(middle - first)};
  ::std::size_t const right{static_cast<::std::size_t>(last - middle)};
  ::std::size_t i{right};
  ::std::size_t gcd{right};
  T tmp{::std::move(first[0])};
  for (;;)
  {
    tmp = ::std::exchange(first[i], ::std::move(tmp));
    if (i >= left)
    {
      i -= left;
      if (i == 0)
      {
        first[0] = ::std::move(tmp);
        break;
      }
      if (i < gcd)
      {
        gcd = i;
      }
    }
    else
    {
      i += right;
    }
  }
  for (::std::size_t start{1}; start < gcd; ++start)
  {
    tmp = ::std::move(first[start]);
    i = start + right;
    for (;;)
    {
      tmp = ::std::exchange(first[i], ::std::move(tmp));
      if (i >= left)
      {
        i -= left;
        if (i == start)
        {
          first[start] = ::std::move(tmp);
          break;
        }
      }
      else
      {
        i += right;
      }
    }
  }
}

// Exchange two equally long contiguous blocks element by element, preferring
// the element type's own swap. The four-element form lets the compiler use
// vector loads and stores for small trivially copyable types.
template<typename T>
FAST_IO_UNSAFE_API
inline constexpr void rotate_ptr_swap_block_elements(T *a, T *b, ::std::size_t n)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::swap_no_throw_v<T>)
{
  ::std::size_t i{};
  if constexpr (::fast_io::details::algorithms::reverse_block_v<T>)
  {
    while (n - i >= 4)
    {
      T a0{::std::move(a[i + 0])};
      T a1{::std::move(a[i + 1])};
      T a2{::std::move(a[i + 2])};
      T a3{::std::move(a[i + 3])};
      T b0{::std::move(b[i + 0])};
      T b1{::std::move(b[i + 1])};
      T b2{::std::move(b[i + 2])};
      T b3{::std::move(b[i + 3])};
      a[i + 0] = ::std::move(b0);
      a[i + 1] = ::std::move(b1);
      a[i + 2] = ::std::move(b2);
      a[i + 3] = ::std::move(b3);
      b[i + 0] = ::std::move(a0);
      b[i + 1] = ::std::move(a1);
      b[i + 2] = ::std::move(a2);
      b[i + 3] = ::std::move(a3);
      i += 4;
    }
  }
  for (; i != n; ++i)
  {
    ::fast_io::algorithms::swap(a[i], b[i]);
  }
}

// Exchange two blocks of trivially copyable values. A block is moved through
// the stack buffer, so large blocks become a few memmoves instead of one
// element exchange per value. Constant evaluation keeps the element form.
template<typename T>
FAST_IO_UNSAFE_API
inline constexpr void rotate_ptr_swap_block(T *a, T *b, ::std::size_t n)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::swap_no_throw_v<T>)
{
  if constexpr (::fast_io::details::algorithms::rotate_byte_copyable<T> &&
                !::fast_io::details::algorithms::reverse_block_v<T> &&
                sizeof(T) <= ::fast_io::details::algorithms::rotate_buffer_bytes)
  {
    if consteval
    {
      ::fast_io::details::algorithms::rotate_ptr_swap_block_elements(a, b, n);
    }
    else
    {
      constexpr ::std::size_t chunk{
        ::fast_io::details::algorithms::rotate_buffer_bytes / sizeof(T)};
      alignas(T) ::std::byte buffer[::fast_io::details::algorithms::rotate_buffer_bytes];
      ::std::size_t i{};
      while (n - i >= chunk)
      {
        ::fast_io::freestanding::my_memcpy(buffer, a + i, chunk * sizeof(T));
        ::fast_io::freestanding::my_memmove(a + i, b + i, chunk * sizeof(T));
        ::fast_io::freestanding::my_memcpy(b + i, buffer, chunk * sizeof(T));
        i += chunk;
      }
      if (i != n)
      {
        ::std::size_t const tail{n - i};
        ::fast_io::freestanding::my_memcpy(buffer, a + i, tail * sizeof(T));
        ::fast_io::freestanding::my_memmove(a + i, b + i, tail * sizeof(T));
        ::fast_io::freestanding::my_memcpy(b + i, buffer, tail * sizeof(T));
      }
    }
  }
  else
  {
    ::fast_io::details::algorithms::rotate_ptr_swap_block_elements(a, b, n);
  }
}

// Gries-Mills block rotation: repeatedly exchange the smaller side with an
// equally long piece of the larger side, then recurse on the remainder. The
// inner loops reduce to the classic two-block swap.
template<typename T>
FAST_IO_UNSAFE_API
inline constexpr void rotate_ptr_swap(T *first, T *middle, T *last)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::swap_no_throw_v<T>)
{
  ::std::size_t left{static_cast<::std::size_t>(middle - first)};
  ::std::size_t right{static_cast<::std::size_t>(last - middle)};
  for (;;)
  {
    if (left >= right)
    {
      do
      {
        ::fast_io::details::algorithms::rotate_ptr_swap_block(
          middle - right, middle, right);
        middle -= right;
        left -= right;
        if (left < right)
        {
          break;
        }
      } while (true);
    }
    else
    {
      do
      {
        ::fast_io::details::algorithms::rotate_ptr_swap_block(
          middle - left, middle, left);
        middle += left;
        right -= left;
        if (right < left)
        {
          break;
        }
      } while (true);
    }
    if (left == 0 || right == 0)
    {
      return;
    }
    // Recompute the absolute ends so the next round can use them again.
    first = middle - left;
    last = middle + right;
    if constexpr (::fast_io::details::algorithms::rotate_byte_copyable<T>)
    {
      if consteval
      {
        // The stack buffer is not usable during constant evaluation.
      }
      else
      {
        // The block swap can leave one small side behind. Sliding a small side
        // through the stack buffer beats continuing with element exchanges.
        if (left <= right
              ? left <= ::fast_io::details::algorithms::rotate_buffer_bytes / sizeof(T)
              : right <= ::fast_io::details::algorithms::rotate_buffer_bytes / sizeof(T))
        {
          ::fast_io::details::algorithms::rotate_ptr_memmove(first, middle, last);
          return;
        }
      }
    }
  }
}

// Rotation by one position is a single slide, so it only needs one temporary
// and one memmove/assignment pass. This is the common `rotate(begin, begin +
// 1, end)` case and is much cheaper than the general block algorithms for
// large element types.
template<typename T>
FAST_IO_UNSAFE_API
inline constexpr void rotate_ptr_left_one(T *first, T *last)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::move_no_throw<T>)
{
  T tmp{::std::move(first[0])};
  ::std::size_t const count{static_cast<::std::size_t>(last - first - 1)};
  if constexpr (::fast_io::details::algorithms::rotate_byte_copyable<T>)
  {
    if (count != 0)
    {
      ::fast_io::freestanding::my_memmove(first, first + 1, count * sizeof(T));
    }
  }
  else
  {
    for (T *p{first}; p + 1 != last; ++p)
    {
      *p = ::std::move(*(p + 1));
    }
  }
  last[-1] = ::std::move(tmp);
}

template<typename T>
FAST_IO_UNSAFE_API
inline constexpr void rotate_ptr_right_one(T *first, T *last)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::move_no_throw<T>)
{
  T tmp{::std::move(last[-1])};
  ::std::size_t const count{static_cast<::std::size_t>(last - first - 1)};
  if constexpr (::fast_io::details::algorithms::rotate_byte_copyable<T>)
  {
    if (count != 0)
    {
      ::fast_io::freestanding::my_memmove(first + 1, first, count * sizeof(T));
    }
  }
  else
  {
    for (T *p{last - 1}; p != first; --p)
    {
      *p = ::std::move(*(p - 1));
    }
  }
  first[0] = ::std::move(tmp);
}

template<typename T>
FAST_IO_UNSAFE_API
inline constexpr T *rotate_ptr(T *first, T *middle, T *last)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !(::fast_io::details::algorithms::move_no_throw<T> &&
      ::fast_io::details::algorithms::swap_no_throw_v<T>))
{
  if (first == middle)
  {
    return last;
  }
  if (middle == last)
  {
    return first;
  }
  ::std::size_t const left{static_cast<::std::size_t>(middle - first)};
  ::std::size_t const right{static_cast<::std::size_t>(last - middle)};
  T *const result{first + right};
  ::std::size_t const small{left < right ? left : right};
  if constexpr (::fast_io::details::algorithms::rotate_byte_copyable<T>)
  {
    if consteval
    {
      if (left + right < 24 || sizeof(T) > 4 * sizeof(::std::size_t))
      {
        ::fast_io::details::algorithms::rotate_ptr_gcd(first, middle, last);
      }
      else
      {
        ::fast_io::details::algorithms::rotate_ptr_swap(first, middle, last);
      }
    }
    else
    {
      if (left == 1)
      {
        ::fast_io::details::algorithms::rotate_ptr_left_one(first, last);
      }
      else if (right == 1)
      {
        ::fast_io::details::algorithms::rotate_ptr_right_one(first, last);
      }
      else if constexpr (sizeof(T) > 4 * sizeof(::std::size_t) &&
                         sizeof(T) % sizeof(::std::size_t) == 0 &&
                         alignof(T) >= alignof(::std::size_t))
      {
        // Large trivially copyable elements are rotated as machine words. This
        // gives the block algorithms cheap, vectorizable 8-byte elements and
        // avoids the poor locality of element-sized juggling.
        constexpr ::std::size_t ratio{sizeof(T) / sizeof(::std::size_t)};
        auto *const words{reinterpret_cast<::std::size_t *>(first)};
        ::fast_io::details::algorithms::rotate_ptr<::std::size_t>(
          words, words + left * ratio, words + (left + right) * ratio);
      }
      else
      {
        if (small <= ::fast_io::details::algorithms::rotate_buffer_bytes / sizeof(T))
        {
          ::fast_io::details::algorithms::rotate_ptr_memmove(first, middle, last);
        }
        else if (left + right < 24)
        {
          ::fast_io::details::algorithms::rotate_ptr_gcd(first, middle, last);
        }
        else
        {
          ::fast_io::details::algorithms::rotate_ptr_swap(first, middle, last);
        }
      }
    }
  }
  else
  {
    if (left + right < 24 || sizeof(T) > 4 * sizeof(::std::size_t))
    {
      ::fast_io::details::algorithms::rotate_ptr_gcd(first, middle, last);
    }
    else
    {
      ::fast_io::details::algorithms::rotate_ptr_swap(first, middle, last);
    }
  }
  return result;
}

template<typename Iter>
concept rotate_iter_no_throw =
  ::fast_io::details::algorithms::iter_no_throw<Iter> &&
  ::fast_io::details::algorithms::move_no_throw<::std::iter_value_t<Iter>> &&
  ::fast_io::details::algorithms::swap_no_throw_v<::std::iter_value_t<Iter>>;

template<typename Iter, typename Snt>
concept rotate_iters_no_throw =
  ::std::forward_iterator<Iter> &&
  ::std::sentinel_for<Snt, Iter> &&
  ::fast_io::details::algorithms::rotate_iter_no_throw<Iter> &&
  requires(Iter const &it, Snt const &snt) {
    { it == snt } noexcept;
    { snt == it } noexcept;
  };

// Classic forward rotation: one exchange per element, no temporary and no
// distance requirement. It works for forward, bidirectional and non-contiguous
// random-access iterators.
template<typename Iter>
FAST_IO_UNSAFE_API
inline constexpr Iter rotate_forward_iters_impl(Iter first, Iter middle, Iter last)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::rotate_iter_no_throw<Iter>)
{
  Iter i{middle};
  for (;;)
  {
    ::fast_io::algorithms::iter_swap(first, i);
    ++first;
    if (++i == last)
    {
      break;
    }
    if (first == middle)
    {
      middle = i;
    }
  }
  Iter const result{first};
  if (first != middle)
  {
    i = middle;
    for (;;)
    {
      ::fast_io::algorithms::iter_swap(first, i);
      ++first;
      if (++i == last)
      {
        if (first == middle)
        {
          break;
        }
        i = middle;
      }
      else if (first == middle)
      {
        middle = i;
      }
    }
  }
  return result;
}

template<typename Iter, typename Snt>
FAST_IO_UNSAFE_API
inline constexpr Iter rotate_iters_impl(Iter first, Iter middle, Snt last)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::rotate_iters_no_throw<Iter, Snt>)
{
  if constexpr (::std::contiguous_iterator<Iter> &&
                ::std::sized_sentinel_for<Snt, Iter>)
  {
    using T = ::std::iter_value_t<Iter>;
    auto const count{last - first};
    auto const offset{middle - first};
    T *const base{::std::to_address(first)};
    ::fast_io::details::algorithms::rotate_ptr(
      base, base + offset, base + count);
    return first + (count - offset);
  }
  else
  {
    if (middle == last)
    {
      return first;
    }
    Iter last_it{};
    if constexpr (::std::same_as<Iter, Snt>)
    {
      last_it = last;
    }
    else
    {
      last_it = first;
      while (last_it != last)
      {
        ++last_it;
      }
    }
    if (first == middle)
    {
      return last_it;
    }
    return ::fast_io::details::algorithms::rotate_forward_iters_impl(
      first, middle, last_it);
  }
}

template<bool throwing_violations>
inline constexpr ::std::size_t rotate_index_range_check(
  ::std::size_t n, ::std::size_t firstidx, ::std::size_t middleidx,
  ::std::size_t lastidx)
  FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_violations)
{
  ::std::size_t const last{
    ::fast_io::details::algorithms::algo_index_check<throwing_violations>(
      n, firstidx, lastidx)};
  if (middleidx < firstidx || last < middleidx) [[unlikely]]
  {
    ::fast_io::containers::details::violation_report_on_invalid_argument<
      throwing_violations>();
  }
  return last;
}

template<typename R>
concept rotate_index_range_no_throw =
  ::std::ranges::random_access_range<R> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R> &&
  ::fast_io::details::algorithms::rotate_iters_no_throw<
    ::fast_io::details::algorithms::algo_index_iterator<R>,
    ::fast_io::details::algorithms::algo_index_iterator<R>>;


template<typename Iter, typename Snt, typename OIter>
concept rotate_copy_iters_no_throw =
  ::fast_io::details::algorithms::sentinel_no_throw<Iter, Snt> &&
  ::fast_io::details::algorithms::output_iter_no_throw<
    OIter, ::std::iter_value_t<Iter>> &&
  requires(Iter &it, OIter &dest)
{
  { *dest = *it } noexcept;
};

template<typename Iter, typename Snt, typename OIter>
FAST_IO_UNSAFE_API
inline constexpr OIter rotate_copy_iters_impl(
  Iter first, Iter middle, Snt last, OIter dest)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::rotate_copy_iters_no_throw<
      Iter, Snt, OIter>)
{
  for (Iter it{middle}; it != last; ++it, ++dest)
  {
    *dest = *it;
  }
  for (Iter it{first}; it != middle; ++it, ++dest)
  {
    *dest = *it;
  }
  return dest;
}

} // namespace fast_io::details::algorithms

namespace fast_io::algorithms
{

// Rotate `[first, last)` so that `middle` becomes the first element. Returns
// the iterator that now holds the old first element, i.e. `first + (last -
// middle)`.
template<::std::forward_iterator Iter, ::std::sentinel_for<Iter> Snt>
FAST_IO_UNSAFE_API
inline constexpr Iter rotate_iters(Iter first, Iter middle, Snt last)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::rotate_iters_no_throw<Iter, Snt>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first)>),
    rotate_iters_define, first, middle, last);
  return ::fast_io::details::algorithms::rotate_iters_impl(first, middle, last);
}

template<::std::ranges::forward_range R>
inline constexpr ::std::ranges::iterator_t<R> rotate(
  R &&r, ::std::ranges::iterator_t<R> middle)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::rotate_iters_no_throw<
      ::std::ranges::iterator_t<R>, ::std::ranges::sentinel_t<R>>)
{
  return ::fast_io::algorithms::rotate_iters(
    ::std::ranges::begin(r), middle, ::std::ranges::end(r));
}

// Rotate the subrange `[firstidx, lastidx)` around `middleidx`. Returns the
// index that now holds the element formerly at `firstidx`.
template<::std::ranges::random_access_range R>
inline constexpr ::std::size_t rotate_index_range(
  R &&r, ::std::size_t firstidx, ::std::size_t middleidx,
  ::std::size_t lastidx)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::rotate_index_range_no_throw<R>)
{
  constexpr bool throwing_violations{
    ::fast_io::details::algorithms::range_throws_on_violations<R>};
  ::std::size_t const last{
    ::fast_io::details::algorithms::rotate_index_range_check<throwing_violations>(
      ::std::ranges::size(r), firstidx, middleidx, lastidx)};
  auto [first, last_it] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R>(r), firstidx, last);
  ::fast_io::algorithms::rotate_iters(
    first, first + (middleidx - firstidx), last_it);
  return firstidx + (last - middleidx);
}

template<::std::ranges::random_access_range R>
inline constexpr ::std::size_t rotate_index_of(
  R &&r, ::std::ranges::iterator_t<R> middle)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::rotate_iters_no_throw<
      ::std::ranges::iterator_t<R>, ::std::ranges::sentinel_t<R>>)
{
  auto const first{::std::ranges::begin(r)};
  auto const result{::fast_io::algorithms::rotate(
    ::std::forward<R>(r), middle)};
  return static_cast<::std::size_t>(result - first);
}


// Copy `[first, last)` to `dest` rotated so that `middle` becomes first.
template<::std::forward_iterator Iter, ::std::sentinel_for<Iter> Snt,
  typename OIter>
FAST_IO_UNSAFE_API
inline constexpr OIter rotate_copy_iters(
  Iter first, Iter middle, Snt last, OIter dest)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::rotate_copy_iters_no_throw<
      Iter, Snt, OIter>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first), decltype(dest)>),
    rotate_copy_iters_define, first, middle, last, dest);
  return ::fast_io::details::algorithms::rotate_copy_iters_impl(
    first, middle, last, dest);
}

template<::std::ranges::forward_range R, typename OIter>
FAST_IO_UNSAFE_API
inline constexpr OIter rotate_copy(R &&r, ::std::ranges::iterator_t<R> middle,
  OIter dest)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::rotate_copy_iters_no_throw<
      ::std::ranges::iterator_t<R>, ::std::ranges::sentinel_t<R>,
      OIter>)
{
  return ::fast_io::algorithms::rotate_copy_iters(
    ::std::ranges::begin(r), middle, ::std::ranges::end(r), dest);
}

template<::std::ranges::random_access_range R, typename OIter>
FAST_IO_UNSAFE_API
inline constexpr OIter rotate_copy_index_range(
  R &&r, ::std::size_t firstidx, ::std::size_t middleidx,
  ::std::size_t lastidx, OIter dest)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::rotate_copy_iters_no_throw<
      ::fast_io::details::algorithms::algo_index_iterator<R>,
      ::fast_io::details::algorithms::algo_index_iterator<R>, OIter> &&
    !::fast_io::details::algorithms::range_throws_on_violations<R>)
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R>(r), firstidx, lastidx);
  if (middleidx < firstidx || lastidx < middleidx) [[unlikely]]
  {
    ::fast_io::containers::details::violation_report_on_invalid_argument<
      ::fast_io::details::algorithms::range_throws_on_violations<R>>();
  }
  return ::fast_io::algorithms::rotate_copy_iters(
    first, first + static_cast<::std::iter_difference_t<
      ::fast_io::details::algorithms::algo_index_iterator<R>>>(
      middleidx - firstidx),
    last, dest);
}

} // namespace fast_io::algorithms
