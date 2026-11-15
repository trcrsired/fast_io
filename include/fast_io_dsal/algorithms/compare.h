#pragma once

namespace fast_io::algorithms
{

// Default three-way comparator, written against `<=>` directly so the
// algorithms do not depend on fast_io::freestanding or on <algorithm>.
struct compare_three_way
{
  template<typename T, typename U>
  inline constexpr auto compare(T const &t, U const &u) const
    FAST_IO_HERBCEPTIONS_THROWS_IF(!noexcept(t <=> u))
  {
    return t <=> u;
  }

  // Optional equality fast path. Algorithms that need an equality test use
  // `equal` when a comparator provides it, and otherwise fall back to
  // `compare(a, b) == 0`.
  template<typename T, typename U>
  inline constexpr bool equal(T const &t, U const &u) const
    FAST_IO_HERBCEPTIONS_THROWS_IF(!noexcept((t <=> u) == 0))
  {
    return (t <=> u) == 0;
  }
};

// Equality-only comparator. It exposes only the optional `equal` fast path, so
// equality-based algorithms accept element types that define `==` without
// `<=>`. Ordering algorithms still require `compare`.
struct equal_to
{
  template<typename T, typename U>
  inline constexpr bool equal(T const &t, U const &u) const
    FAST_IO_HERBCEPTIONS_THROWS_IF(!noexcept(t == u))
  {
    return t == u;
  }
};

// Apply a projection. A member object pointer is dereferenced directly; any
// other projection is called. `std::invoke` is avoided so this header stays
// free of includes and usable in freestanding builds.
template<typename Projection, typename T>
inline constexpr bool compare_by_apply_no_throw_v = []() constexpr
{
  if constexpr (::std::is_member_object_pointer_v<Projection>)
  {
    return noexcept(::std::declval<T const &>().*::std::declval<Projection const &>());
  }
  else
  {
    return noexcept(::std::declval<Projection const &>()(::std::declval<T const &>()));
  }
}();

template<typename Projection, typename T>
inline constexpr decltype(auto) compare_by_apply(
  Projection const &projection, T const &value)
  FAST_IO_HERBCEPTIONS_THROWS_IF(
    !::fast_io::algorithms::compare_by_apply_no_throw_v<Projection, T>)
{
  if constexpr (::std::is_member_object_pointer_v<Projection>)
  {
    return value.*projection;
  }
  else
  {
    return projection(value);
  }
}

// Comparator over a projected key. This is the single place a projection is
// allowed: the algorithm still sees one comparator with a real `compare`
// member, so no projector template axis and no closure type reaches the core.
//
// There is deliberately no default projection. Identity ordering is expressed
// by passing no comparator at all, which selects `compare_three_way`.
template<typename Projection, typename Comparator = compare_three_way>
struct compare_by
{
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
  [[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address)
  [[no_unique_address]]
#endif
#endif
  Projection projection;

#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
  [[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address)
  [[no_unique_address]]
#endif
#endif
  Comparator comparator;

  template<typename T, typename U>
  inline constexpr auto compare(T const &a, U const &b) const
    FAST_IO_HERBCEPTIONS_THROWS_IF(
      !(::fast_io::algorithms::compare_by_apply_no_throw_v<Projection, T> &&
        ::fast_io::algorithms::compare_by_apply_no_throw_v<Projection, U> &&
        noexcept(::std::declval<Comparator const &>().compare(
          ::fast_io::algorithms::compare_by_apply(
            ::std::declval<Projection const &>(), ::std::declval<T const &>()),
          ::fast_io::algorithms::compare_by_apply(
            ::std::declval<Projection const &>(), ::std::declval<U const &>())))))
  {
    decltype(auto) ka{::fast_io::algorithms::compare_by_apply(projection, a)};
    decltype(auto) kb{::fast_io::algorithms::compare_by_apply(projection, b)};
    return comparator.compare(ka, kb);
  }

  template<typename T, typename U>
  inline constexpr bool equal(T const &a, U const &b) const
    FAST_IO_HERBCEPTIONS_THROWS_IF(
      !(::fast_io::algorithms::compare_by_apply_no_throw_v<Projection, T> &&
        ::fast_io::algorithms::compare_by_apply_no_throw_v<Projection, U> &&
        ::fast_io::details::algorithms::equal_call_no_throw_v<
          Comparator,
          decltype(::fast_io::algorithms::compare_by_apply(
            ::std::declval<Projection const &>(), ::std::declval<T const &>())),
          decltype(::fast_io::algorithms::compare_by_apply(
            ::std::declval<Projection const &>(), ::std::declval<U const &>()))>))
  {
    decltype(auto) ka{::fast_io::algorithms::compare_by_apply(projection, a)};
    decltype(auto) kb{::fast_io::algorithms::compare_by_apply(projection, b)};
    return ::fast_io::details::algorithms::equal(comparator, ka, kb);
  }
};

template<typename Projection>
compare_by(Projection) -> compare_by<Projection, compare_three_way>;

template<typename Projection, typename Comparator>
compare_by(Projection, Comparator) -> compare_by<Projection, Comparator>;

// Same as `compare_by`, but the wrapped comparator is invoked with the two
// projected keys swapped, so the ordering is reversed. Equality is symmetric
// and is forwarded unchanged.
template<typename Projection, typename Comparator = compare_three_way>
struct descend_compare_by
{
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
  [[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address)
  [[no_unique_address]]
#endif
#endif
  Projection projection;

#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
  [[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address)
  [[no_unique_address]]
#endif
#endif
  Comparator comparator;

  template<typename T, typename U>
  inline constexpr auto compare(T const &a, U const &b) const
    FAST_IO_HERBCEPTIONS_THROWS_IF(
      !(::fast_io::algorithms::compare_by_apply_no_throw_v<Projection, T> &&
        ::fast_io::algorithms::compare_by_apply_no_throw_v<Projection, U> &&
        noexcept(::std::declval<Comparator const &>().compare(
          ::fast_io::algorithms::compare_by_apply(
            ::std::declval<Projection const &>(), ::std::declval<U const &>()),
          ::fast_io::algorithms::compare_by_apply(
            ::std::declval<Projection const &>(), ::std::declval<T const &>())))))
  {
    decltype(auto) ka{::fast_io::algorithms::compare_by_apply(projection, a)};
    decltype(auto) kb{::fast_io::algorithms::compare_by_apply(projection, b)};
    return comparator.compare(kb, ka);
  }

  template<typename T, typename U>
  inline constexpr bool equal(T const &a, U const &b) const
    FAST_IO_HERBCEPTIONS_THROWS_IF(
      !(::fast_io::algorithms::compare_by_apply_no_throw_v<Projection, T> &&
        ::fast_io::algorithms::compare_by_apply_no_throw_v<Projection, U> &&
        ::fast_io::details::algorithms::equal_call_no_throw_v<
          Comparator,
          decltype(::fast_io::algorithms::compare_by_apply(
            ::std::declval<Projection const &>(), ::std::declval<T const &>())),
          decltype(::fast_io::algorithms::compare_by_apply(
            ::std::declval<Projection const &>(), ::std::declval<U const &>()))>))
  {
    decltype(auto) ka{::fast_io::algorithms::compare_by_apply(projection, a)};
    decltype(auto) kb{::fast_io::algorithms::compare_by_apply(projection, b)};
    return ::fast_io::details::algorithms::equal(comparator, ka, kb);
  }
};

template<typename Projection>
descend_compare_by(Projection) -> descend_compare_by<Projection, compare_three_way>;

template<typename Projection, typename Comparator>
descend_compare_by(Projection, Comparator)
  -> descend_compare_by<Projection, Comparator>;

} // namespace fast_io::algorithms
