#pragma once

namespace fast_io 
{

namespace details 
{
template<int write = 0, int level = 3>
#if __has_cpp_attribute(__gnu__::__always_inline__)
[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
[[msvc::forceinline]]
#endif
#if __has_cpp_attribute(__gnu__::__artificial__)
[[__gnu__::__artificial__]]
#endif
inline constexpr void prefetch(void const* const ptr) noexcept {
#if __cpp_if_consteval >= 202106L
	if consteval
#else
	if (__builtin_is_constant_evaluated())
#endif
	{
#if defined(__has_builtin)
#if __has_builtin(__builtin_prefetch)
		__builtin_prefetch(ptr, write, level);
#else
		__mm_prefetch(ptr, level);
#endif
#else
		__mm_prefetch(ptr, level);
#endif
	}
}

}
}
