namespace fast_io::details {
	template<[[maybe_unused]]int rw>
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
#if __has_cpp_attribute(__gnu__::__artificial__)
	[[__gnu__::__artificial__]]
#endif
	inline constexpr void prefetch(void const* ptr) noexcept {
#if __cpp_if_consteval >= 202106L
		if !consteval
#else
		if (!__builtin_is_constant_evaluated())
#endif
		{
#ifdef __has_builtin
#if __has_builtin(__builtin_prefetch)
			__builtin_prefetch(ptr, rw, 3);
#else
			_mm_prefetch(ptr, 0);
#endif
#else
			_mm_prefetch(ptr, 0);
#endif
		}

	}
}
