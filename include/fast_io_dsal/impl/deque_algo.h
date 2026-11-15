#pragma once

// Container-specific paths for fast_io's segmented deque. The generic
// algorithms are correct for deque iterators, but every element access has to
// re-establish the current block. These paths walk the blocks directly and
// hand each contiguous block to the pointer specialisations.
//
// This header is included by deque.h after the deque implementation. The
// algorithms themselves arrive through fast_io_core.h, which deque.h includes
// first, so the customization overloads below are found by ADL when a deque
// iterator is used. User-defined containers get the same treatment by
// providing their own `*_iters_define` overloads in a namespace associated
// with their iterator.

namespace fast_io::details::algorithms
{

template<typename T, bool isconst>
using deque_algo_iterator =
  ::fast_io::containers::details::deque_iterator<T, isconst>;

template<typename T, bool isconst, typename State, typename F>
inline constexpr State deque_forward_segments(
  deque_algo_iterator<T, isconst> first,
  deque_algo_iterator<T, isconst> last, State state, F f) noexcept
{
  constexpr ::std::size_t block_size{
    ::fast_io::containers::details::deque_block_size<sizeof(T)>};
  if (first == last)
  {
    return state;
  }
  T **controller{first.itercontent.controller_ptr};
  T **const last_controller{last.itercontent.controller_ptr};
  T *begin{first.itercontent.begin_ptr};
  T *current{first.itercontent.curr_ptr};
  for (;;)
  {
    T *const segment_end{
      controller == last_controller ? last.itercontent.curr_ptr
                                    : begin + block_size};
    state = f(state, current, segment_end);
    if (controller == last_controller)
    {
      break;
    }
    ++controller;
    begin = *controller;
    current = begin;
  }
  return state;
}

template<typename T, bool isconst, typename State, typename F>
inline constexpr State deque_backward_segments(
  deque_algo_iterator<T, isconst> first,
  deque_algo_iterator<T, isconst> last, State state, F f) noexcept
{
  constexpr ::std::size_t block_size{
    ::fast_io::containers::details::deque_block_size<sizeof(T)>};
  while (first != last)
  {
    auto tail{last};
    --tail;
    T **controller{tail.itercontent.controller_ptr};
    T *const begin{tail.itercontent.begin_ptr};
    T *const segment_end{tail.itercontent.curr_ptr + 1};
    T *const segment_begin{
      controller == first.itercontent.controller_ptr
        ? first.itercontent.curr_ptr
        : begin};
    state = f(state, segment_begin, segment_end);
    if (controller == first.itercontent.controller_ptr)
    {
      break;
    }
    auto *const previous_controller{controller - 1};
    T *const previous_begin{*previous_controller};
    last.itercontent.controller_ptr = previous_controller;
    last.itercontent.begin_ptr = previous_begin;
    last.itercontent.curr_ptr = previous_begin + block_size;
  }
  return state;
}

template<typename T, bool isconst>
inline constexpr void deque_reverse_segments(
  deque_algo_iterator<T, isconst> first,
  deque_algo_iterator<T, isconst> last)
  noexcept(::fast_io::details::algorithms::move_no_throw<T>)
{
  constexpr ::std::size_t block_size{
    ::fast_io::containers::details::deque_block_size<sizeof(T)>};
  auto front{first};
  auto back{last};
  while (front != back)
  {
    auto tail{back};
    --tail;
    if (front.itercontent.controller_ptr ==
        tail.itercontent.controller_ptr)
    {
      T *low{front.itercontent.curr_ptr};
      T *high{tail.itercontent.curr_ptr};
      while (low < high)
      {
        T value{::std::move(*low)};
        *low = ::std::move(*high);
        *high = ::std::move(value);
        ++low;
        --high;
      }
      return;
    }
    T *const front_begin{front.itercontent.curr_ptr};
    T *const front_end{front.itercontent.begin_ptr + block_size};
    T *const back_begin{tail.itercontent.begin_ptr};
    T *const back_end{tail.itercontent.curr_ptr};
    ::std::size_t const front_count{
      static_cast<::std::size_t>(front_end - front_begin)};
    ::std::size_t const back_count{
      static_cast<::std::size_t>(back_end - back_begin) + 1u};
    ::std::size_t const count{
      front_count < back_count ? front_count : back_count};
    for (::std::size_t i{}; i != count; ++i)
    {
      T value{::std::move(front_begin[i])};
      front_begin[i] = ::std::move(
        back_end[-static_cast<::std::ptrdiff_t>(i)]);
      back_end[-static_cast<::std::ptrdiff_t>(i)] = ::std::move(value);
    }
    front += static_cast<::std::iter_difference_t<
      deque_algo_iterator<T, isconst>>>(count);
    back -= static_cast<::std::iter_difference_t<
      deque_algo_iterator<T, isconst>>>(count);
  }
}

template<typename T>
struct deque_copy_segment
{
  template<typename OIter>
  inline constexpr OIter operator()(
    OIter dest, T *first, T *last) const
    noexcept(noexcept(
      ::fast_io::details::algorithms::copy_iters_impl(first, last, dest)))
  {
    return ::fast_io::details::algorithms::copy_iters_impl(
      first, last, dest);
  }
};

template<typename T>
struct deque_non_overlapped_copy_segment
{
  template<typename OIter>
  inline constexpr OIter operator()(
    OIter dest, T *first, T *last) const
    noexcept(noexcept(
      ::fast_io::details::algorithms::non_overlapped_copy_iters_impl(
        first, last, dest)))
  {
    return ::fast_io::details::algorithms::non_overlapped_copy_iters_impl(
      first, last, dest);
  }
};

template<typename T>
struct deque_copy_backward_segment
{
  template<typename OIter>
  inline constexpr OIter operator()(
    OIter dest, T *first, T *last) const
    noexcept(noexcept(
      ::fast_io::details::algorithms::copy_backward_iters_impl(
        first, last, dest)))
  {
    return ::fast_io::details::algorithms::copy_backward_iters_impl(
      first, last, dest);
  }
};

template<typename T>
struct deque_move_segment
{
  template<typename OIter>
  inline constexpr OIter operator()(
    OIter dest, T *first, T *last) const
    noexcept(noexcept(
      ::fast_io::details::algorithms::move_iters_impl(first, last, dest)))
  {
    return ::fast_io::details::algorithms::move_iters_impl(
      first, last, dest);
  }
};

template<typename T>
struct deque_move_backward_segment
{
  template<typename OIter>
  inline constexpr OIter operator()(
    OIter dest, T *first, T *last) const
    noexcept(noexcept(
      ::fast_io::details::algorithms::move_backward_iters_impl(
        first, last, dest)))
  {
    return ::fast_io::details::algorithms::move_backward_iters_impl(
      first, last, dest);
  }
};

template<typename T>
struct deque_uninitialized_copy_segment
{
  template<typename OIter>
  inline constexpr OIter operator()(
    OIter dest, T *first, T *last) const
    noexcept(noexcept(
      ::fast_io::details::algorithms::uninitialized_copy_iters_impl(
        first, last, dest)))
  {
    return ::fast_io::details::algorithms::uninitialized_copy_iters_impl(
      first, last, dest);
  }
};

template<typename T>
struct deque_uninitialized_copy_backward_segment
{
  template<typename OIter>
  inline constexpr OIter operator()(
    OIter dest, T *first, T *last) const
    noexcept(noexcept(
      ::fast_io::details::algorithms::uninitialized_copy_backward_iters_impl(
        first, last, dest)))
  {
    return ::fast_io::details::algorithms::uninitialized_copy_backward_iters_impl(
      first, last, dest);
  }
};

template<typename T>
struct deque_uninitialized_move_segment
{
  template<typename OIter>
  inline constexpr OIter operator()(
    OIter dest, T *first, T *last) const
    noexcept(noexcept(
      ::fast_io::details::algorithms::uninitialized_move_iters_impl(
        first, last, dest)))
  {
    return ::fast_io::details::algorithms::uninitialized_move_iters_impl(
      first, last, dest);
  }
};

template<typename T>
struct deque_uninitialized_move_backward_segment
{
  template<typename OIter>
  inline constexpr OIter operator()(
    OIter dest, T *first, T *last) const
    noexcept(noexcept(
      ::fast_io::details::algorithms::uninitialized_move_backward_iters_impl(
        first, last, dest)))
  {
    return ::fast_io::details::algorithms::uninitialized_move_backward_iters_impl(
      first, last, dest);
  }
};

template<typename T>
struct deque_uninitialized_relocate_segment
{
  template<typename OIter>
  inline constexpr OIter operator()(
    OIter dest, T *first, T *last) const
    noexcept(noexcept(
      ::fast_io::details::algorithms::uninitialized_relocate_iters_impl(
        first, last, dest)))
  {
    return ::fast_io::details::algorithms::uninitialized_relocate_iters_impl(
      first, last, dest);
  }
};

template<typename T>
struct deque_uninitialized_relocate_backward_segment
{
  template<typename OIter>
  inline constexpr OIter operator()(
    OIter dest, T *first, T *last) const
    noexcept(noexcept(
      ::fast_io::details::algorithms::uninitialized_relocate_backward_iters_impl(
        first, last, dest)))
  {
    return ::fast_io::details::algorithms::uninitialized_relocate_backward_iters_impl(
      first, last, dest);
  }
};

template<typename T>
struct deque_reverse_copy_segment
{
  template<typename OIter>
  inline constexpr OIter operator()(
    OIter dest, T *first, T *last) const
    noexcept(noexcept(
      ::fast_io::details::algorithms::reverse_copy_iters_impl(
        first, last, dest)))
  {
    return ::fast_io::details::algorithms::reverse_copy_iters_impl(
      first, last, dest);
  }
};

} // namespace fast_io::details::algorithms

namespace fast_io::containers::details
{

using ::fast_io::details::algorithms::deque_algo_iterator;

template<typename T, bool isconst, typename OIter>
inline constexpr OIter copy_iters_define(
  ::fast_io::io_algorithm_t, deque_algo_iterator<T, isconst> first,
  deque_algo_iterator<T, isconst> last, OIter dest)
  noexcept(noexcept(
    ::fast_io::details::algorithms::copy_iters_impl(
      ::std::declval<T *>(), ::std::declval<T *>(),
      ::std::declval<OIter>())))
{
  return ::fast_io::details::algorithms::deque_forward_segments(
    first, last, dest,
    ::fast_io::details::algorithms::deque_copy_segment<T>{});
}

template<typename T, bool isconst, typename OIter>
inline constexpr OIter copy_n_iters_define(
  ::fast_io::io_algorithm_t, deque_algo_iterator<T, isconst> first,
  ::std::size_t count, OIter dest)
  noexcept(noexcept(
    ::fast_io::details::algorithms::copy_n_iters_impl(
      ::std::declval<T *>(), ::std::declval<::std::size_t>(),
      ::std::declval<OIter>())))
{
  return ::fast_io::details::algorithms::deque_forward_segments(
    first, first + count, dest,
    ::fast_io::details::algorithms::deque_copy_segment<T>{});
}

template<typename T, bool isconst, typename OIter>
inline constexpr OIter copy_backward_iters_define(
  ::fast_io::io_algorithm_t, deque_algo_iterator<T, isconst> first,
  deque_algo_iterator<T, isconst> last, OIter d_last)
  noexcept(noexcept(
    ::fast_io::details::algorithms::copy_backward_iters_impl(
      ::std::declval<T *>(), ::std::declval<T *>(),
      ::std::declval<OIter>())))
{
  return ::fast_io::details::algorithms::deque_backward_segments(
    first, last, d_last,
    ::fast_io::details::algorithms::deque_copy_backward_segment<T>{});
}

template<typename T, bool isconst, typename OIter>
inline constexpr OIter non_overlapped_copy_iters_define(
  ::fast_io::io_algorithm_t,
  deque_algo_iterator<T, isconst> first,
  deque_algo_iterator<T, isconst> last, OIter dest)
  noexcept(noexcept(
    ::fast_io::details::algorithms::non_overlapped_copy_iters_impl(
      ::std::declval<T *>(), ::std::declval<T *>(),
      ::std::declval<OIter>())))
{
  return ::fast_io::details::algorithms::deque_forward_segments(
    first, last, dest,
    ::fast_io::details::algorithms::deque_non_overlapped_copy_segment<T>{});
}

template<typename T, bool isconst, typename OIter>
inline constexpr OIter non_overlapped_copy_n_iters_define(
  ::fast_io::io_algorithm_t,
  deque_algo_iterator<T, isconst> first, ::std::size_t count,
  OIter dest)
  noexcept(noexcept(
    ::fast_io::details::algorithms::non_overlapped_copy_n_iters_impl(
      ::std::declval<T *>(), ::std::declval<::std::size_t>(),
      ::std::declval<OIter>())))
{
  return ::fast_io::details::algorithms::deque_forward_segments(
    first, first + count, dest,
    ::fast_io::details::algorithms::deque_non_overlapped_copy_segment<T>{});
}

template<typename T, bool isconst, typename OIter>
  requires(!isconst)
inline constexpr OIter move_iters_define(
  ::fast_io::io_algorithm_t, deque_algo_iterator<T, isconst> first,
  deque_algo_iterator<T, isconst> last, OIter dest)
  noexcept(noexcept(
    ::fast_io::details::algorithms::move_iters_impl(
      ::std::declval<T *>(), ::std::declval<T *>(),
      ::std::declval<OIter>())))
{
  return ::fast_io::details::algorithms::deque_forward_segments(
    first, last, dest,
    ::fast_io::details::algorithms::deque_move_segment<T>{});
}

template<typename T, bool isconst, typename OIter>
  requires(!isconst)
inline constexpr OIter move_backward_iters_define(
  ::fast_io::io_algorithm_t, deque_algo_iterator<T, isconst> first,
  deque_algo_iterator<T, isconst> last, OIter d_last)
  noexcept(noexcept(
    ::fast_io::details::algorithms::move_backward_iters_impl(
      ::std::declval<T *>(), ::std::declval<T *>(),
      ::std::declval<OIter>())))
{
  return ::fast_io::details::algorithms::deque_backward_segments(
    first, last, d_last,
    ::fast_io::details::algorithms::deque_move_backward_segment<T>{});
}

template<typename T, bool isconst, typename OIter>
inline constexpr OIter uninitialized_copy_iters_define(
  ::fast_io::io_algorithm_t,
  deque_algo_iterator<T, isconst> first,
  deque_algo_iterator<T, isconst> last, OIter dest)
  noexcept(noexcept(
    ::fast_io::details::algorithms::uninitialized_copy_iters_impl(
      ::std::declval<T *>(), ::std::declval<T *>(),
      ::std::declval<OIter>())))
{
  return ::fast_io::details::algorithms::deque_forward_segments(
    first, last, dest,
    ::fast_io::details::algorithms::deque_uninitialized_copy_segment<T>{});
}

template<typename T, bool isconst, typename OIter>
inline constexpr OIter uninitialized_copy_n_iters_define(
  ::fast_io::io_algorithm_t,
  deque_algo_iterator<T, isconst> first, ::std::size_t count,
  OIter dest)
  noexcept(noexcept(
    ::fast_io::details::algorithms::uninitialized_copy_n_iters_impl(
      ::std::declval<T *>(), ::std::declval<::std::size_t>(),
      ::std::declval<OIter>())))
{
  return ::fast_io::details::algorithms::deque_forward_segments(
    first, first + count, dest,
    ::fast_io::details::algorithms::deque_uninitialized_copy_segment<T>{});
}

template<typename T, bool isconst, typename OIter>
inline constexpr OIter uninitialized_copy_backward_iters_define(
  ::fast_io::io_algorithm_t,
  deque_algo_iterator<T, isconst> first,
  deque_algo_iterator<T, isconst> last, OIter d_last)
  noexcept(noexcept(
    ::fast_io::details::algorithms::uninitialized_copy_backward_iters_impl(
      ::std::declval<T *>(), ::std::declval<T *>(),
      ::std::declval<OIter>())))
{
  return ::fast_io::details::algorithms::deque_backward_segments(
    first, last, d_last,
    ::fast_io::details::algorithms::
      deque_uninitialized_copy_backward_segment<T>{});
}

template<typename T, bool isconst, typename OIter>
  requires(!isconst)
inline constexpr OIter uninitialized_move_iters_define(
  ::fast_io::io_algorithm_t,
  deque_algo_iterator<T, isconst> first,
  deque_algo_iterator<T, isconst> last, OIter dest)
  noexcept(noexcept(
    ::fast_io::details::algorithms::uninitialized_move_iters_impl(
      ::std::declval<T *>(), ::std::declval<T *>(),
      ::std::declval<OIter>())))
{
  return ::fast_io::details::algorithms::deque_forward_segments(
    first, last, dest,
    ::fast_io::details::algorithms::deque_uninitialized_move_segment<T>{});
}

template<typename T, bool isconst, typename OIter>
  requires(!isconst)
inline constexpr OIter uninitialized_move_n_iters_define(
  ::fast_io::io_algorithm_t,
  deque_algo_iterator<T, isconst> first, ::std::size_t count,
  OIter dest)
  noexcept(noexcept(
    ::fast_io::details::algorithms::uninitialized_move_n_iters_impl(
      ::std::declval<T *>(), ::std::declval<::std::size_t>(),
      ::std::declval<OIter>())))
{
  return ::fast_io::details::algorithms::deque_forward_segments(
    first, first + count, dest,
    ::fast_io::details::algorithms::deque_uninitialized_move_segment<T>{});
}

template<typename T, bool isconst, typename OIter>
  requires(!isconst)
inline constexpr OIter uninitialized_move_backward_iters_define(
  ::fast_io::io_algorithm_t,
  deque_algo_iterator<T, isconst> first,
  deque_algo_iterator<T, isconst> last, OIter d_last)
  noexcept(noexcept(
    ::fast_io::details::algorithms::uninitialized_move_backward_iters_impl(
      ::std::declval<T *>(), ::std::declval<T *>(),
      ::std::declval<OIter>())))
{
  return ::fast_io::details::algorithms::deque_backward_segments(
    first, last, d_last,
    ::fast_io::details::algorithms::
      deque_uninitialized_move_backward_segment<T>{});
}

template<typename T, bool isconst, typename OIter>
  requires(!isconst)
inline constexpr OIter uninitialized_relocate_iters_define(
  ::fast_io::io_algorithm_t,
  deque_algo_iterator<T, isconst> first,
  deque_algo_iterator<T, isconst> last, OIter dest)
  noexcept(noexcept(
    ::fast_io::details::algorithms::uninitialized_relocate_iters_impl(
      ::std::declval<T *>(), ::std::declval<T *>(),
      ::std::declval<OIter>())))
{
  return ::fast_io::details::algorithms::deque_forward_segments(
    first, last, dest,
    ::fast_io::details::algorithms::deque_uninitialized_relocate_segment<T>{});
}

template<typename T, bool isconst, typename OIter>
  requires(!isconst)
inline constexpr OIter uninitialized_relocate_n_iters_define(
  ::fast_io::io_algorithm_t,
  deque_algo_iterator<T, isconst> first, ::std::size_t count,
  OIter dest)
  noexcept(noexcept(
    ::fast_io::details::algorithms::uninitialized_relocate_n_iters_impl(
      ::std::declval<T *>(), ::std::declval<::std::size_t>(),
      ::std::declval<OIter>())))
{
  return ::fast_io::details::algorithms::deque_forward_segments(
    first, first + count, dest,
    ::fast_io::details::algorithms::deque_uninitialized_relocate_segment<T>{});
}

template<typename T, bool isconst, typename OIter>
  requires(!isconst)
inline constexpr OIter uninitialized_relocate_backward_iters_define(
  ::fast_io::io_algorithm_t,
  deque_algo_iterator<T, isconst> first,
  deque_algo_iterator<T, isconst> last, OIter d_last)
  noexcept(noexcept(
    ::fast_io::details::algorithms::
      uninitialized_relocate_backward_iters_impl(
        ::std::declval<T *>(), ::std::declval<T *>(),
        ::std::declval<OIter>())))
{
  return ::fast_io::details::algorithms::deque_backward_segments(
    first, last, d_last,
    ::fast_io::details::algorithms::
      deque_uninitialized_relocate_backward_segment<T>{});
}

template<typename T, bool isconst, typename OIter>
inline constexpr OIter reverse_copy_iters_define(
  ::fast_io::io_algorithm_t, deque_algo_iterator<T, isconst> first,
  deque_algo_iterator<T, isconst> last, OIter dest)
  noexcept(noexcept(
    ::fast_io::details::algorithms::reverse_copy_iters_impl(
      ::std::declval<T *>(), ::std::declval<T *>(),
      ::std::declval<OIter>())))
{
  return ::fast_io::details::algorithms::deque_backward_segments(
    first, last, dest,
    ::fast_io::details::algorithms::deque_reverse_copy_segment<T>{});
}

template<typename T, bool isconst>
  requires(!isconst)
inline constexpr void reverse_iters_define(
  ::fast_io::io_algorithm_t, deque_algo_iterator<T, isconst> first,
  deque_algo_iterator<T, isconst> last)
  noexcept(::fast_io::details::algorithms::move_no_throw<T>)
{
  ::fast_io::details::algorithms::deque_reverse_segments(first, last);
}

template<typename T, bool isconst>
  requires(!isconst)
inline constexpr deque_algo_iterator<T, isconst> rotate_iters_define(
  ::fast_io::io_algorithm_t, deque_algo_iterator<T, isconst> first,
  deque_algo_iterator<T, isconst> middle,
  deque_algo_iterator<T, isconst> last)
  noexcept(::fast_io::details::algorithms::move_no_throw<T>)
{
  if (first == middle)
  {
    return last;
  }
  if (middle == last)
  {
    return first;
  }
  auto const result{
    first + static_cast<::std::iter_difference_t<
      deque_algo_iterator<T, isconst>>>(last - middle)};
  ::fast_io::details::algorithms::deque_reverse_segments(first, middle);
  ::fast_io::details::algorithms::deque_reverse_segments(middle, last);
  ::fast_io::details::algorithms::deque_reverse_segments(first, last);
  return result;
}

template<typename T, bool isconst, typename OIter>
inline constexpr OIter rotate_copy_iters_define(
  ::fast_io::io_algorithm_t, deque_algo_iterator<T, isconst> first,
  deque_algo_iterator<T, isconst> middle,
  deque_algo_iterator<T, isconst> last, OIter dest)
  noexcept(noexcept(
    ::fast_io::details::algorithms::copy_iters_impl(
      ::std::declval<T *>(), ::std::declval<T *>(),
      ::std::declval<OIter>())))
{
  auto const copy{::fast_io::details::algorithms::deque_copy_segment<T>{}};
  dest = ::fast_io::details::algorithms::deque_forward_segments(
    middle, last, dest, copy);
  return ::fast_io::details::algorithms::deque_forward_segments(
    first, middle, dest, copy);
}

} // namespace fast_io::containers::details
