#pragma once

namespace fast_io::details::algorithms
{

template<::std::input_iterator Iter,
  ::std::sentinel_for<Iter> Snt>
struct iter_pairs
{
  using iterator = Iter;
  using sentinel = Snt;
  Iter first;
  Snt last;
};

template<bool throwing_violations>
inline constexpr ::std::size_t algo_index_check(::std::size_t n, ::std::size_t firstidx, ::std::size_t lastidx)
  FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_violations)
{
  if (lastidx < firstidx || n < lastidx) [[unlikely]]
  {
    ::fast_io::containers::details::violation_report_on_invalid_argument<throwing_violations>();
  }
  return lastidx;
}

template<bool throwing_violations, typename T>
inline constexpr ::fast_io::details::algorithms::iter_pairs<T*, T*> algo_index_to_ptrs(
  T* dt, ::std::size_t n, ::std::size_t firstidx, ::std::size_t lastidx)
  FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_violations)
{
  ::std::size_t const last{
    ::fast_io::details::algorithms::algo_index_check<throwing_violations>(n, firstidx, lastidx)};
  return {dt + firstidx, dt + last};
}

template<bool throwing_violations>
inline constexpr ::fast_io::details::algorithms::iter_pairs<::std::byte const*, ::std::byte const*> algo_index_to_ptrs_common(
  ::std::byte const* dt, ::std::size_t n, ::std::size_t firstidx, ::std::size_t lastidx, ::std::size_t sizeoft)
  FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_violations)
{
  ::std::size_t const last{
    ::fast_io::details::algorithms::algo_index_check<throwing_violations>(n, firstidx, lastidx)};
  return {dt + firstidx * sizeoft, dt + last * sizeoft};
}

template<typename R>
inline constexpr ::fast_io::allocator_adapter_flags range_allocator_flags = []() constexpr
{
  using type = ::std::remove_cvref_t<R>;
  if constexpr(requires{
    type::allocator_flags;
  })
  {
    static_assert(::std::same_as<::std::remove_cv_t<decltype(type::allocator_flags)>, ::fast_io::allocator_adapter_flags>);
    return type::allocator_flags;
  }
  else
  {
    return ::fast_io::allocator_adapter_flags::none;
  }
}();

template<typename R>
inline constexpr bool range_throws_on_violations{
  (::fast_io::details::algorithms::range_allocator_flags<R> &
   ::fast_io::allocator_adapter_flags::throws_on_violations) != ::fast_io::allocator_adapter_flags::none};

template<typename R>
struct algo_index_iterator_impl
{
  using type = decltype(::std::ranges::begin(::std::declval<R&>()));
};

template<typename R>
  requires ::std::ranges::contiguous_range<R>
struct algo_index_iterator_impl<R>
{
  using type = decltype(::std::to_address(::std::ranges::data(::std::declval<R&>())));
};

template<typename R>
using algo_index_iterator = typename ::fast_io::details::algorithms::algo_index_iterator_impl<R>::type;

template<typename R>
using algo_index_iters = ::fast_io::details::algorithms::iter_pairs<
  ::fast_io::details::algorithms::algo_index_iterator<R>,
  ::fast_io::details::algorithms::algo_index_iterator<R>>;

template<::std::ranges::random_access_range R>
inline constexpr ::fast_io::details::algorithms::algo_index_iters<R> algo_index_to_iters(
  R&& r, ::std::size_t firstidx, ::std::size_t lastidx)
  FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::details::algorithms::range_throws_on_violations<R>)
{
  constexpr bool throwing_violations{
    ::fast_io::details::algorithms::range_throws_on_violations<R>};
  if constexpr(::std::ranges::contiguous_range<R>)
  {
    using pointer_type = ::fast_io::details::algorithms::algo_index_iterator<R>;
    using pointer_pairs = ::fast_io::details::algorithms::iter_pairs<pointer_type, pointer_type>;
    if consteval
    {
      return ::fast_io::details::algorithms::algo_index_to_ptrs<throwing_violations>(
        ::std::to_address(::std::ranges::data(r)), ::std::ranges::size(r), firstidx, lastidx);
    }
    else
    {
      static_assert(sizeof(pointer_pairs) == sizeof(::fast_io::details::algorithms::iter_pairs<::std::byte const*, ::std::byte const*>));
      static_assert(::std::is_trivially_copyable_v<pointer_pairs>);
      return ::std::bit_cast<pointer_pairs>(
        ::fast_io::details::algorithms::algo_index_to_ptrs_common<throwing_violations>(
          reinterpret_cast<::std::byte const*>(::std::to_address(::std::ranges::data(r))),
          ::std::ranges::size(r), firstidx, lastidx, sizeof(::std::ranges::range_value_t<R>)));
    }
  }
  else
  {
    ::std::size_t const last{
      ::fast_io::details::algorithms::algo_index_check<throwing_violations>(
        ::std::ranges::size(r), firstidx, lastidx)};
    auto bg{::std::ranges::begin(r)};
    return ::fast_io::details::algorithms::iter_pairs<decltype(bg), decltype(bg)>{bg + firstidx, bg + last};
  }
}

template<typename Iter>
concept iter_no_throw =
  requires(Iter& it) {
    { ++it } noexcept;
  };

template<typename Iter>
concept decrement_iter_no_throw =
  ::fast_io::details::algorithms::iter_no_throw<Iter> &&
  requires(Iter& it) {
    { --it } noexcept;
  };

template<typename T>
inline constexpr bool move_no_throw{
  ::std::is_nothrow_move_constructible_v<T> &&
  ::std::is_nothrow_move_assignable_v<T> &&
  ::std::is_nothrow_destructible_v<T>};

template<typename T>
inline constexpr bool copy_trivial_v{
  ::std::is_trivially_copyable_v<T> &&
  ::std::is_copy_constructible_v<T> &&
  ::std::is_copy_assignable_v<T>};

template<typename Iter, typename Snt>
concept sentinel_no_throw =
  ::fast_io::details::algorithms::iter_no_throw<Iter> &&
  ::std::sentinel_for<Snt, Iter>;

template<typename OIter, typename T>
concept output_iter_no_throw =
  ::std::output_iterator<OIter, T> &&
  ::fast_io::details::algorithms::iter_no_throw<OIter>;

// Customization is skipped when every iterator involved is contiguous: the
// algorithm already takes a pointer fast path in that case.
template<typename... Iters>
concept contiguous_iters =
  (::std::contiguous_iterator<Iters> && ...);

template<typename Iter, typename Snt>
inline constexpr auto copy_source_last(Iter first, Snt last) noexcept
{
  if constexpr (::std::same_as<Iter, Snt> || ::std::contiguous_iterator<Snt>)
  {
    return ::std::to_address(last);
  }
  else
  {
    return ::std::to_address(first) + (last - first);
  }
}

template<typename Iter, typename Snt, typename OIter>
inline constexpr bool copy_can_decay{
  ::std::contiguous_iterator<Iter> &&
  ::std::contiguous_iterator<OIter> &&
  (::std::same_as<Iter, Snt> ||
   ::std::contiguous_iterator<Snt> ||
   ::std::sized_sentinel_for<Snt, Iter>)};

template<typename OIter, typename T>
struct uninitialized_guard
{
  OIter first;
  OIter current;
  inline constexpr ~uninitialized_guard() noexcept
  {
    if constexpr (!::std::is_trivially_destructible_v<T>)
    {
      for (; first != current; ++first)
      {
        __builtin_addressof(*first)->~T();
      }
    }
  }
};

template<typename Cmp>
concept comparator_pass_no_throw =
  ::std::is_nothrow_copy_constructible_v<Cmp> &&
  ::std::is_nothrow_move_constructible_v<Cmp> &&
  ::std::is_nothrow_default_constructible_v<Cmp>;

// A comparator reports ordering through a three-way result: one of the
// comparison categories. A bool is the trap. It satisfies every expression the
// algorithms spell -- `0 > false` is always false and `false == 0` is always
// true -- so a predicate-shaped `compare` compiles and then turns the
// algorithm into a silent no-op. The result type is constrained here instead,
// which makes such a comparator a hard error at the call site.
#if defined(__cpp_lib_three_way_comparison) && __cpp_lib_three_way_comparison >= 201907L
template<typename T>
concept three_way_result =
  ::std::same_as<::std::remove_cvref_t<T>, ::std::strong_ordering> ||
  ::std::same_as<::std::remove_cvref_t<T>, ::std::weak_ordering> ||
  ::std::same_as<::std::remove_cvref_t<T>, ::std::partial_ordering>;
#else
// Without <compare> no category type can be named. Require the shape of a
// three-way result instead: comparable to literal zero both ways and not a
// bool.
template<typename T>
concept three_way_result =
  !::std::same_as<::std::remove_cvref_t<T>, bool> &&
  requires(::std::remove_cvref_t<T> const r)
{
  { 0 < r } -> ::std::convertible_to<bool>;
  { 0 > r } -> ::std::convertible_to<bool>;
  { r == 0 } -> ::std::convertible_to<bool>;
};
#endif

// `compare(a, b)` must yield a three-way result and must not throw. This is the
// one-direction form; algorithms that compare both ways also instantiate the
// reversed form below.
template<typename Cmp, typename T, typename U>
concept three_way_compare_no_throw =
  requires(Cmp cmp, T const &a, U const &b)
{
  { cmp.compare(a, b) } noexcept -> three_way_result;
};

// Both operand orders the ordering algorithms use. A throwing `compare` is
// excluded, matching the `FAST_IO_HERBCEPTIONS_THROWS_IF` contracts.
template<typename Cmp, typename T, typename U = T>
concept three_way_comparator_for =
  ::fast_io::details::algorithms::three_way_compare_no_throw<Cmp, T, U> &&
  ::fast_io::details::algorithms::three_way_compare_no_throw<Cmp, U, T>;

// The same result contract without the nothrow requirement. Algorithms use
// this one in a requires-clause: a throwing comparator is still allowed (its
// `throws` contract just says so), but a bool-returning one is rejected before
// the body is instantiated, where it would otherwise sort nothing.
template<typename Cmp, typename T, typename U = T>
concept three_way_compare =
  requires(Cmp &cmp, T const &a, U const &b)
{
  { cmp.compare(a, b) } -> three_way_result;
};

template<typename Cmp, typename T, typename U = T>
concept three_way_comparator =
  ::fast_io::details::algorithms::three_way_compare<Cmp, T, U> &&
  ::fast_io::details::algorithms::three_way_compare<Cmp, U, T>;

// Equality may be spelled as `equal(a, b)` or as `compare(a, b) == 0`. The
// latter is only meaningful for a three-way result, so a comparator that only
// offers a bool `compare` is rejected.
template<typename Cmp, typename T, typename U = T>
concept equality_comparator =
  requires(Cmp &cmp, T const &a, U const &b) { cmp.equal(a, b); } ||
  ::fast_io::details::algorithms::three_way_compare<Cmp, T, U>;

// Operations are passed by value and invoked through their `compute` member,
// mirroring comparators. A closure has no `compute` member, so lambdas are
// rejected rather than adapted.
template<typename Op>
concept operation_pass_no_throw =
  ::std::is_nothrow_copy_constructible_v<Op> &&
  ::std::is_nothrow_move_constructible_v<Op> &&
  ::std::is_nothrow_default_constructible_v<Op>;

// Comparators may provide `equal(a, b)` as a cheaper equality test. When it is
// absent, equality falls back to `compare(a, b) == 0`.
template<typename Cmp, typename T, typename U>
inline constexpr bool equal_call_no_throw_v = []() constexpr
{
  if constexpr (requires(Cmp cmp, T const &a, U const &b) { cmp.equal(a, b); })
  {
    return requires(Cmp cmp, T const &a, U const &b)
    {
      { cmp.equal(a, b) } noexcept;
    };
  }
  else
  {
    return ::fast_io::details::algorithms::three_way_compare_no_throw<Cmp, T, U> &&
           requires(Cmp cmp, T const &a, U const &b)
    {
      { cmp.compare(a, b) == 0 } noexcept;
    };
  }
}();

template<typename Cmp, typename T, typename U>
inline constexpr bool equal(Cmp cmp, T const &a, U const &b)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::details::algorithms::equal_call_no_throw_v<Cmp, T, U>)
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

}
