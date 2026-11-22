#pragma once

// Safe C++ experiment: the span kernel stores an opaque pointer and a length.
// It lives in an [[unsafe]] fence so it may name pointer/reference types; the
// public API is classified explicitly, as for vector.h. Iterator accessors,
// unchecked operations and raw-pointer/byte views are call-restricted; checked
// indexing and size queries stay callable from safe source.
#if defined(__SAFECXX__)
#define FAST_IO_DSAL_UNSAFE [[unsafe]]
#define FAST_IO_DSAL_KERNEL_BEGIN [[unsafe]] {
#define FAST_IO_DSAL_KERNEL_END }
// `forward` is the d0708 (draft 32) parameter kind for an argument the callee
// passes along: it preserves the argument's cv-qualification and value
// category. Here it keeps the range's constness without a `const_cast`.
// lifetimebound tells clang's lifetime-safety analysis that the constructed
// span refers to the range; under -fsafecxx that analysis is an error.
#define FAST_IO_DSAL_FORWARD forward
#define FAST_IO_DSAL_FWD_TYPE(R) R
#define FAST_IO_DSAL_LIFETIMEBOUND [[clang::lifetimebound]]
#else
#define FAST_IO_DSAL_UNSAFE
#define FAST_IO_DSAL_KERNEL_BEGIN
#define FAST_IO_DSAL_KERNEL_END
// Plain C++ has no `forward` keyword; use a forwarding reference instead.
#define FAST_IO_DSAL_FORWARD
#define FAST_IO_DSAL_FWD_TYPE(R) R &&
#define FAST_IO_DSAL_LIFETIMEBOUND
#endif

FAST_IO_DSAL_KERNEL_BEGIN

namespace fast_io::containers
{

template <typename T, ::fast_io::allocator_adapter_flags flags = ::fast_io::allocator_adapter_flags::none>
class span
{
	static inline constexpr bool throwing_violations{
		flags == ::fast_io::allocator_adapter_flags::throws_on_violations};

public:
	using element_type = T;
	using value_type = ::std::remove_cv_t<element_type>;
	using size_type = ::std::size_t;
	using difference_type = ::std::ptrdiff_t;
	using pointer = element_type *;
	using const_pointer = element_type const *;
	using reference = element_type &;
	using const_reference = element_type const &;
	using iterator = pointer;
	using reverse_iterator = ::std::reverse_iterator<iterator>;
#ifdef __cpp_lib_ranges_as_const
#if __cpp_lib_ranges_as_const >= 202311L
	using const_iterator = ::std::basic_const_iterator<iterator>;
	using const_reverse_iterator = ::std::basic_const_iterator<reverse_iterator>;
#else
	using const_iterator = const_pointer;
	using const_reverse_iterator = ::std::reverse_iterator<const_iterator>;
#endif
#else
	using const_iterator = const_pointer;
	using const_reverse_iterator = ::std::reverse_iterator<const_iterator>;
#endif
	pointer ptr{};
	size_type n{};
	inline constexpr explicit span() noexcept = default;
	// Raw iterator construction is unsafe: an iterator carries no owner to
	// borrow from, so the lifetime analysis cannot protect the resulting span.
	template <::std::contiguous_iterator Iter>
		requires ::std::same_as<value_type, ::std::iter_value_t<Iter>>
	FAST_IO_DSAL_UNSAFE
	inline constexpr explicit span(Iter first, size_type count) noexcept(noexcept(::std::to_address(first)))
		: ptr{::std::to_address(first)}, n{count}
	{}
	template <::std::contiguous_iterator Iter, ::std::sentinel_for<Iter> S>
		requires ::std::same_as<value_type, ::std::iter_value_t<Iter>>
	FAST_IO_DSAL_UNSAFE
	inline constexpr explicit span(Iter first, S snt) noexcept(noexcept(::std::to_address(first)))
		: ptr{::std::to_address(first)}, n{static_cast<size_type>(snt - first)}
	{}
	// `forward R` is d0708's kind for an argument the callee passes along:
	// it preserves the range's constness (no const_cast needed) and value
	// category. lifetimebound ties the borrowed range to the resulting span,
	// so clang's lifetime-safety analysis rejects a span outliving its owner.
	template <::std::ranges::contiguous_range R>
		requires ::std::same_as<value_type, ::std::ranges::range_value_t<R>>
	inline constexpr explicit span(::fast_io::freestanding::from_range_t,
		FAST_IO_DSAL_FORWARD FAST_IO_DSAL_FWD_TYPE(R) range FAST_IO_DSAL_LIFETIMEBOUND)
		noexcept(noexcept(::std::ranges::data(range)) && noexcept(::std::ranges::size(range)))
		: ptr{::std::ranges::data(range)}, n{::std::ranges::size(range)}
	{}

	FAST_IO_DSAL_UNSAFE
	inline constexpr pointer data() const noexcept
	{
		return ptr;
	}
	FAST_IO_DSAL_UNSAFE
	inline constexpr iterator begin() const noexcept
	{
		return iterator(ptr);
	}
	FAST_IO_DSAL_UNSAFE
	inline constexpr const_iterator cbegin() const noexcept
	{
		return const_iterator(ptr);
	}
	FAST_IO_DSAL_UNSAFE
	inline constexpr iterator end() const noexcept
	{
		return iterator(ptr + n);
	}
	FAST_IO_DSAL_UNSAFE
	inline constexpr const_iterator cend() const noexcept
	{
		return const_iterator(ptr + n);
	}

	FAST_IO_DSAL_UNSAFE
	inline constexpr reverse_iterator rbegin() const noexcept
	{
		return reverse_iterator(ptr + n);
	}
	FAST_IO_DSAL_UNSAFE
	inline constexpr const_reverse_iterator crbegin() const noexcept
	{
		return const_reverse_iterator(ptr + n);
	}
	FAST_IO_DSAL_UNSAFE
	inline constexpr reverse_iterator rend() const noexcept
	{
		return reverse_iterator(ptr);
	}
	FAST_IO_DSAL_UNSAFE
	inline constexpr const_reverse_iterator crend() const noexcept
	{
		return const_reverse_iterator(ptr);
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr reference
	operator[](size_type idx) const FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_violations)
	{
		if (n <= idx) [[unlikely]]
		{
			::fast_io::containers::details::violation_report_on_invalid_argument<throwing_violations>();
		}
		return ptr[idx];
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	FAST_IO_DSAL_UNSAFE
	inline constexpr reference
	index_unchecked(size_type idx) const noexcept
	{
		return ptr[idx];
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr reference
	back() const FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_violations)
	{
		if (!n) [[unlikely]]
		{
			::fast_io::containers::details::violation_report_on_invalid_argument<throwing_violations>();
		}
		size_type const nm1{n - 1};
		return ptr[nm1];
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	FAST_IO_DSAL_UNSAFE
	inline constexpr reference
	back_unchecked() const noexcept
	{
		size_type const nm1{n - 1};
		return ptr[nm1];
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr reference
	front() const FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_violations)
	{
		if (!n) [[unlikely]]
		{
			::fast_io::containers::details::violation_report_on_invalid_argument<throwing_violations>();
		}
		return *ptr;
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	FAST_IO_DSAL_UNSAFE
	inline constexpr reference
	front_unchecked() const noexcept
	{
		return *ptr;
	}

	inline constexpr bool is_empty() const noexcept
	{
		return !n;
	}


	inline constexpr size_type size() const noexcept
	{
		return n;
	}

	inline constexpr size_type size_bytes() const noexcept
	{
		return n * sizeof(element_type);
	}

	inline static constexpr size_type max_size() noexcept
	{
		constexpr size_type mxsz{SIZE_MAX / sizeof(element_type)};
		return mxsz;
	}

	inline static constexpr size_type max_size_bytes() noexcept
	{
		constexpr size_type mxsz{SIZE_MAX / sizeof(element_type) * sizeof(element_type)};
		return mxsz;
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr span subspan_front(size_type count) const FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_violations)
	{
		if (this->n < count) [[unlikely]]
		{
			::fast_io::containers::details::violation_report_on_invalid_argument<throwing_violations>();
		}
		return span(this->ptr, count);
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	FAST_IO_DSAL_UNSAFE
	inline constexpr span subspan_front_unchecked(size_type count) const noexcept
	{
		return span(this->ptr, count);
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr span subspan_back(size_type count) const FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_violations)
	{
		if (this->n < count) [[unlikely]]
		{
			::fast_io::containers::details::violation_report_on_invalid_argument<throwing_violations>();
		}
		return span(this->ptr + (this->n - count), count);
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	FAST_IO_DSAL_UNSAFE
	inline constexpr span subspan_back_unchecked(size_type count) const noexcept
	{
		return span(this->ptr + (this->n - count), count);
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr span subspan(size_type pos, size_type count = ::fast_io::containers::npos) const FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_violations)
	{
		if (this->n < pos) [[unlikely]]
		{
			::fast_io::containers::details::violation_report_on_invalid_argument<throwing_violations>();
		}
		size_type const val{this->n - pos};
		if (val < count)
		{
			if (count != ::fast_io::containers::npos) [[unlikely]]
			{
				::fast_io::containers::details::violation_report_on_invalid_argument<throwing_violations>();
			}
			count = val;
		}
		return span(this->ptr + pos, count);
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	FAST_IO_DSAL_UNSAFE
	inline constexpr span subspan_unchecked(size_type pos, size_type count = ::fast_io::containers::npos) const noexcept
	{
		size_type const val{this->n - pos};
		if (count == ::fast_io::containers::npos)
		{
			count = val;
		}
		return span(this->ptr + pos, count);
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr size_type copy(element_type *dest, size_type count, size_type pos = 0) const FAST_IO_HERBCEPTIONS_THROWS_IF(throwing_violations)
	{
		size_type const thisn{this->n};
		if (thisn < pos) [[unlikely]]
		{
			::fast_io::containers::details::violation_report_on_invalid_argument<throwing_violations>();
		}
		return this->copy_unchecked(dest, count, pos);
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	FAST_IO_DSAL_UNSAFE
	inline constexpr size_type copy_unchecked(element_type *dest, size_type count, size_type pos = 0) const noexcept
	{
		size_type const thisn{this->n};
		size_type const val{thisn - pos};
		if (val < count)
		{
			count = val;
		}
		auto start{this->ptr + pos};
		::fast_io::algorithms::copy_iters(start, start + count, dest);
		return count;
	}
};

template <::std::contiguous_iterator Iter>
span(Iter, ::std::size_t) -> span<::std::remove_reference_t<::std::iter_reference_t<Iter>>>;

template <::std::contiguous_iterator Iter, ::std::sentinel_for<Iter> S>
span(Iter, S) -> span<::std::remove_reference_t<::std::iter_reference_t<Iter>>>;

template <typename T, ::fast_io::allocator_adapter_flags flags>
FAST_IO_DSAL_UNSAFE
inline constexpr ::fast_io::containers::span<::std::byte const, flags> as_bytes(::fast_io::containers::span<T, flags> sp) noexcept
{
	return ::fast_io::containers::span<::std::byte const, flags>(static_cast<::std::byte const *>(static_cast<void const *>(sp.ptr)), sp.n * sizeof(T));
}

template <typename T, ::fast_io::allocator_adapter_flags flags>
	requires(!::std::is_const_v<T>)
FAST_IO_DSAL_UNSAFE
inline constexpr ::fast_io::containers::span<::std::byte, flags> as_writable_bytes(::fast_io::containers::span<T, flags> sp) noexcept
{
	return ::fast_io::containers::span<::std::byte, flags>(static_cast<::std::byte *>(static_cast<void *>(sp.ptr)), sp.n * sizeof(T));
}

template <::std::ranges::contiguous_range R>
span(::fast_io::freestanding::from_range_t, R &&r) -> span<::std::remove_reference_t<::std::ranges::range_reference_t<R>>>;

template <typename T, ::fast_io::allocator_adapter_flags f1, ::fast_io::allocator_adapter_flags f2>
	requires(::std::equality_comparable<T>)
inline constexpr bool operator==(::fast_io::containers::span<T, f1> a, ::fast_io::containers::span<T, f2> b)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!noexcept(*a.ptr == *b.ptr))
{
	return ::fast_io::algorithms::equal_iters(a.ptr, a.ptr + a.n, b.ptr, b.ptr + b.n);
}

#if __cpp_impl_three_way_comparison >= 201907L
template <typename T, ::fast_io::allocator_adapter_flags f1, ::fast_io::allocator_adapter_flags f2>
	requires(::std::three_way_comparable<T>)
inline constexpr auto operator<=>(::fast_io::containers::span<T, f1> a, ::fast_io::containers::span<T, f2> b)
{
	return ::fast_io::algorithms::lexicographical_compare_three_way_iters(a.ptr, a.ptr + a.n, b.ptr, b.ptr + b.n, ::fast_io::algorithms::compare_three_way{});
}
#endif

} // namespace fast_io::containers

FAST_IO_DSAL_KERNEL_END

#undef FAST_IO_DSAL_UNSAFE
#undef FAST_IO_DSAL_KERNEL_BEGIN
#undef FAST_IO_DSAL_KERNEL_END
#undef FAST_IO_DSAL_FORWARD
#undef FAST_IO_DSAL_FWD_TYPE
#undef FAST_IO_DSAL_LIFETIMEBOUND
