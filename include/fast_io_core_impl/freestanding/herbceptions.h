#pragma once

namespace fast_io::freestanding
{
#ifdef __HERBCEPTIONS__

#if defined(_WIN32) || defined(__CYGWIN__)
using ::std::win32_errc;
using ::std::nt_errc;
using ::std::wine_errc;
using ::std::com_errc;
#endif

using ::std::cmath_errc;
using ::std::parse_errc;

#else
#if defined(_WIN32) || defined(__CYGWIN__)
//placeholders
enum class win32_errc : ::std::uint_least32_t{};
enum class nt_errc : ::std::uint_least32_t{};
enum class wine_errc : ::std::uint_least32_t{};
enum class com_errc : ::std::uint_least32_t{};
#endif

enum class cmath_errc : ::std::uint_least32_t{};
enum class parse_errc : ::std::uint_least32_t
{
ok = 0,
end_of_file = 1,
partial = 2,
invalid = 3,
overflow = 4
};

#endif
}

