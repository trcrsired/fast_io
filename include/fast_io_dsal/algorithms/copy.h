#pragma once

namespace fast_io::details::algorithms
{

template<typename T, typename U>
inline constexpr bool copy_byte_copyable{
  ::std::is_trivially_copyable_v<::std::remove_cv_t<T>> &&
  ::std::is_trivially_copyable_v<::std::remove_cv_t<U>> &&
  (::std::same_as<::std::remove_cv_t<T>, ::std::remove_cv_t<U>> ||
   ((::std::integral<::std::remove_cv_t<T>> || ::std::same_as<::std::remove_cv_t<T>, ::std::byte>) &&
    (::std::integral<::std::remove_cv_t<U>> || ::std::same_as<::std::remove_cv_t<U>, ::std::byte>) &&
    sizeof(T) == sizeof(U)))};

template<typename Iter, typename OIter>
concept copy_assign_no_throw =
  requires(Iter& first, OIter& dest) {
    { *dest = *first } noexcept;
  };

template<typename Iter, typename Snt, typename OIter>
concept copy_iters_no_throw =
  ::fast_io::details::algorithms::sentinel_no_throw<Iter, Snt> &&
  ::fast_io::details::algorithms::output_iter_no_throw<OIter, ::std::iter_value_t<Iter>> &&
  (::fast_io::details::algorithms::copy_byte_copyable<
     ::std::iter_value_t<Iter>, ::std::iter_value_t<OIter>> ||
   ::fast_io::details::algorithms::copy_assign_no_throw<Iter, OIter>);

template<typename Iter, typename OIter>
concept copy_n_iters_no_throw =
  ::fast_io::details::algorithms::iter_no_throw<Iter> &&
  ::fast_io::details::algorithms::output_iter_no_throw<OIter, ::std::iter_value_t<Iter>> &&
  (::fast_io::details::algorithms::copy_byte_copyable<
     ::std::iter_value_t<Iter>, ::std::iter_value_t<OIter>> ||
   ::fast_io::details::algorithms::copy_assign_no_throw<Iter, OIter>);

template<typename T, typename U>
inline constexpr U* copy_pointer(T* first, T* last, U* dest)
  FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::details::algorithms::copy_byte_copyable<T, U> &&
    !noexcept(*dest = *first))
{
  if constexpr (::fast_io::details::algorithms::copy_byte_copyable<T, U>)
  {
    auto const count{static_cast<::std::size_t>(last - first)};
    if consteval
    {
      for (::std::size_t i{}; i != count; ++i)
      {
        dest[i] = first[i];
      }
    }
    else
    {
      if (count != 0)
      {
        ::fast_io::freestanding::my_memmove(dest, first, count * sizeof(T));
      }
    }
    return dest + count;
  }
  else
  {
    for (; first != last; ++first, ++dest)
    {
      *dest = *first;
    }
    return dest;
  }
}

template<typename T, typename U>
inline constexpr U* copy_n_pointer(T* first, ::std::size_t count, U* dest)
  FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::details::algorithms::copy_byte_copyable<T, U> &&
    !noexcept(*dest = *first))
{
  if constexpr (::fast_io::details::algorithms::copy_byte_copyable<T, U>)
  {
    if consteval
    {
      for (::std::size_t i{}; i != count; ++i)
      {
        dest[i] = first[i];
      }
    }
    else
    {
      if (count != 0)
      {
        ::fast_io::freestanding::my_memmove(dest, first, count * sizeof(T));
      }
    }
    return dest + count;
  }
  else
  {
    for (::std::size_t i{}; i != count; ++i, ++first, ++dest)
    {
      *dest = *first;
    }
    return dest;
  }
}

template<typename T, typename U>
inline constexpr U* copy_backward_pointer(T* first, T* last, U* d_last)
  FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::details::algorithms::copy_byte_copyable<T, U> &&
    !noexcept(*d_last = *last))
{
  auto const count{static_cast<::std::size_t>(last - first)};
  d_last -= count;
  if constexpr (::fast_io::details::algorithms::copy_byte_copyable<T, U>)
  {
    if consteval
    {
      for (::std::size_t i{}; i != count; ++i)
      {
        d_last[i] = first[i];
      }
    }
    else
    {
      if (count != 0)
      {
        ::fast_io::freestanding::my_memmove(d_last, first, count * sizeof(T));
      }
    }
  }
  else
  {
    auto source_last{last};
    auto dest_last{d_last + count};
    for (; source_last != first;)
    {
      *(--dest_last) = *(--source_last);
    }
  }
  return d_last;
}

template<typename T, typename U>
inline constexpr U* non_overlapped_copy_pointer(T* first, T* last, U* dest)
  FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::details::algorithms::copy_byte_copyable<T, U> &&
    !noexcept(*dest = *first))
{
  if constexpr (::fast_io::details::algorithms::copy_byte_copyable<T, U>)
  {
    auto const count{static_cast<::std::size_t>(last - first)};
    if consteval
    {
      for (::std::size_t i{}; i != count; ++i)
      {
        dest[i] = first[i];
      }
    }
    else
    {
      if (count != 0)
      {
        ::fast_io::freestanding::my_memcpy(dest, first, count * sizeof(T));
      }
    }
    return dest + count;
  }
  else
  {
    for (; first != last; ++first, ++dest)
    {
      *dest = *first;
    }
    return dest;
  }
}

template<typename T, typename U>
inline constexpr U* non_overlapped_copy_n_pointer(T* first, ::std::size_t count, U* dest)
  FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::details::algorithms::copy_byte_copyable<T, U> &&
    !noexcept(*dest = *first))
{
  if constexpr (::fast_io::details::algorithms::copy_byte_copyable<T, U>)
  {
    if consteval
    {
      for (::std::size_t i{}; i != count; ++i)
      {
        dest[i] = first[i];
      }
    }
    else
    {
      if (count != 0)
      {
        ::fast_io::freestanding::my_memcpy(dest, first, count * sizeof(T));
      }
    }
    return dest + count;
  }
  else
  {
    for (::std::size_t i{}; i != count; ++i, ++first, ++dest)
    {
      *dest = *first;
    }
    return dest;
  }
}

template<typename Iter, typename Snt, typename OIter>
inline constexpr OIter copy_iters_impl(Iter first, Snt last, OIter dest)
  FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::details::algorithms::copy_iters_no_throw<Iter, Snt, OIter>)
{
  if constexpr (::fast_io::details::algorithms::copy_can_decay<Iter, Snt, OIter>)
  {
    auto const first_pointer{::std::to_address(first)};
    auto const last_pointer{::fast_io::details::algorithms::copy_source_last(first, last)};
    auto const dest_pointer{::std::to_address(dest)};
    auto const result_pointer{::fast_io::details::algorithms::copy_pointer(
      first_pointer, last_pointer, dest_pointer)};
    return dest + static_cast<::std::iter_difference_t<OIter>>(result_pointer - dest_pointer);
  }
  else
  {
    for (; first != last; ++first, ++dest)
    {
      *dest = *first;
    }
    return dest;
  }
}

template<typename Iter, typename OIter>
inline constexpr OIter copy_n_iters_impl(Iter first, ::std::size_t count, OIter dest)
  FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::details::algorithms::copy_n_iters_no_throw<Iter, OIter>)
{
  if constexpr (::std::contiguous_iterator<Iter> && ::std::contiguous_iterator<OIter> &&
    requires(Iter& it, OIter& out, ::std::iter_difference_t<OIter> difference) {
      { ::std::to_address(it) } noexcept;
      { ::std::to_address(out) } noexcept;
      { out + difference } noexcept;
    })
  {
    auto const first_pointer{::std::to_address(first)};
    auto const dest_pointer{::std::to_address(dest)};
    auto const result_pointer{::fast_io::details::algorithms::copy_n_pointer(
      first_pointer, count, dest_pointer)};
    return dest + static_cast<::std::iter_difference_t<OIter>>(result_pointer - dest_pointer);
  }
  else
  {
    for (::std::size_t i{}; i != count; ++i, ++first, ++dest)
    {
      *dest = *first;
    }
    return dest;
  }
}

template<typename Iter, typename Snt, typename OIter>
inline constexpr OIter copy_backward_iters_impl(Iter first, Snt last, OIter d_last)
  FAST_IO_HERBCEPTIONS_THROWS_IF(!(::std::bidirectional_iterator<Iter> &&
    ::std::bidirectional_iterator<OIter> &&
    requires(Iter& source_last, OIter& dest_last) {
      { --source_last } noexcept;
      { --dest_last } noexcept;
    } &&
    (::fast_io::details::algorithms::copy_byte_copyable<
       ::std::iter_value_t<Iter>, ::std::iter_value_t<OIter>> ||
     noexcept(*d_last = *last))) &&
    (!(::std::contiguous_iterator<Iter> && ::std::contiguous_iterator<OIter>) ||
     !(::std::same_as<Iter, Snt> || ::std::sized_sentinel_for<Snt, Iter>) ||
     (requires(Iter& source_last, OIter& dest_last, ::std::iter_difference_t<OIter> difference) {
        { ::std::to_address(source_last) } noexcept;
        { ::std::to_address(dest_last) } noexcept;
        { dest_last - difference } noexcept;
      })))
{
  if constexpr (::std::contiguous_iterator<Iter> && ::std::contiguous_iterator<OIter> &&
    (::std::same_as<Iter, Snt> || ::std::sized_sentinel_for<Snt, Iter>) &&
    requires(Iter& source_last, OIter& dest_last, ::std::iter_difference_t<OIter> difference) {
      { ::std::to_address(source_last) } noexcept;
      { ::std::to_address(dest_last) } noexcept;
      { dest_last - difference } noexcept;
    })
  {
    auto const first_pointer{::std::to_address(first)};
    auto const last_pointer{::fast_io::details::algorithms::copy_source_last(first, last)};
    auto const dest_last_pointer{::std::to_address(d_last)};
    auto const result_pointer{::fast_io::details::algorithms::copy_backward_pointer(
      first_pointer, last_pointer, dest_last_pointer)};
    return d_last - static_cast<::std::iter_difference_t<OIter>>(dest_last_pointer - result_pointer);
  }
  else
  {
    for (; first != last;)
    {
      --last;
      --d_last;
      *d_last = *last;
    }
    return d_last;
  }
}

template<typename Iter, typename Snt, typename OIter>
inline constexpr OIter non_overlapped_copy_iters_impl(Iter first, Snt last, OIter dest)
  FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::details::algorithms::copy_iters_no_throw<Iter, Snt, OIter>)
{
  if constexpr (::fast_io::details::algorithms::copy_can_decay<Iter, Snt, OIter>)
  {
    auto const first_pointer{::std::to_address(first)};
    auto const last_pointer{::fast_io::details::algorithms::copy_source_last(first, last)};
    auto const dest_pointer{::std::to_address(dest)};
    auto const result_pointer{::fast_io::details::algorithms::non_overlapped_copy_pointer(
      first_pointer, last_pointer, dest_pointer)};
    return dest + static_cast<::std::iter_difference_t<OIter>>(result_pointer - dest_pointer);
  }
  else
  {
    for (; first != last; ++first, ++dest)
    {
      *dest = *first;
    }
    return dest;
  }
}

template<typename Iter, typename OIter>
inline constexpr OIter non_overlapped_copy_n_iters_impl(Iter first, ::std::size_t count, OIter dest)
  FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::details::algorithms::copy_n_iters_no_throw<Iter, OIter>)
{
  if constexpr (::std::contiguous_iterator<Iter> && ::std::contiguous_iterator<OIter> &&
    requires(Iter& it, OIter& out, ::std::iter_difference_t<OIter> difference) {
      { ::std::to_address(it) } noexcept;
      { ::std::to_address(out) } noexcept;
      { out + difference } noexcept;
    })
  {
    auto const first_pointer{::std::to_address(first)};
    auto const dest_pointer{::std::to_address(dest)};
    auto const result_pointer{::fast_io::details::algorithms::non_overlapped_copy_n_pointer(
      first_pointer, count, dest_pointer)};
    return dest + static_cast<::std::iter_difference_t<OIter>>(result_pointer - dest_pointer);
  }
  else
  {
    for (::std::size_t i{}; i != count; ++i, ++first, ++dest)
    {
      *dest = *first;
    }
    return dest;
  }
}

template<typename Iter, typename Snt, typename OIter>
concept uninitialized_copy_iters_no_throw =
  ::fast_io::details::algorithms::sentinel_no_throw<Iter, Snt> &&
  ::fast_io::details::algorithms::output_iter_no_throw<OIter, ::std::iter_value_t<Iter>> &&
  ::std::is_nothrow_constructible_v<
    ::std::iter_value_t<OIter>, ::std::iter_reference_t<Iter>> &&
  ::std::is_nothrow_destructible_v<::std::iter_value_t<OIter>>;

template<typename Iter, typename OIter>
concept uninitialized_copy_n_iters_no_throw =
  ::fast_io::details::algorithms::iter_no_throw<Iter> &&
  ::fast_io::details::algorithms::output_iter_no_throw<OIter, ::std::iter_value_t<Iter>> &&
  ::std::is_nothrow_constructible_v<
    ::std::iter_value_t<OIter>, ::std::iter_reference_t<Iter>> &&
  ::std::is_nothrow_destructible_v<::std::iter_value_t<OIter>>;

template<typename T, typename U>
inline constexpr U* uninitialized_copy_pointer(T* first, T* last, U* dest)
  FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::details::algorithms::copy_byte_copyable<T, U> &&
    (!::std::is_nothrow_constructible_v<U, T const&> || !::std::is_nothrow_destructible_v<U>))
{
  auto const count{static_cast<::std::size_t>(last - first)};
  if constexpr (::fast_io::details::algorithms::copy_byte_copyable<T, U>)
  {
    if consteval
    {
      for (::std::size_t i{}; i != count; ++i)
      {
        ::new (static_cast<void *>(dest + i)) U(first[i]);
      }
    }
    else
    {
      if (count != 0)
      {
        ::fast_io::freestanding::my_memmove(dest, first, count * sizeof(T));
      }
    }
    return dest + count;
  }
  else
  {
    ::fast_io::details::algorithms::uninitialized_guard<U *, U> guard{dest, dest};
    for (; first != last; ++first, ++dest)
    {
      ::new (static_cast<void *>(dest)) U(*first);
      ++guard.current;
    }
    guard.current = guard.first;
    return dest;
  }
}

template<typename T, typename U>
inline constexpr U* uninitialized_copy_n_pointer(T* first, ::std::size_t count, U* dest)
  FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::details::algorithms::copy_byte_copyable<T, U> &&
    (!::std::is_nothrow_constructible_v<U, T const&> || !::std::is_nothrow_destructible_v<U>))
{
  if constexpr (::fast_io::details::algorithms::copy_byte_copyable<T, U>)
  {
    if consteval
    {
      for (::std::size_t i{}; i != count; ++i)
      {
        ::new (static_cast<void *>(dest + i)) U(first[i]);
      }
    }
    else
    {
      if (count != 0)
      {
        ::fast_io::freestanding::my_memmove(dest, first, count * sizeof(T));
      }
    }
    return dest + count;
  }
  else
  {
    ::fast_io::details::algorithms::uninitialized_guard<U *, U> guard{dest, dest};
    for (::std::size_t i{}; i != count; ++i, ++first, ++dest)
    {
      ::new (static_cast<void *>(dest)) U(*first);
      ++guard.current;
    }
    guard.current = guard.first;
    return dest;
  }
}

template<typename T, typename U>
inline constexpr U* uninitialized_copy_backward_pointer(T* first, T* last, U* d_last)
  FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::details::algorithms::copy_byte_copyable<T, U> &&
    (!::std::is_nothrow_constructible_v<U, T const&> || !::std::is_nothrow_destructible_v<U>))
{
  auto const count{static_cast<::std::size_t>(last - first)};
  if constexpr (::fast_io::details::algorithms::copy_byte_copyable<T, U>)
  {
    d_last -= count;
    if consteval
    {
      for (::std::size_t i{}; i != count; ++i)
      {
        ::new (static_cast<void *>(d_last + i)) U(first[i]);
      }
    }
    else
    {
      if (count != 0)
      {
        ::fast_io::freestanding::my_memmove(d_last, first, count * sizeof(T));
      }
    }
    return d_last;
  }
  else
  {
    auto const dest_last{d_last};
    ::fast_io::details::algorithms::uninitialized_guard<U *, U> guard{dest_last, dest_last};
    for (; first != last;)
    {
      --last;
      --d_last;
      ::new (static_cast<void *>(d_last)) U(*last);
      guard.first = d_last;
    }
    guard.first = guard.current;
    return d_last;
  }
}

template<typename Iter, typename Snt, typename OIter>
inline constexpr OIter uninitialized_copy_iters_impl(Iter first, Snt last, OIter dest)
  FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::details::algorithms::uninitialized_copy_iters_no_throw<Iter, Snt, OIter>)
{
  if constexpr (::fast_io::details::algorithms::copy_can_decay<Iter, Snt, OIter>)
  {
    auto const first_pointer{::std::to_address(first)};
    auto const last_pointer{::fast_io::details::algorithms::copy_source_last(first, last)};
    auto const dest_pointer{::std::to_address(dest)};
    auto const result_pointer{::fast_io::details::algorithms::uninitialized_copy_pointer(
      first_pointer, last_pointer, dest_pointer)};
    return dest + static_cast<::std::iter_difference_t<OIter>>(result_pointer - dest_pointer);
  }
  else
  {
    using output_value_type = ::std::iter_value_t<OIter>;
    ::fast_io::details::algorithms::uninitialized_guard<OIter, output_value_type> guard{dest, dest};
    for (; first != last; ++first, ++dest)
    {
      ::new (static_cast<void *>(__builtin_addressof(*dest))) output_value_type(*first);
      ++guard.current;
    }
    guard.current = guard.first;
    return dest;
  }
}

template<typename Iter, typename OIter>
inline constexpr OIter uninitialized_copy_n_iters_impl(Iter first, ::std::size_t count, OIter dest)
  FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::details::algorithms::uninitialized_copy_n_iters_no_throw<Iter, OIter>)
{
  if constexpr (::std::contiguous_iterator<Iter> && ::std::contiguous_iterator<OIter> &&
    requires(Iter& it, OIter& out, ::std::iter_difference_t<OIter> difference) {
      { ::std::to_address(it) } noexcept;
      { ::std::to_address(out) } noexcept;
      { out + difference } noexcept;
    })
  {
    auto const first_pointer{::std::to_address(first)};
    auto const dest_pointer{::std::to_address(dest)};
    auto const result_pointer{::fast_io::details::algorithms::uninitialized_copy_n_pointer(
      first_pointer, count, dest_pointer)};
    return dest + static_cast<::std::iter_difference_t<OIter>>(result_pointer - dest_pointer);
  }
  else
  {
    using output_value_type = ::std::iter_value_t<OIter>;
    ::fast_io::details::algorithms::uninitialized_guard<OIter, output_value_type> guard{dest, dest};
    for (::std::size_t i{}; i != count; ++i, ++first, ++dest)
    {
      ::new (static_cast<void *>(__builtin_addressof(*dest))) output_value_type(*first);
      ++guard.current;
    }
    guard.current = guard.first;
    return dest;
  }
}

template<typename Iter, typename Snt, typename OIter>
inline constexpr OIter uninitialized_copy_backward_iters_impl(Iter first, Snt last, OIter d_last)
  FAST_IO_HERBCEPTIONS_THROWS_IF(!(::std::bidirectional_iterator<Iter> &&
    ::std::bidirectional_iterator<OIter> &&
    requires(Iter& source_last, OIter& dest_last) {
      { --source_last } noexcept;
      { --dest_last } noexcept;
    } &&
    ::std::is_nothrow_constructible_v<
      ::std::iter_value_t<OIter>, ::std::iter_reference_t<Iter>> &&
    ::std::is_nothrow_destructible_v<::std::iter_value_t<OIter>>) &&
    (!(::std::contiguous_iterator<Iter> && ::std::contiguous_iterator<OIter>) ||
     !(::std::same_as<Iter, Snt> || ::std::sized_sentinel_for<Snt, Iter>) ||
     (requires(Iter& source_last, OIter& dest_last, ::std::iter_difference_t<OIter> difference) {
        { ::std::to_address(source_last) } noexcept;
        { ::std::to_address(dest_last) } noexcept;
        { dest_last - difference } noexcept;
      })))
{
  if constexpr (::std::contiguous_iterator<Iter> && ::std::contiguous_iterator<OIter> &&
    (::std::same_as<Iter, Snt> || ::std::sized_sentinel_for<Snt, Iter>) &&
    requires(Iter& source_last, OIter& dest_last, ::std::iter_difference_t<OIter> difference) {
      { ::std::to_address(source_last) } noexcept;
      { ::std::to_address(dest_last) } noexcept;
      { dest_last - difference } noexcept;
    })
  {
    auto const first_pointer{::std::to_address(first)};
    auto const last_pointer{::fast_io::details::algorithms::copy_source_last(first, last)};
    auto const dest_last_pointer{::std::to_address(d_last)};
    auto const result_pointer{::fast_io::details::algorithms::uninitialized_copy_backward_pointer(
      first_pointer, last_pointer, dest_last_pointer)};
    return d_last - static_cast<::std::iter_difference_t<OIter>>(dest_last_pointer - result_pointer);
  }
  else
  {
    using output_value_type = ::std::iter_value_t<OIter>;
    ::fast_io::details::algorithms::uninitialized_guard<OIter, output_value_type> guard{d_last, d_last};
    for (; first != last;)
    {
      --last;
      --d_last;
      ::new (static_cast<void *>(__builtin_addressof(*d_last))) output_value_type(*last);
      guard.first = d_last;
    }
    guard.first = guard.current;
    return d_last;
  }
}

template<typename Iter, typename Snt, typename OIter, typename F>
concept copy_if_iters_no_throw =
  ::fast_io::details::algorithms::sentinel_no_throw<Iter, Snt> &&
  ::fast_io::details::algorithms::output_iter_no_throw<
    OIter, ::std::iter_value_t<Iter>> &&
  requires(Iter &it, F &f)
{
  { static_cast<bool>(f(*it)) } noexcept;
} &&
  (::fast_io::details::algorithms::copy_byte_copyable<
     ::std::iter_value_t<Iter>, ::std::iter_value_t<OIter>> ||
   ::fast_io::details::algorithms::copy_assign_no_throw<Iter, OIter>);

template<typename Iter, typename Snt, typename OIter, typename F>
inline constexpr OIter copy_if_iters_impl(
  Iter first, Snt last, OIter dest, F &f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::copy_if_iters_no_throw<
      Iter, Snt, OIter, F>)
{
  for (; first != last; ++first)
  {
    if (static_cast<bool>(f(*first)))
    {
      *dest = *first;
      ++dest;
    }
  }
  return dest;
}

}

namespace fast_io::algorithms
{

template<::std::input_iterator Iter, ::std::sentinel_for<Iter> Snt, typename OIter>
inline constexpr OIter copy_iters(Iter first, Snt last, OIter dest)
  FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::details::algorithms::copy_iters_no_throw<Iter, Snt, OIter>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first), decltype(dest)>),
    copy_iters_define, first, last, dest);
  return ::fast_io::details::algorithms::copy_iters_impl(first, last, dest);
}

template<::std::ranges::range R, typename OIter>
inline constexpr OIter copy(R&& r, OIter dest)
  FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::details::algorithms::copy_iters_no_throw<
    ::std::ranges::iterator_t<R>, ::std::ranges::sentinel_t<R>, OIter>)
{
  return ::fast_io::algorithms::copy_iters(::std::ranges::begin(r), ::std::ranges::end(r), dest);
}

template<::std::input_iterator Iter, typename OIter>
inline constexpr OIter copy_n_iters(Iter first, ::std::size_t count, OIter dest)
  FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::details::algorithms::copy_n_iters_no_throw<Iter, OIter>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first), decltype(dest)>),
    copy_n_iters_define, first, count, dest);
  return ::fast_io::details::algorithms::copy_n_iters_impl(first, count, dest);
}

template<::std::ranges::input_range R, typename OIter>
inline constexpr OIter copy_n(R&& r, ::std::size_t count, OIter dest)
  FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::details::algorithms::copy_n_iters_no_throw<
    ::std::ranges::iterator_t<R>, OIter>)
{
  return ::fast_io::algorithms::copy_n_iters(::std::ranges::begin(r), count, dest);
}

template<::std::bidirectional_iterator Iter, ::std::bidirectional_iterator OIter>
inline constexpr OIter copy_backward_iters(Iter first, Iter last, OIter d_last)
  FAST_IO_HERBCEPTIONS_THROWS_IF(!(::fast_io::details::algorithms::iter_no_throw<Iter> &&
    ::fast_io::details::algorithms::iter_no_throw<OIter> &&
    (::fast_io::details::algorithms::copy_byte_copyable<
       ::std::iter_value_t<Iter>, ::std::iter_value_t<OIter>> ||
     noexcept(*d_last = *last))))
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first), decltype(d_last)>),
    copy_backward_iters_define, first, last, d_last);
  return ::fast_io::details::algorithms::copy_backward_iters_impl(first, last, d_last);
}

template<::std::ranges::bidirectional_range R, typename OIter>
inline constexpr OIter copy_backward(R&& r, OIter d_last)
  FAST_IO_HERBCEPTIONS_THROWS_IF(!(::std::bidirectional_iterator<OIter> &&
    ::fast_io::details::algorithms::iter_no_throw<::std::ranges::iterator_t<R>> &&
    ::fast_io::details::algorithms::iter_no_throw<OIter> &&
    (::fast_io::details::algorithms::copy_byte_copyable<
       ::std::ranges::range_value_t<R>, ::std::iter_value_t<OIter>> ||
     noexcept(*d_last = *::std::ranges::begin(r)))))
{
  return ::fast_io::algorithms::copy_backward_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), d_last);
}

template<::std::input_iterator Iter, ::std::sentinel_for<Iter> Snt, typename OIter>
inline constexpr OIter non_overlapped_copy_iters(Iter first, Snt last, OIter dest)
  FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::details::algorithms::copy_iters_no_throw<Iter, Snt, OIter>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first), decltype(dest)>),
    non_overlapped_copy_iters_define, first, last, dest);
  return ::fast_io::details::algorithms::non_overlapped_copy_iters_impl(first, last, dest);
}

template<::std::ranges::range R, typename OIter>
inline constexpr OIter non_overlapped_copy(R&& r, OIter dest)
  FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::details::algorithms::copy_iters_no_throw<
    ::std::ranges::iterator_t<R>, ::std::ranges::sentinel_t<R>, OIter>)
{
  return ::fast_io::algorithms::non_overlapped_copy_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), dest);
}

template<::std::input_iterator Iter, typename OIter>
inline constexpr OIter non_overlapped_copy_n_iters(Iter first, ::std::size_t count, OIter dest)
  FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::details::algorithms::copy_n_iters_no_throw<Iter, OIter>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first), decltype(dest)>),
    non_overlapped_copy_n_iters_define, first, count, dest);
  return ::fast_io::details::algorithms::non_overlapped_copy_n_iters_impl(first, count, dest);
}

template<::std::ranges::input_range R, typename OIter>
inline constexpr OIter non_overlapped_copy_n(R&& r, ::std::size_t count, OIter dest)
  FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::details::algorithms::copy_n_iters_no_throw<
    ::std::ranges::iterator_t<R>, OIter>)
{
  return ::fast_io::algorithms::non_overlapped_copy_n_iters(
    ::std::ranges::begin(r), count, dest);
}

template<::std::input_iterator Iter, ::std::sentinel_for<Iter> Snt, typename OIter>
inline constexpr OIter uninitialized_copy_iters(Iter first, Snt last, OIter dest)
  FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::details::algorithms::uninitialized_copy_iters_no_throw<Iter, Snt, OIter>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first), decltype(dest)>),
    uninitialized_copy_iters_define, first, last, dest);
  return ::fast_io::details::algorithms::uninitialized_copy_iters_impl(first, last, dest);
}

template<::std::ranges::range R, typename OIter>
inline constexpr OIter uninitialized_copy(R&& r, OIter dest)
  FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::details::algorithms::uninitialized_copy_iters_no_throw<
    ::std::ranges::iterator_t<R>, ::std::ranges::sentinel_t<R>, OIter>)
{
  return ::fast_io::algorithms::uninitialized_copy_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), dest);
}

template<::std::input_iterator Iter, typename OIter>
inline constexpr OIter uninitialized_copy_n_iters(Iter first, ::std::size_t count, OIter dest)
  FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::details::algorithms::uninitialized_copy_n_iters_no_throw<Iter, OIter>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first), decltype(dest)>),
    uninitialized_copy_n_iters_define, first, count, dest);
  return ::fast_io::details::algorithms::uninitialized_copy_n_iters_impl(first, count, dest);
}

template<::std::ranges::input_range R, typename OIter>
inline constexpr OIter uninitialized_copy_n(R&& r, ::std::size_t count, OIter dest)
  FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::details::algorithms::uninitialized_copy_n_iters_no_throw<
    ::std::ranges::iterator_t<R>, OIter>)
{
  return ::fast_io::algorithms::uninitialized_copy_n_iters(
    ::std::ranges::begin(r), count, dest);
}

template<::std::bidirectional_iterator Iter, ::std::bidirectional_iterator OIter>
inline constexpr OIter uninitialized_copy_backward_iters(Iter first, Iter last, OIter d_last)
  FAST_IO_HERBCEPTIONS_THROWS_IF(!(::fast_io::details::algorithms::iter_no_throw<Iter> &&
    ::fast_io::details::algorithms::iter_no_throw<OIter> &&
    ::std::is_nothrow_constructible_v<
      ::std::iter_value_t<OIter>, ::std::iter_reference_t<Iter>> &&
    ::std::is_nothrow_destructible_v<::std::iter_value_t<OIter>>))
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first), decltype(d_last)>),
    uninitialized_copy_backward_iters_define, first, last, d_last);
  return ::fast_io::details::algorithms::uninitialized_copy_backward_iters_impl(first, last, d_last);
}

template<::std::ranges::bidirectional_range R, typename OIter>
inline constexpr OIter uninitialized_copy_backward(R&& r, OIter d_last)
  FAST_IO_HERBCEPTIONS_THROWS_IF(!(::std::bidirectional_iterator<OIter> &&
    ::fast_io::details::algorithms::iter_no_throw<::std::ranges::iterator_t<R>> &&
    ::fast_io::details::algorithms::iter_no_throw<OIter> &&
    ::std::is_nothrow_constructible_v<
      ::std::iter_value_t<OIter>, ::std::ranges::range_reference_t<R>> &&
    ::std::is_nothrow_destructible_v<::std::iter_value_t<OIter>>))
{
  return ::fast_io::algorithms::uninitialized_copy_backward_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), d_last);
}


// Copy only the elements for which `f` returns true.
template<::std::input_iterator Iter, ::std::sentinel_for<Iter> Snt,
  typename OIter, typename F>
inline constexpr OIter copy_if_iters(
  Iter first, Snt last, OIter dest, F f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::copy_if_iters_no_throw<
      Iter, Snt, OIter, F>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first), decltype(dest)>),
    copy_if_iters_define, first, last, dest, f);
  return ::fast_io::details::algorithms::copy_if_iters_impl(
    first, last, dest, f);
}

template<::std::ranges::input_range R, typename OIter, typename F>
inline constexpr OIter copy_if(R &&r, OIter dest, F f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::copy_if_iters_no_throw<
      ::std::ranges::iterator_t<R>, ::std::ranges::sentinel_t<R>,
      OIter, F>)
{
  return ::fast_io::algorithms::copy_if_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), dest,
    ::std::move(f));
}

}
