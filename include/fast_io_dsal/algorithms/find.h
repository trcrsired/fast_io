#pragma once

namespace fast_io::details::algorithms
{

template<typename T>
inline constexpr bool find_small_trivially_copyable{
  !::std::is_array_v<T> &&
  ::std::is_trivially_copy_constructible_v<T> &&
  ::std::is_trivially_copyable_v<T> &&
  sizeof(T) <= 2 * sizeof(::std::size_t)};

template<typename T>
inline constexpr bool find_bitwise_value{
  !::std::is_volatile_v<T> &&
  ::std::is_trivially_copyable_v<T> &&
  ::std::has_unique_object_representations_v<T>};

template<typename T>
inline constexpr bool find_simd_value_type{
  ::fast_io::details::algorithms::find_bitwise_value<T> &&
  (sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4 ||
   sizeof(T) == 8 || sizeof(T) == 16) &&
  sizeof(T) <= 2 * sizeof(::std::size_t) &&
  ::fast_io::intrinsics::optimal_simd_vector_run_with_cpu_instruction_size != 0};

inline constexpr bool find_hosted_memchr{
#if (__STDC_HOSTED__ == 1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED == 1)) && !defined(_LIBCPP_FREESTANDING)
#if defined(__GLIBC__) && defined(__GLIBC_MINOR__)
#if (__GLIBC__ > 2 || (__GLIBC__ == 2 && __GLIBC_MINOR__ >= 26))
  true
#endif
#endif
#endif
};

template<typename T, typename V>
inline constexpr bool find_use_memchr{
  ::fast_io::details::algorithms::find_hosted_memchr &&
  sizeof(::std::remove_cv_t<T>) == 1 &&
  ::std::same_as<::std::remove_cvref_t<V>, ::std::remove_cv_t<T>> &&
  ::fast_io::details::algorithms::find_bitwise_value<::std::remove_cv_t<T>>};

template<typename T, typename V>
inline constexpr bool find_use_wmemchr{
  ::fast_io::details::algorithms::find_hosted_memchr &&
  sizeof(::std::remove_cv_t<T>) == 4 && sizeof(wchar_t) == 4 &&
  alignof(::std::remove_cv_t<T>) >= alignof(wchar_t) &&
  ::std::same_as<::std::remove_cvref_t<V>, ::std::remove_cv_t<T>> &&
  ::fast_io::details::algorithms::find_bitwise_value<::std::remove_cv_t<T>>};

template<typename T, typename V>
inline constexpr bool find_use_simd{
  ::std::same_as<::std::remove_cvref_t<V>, ::std::remove_cv_t<T>> &&
  ::fast_io::details::algorithms::find_simd_value_type<::std::remove_cv_t<T>>};

template<typename T>
inline constexpr bool find_bitwise_equal(T const& a, T const& b) noexcept
{
  using bytes_type = ::fast_io::freestanding::array<::std::uint_least8_t, sizeof(T)>;
  auto const left{::std::bit_cast<bytes_type>(a)};
  auto const right{::std::bit_cast<bytes_type>(b)};
  for (::std::size_t i{}; i < sizeof(T); ++i)
  {
    if (left[i] != right[i])
    {
      return false;
    }
  }
  return true;
}

template<typename T>
FAST_IO_UNSAFE_API
inline constexpr T* find_ptr_simd(T* first, T* last, ::std::remove_cv_t<T> const& value) noexcept
{
  using value_type = ::std::remove_cv_t<T>;
  constexpr ::std::size_t vector_size{
    ::fast_io::intrinsics::optimal_simd_vector_run_with_cpu_instruction_size};
  using unit_type = ::std::conditional_t<
    sizeof(value_type) >= 8, ::std::uint_least64_t,
    ::std::conditional_t<
      sizeof(value_type) >= 4, ::std::uint_least32_t,
      ::std::conditional_t<
        sizeof(value_type) >= 2, ::std::uint_least16_t,
        ::std::uint_least8_t>>>;
  constexpr ::std::size_t unit_count{sizeof(value_type) / sizeof(unit_type)};
  constexpr ::std::size_t lane_count{vector_size / sizeof(unit_type)};
  constexpr ::std::size_t elements_per_vector{vector_size / sizeof(value_type)};
  using simd_vector_type = ::fast_io::intrinsics::simd_vector<unit_type, lane_count>;
  FAST_IO_IF_NOT_CONSTEVAL
  {
    unit_type pattern[lane_count];
    __builtin_memcpy(pattern, __builtin_addressof(value), sizeof(value_type));
    for (::std::size_t i{unit_count}; i < lane_count; ++i)
    {
      pattern[i] = pattern[i - unit_count];
    }
    simd_vector_type needle;
    needle.load(pattern);
    auto const lane_offset = [](simd_vector_type const &compared) constexpr noexcept -> ::std::size_t
    {
      unit_type mask[lane_count];
      compared.store(mask);
      for (::std::size_t i{}; i < lane_count; i += unit_count)
      {
        ::std::size_t j{};
        for (; j < unit_count; ++j)
        {
          if (!mask[i + j])
          {
            break;
          }
        }
        if (j == unit_count)
        {
          return i / unit_count;
        }
      }
      return elements_per_vector;
    };
    constexpr ::std::size_t unroll{4};
    for (; static_cast<::std::size_t>(last - first) >= unroll * elements_per_vector;
         first += unroll * elements_per_vector)
    {
      simd_vector_type h0;
      simd_vector_type h1;
      simd_vector_type h2;
      simd_vector_type h3;
      h0.load(first);
      h1.load(first + elements_per_vector);
      h2.load(first + 2 * elements_per_vector);
      h3.load(first + 3 * elements_per_vector);
      auto const c0{h0 == needle};
      auto const c1{h1 == needle};
      auto const c2{h2 == needle};
      auto const c3{h3 == needle};
      auto const any{(c0 | c1) | (c2 | c3)};
      if (::fast_io::intrinsics::is_all_zeros(any))
      {
        continue;
      }
      if (auto const offset{lane_offset(c0)}; offset != elements_per_vector)
      {
        return first + offset;
      }
      if (auto const offset{lane_offset(c1)}; offset != elements_per_vector)
      {
        return first + elements_per_vector + offset;
      }
      if (auto const offset{lane_offset(c2)}; offset != elements_per_vector)
      {
        return first + 2 * elements_per_vector + offset;
      }
      if (auto const offset{lane_offset(c3)}; offset != elements_per_vector)
      {
        return first + 3 * elements_per_vector + offset;
      }
    }
    for (; static_cast<::std::size_t>(last - first) >= elements_per_vector;
         first += elements_per_vector)
    {
      simd_vector_type haystack;
      haystack.load(first);
      auto const compared{haystack == needle};
      if (::fast_io::intrinsics::is_all_zeros(compared))
      {
        continue;
      }
      if (auto const offset{lane_offset(compared)}; offset != elements_per_vector)
      {
        return first + offset;
      }
    }
  }
  for (; first != last; ++first)
  {
    if (::fast_io::details::algorithms::find_bitwise_equal(*first, value))
    {
      return first;
    }
  }
  return first;
}

template<typename T, typename V>
inline constexpr bool find_ptr_no_throw =
  ::fast_io::details::algorithms::find_use_memchr<T, V> ||
  ::fast_io::details::algorithms::find_use_wmemchr<T, V> ||
  ::fast_io::details::algorithms::find_use_simd<T, V> ||
  noexcept(*::std::declval<T*>() == ::std::declval<V const&>());

template<typename T, typename V>
FAST_IO_UNSAFE_API
inline constexpr T* find_ptr(T* first, T* last, V const& value)
  FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::details::algorithms::find_ptr_no_throw<T, V>)
{
  if constexpr (::fast_io::details::algorithms::find_use_memchr<T, V>)
  {
    FAST_IO_IF_NOT_CONSTEVAL
    {
      auto const count{static_cast<::std::size_t>(last - first)};
      if (count == 0)
      {
        return first;
      }
      void *const found{__builtin_memchr(
        first, static_cast<int>(static_cast<unsigned char>(value)), count)};
      return found == nullptr ? last : static_cast<T *>(found);
    }
  }
  else if constexpr (::fast_io::details::algorithms::find_use_wmemchr<T, V>)
  {
    FAST_IO_IF_NOT_CONSTEVAL
    {
      auto const count{static_cast<::std::size_t>(last - first)};
      if (count == 0)
      {
        return first;
      }
      auto const needle{::std::bit_cast<wchar_t>(value)};
      void *const found{__builtin_wmemchr(
        reinterpret_cast<wchar_t const *>(first), needle, count)};
      return found == nullptr ? last : static_cast<T *>(found);
    }
  }
  if constexpr (::fast_io::details::algorithms::find_use_simd<T, V>)
  {
    return ::fast_io::details::algorithms::find_ptr_simd(first, last, value);
  }
  else
  {
    for (; first != last; ++first)
    {
      if (*first == value)
      {
        return first;
      }
    }
    return first;
  }
}

template<typename Iter, typename V>
concept find_iter_no_throw =
  ::fast_io::details::algorithms::iter_no_throw<Iter> &&
  requires(Iter& first, V const& value)
{
  { *first == value } noexcept;
};

template<typename Iter, typename Snt, typename V>
concept find_iters_no_throw =
  ::fast_io::details::algorithms::sentinel_no_throw<Iter, Snt> &&
  ::fast_io::details::algorithms::find_iter_no_throw<Iter, V>;

template<typename R, typename T>
concept find_ranges_no_throw =
  ::std::ranges::range<R> &&
  ::fast_io::details::algorithms::find_iter_no_throw<
    ::std::ranges::iterator_t<R>, T>;

template<typename R, typename T>
concept find_index_range_no_throw =
  ::std::ranges::random_access_range<R> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R> &&
  ::fast_io::details::algorithms::find_iter_no_throw<
    ::fast_io::details::algorithms::algo_index_iterator<R>, T>;

template<typename Iter, typename Snt, typename V>
FAST_IO_UNSAFE_API
inline constexpr Iter find_iters_value_impl(Iter first, Snt last, V const& value)
  FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::details::algorithms::find_iters_no_throw<Iter, Snt, V>)
{
  if constexpr (::std::is_pointer_v<Iter> && ::std::same_as<Iter, Snt>)
  {
    return ::fast_io::details::algorithms::find_ptr(first, last, value);
  }
  else if constexpr (::std::contiguous_iterator<Iter> && !::std::is_pointer_v<Iter> &&
    ::std::contiguous_iterator<Snt>)
  {
    auto const first_pointer{::std::to_address(first)};
    auto const found{::fast_io::details::algorithms::find_ptr(
      first_pointer, ::std::to_address(last), value)};
    return first + static_cast<::std::iter_difference_t<Iter>>(found - first_pointer);
  }
  else if constexpr (::std::contiguous_iterator<Iter> && !::std::is_pointer_v<Iter> &&
    !::std::contiguous_iterator<Snt> && ::std::sized_sentinel_for<Snt, Iter>)
  {
    auto const count{last - first};
    auto const first_pointer{::std::to_address(first)};
    auto const found{::fast_io::details::algorithms::find_ptr(
      first_pointer, first_pointer + count, value)};
    return first + static_cast<::std::iter_difference_t<Iter>>(found - first_pointer);
  }
  else
  {
    for (; first != last; ++first)
    {
      if (*first == value)
      {
        return first;
      }
    }
    return first;
  }
}

template<typename Iter, typename Snt, typename T>
FAST_IO_UNSAFE_API
inline constexpr Iter find_iters_impl(Iter first, Snt last, T const& value)
  FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::details::algorithms::find_iters_no_throw<Iter, Snt, T>)
{
  using value_type = ::std::remove_cvref_t<T>;
  if constexpr (::fast_io::details::algorithms::find_small_trivially_copyable<value_type>)
  {
    value_type const local{value};
    return ::fast_io::details::algorithms::find_iters_value_impl(first, last, local);
  }
  else
  {
    return ::fast_io::details::algorithms::find_iters_value_impl(first, last, value);
  }
}

template<typename Iter, typename F>
concept find_if_iter_no_throw =
  ::fast_io::details::algorithms::iter_no_throw<Iter> &&
  requires(Iter &it, F &f)
{
  { static_cast<bool>(f(*it)) } noexcept;
};

template<typename Iter, typename Snt, typename F>
concept find_if_iters_no_throw =
  ::fast_io::details::algorithms::sentinel_no_throw<Iter, Snt> &&
  ::fast_io::details::algorithms::find_if_iter_no_throw<Iter, F>;

template<typename R, typename F>
concept find_if_ranges_no_throw =
  ::std::ranges::input_range<R> &&
  ::std::is_nothrow_move_constructible_v<F> &&
  ::fast_io::details::algorithms::find_if_iter_no_throw<
    ::std::ranges::iterator_t<R>, F>;

template<typename R, typename F>
concept find_if_index_range_no_throw =
  ::std::ranges::random_access_range<R> &&
  !::fast_io::details::algorithms::range_throws_on_violations<R> &&
  ::std::is_nothrow_move_constructible_v<F> &&
  ::fast_io::details::algorithms::find_if_iter_no_throw<
    ::fast_io::details::algorithms::algo_index_iterator<R>, F>;

template<typename Iter, typename Snt, typename F>
FAST_IO_UNSAFE_API
inline constexpr Iter find_if_iters_impl(Iter first, Snt last, F &f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::find_if_iters_no_throw<Iter, Snt, F>)
{
  for (; first != last; ++first)
  {
    if (static_cast<bool>(f(*first)))
    {
      break;
    }
  }
  return first;
}

template<typename Iter, typename Snt, typename F>
FAST_IO_UNSAFE_API
inline constexpr Iter find_if_not_iters_impl(Iter first, Snt last, F &f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::find_if_iters_no_throw<Iter, Snt, F>)
{
  for (; first != last; ++first)
  {
    if (!static_cast<bool>(f(*first)))
    {
      break;
    }
  }
  return first;
}

}

namespace fast_io::algorithms
{
template<::std::input_iterator Iter, ::std::sentinel_for<Iter> Snt, typename T>
FAST_IO_UNSAFE_API
inline constexpr Iter find_iters(Iter first, Snt last, T const& value)
  FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::details::algorithms::find_iters_no_throw<Iter, Snt, T>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first)>),
    find_iters_define, first, last, value);
  return ::fast_io::details::algorithms::find_iters_impl(first, last, value);
}

template<::std::ranges::range R, typename T>
FAST_IO_UNSAFE_API
inline constexpr ::std::ranges::iterator_t<R> find(R&& r, T const& value)
  FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::details::algorithms::find_ranges_no_throw<R, T>)
{
  return ::fast_io::algorithms::find_iters(::std::ranges::begin(r), ::std::ranges::end(r), value);
}

template<::std::ranges::random_access_range R, typename T>
inline constexpr ::std::size_t find_index_range(R&& r, ::std::size_t firstidx, ::std::size_t lastidx, T const& value)
  FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::details::algorithms::find_index_range_no_throw<R, T>)
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(::std::forward<R>(r), firstidx, lastidx);
  auto const found{::fast_io::algorithms::find_iters(first, last, value)};
  return firstidx + static_cast<::std::size_t>(found - first);
}

template<::std::ranges::random_access_range R, typename T>
inline constexpr ::std::size_t find_index_of(R &&r, T const &value)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::find_ranges_no_throw<R, T>)
{
  auto const first{::std::ranges::begin(r)};
  auto const found{::fast_io::algorithms::find(
    ::std::forward<R>(r), value)};
  return static_cast<::std::size_t>(found - first);
}

// Find the first element for which `f` returns true.
template<::std::input_iterator Iter, ::std::sentinel_for<Iter> Snt,
  typename F>
FAST_IO_UNSAFE_API
inline constexpr Iter find_if_iters(Iter first, Snt last, F f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::find_if_iters_no_throw<Iter, Snt, F>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first)>),
    find_if_iters_define, first, last, f);
  return ::fast_io::details::algorithms::find_if_iters_impl(first, last, f);
}

template<::std::ranges::input_range R, typename F>
FAST_IO_UNSAFE_API
inline constexpr ::std::ranges::iterator_t<R> find_if(R &&r, F f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::find_if_ranges_no_throw<R, F>)
{
  return ::fast_io::algorithms::find_if_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), ::std::move(f));
}

template<::std::ranges::random_access_range R, typename F>
inline constexpr ::std::size_t find_if_index_range(
  R &&r, ::std::size_t firstidx, ::std::size_t lastidx, F f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::find_if_index_range_no_throw<R, F>)
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R>(r), firstidx, lastidx);
  auto const found{::fast_io::algorithms::find_if_iters(
    first, last, ::std::move(f))};
  return firstidx + static_cast<::std::size_t>(found - first);
}

template<::std::ranges::random_access_range R, typename F>
inline constexpr ::std::size_t find_if_index_of(R &&r, F f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::find_if_ranges_no_throw<R, F>)
{
  auto const first{::std::ranges::begin(r)};
  auto const found{::fast_io::algorithms::find_if(
    ::std::forward<R>(r), ::std::move(f))};
  return static_cast<::std::size_t>(found - first);
}

// Find the first element for which `f` returns false.
template<::std::input_iterator Iter, ::std::sentinel_for<Iter> Snt,
  typename F>
FAST_IO_UNSAFE_API
inline constexpr Iter find_if_not_iters(Iter first, Snt last, F f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::find_if_iters_no_throw<Iter, Snt, F>)
{
  FAST_IO_ALGORITHMS_CUSTOMIZE_IF(
    (!::fast_io::details::algorithms::contiguous_iters<decltype(first)>),
    find_if_not_iters_define, first, last, f);
  return ::fast_io::details::algorithms::find_if_not_iters_impl(
    first, last, f);
}

template<::std::ranges::input_range R, typename F>
FAST_IO_UNSAFE_API
inline constexpr ::std::ranges::iterator_t<R> find_if_not(R &&r, F f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::find_if_ranges_no_throw<R, F>)
{
  return ::fast_io::algorithms::find_if_not_iters(
    ::std::ranges::begin(r), ::std::ranges::end(r), ::std::move(f));
}

template<::std::ranges::random_access_range R, typename F>
inline constexpr ::std::size_t find_if_not_index_range(
  R &&r, ::std::size_t firstidx, ::std::size_t lastidx, F f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::find_if_index_range_no_throw<R, F>)
{
  auto [first, last] = ::fast_io::details::algorithms::algo_index_to_iters(
    ::std::forward<R>(r), firstidx, lastidx);
  auto const found{::fast_io::algorithms::find_if_not_iters(
    first, last, ::std::move(f))};
  return firstidx + static_cast<::std::size_t>(found - first);
}

template<::std::ranges::random_access_range R, typename F>
inline constexpr ::std::size_t find_if_not_index_of(R &&r, F f)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::find_if_ranges_no_throw<R, F>)
{
  auto const first{::std::ranges::begin(r)};
  auto const found{::fast_io::algorithms::find_if_not(
    ::std::forward<R>(r), ::std::move(f))};
  return static_cast<::std::size_t>(found - first);
}

}
