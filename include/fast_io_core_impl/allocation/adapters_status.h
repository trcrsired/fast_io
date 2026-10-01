#pragma once

#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
static inline void *handle_allocate(handle_type handle, ::std::size_t n) noexcept
	requires(has_status && has_native_handle_allocate)
{
#if __cpp_if_consteval >= 202106L
	if consteval
#elif __cpp_lib_is_constant_evaluated >= 201811L && __cpp_constexpr_dynamic_alloc >= 201907L
	if (__builtin_is_constant_evaluated())
#else
	if (false)
#endif
	{
		return static_cast<void *>(::fast_io::freestanding::allocator<::std::byte>{}.allocate(n));
	}
	else
	{
		if constexpr (::fast_io::details::has_handle_allocate_impl<alloc>)
		{
			return allocator_type::handle_allocate(handle, n);
		}
		else if constexpr (::fast_io::details::has_handle_allocate_at_least_impl<alloc>)
		{
			return allocator_type::handle_allocate_at_least(handle, n).ptr;
		}
		else
		{
			return generic_allocator_adapter::handle_allocate_conditional_zero(handle, n, false);
		}
	}
}

#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
static inline void *handle_allocate_zero(handle_type handle, ::std::size_t n) noexcept
	requires(has_status && has_native_handle_allocate)
{
	if constexpr (::fast_io::details::has_handle_allocate_zero_impl<alloc>)
	{
		return allocator_type::handle_allocate_zero(handle, n);
	}
	else if constexpr (::fast_io::details::has_handle_allocate_zero_at_least_impl<alloc>)
	{
		return allocator_type::handle_allocate_zero_at_least(handle, n).ptr;
	}
	else
	{
		return generic_allocator_adapter::handle_allocate_conditional_zero(handle, n, true);
	}
}

static inline constexpr bool has_handle_reallocate{
	has_status && (::fast_io::details::has_handle_reallocate_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_aligned_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_zero_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_aligned_zero_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_at_least_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_aligned_at_least_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_zero_at_least_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_aligned_zero_at_least_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_conditional_zero_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_aligned_conditional_zero_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_conditional_zero_at_least_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_aligned_conditional_zero_at_least_impl<alloc>)};

#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
static inline void *handle_reallocate_conditional_zero(handle_type handle, void *p, ::std::size_t n, bool zero) noexcept
	requires(has_status && has_handle_reallocate)
{
	if constexpr (::fast_io::details::has_handle_reallocate_conditional_zero_impl<alloc>)
	{
		return allocator_type::handle_reallocate_conditional_zero(handle, p, n, zero);
	}
	else if constexpr (::fast_io::details::has_handle_reallocate_conditional_zero_at_least_impl<alloc>)
	{
		return allocator_type::handle_reallocate_conditional_zero_at_least(handle, p, n, zero).ptr;
	}
	else if constexpr (::fast_io::details::has_handle_reallocate_aligned_conditional_zero_impl<alloc>)
	{
		return allocator_type::handle_reallocate_aligned_conditional_zero(handle, p, default_alignment, n, zero);
	}
	else if constexpr (::fast_io::details::has_handle_reallocate_aligned_conditional_zero_at_least_impl<alloc>)
	{
		return allocator_type::handle_reallocate_aligned_conditional_zero_at_least(handle, p, default_alignment, n, zero).ptr;
	}
	else if constexpr (::fast_io::details::has_handle_reallocate_impl<alloc> ||
					   ::fast_io::details::has_handle_reallocate_at_least_impl<alloc> ||
					   ::fast_io::details::has_handle_reallocate_aligned_impl<alloc> ||
					   ::fast_io::details::has_handle_reallocate_aligned_at_least_impl<alloc>)
	{
		if (zero)
		{
			if constexpr (::fast_io::details::has_handle_reallocate_zero_impl<alloc>)
			{
				return allocator_type::handle_reallocate_zero(handle, p, n);
			}
			else if constexpr (::fast_io::details::has_handle_reallocate_zero_at_least_impl<alloc>)
			{
				return allocator_type::handle_reallocate_zero_at_least(handle, p, n).ptr;
			}
			else if constexpr (::fast_io::details::has_handle_reallocate_aligned_zero_impl<alloc>)
			{
				return allocator_type::handle_reallocate_aligned_zero(handle, p, default_alignment, n);
			}
			else if constexpr (::fast_io::details::has_handle_reallocate_aligned_zero_at_least_impl<alloc>)
			{
				return allocator_type::handle_reallocate_aligned_zero_at_least(handle, p, default_alignment, n).ptr;
			}
			else
			{
				::fast_io::fast_terminate();
			}
		}
		else
		{
			if constexpr (::fast_io::details::has_handle_reallocate_impl<alloc>)
			{
				return allocator_type::handle_reallocate(handle, p, n);
			}
			else if constexpr (::fast_io::details::has_handle_reallocate_at_least_impl<alloc>)
			{
				return allocator_type::handle_reallocate_at_least(handle, p, n).ptr;
			}
			else if constexpr (::fast_io::details::has_handle_reallocate_aligned_impl<alloc>)
			{
				return allocator_type::handle_reallocate_aligned(handle, p, default_alignment, n);
			}
			else
			{
				return allocator_type::handle_reallocate_aligned_at_least(handle, p, default_alignment, n).ptr;
			}
		}
	}
	else
	{
		if constexpr (::fast_io::details::has_handle_reallocate_zero_impl<alloc>)
		{
			return allocator_type::handle_reallocate_zero(handle, p, n);
		}
		else if constexpr (::fast_io::details::has_handle_reallocate_zero_at_least_impl<alloc>)
		{
			return allocator_type::handle_reallocate_zero_at_least(handle, p, n).ptr;
		}
		else if constexpr (::fast_io::details::has_handle_reallocate_aligned_zero_impl<alloc>)
		{
			return allocator_type::handle_reallocate_aligned_zero(handle, p, default_alignment, n);
		}
		else if constexpr (::fast_io::details::has_handle_reallocate_aligned_zero_at_least_impl<alloc>)
		{
			return allocator_type::handle_reallocate_aligned_zero_at_least(handle, p, default_alignment, n).ptr;
		}
		else
		{
			::fast_io::fast_terminate();
		}
	}
}

#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
static inline void *handle_reallocate(handle_type handle, void *p, ::std::size_t n) noexcept
	requires(has_status && has_handle_reallocate)
{
	if constexpr (::fast_io::details::has_handle_reallocate_impl<alloc>)
	{
		return allocator_type::handle_reallocate(handle, p, n);
	}
	else if constexpr (::fast_io::details::has_handle_reallocate_at_least_impl<alloc>)
	{
		return allocator_type::handle_reallocate_at_least(handle, p, n).ptr;
	}
	else
	{
		return generic_allocator_adapter::handle_reallocate_conditional_zero(handle, p, n, false);
	}
}

#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
static inline void *handle_reallocate_zero(handle_type handle, void *p, ::std::size_t n) noexcept
	requires(has_status && has_handle_reallocate)
{
	if constexpr (::fast_io::details::has_handle_reallocate_zero_impl<alloc>)
	{
		return allocator_type::handle_reallocate_zero(handle, p, n);
	}
	else if constexpr (::fast_io::details::has_handle_reallocate_zero_at_least_impl<alloc>)
	{
		return allocator_type::handle_reallocate_zero_at_least(handle, p, n).ptr;
	}
	else
	{
		return generic_allocator_adapter::handle_reallocate_conditional_zero(handle, p, n, true);
	}
}

static inline constexpr bool has_handle_reallocate_n{
	has_status && (::fast_io::details::has_handle_reallocate_n_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_aligned_n_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_zero_n_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_aligned_zero_n_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_n_at_least_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_aligned_n_at_least_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_zero_n_at_least_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_aligned_zero_n_at_least_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_n_conditional_zero_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_aligned_n_conditional_zero_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_n_conditional_zero_at_least_impl<alloc> ||
				   ::fast_io::details::has_handle_reallocate_aligned_n_conditional_zero_at_least_impl<alloc>)};

#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
static inline void *handle_reallocate_n_conditional_zero(handle_type handle, void *p, ::std::size_t oldn, ::std::size_t n, bool zero) noexcept
	requires(has_status && has_handle_reallocate_n)
{
	if (p != nullptr && oldn == n)
	{
		return p;
	}
	if constexpr (::fast_io::details::has_handle_reallocate_n_conditional_zero_impl<alloc>)
	{
		return allocator_type::handle_reallocate_n_conditional_zero(handle, p, oldn, n, zero);
	}
	else if constexpr (::fast_io::details::has_handle_reallocate_n_conditional_zero_at_least_impl<alloc>)
	{
		return allocator_type::handle_reallocate_n_conditional_zero_at_least(handle, p, oldn, n, zero).ptr;
	}
	else if constexpr (::fast_io::details::has_handle_reallocate_aligned_n_conditional_zero_impl<alloc>)
	{
		return allocator_type::handle_reallocate_aligned_n_conditional_zero(handle, p, oldn, default_alignment, n, zero);
	}
	else if constexpr (::fast_io::details::has_handle_reallocate_aligned_n_conditional_zero_at_least_impl<alloc>)
	{
		return allocator_type::handle_reallocate_aligned_n_conditional_zero_at_least(handle, p, oldn, default_alignment, n, zero).ptr;
	}
	else if constexpr (::fast_io::details::has_handle_reallocate_n_impl<alloc> ||
					   ::fast_io::details::has_handle_reallocate_n_at_least_impl<alloc> ||
					   ::fast_io::details::has_handle_reallocate_aligned_n_impl<alloc> ||
					   ::fast_io::details::has_handle_reallocate_aligned_n_at_least_impl<alloc>)
	{
		if (zero)
		{
			if constexpr (::fast_io::details::has_handle_reallocate_zero_n_impl<alloc>)
			{
				return allocator_type::handle_reallocate_zero_n(handle, p, oldn, n);
			}
			else if constexpr (::fast_io::details::has_handle_reallocate_zero_n_at_least_impl<alloc>)
			{
				return allocator_type::handle_reallocate_zero_n_at_least(handle, p, oldn, n).ptr;
			}
			else if constexpr (::fast_io::details::has_handle_reallocate_aligned_zero_n_impl<alloc>)
			{
				return allocator_type::handle_reallocate_aligned_zero_n(handle, p, oldn, default_alignment, n);
			}
			else if constexpr (::fast_io::details::has_handle_reallocate_aligned_zero_n_at_least_impl<alloc>)
			{
				return allocator_type::handle_reallocate_aligned_zero_n_at_least(handle, p, oldn, default_alignment, n).ptr;
			}
			else
			{
				auto newptr{generic_allocator_adapter::handle_reallocate_n(handle, p, oldn, n)};
				if (oldn < n)
				{
					::fast_io::freestanding::bytes_clear_n(reinterpret_cast<::std::byte *>(newptr) + oldn, n - oldn);
				}
				return newptr;
			}
		}
		else
		{
			if constexpr (::fast_io::details::has_handle_reallocate_n_impl<alloc>)
			{
				return allocator_type::handle_reallocate_n(handle, p, oldn, n);
			}
			else if constexpr (::fast_io::details::has_handle_reallocate_n_at_least_impl<alloc>)
			{
				return allocator_type::handle_reallocate_n_at_least(handle, p, oldn, n).ptr;
			}
			else if constexpr (::fast_io::details::has_handle_reallocate_aligned_n_impl<alloc>)
			{
				return allocator_type::handle_reallocate_aligned_n(handle, p, oldn, default_alignment, n);
			}
			else
			{
				return allocator_type::handle_reallocate_aligned_n_at_least(handle, p, oldn, default_alignment, n).ptr;
			}
		}
	}
	else
	{
		if constexpr (::fast_io::details::has_handle_reallocate_zero_n_impl<alloc>)
		{
			return allocator_type::handle_reallocate_zero_n(handle, p, oldn, n);
		}
		else if constexpr (::fast_io::details::has_handle_reallocate_zero_n_at_least_impl<alloc>)
		{
			return allocator_type::handle_reallocate_zero_n_at_least(handle, p, oldn, n).ptr;
		}
		else if constexpr (::fast_io::details::has_handle_reallocate_aligned_zero_n_impl<alloc>)
		{
			return allocator_type::handle_reallocate_aligned_zero_n(handle, p, oldn, default_alignment, n);
		}
		else if constexpr (::fast_io::details::has_handle_reallocate_aligned_zero_n_at_least_impl<alloc>)
		{
			return allocator_type::handle_reallocate_aligned_zero_n_at_least(handle, p, oldn, default_alignment, n).ptr;
		}
		else
		{
			auto newptr{generic_allocator_adapter::handle_allocate(handle, n)};
			if (p != nullptr)
			{
				if (n)
				{
					::std::size_t copyn{oldn < n ? oldn : n};
					::fast_io::freestanding::nonoverlapped_bytes_copy_n(reinterpret_cast<::std::byte const *>(p), copyn, reinterpret_cast<::std::byte *>(newptr));
				}
				generic_allocator_adapter::handle_deallocate_n(handle, p, oldn);
			}
			if (zero && oldn < n)
			{
				::fast_io::freestanding::bytes_clear_n(reinterpret_cast<::std::byte *>(newptr) + oldn, n - oldn);
			}
			return newptr;
		}
	}
}

#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
static inline void *handle_reallocate_n(handle_type handle, void *p, ::std::size_t oldn, ::std::size_t n) noexcept
	requires(has_status && has_handle_reallocate_n)
{
	if (p != nullptr && oldn == n)
	{
		return p;
	}
	if constexpr (::fast_io::details::has_handle_reallocate_n_impl<alloc>)
	{
		return allocator_type::handle_reallocate_n(handle, p, oldn, n);
	}
	else if constexpr (::fast_io::details::has_handle_reallocate_n_at_least_impl<alloc>)
	{
		return allocator_type::handle_reallocate_n_at_least(handle, p, oldn, n).ptr;
	}
	else
	{
		return generic_allocator_adapter::handle_reallocate_n_conditional_zero(handle, p, oldn, n, false);
	}
}

#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
static inline void *handle_reallocate_zero_n(handle_type handle, void *p, ::std::size_t oldn, ::std::size_t n) noexcept
	requires(has_status && has_handle_reallocate_n)
{
	if (p != nullptr && oldn == n)
	{
		return p;
	}
	if constexpr (::fast_io::details::has_handle_reallocate_zero_n_impl<alloc>)
	{
		return allocator_type::handle_reallocate_zero_n(handle, p, oldn, n);
	}
	else if constexpr (::fast_io::details::has_handle_reallocate_zero_n_at_least_impl<alloc>)
	{
		return allocator_type::handle_reallocate_zero_n_at_least(handle, p, oldn, n).ptr;
	}
	else
	{
		return generic_allocator_adapter::handle_reallocate_n_conditional_zero(handle, p, oldn, n, true);
	}
}

static inline constexpr bool has_handle_deallocate{
	has_status && (::fast_io::details::has_handle_deallocate_impl<alloc> ||
				   ::fast_io::details::has_handle_deallocate_aligned_impl<alloc> ||
				   ::fast_io::details::has_handle_deallocate_n_impl<alloc> ||
				   ::fast_io::details::has_handle_deallocate_aligned_n_impl<alloc>)};

static inline void handle_deallocate(handle_type handle, void *p) noexcept
	requires(has_status && has_handle_deallocate)
{
	if constexpr (::fast_io::details::has_handle_deallocate_impl<alloc>)
	{
		allocator_type::handle_deallocate(handle, p);
	}
	else if constexpr (::fast_io::details::has_handle_deallocate_aligned_impl<alloc>)
	{
		allocator_type::handle_deallocate_aligned(handle, p, default_alignment);
	}
	else if constexpr (::fast_io::details::has_handle_deallocate_n_impl<alloc>)
	{
		allocator_type::handle_deallocate_n(handle, p, 0);
	}
	else
	{
		allocator_type::handle_deallocate_aligned_n(handle, p, default_alignment, 0);
	}
}

static inline void handle_deallocate_n(handle_type handle, void *p, ::std::size_t n) noexcept
	requires(has_status && has_handle_deallocate)
{
	if constexpr (::fast_io::details::has_handle_deallocate_n_impl<alloc>)
	{
		allocator_type::handle_deallocate_n(handle, p, n);
	}
	else if constexpr (::fast_io::details::has_handle_deallocate_aligned_n_impl<alloc>)
	{
		allocator_type::handle_deallocate_aligned_n(handle, p, default_alignment, n);
	}
	else if constexpr (::fast_io::details::has_handle_deallocate_impl<alloc>)
	{
		allocator_type::handle_deallocate(handle, p);
	}
	else
	{
		allocator_type::handle_deallocate_aligned(handle, p, default_alignment);
	}
}
}
;

template <typename alloc, typename T>
class typed_generic_allocator_adapter
{
public:
	using allocator_adaptor = alloc;
	static inline constexpr bool has_status{allocator_adaptor::has_status};
	using handle_type = typename allocator_adaptor::handle_type;
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
	[[__gnu__::__returns_nonnull__]]
#endif
	static inline
#if __cpp_constexpr_dynamic_alloc >= 201907L
	constexpr
#endif
	T *
	allocate(::std::size_t n) noexcept
		requires(!has_status)
	{
#if __cpp_constexpr_dynamic_alloc >= 201907L
		if (__builtin_is_constant_evaluated())
		{
			return ::fast_io::freestanding::allocator<T>{}.allocate(n);
		}
#endif
		constexpr ::std::size_t mxn{::std::numeric_limits<::std::size_t>::max() / sizeof(T)};
		if (n > mxn)
		{
			::fast_io::fast_terminate();
		}
		if constexpr (alignof(T) <= alloc::default_alignment)
		{
			return static_cast<T *>(alloc::allocate(n * sizeof(T)));
		}
		else
		{
			return static_cast<T *>(alloc::allocate_aligned(alignof(T), n * sizeof(T)));
		}
	}

	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && \
	__cpp_constexpr_dynamic_alloc >= 201907L
		constexpr
#endif

		basic_allocation_least_result<T *>
		allocate_at_least(::std::size_t n) noexcept
		requires(!has_status)
	{
#if __cpp_constexpr_dynamic_alloc >= 201907L
		if (__builtin_is_constant_evaluated())
		{
			return {::fast_io::freestanding::allocator<T>{}.allocate(n), n};
		}
#endif
		constexpr ::std::size_t mxn{::std::numeric_limits<::std::size_t>::max() / sizeof(T)};
		if (n > mxn)
		{
			::fast_io::fast_terminate();
		}
		if constexpr (alignof(T) <= alloc::default_alignment)
		{
			auto newres{alloc::allocate_at_least(n * sizeof(T))};
			return {reinterpret_cast<T *>(newres.ptr), newres.count / sizeof(T)};
		}
		else
		{
			auto newres{alloc::allocate_aligned_at_least(alignof(T), n * sizeof(T))};
			return {reinterpret_cast<T *>(newres.ptr), newres.count / sizeof(T)};
		}
	}

#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
	[[__gnu__::__returns_nonnull__]]
#endif
	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && \
	__cpp_constexpr_dynamic_alloc >= 201907L
	constexpr
#endif

	T *
	allocate_zero(::std::size_t n) noexcept
		requires(!has_status)
	{
		constexpr ::std::size_t mxn{::std::numeric_limits<::std::size_t>::max() / sizeof(T)};
		if (n > mxn)
		{
			::fast_io::fast_terminate();
		}
		if constexpr (alignof(T) <= alloc::default_alignment)
		{
			return static_cast<T *>(alloc::allocate_zero(n * sizeof(T)));
		}
		else
		{
			return static_cast<T *>(alloc::allocate_aligned_zero(alignof(T), n * sizeof(T)));
		}
	}

	static inline
#if __cpp_constexpr_dynamic_alloc >= 201907L
		constexpr
#endif
		basic_allocation_least_result<T *>
		allocate_zero_at_least(::std::size_t n) noexcept
		requires(!has_status)
	{
#if __cpp_constexpr_dynamic_alloc >= 201907L
		if (__builtin_is_constant_evaluated())
		{
			return {::fast_io::freestanding::allocator<T>{}.allocate(n), n};
		}
#endif
		constexpr ::std::size_t mxn{::std::numeric_limits<::std::size_t>::max() / sizeof(T)};
		if (n > mxn)
		{
			::fast_io::fast_terminate();
		}
		if constexpr (alignof(T) <= alloc::default_alignment)
		{
			auto newres{alloc::allocate_zero_at_least(n * sizeof(T))};
			return {reinterpret_cast<T *>(newres.ptr), newres.count / sizeof(T)};
		}
		else
		{
			auto newres{alloc::allocate_aligned_zero_at_least(alignof(T), n * sizeof(T))};
			return {reinterpret_cast<T *>(newres.ptr), newres.count / sizeof(T)};
		}
	}

	static inline constexpr bool has_reallocate = allocator_adaptor::has_reallocate;

#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
	[[__gnu__::__returns_nonnull__]]
#endif
	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && \
	__cpp_constexpr_dynamic_alloc >= 201907L
	constexpr
#endif
	T *
	reallocate(T *ptr, ::std::size_t n) noexcept
		requires(!has_status && has_reallocate)
	{
		constexpr ::std::size_t mxn{::std::numeric_limits<::std::size_t>::max() / sizeof(T)};
		if (n > mxn)
		{
			::fast_io::fast_terminate();
		}
		if constexpr (alignof(T) <= alloc::default_alignment)
		{
			return static_cast<T *>(alloc::reallocate(ptr, n * sizeof(T)));
		}
		else
		{
			return static_cast<T *>(alloc::reallocate_aligned(ptr, alignof(T), n * sizeof(T)));
		}
	}

	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && \
	__cpp_constexpr_dynamic_alloc >= 201907L
		constexpr
#endif
		basic_allocation_least_result<T *>
		reallocate_at_least(T *ptr, ::std::size_t n) noexcept
		requires(!has_status && has_reallocate)
	{
		constexpr ::std::size_t mxn{::std::numeric_limits<::std::size_t>::max() / sizeof(T)};
		if (n > mxn)
		{
			::fast_io::fast_terminate();
		}
		if constexpr (alignof(T) <= alloc::default_alignment)
		{
			auto newres{alloc::reallocate_at_least(ptr, n * sizeof(T))};
			return {reinterpret_cast<T *>(newres.ptr), newres.count / sizeof(T)};
		}
		else
		{
			auto newres{alloc::reallocate_aligned_at_least(ptr, alignof(T), n * sizeof(T))};
			return {reinterpret_cast<T *>(newres.ptr), newres.count / sizeof(T)};
		}
	}

	static inline constexpr bool has_reallocate_zero = allocator_adaptor::has_reallocate_zero;
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
	[[__gnu__::__returns_nonnull__]]
#endif
	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && \
	__cpp_constexpr_dynamic_alloc >= 201907L
	constexpr
#endif
	T *
	reallocate_zero(T *ptr, ::std::size_t n) noexcept
		requires(!has_status && has_reallocate_zero)
	{
		constexpr ::std::size_t mxn{::std::numeric_limits<::std::size_t>::max() / sizeof(T)};
		if (n > mxn)
		{
			::fast_io::fast_terminate();
		}
		if constexpr (alignof(T) <= alloc::default_alignment)
		{
			return static_cast<T *>(alloc::reallocate_zero(ptr, n * sizeof(T)));
		}
		else
		{
			return static_cast<T *>(alloc::reallocate_aligned_zero(ptr, alignof(T), n * sizeof(T)));
		}
	}

	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && \
	__cpp_constexpr_dynamic_alloc >= 201907L
		constexpr
#endif
		basic_allocation_least_result<T *>
		reallocate_zero_at_least(T *ptr, ::std::size_t n) noexcept
		requires(!has_status && has_reallocate_zero)
	{
		constexpr ::std::size_t mxn{::std::numeric_limits<::std::size_t>::max() / sizeof(T)};
		if (n > mxn)
		{
			::fast_io::fast_terminate();
		}
		if constexpr (alignof(T) <= alloc::default_alignment)
		{
			auto newres{alloc::reallocate_zero_at_least(ptr, n * sizeof(T))};
			return {reinterpret_cast<T *>(newres.ptr), newres.count / sizeof(T)};
		}
		else
		{
			auto newres{alloc::reallocate_aligned_zero_at_least(ptr, alignof(T), n * sizeof(T))};
			return {reinterpret_cast<T *>(newres.ptr), newres.count / sizeof(T)};
		}
	}

#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
	[[__gnu__::__returns_nonnull__]]
#endif
	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && \
	__cpp_constexpr_dynamic_alloc >= 201907L
	constexpr
#endif
	T *
	reallocate_n(T *ptr, ::std::size_t oldn, ::std::size_t n) noexcept
		requires(!has_status)
	{
		constexpr ::std::size_t mxn{::std::numeric_limits<::std::size_t>::max() / sizeof(T)};
		if (n > mxn)
		{
			::fast_io::fast_terminate();
		}
		if constexpr (alignof(T) <= alloc::default_alignment)
		{
			return static_cast<T *>(alloc::reallocate_n(ptr, oldn * sizeof(T), n * sizeof(T)));
		}
		else
		{
			return static_cast<T *>(alloc::reallocate_aligned_n(ptr, oldn * sizeof(T), alignof(T), n * sizeof(T)));
		}
	}

	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && \
	__cpp_constexpr_dynamic_alloc >= 201907L
		constexpr
#endif
		basic_allocation_least_result<T *>
		reallocate_n_at_least(T *ptr, ::std::size_t oldn, ::std::size_t n) noexcept
		requires(!has_status)
	{
		constexpr ::std::size_t mxn{::std::numeric_limits<::std::size_t>::max() / sizeof(T)};
		if (n > mxn)
		{
			::fast_io::fast_terminate();
		}
		if constexpr (alignof(T) <= alloc::default_alignment)
		{
			auto newres{alloc::reallocate_n_at_least(ptr, oldn * sizeof(T), n * sizeof(T))};
			return {reinterpret_cast<T *>(newres.ptr), newres.count / sizeof(T)};
		}
		else
		{
			auto newres{alloc::reallocate_aligned_n_at_least(ptr, oldn * sizeof(T), alignof(T), n * sizeof(T))};
			return {reinterpret_cast<T *>(newres.ptr), newres.count / sizeof(T)};
		}
	}

#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
	[[__gnu__::__returns_nonnull__]]
#endif
	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && \
	__cpp_constexpr_dynamic_alloc >= 201907L
	constexpr
#endif
	T *
	reallocate_zero_n(T *ptr, ::std::size_t oldn, ::std::size_t n) noexcept
		requires(!has_status)
	{
		constexpr ::std::size_t mxn{::std::numeric_limits<::std::size_t>::max() / sizeof(T)};
		if (n > mxn)
		{
			::fast_io::fast_terminate();
		}
		if constexpr (alignof(T) <= alloc::default_alignment)
		{
			return static_cast<T *>(alloc::reallocate_zero_n(ptr, oldn * sizeof(T), n * sizeof(T)));
		}
		else
		{
			return static_cast<T *>(alloc::reallocate_aligned_zero_n(ptr, oldn * sizeof(T), alignof(T), n * sizeof(T)));
		}
	}

	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && \
	__cpp_constexpr_dynamic_alloc >= 201907L
		constexpr
#endif
		basic_allocation_least_result<T *>
		reallocate_zero_n_at_least(T *ptr, ::std::size_t oldn, ::std::size_t n) noexcept
		requires(!has_status)
	{
		constexpr ::std::size_t mxn{::std::numeric_limits<::std::size_t>::max() / sizeof(T)};
		if (n > mxn)
		{
			::fast_io::fast_terminate();
		}
		if constexpr (alignof(T) <= alloc::default_alignment)
		{
			auto newres{alloc::reallocate_zero_n_at_least(ptr, oldn * sizeof(T), n * sizeof(T))};
			return {reinterpret_cast<T *>(newres.ptr), newres.count / sizeof(T)};
		}
		else
		{
			auto newres{alloc::reallocate_aligned_zero_n_at_least(ptr, oldn * sizeof(T), alignof(T), n * sizeof(T))};
			return {reinterpret_cast<T *>(newres.ptr), newres.count / sizeof(T)};
		}
	}

	static inline constexpr bool has_deallocate = allocator_adaptor::has_deallocate;

	static inline
#if __cpp_constexpr_dynamic_alloc >= 201907L
		constexpr
#endif
		void
		deallocate(T *ptr) noexcept
		requires(!has_status && has_deallocate)
	{
#if __cpp_constexpr_dynamic_alloc >= 201907L
		if (__builtin_is_constant_evaluated())
		{
			if (ptr)
			{
				return ::fast_io::freestanding::allocator<T>{}.deallocate(ptr, 1);
			}
			else
			{
				return;
			}
		}
#endif
		if constexpr (alignof(T) <= alloc::default_alignment)
		{
			return alloc::deallocate(ptr);
		}
		else
		{
			return alloc::deallocate_aligned(ptr, alignof(T));
		}
	}

	static inline
#if __cpp_constexpr_dynamic_alloc >= 201907L
		constexpr
#endif
		void
		deallocate_n(T *ptr, ::std::size_t n) noexcept
		requires(!has_status)
	{
#if __cpp_constexpr_dynamic_alloc >= 201907L
		if (__builtin_is_constant_evaluated())
		{
			if (ptr)
			{
				return ::fast_io::freestanding::allocator<T>{}.deallocate(ptr, n);
			}
			else
			{
				return;
			}
		}
#endif
		if constexpr (alignof(T) <= alloc::default_alignment)
		{
			alloc::deallocate_n(ptr, n * sizeof(T));
		}
		else
		{
			alloc::deallocate_aligned_n(ptr, alignof(T), n * sizeof(T));
		}
	}

#if 0
	static inline
#if __cpp_constexpr_dynamic_alloc >= 201907L
		constexpr
#endif
		T *
		handle_allocate(handle_type handle, ::std::size_t n) noexcept
		requires(has_status)
	{
#if __cpp_constexpr_dynamic_alloc >= 201907L
		if (__builtin_is_constant_evaluated())
		{
			if (n)
			{
				return ::fast_io::freestanding::allocator<T>{}.allocate(n);
			}
			else
			{
				return nullptr;
			}
		}
#endif
		constexpr ::std::size_t mxn{::std::numeric_limits<::std::size_t>::max() / sizeof(T)};
		if (n > mxn)
		{
			::fast_io::fast_terminate();
		}
		if constexpr (alignof(T) <= alloc::default_alignment)
		{
			return static_cast<T *>(alloc::handle_allocate(handle, n * sizeof(T)));
		}
		else
		{
			return static_cast<T *>(alloc::handle_allocate_aligned(handle, alignof(T), n * sizeof(T)));
		}
	}

	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && \
	__cpp_constexpr_dynamic_alloc >= 201907L
		constexpr
#endif
		T *
		handle_allocate_zero(handle_type handle, ::std::size_t n) noexcept
		requires(has_status)
	{
		constexpr ::std::size_t mxn{::std::numeric_limits<::std::size_t>::max() / sizeof(T)};
		if (n > mxn)
		{
			::fast_io::fast_terminate();
		}
		if constexpr (alignof(T) <= alloc::default_alignment)
		{
			return static_cast<T *>(alloc::handle_allocate_zero(handle, n * sizeof(T)));
		}
		else
		{
			return static_cast<T *>(alloc::handle_allocate_zero_aligned(handle, alignof(T), n * sizeof(T)));
		}
	}

	static inline constexpr bool has_handle_reallocate = allocator_adaptor::has_handle_reallocate;
	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && \
	__cpp_constexpr_dynamic_alloc >= 201907L
		constexpr
#endif
		T *
		handle_reallocate(handle_type handle, T *ptr, ::std::size_t n) noexcept
		requires(has_status && has_handle_reallocate)
	{
		constexpr ::std::size_t mxn{::std::numeric_limits<::std::size_t>::max() / sizeof(T)};
		if (n > mxn)
		{
			::fast_io::fast_terminate();
		}
		if constexpr (alignof(T) <= alloc::default_alignment)
		{
			return static_cast<T *>(alloc::handle_reallocate(handle, ptr, n * sizeof(T)));
		}
		else
		{
			return static_cast<T *>(alloc::handle_reallocate_aligned(handle, ptr, alignof(T), n * sizeof(T)));
		}
	}

	static inline constexpr bool has_handle_reallocate_zero = allocator_adaptor::has_handle_reallocate_zero;

	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && \
	__cpp_constexpr_dynamic_alloc >= 201907L
		constexpr
#endif
		T *
		handle_reallocate_zero(handle_type handle, T *ptr, ::std::size_t n) noexcept
		requires(has_status && has_handle_reallocate)
	{
		constexpr ::std::size_t mxn{::std::numeric_limits<::std::size_t>::max() / sizeof(T)};
		if (n > mxn)
		{
			::fast_io::fast_terminate();
		}
		if constexpr (alignof(T) <= alloc::default_alignment)
		{
			return static_cast<T *>(alloc::handle_reallocate_zero(handle, ptr, n * sizeof(T)));
		}
		else
		{
			return static_cast<T *>(alloc::handle_reallocate_aligned_zero(handle, ptr, alignof(T), n * sizeof(T)));
		}
	}

	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && \
	__cpp_constexpr_dynamic_alloc >= 201907L
		constexpr
#endif
		T *
		handle_reallocate_n(handle_type handle, T *ptr, ::std::size_t oldn, ::std::size_t n) noexcept
		requires(has_status)
	{
		constexpr ::std::size_t mxn{::std::numeric_limits<::std::size_t>::max() / sizeof(T)};
		if (n > mxn)
		{
			::fast_io::fast_terminate();
		}
		if constexpr (alignof(T) <= alloc::default_alignment)
		{
			return static_cast<T *>(alloc::handle_reallocate_n(handle, ptr, oldn * sizeof(T), n * sizeof(T)));
		}
		else
		{
			return static_cast<T *>(alloc::handle_reallocate_aligned_n(handle, ptr, oldn * sizeof(T), alignof(T), n * sizeof(T)));
		}
	}

	static inline
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && \
	__cpp_constexpr_dynamic_alloc >= 201907L
		constexpr
#endif
		T *
		handle_reallocate_zero_n(handle_type handle, T *ptr, ::std::size_t oldn, ::std::size_t n) noexcept
		requires(has_status)
	{
		constexpr ::std::size_t mxn{::std::numeric_limits<::std::size_t>::max() / sizeof(T)};
		if (n > mxn)
		{
			::fast_io::fast_terminate();
		}
		if constexpr (alignof(T) <= alloc::default_alignment)
		{
			return static_cast<T *>(alloc::handle_reallocate_zero_n(handle, ptr, oldn * sizeof(T), n * sizeof(T)));
		}
		else
		{
			return static_cast<T *>(alloc::handle_reallocate_aligned_zero_n(handle, ptr, oldn * sizeof(T), alignof(T), n * sizeof(T)));
		}
	}

	static inline constexpr bool has_handle_deallocate = allocator_adaptor::has_handle_deallocate;
	static inline
#if __cpp_constexpr_dynamic_alloc >= 201907L
		constexpr
#endif
		void
		handle_deallocate(handle_type handle, T *ptr) noexcept
		requires(has_status && has_handle_deallocate)
	{
#if __cpp_constexpr_dynamic_alloc >= 201907L
		if (__builtin_is_constant_evaluated())
		{
			if (ptr)
			{
				return ::fast_io::freestanding::allocator<T>{}.deallocate(ptr, 1);
			}
			else
			{
				return;
			}
		}
#endif
		if constexpr (alignof(T) <= alloc::default_alignment)
		{
			return alloc::handle_deallocate(handle, ptr);
		}
		else
		{
			return alloc::handle_deallocate_aligned(handle, ptr, alignof(T));
		}
	}

	static inline
#if __cpp_constexpr_dynamic_alloc >= 201907L
		constexpr
#endif
		void
		handle_deallocate_n(handle_type handle, T *ptr, ::std::size_t n) noexcept
		requires(has_status)
	{
#if __cpp_constexpr_dynamic_alloc >= 201907L
		if (__builtin_is_constant_evaluated())
		{
			if (ptr)
			{
				return ::fast_io::freestanding::allocator<T>{}.deallocate(ptr, n);
			}
			else
			{
				return;
			}
		}
#endif
		if constexpr (alignof(T) <= alloc::default_alignment)
		{
			alloc::handle_deallocate_n(handle, ptr, n * sizeof(T));
		}
		else
		{
			alloc::handle_deallocate_aligned_n(handle, ptr, alignof(T), n * sizeof(T));
		}
	}
#endif