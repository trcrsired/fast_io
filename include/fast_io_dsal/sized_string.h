#pragma once

#if !defined(__cplusplus)
#error "You must be using a C++ compiler"
#endif

#include <version>
#include <type_traits>
#include <concepts>
#include <limits>
#include <cstdint>
#include <cstddef>
#include <new>
#include <initializer_list>
#include <bit>
#include <compare>
#include <algorithm>
#include "../fast_io_core.h"
#include "impl/misc/push_warnings.h"
#include "impl/misc/push_macros.h"
#include "impl/freestanding.h"
#include "impl/common.h"
#include "impl/string_view.h"
#include "impl/cstring_view.h"
#include "impl/sized_string.h"

#if ((__STDC_HOSTED__ == 1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED == 1) && \
	  !defined(_LIBCPP_FREESTANDING)) ||                                             \
	 defined(FAST_IO_ENABLE_HOSTED_FEATURES))


namespace fast_io
{

template <::std::unsigned_integral uinttype, ::std::integral chartype, typename allocator = ::fast_io::native_global_allocator>
using basic_sized_string = ::fast_io::containers::basic_sized_string<uinttype, chartype, allocator>;

template <::std::integral chartype, typename allocator = ::fast_io::native_global_allocator>
using basic_sized_string_zu32 = ::fast_io::containers::basic_sized_string<::fast_io::size32_t, chartype, allocator>;

using sized_string_zu32 = ::fast_io::basic_sized_string_zu32<char>;
using wsized_string_zu32 = ::fast_io::basic_sized_string_zu32<wchar_t>;
using u8sized_string_zu32 = ::fast_io::basic_sized_string_zu32<char8_t>;
using u16sized_string_zu32 = ::fast_io::basic_sized_string_zu32<char16_t>;
using u32sized_string_zu32 = ::fast_io::basic_sized_string_zu32<char32_t>;

namespace tlc
{

template <::std::unsigned_integral uinttype, ::std::integral chartype, typename allocator = ::fast_io::native_thread_local_allocator>
using basic_sized_string = ::fast_io::containers::basic_sized_string<uinttype, chartype, allocator>;

template <::std::integral chartype, typename allocator = ::fast_io::native_thread_local_allocator>
using basic_sized_string_zu32 = ::fast_io::containers::basic_sized_string<::fast_io::size32_t, chartype, allocator>;

using sized_string_zu32 = ::fast_io::tlc::basic_sized_string_zu32<char>;
using wsized_string_zu32 = ::fast_io::tlc::basic_sized_string_zu32<wchar_t>;
using u8sized_string_zu32 = ::fast_io::tlc::basic_sized_string_zu32<char8_t>;
using u16sized_string_zu32 = ::fast_io::tlc::basic_sized_string_zu32<char16_t>;
using u32sized_string_zu32 = ::fast_io::tlc::basic_sized_string_zu32<char32_t>;

} // namespace tlc

} // namespace fast_io

#endif

#include "impl/misc/pop_macros.h"
#include "impl/misc/pop_warnings.h"
