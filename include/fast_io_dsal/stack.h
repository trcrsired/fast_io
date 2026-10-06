#pragma once
#include "impl/misc/push_macros.h"
#include "impl/misc/push_warnings.h"
#include "deque.h"
#include "impl/stack.h"
#include "impl/misc/pop_warnings.h"
#include "impl/misc/pop_macros.h"

#if ((__STDC_HOSTED__ == 1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED == 1) && \
	  !defined(_LIBCPP_FREESTANDING)) ||                                             \
	 defined(FAST_IO_ENABLE_HOSTED_FEATURES))

namespace fast_io
{

template <typename T, typename Container = ::fast_io::deque<T>>
using stack = ::fast_io::containers::stack<Container>;

namespace tlc
{
template <typename T, typename Container = ::fast_io::tlc::deque<T>>
using stack = ::fast_io::containers::stack<Container>;
}

} // namespace fast_io
#endif