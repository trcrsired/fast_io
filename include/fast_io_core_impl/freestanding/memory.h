#pragma once

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC system_header
#endif

// Low-level memory primitives only. The algorithm-like names that used to live
// in fast_io::freestanding (find, copy, fill, and friends) are gone: their
// single implementation is fast_io::algorithms.

namespace fast_io::freestanding
{

inline
#if FAST_IO_HAS_BUILTIN(__builtin_memcpy)
	constexpr
#endif
	void *
	my_memcpy(void *dest, void const *src, ::std::size_t count) noexcept
{
	return
#if FAST_IO_HAS_BUILTIN(__builtin_memcpy)
		__builtin_memcpy
#else
		::std::memcpy
#endif
		(dest, src, count);
}

inline
#if FAST_IO_HAS_BUILTIN(__builtin_memmove)
	constexpr
#endif
	void *
	my_memmove(void *dest, void const *src, ::std::size_t count) noexcept
{
	return
#if FAST_IO_HAS_BUILTIN(__builtin_memmove)
		__builtin_memmove
#else
		::std::memmove
#endif
		(dest, src, count);
}

inline void *my_memset(void *dest, int ch, ::std::size_t count) noexcept
{
	return
#if FAST_IO_HAS_BUILTIN(__builtin_memset)
		__builtin_memset
#else
		::std::memset
#endif
		(dest, ch, count);
}

inline
#if FAST_IO_HAS_BUILTIN(__builtin_memcmp)
	constexpr
#endif
	int
	my_memcmp(void const *dest, void const *src, ::std::size_t count) noexcept
{
	return
#if FAST_IO_HAS_BUILTIN(__builtin_memcmp)
		__builtin_memcmp
#else
		::std::memcmp
#endif
		(dest, src, count);
}

} // namespace fast_io::freestanding

namespace fast_io::details
{
using ::fast_io::freestanding::my_memcmp;
using ::fast_io::freestanding::my_memcpy;
using ::fast_io::freestanding::my_memmove;
using ::fast_io::freestanding::my_memset;
} // namespace fast_io::details
