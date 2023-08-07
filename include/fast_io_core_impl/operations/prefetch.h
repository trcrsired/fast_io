namespace fast_io::details {
	template<bool write>
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr void prefetch(void const* ptr) noexcept {
#ifdef __has_builtin
#if __has_builtin(__builtin_prefetch)
#if __cpp_if_consteval >= 202106L
		if !consteval
#else
		if (!__builtin_is_constant_evaluated())
#endif
		{
			__builtin_prefetch(ptr, write, 3);
		}
#endif
#endif
	}
}
