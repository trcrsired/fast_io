#pragma once
#undef min
#undef max

#if !defined(__cplusplus)
#error "You must be using a C++ compiler"
#endif

#include <algorithm>
#include <limits>
#include <memory>

#include "../fast_io_core_impl/freestanding/impl.h"
#include "../fast_io_core_impl/terminate.h"
#include "../fast_io_core_impl/allocation/impl.h"

#include "impl/deque.h"

#if ((__STDC_HOSTED__ == 1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED == 1) && \
!defined(_LIBCPP_FREESTANDING)) ||                                             \
defined(FAST_IO_ENABLE_HOSTED_FEATURES))

namespace fast_io {
    template<typename T, typename Alloc = native_global_allocator>
    using deque = containers::deque<T, Alloc>;

    namespace containers {
        template<::std::input_iterator U, typename V>
        deque(U, V) -> deque<typename ::std::iterator_traits<U>::value_type,
            generic_allocator_adapter<typename ::std::iterator_traits<U>::value_type> >;

        template<::std::input_iterator U, typename V,
            details::mini_alloc Alloc = generic_allocator_adapter<typename
                ::std::iterator_traits<
                    U>::value_type> >
        deque(U, V, Alloc) -> deque<typename ::std::iterator_traits<U>::value_type, Alloc>;

#ifdef __cpp_lib_containers_ranges
        template<::std::ranges::input_range R>
        deque(::std::from_range_t, R &&)
            -> deque<::std::ranges::range_value_t<R>, ::fast_io::generic_allocator_adapter<::std::ranges::range_value_t<
                R> > >;

        template<::std::ranges::input_range R,
            ::fast_io::containers::details::mini_alloc Alloc = ::fast_io::generic_allocator_adapter<
                ::std::ranges::range_value_t<R> > >
        deque(::std::from_range_t, R &&, Alloc) -> deque<::std::ranges::range_value_t<R>, Alloc>;
#endif
        template<typename T, typename Alloc, typename U = T>
        inline constexpr ::std::size_t erase(deque<T, Alloc> &c, const U &value) {
            auto const it = ::std::remove(c.begin(), c.end(), value);
            auto const r = c.end() - it;
            c.resize(c.size() - r);
            return r;
        }

        template<typename T, typename Alloc, typename Pred>
        inline constexpr ::std::size_t erase_if(deque<T, Alloc> &c, Pred pred) {
            auto const it = ::std::remove_if(c.begin(), c.end(), pred);
            auto const r = c.end() - it;
            c.resize(c.size() - r);
            return r;
        }
    }
}

#endif
