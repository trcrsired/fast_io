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
#include "impl/misc/push_macros.h"
#include "impl/misc/push_warnings.h"
#include "../fast_io_core_impl/freestanding/impl.h"
#include "../fast_io_core_impl/terminate.h"
#include "../fast_io_core_impl/intrinsics/msvc/impl.h"
#include "../fast_io_core_impl/allocation/impl.h"

#include "impl/freestanding.h"
#include "impl/common.h"
#include "string_view.h"
#include "impl/associative_string.h"
#include "../fast_io_crypto/hash/rapidhash.h"
#include "impl/swiss_table_common.h"
#include "impl/str_swiss_map.h"

#if ((__STDC_HOSTED__ == 1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED == 1) && \
	  !defined(_LIBCPP_FREESTANDING)) ||                                             \
	 defined(FAST_IO_ENABLE_HOSTED_FEATURES))

namespace fast_io
{

template <::std::integral chtype, ::std::movable mappedtype, typename Hash = ::fast_io::native_swiss_hasher, typename Alloc = ::fast_io::native_global_allocator>
using basic_str_swiss_map = ::fast_io::containers::basic_str_swiss_map<chtype, mappedtype, Hash, Alloc>;

template <::std::movable mappedtype>
using str_swiss_map = ::fast_io::basic_str_swiss_map<char, mappedtype>;
#if 1
template <::std::movable mappedtype>
using wstr_swiss_map = ::fast_io::basic_str_swiss_map<wchar_t, mappedtype>;
template <::std::movable mappedtype>
using u8str_swiss_map = ::fast_io::basic_str_swiss_map<char8_t, mappedtype>;
template <::std::movable mappedtype>
using u16str_swiss_map = ::fast_io::basic_str_swiss_map<char16_t, mappedtype>;
template <::std::movable mappedtype>
using u32str_swiss_map = ::fast_io::basic_str_swiss_map<char32_t, mappedtype>;

namespace tlc
{
template <::std::integral chtype, ::std::movable mappedtype, typename Hash = ::fast_io::native_swiss_hasher, typename Alloc = ::fast_io::native_thread_local_allocator>
using basic_str_swiss_map = ::fast_io::containers::basic_str_swiss_map<chtype, mappedtype, Hash, Alloc>;

template <::std::movable mappedtype>
using str_swiss_map = ::fast_io::basic_str_swiss_map<char, mappedtype>;
template <::std::movable mappedtype>
using wstr_swiss_map = ::fast_io::basic_str_swiss_map<wchar_t, mappedtype>;
template <::std::movable mappedtype>
using u8str_swiss_map = ::fast_io::basic_str_swiss_map<char8_t, mappedtype>;
template <::std::movable mappedtype>
using u16str_swiss_map = ::fast_io::basic_str_swiss_map<char16_t, mappedtype>;
template <::std::movable mappedtype>
using u32str_swiss_map = ::fast_io::basic_str_swiss_map<char32_t, mappedtype>;
} // namespace tlc
#endif
#endif
} // namespace fast_io

#include "impl/misc/pop_macros.h"
#include "impl/misc/pop_warnings.h"
