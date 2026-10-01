#pragma once

static inline constexpr bool has_native_allocate{
	!has_status && (::fast_io::details::has_allocate_at_least_impl<alloc> ||
					::fast_io::details::has_allocate_aligned_at_least_impl<alloc> ||
					::fast_io::details::has_allocate_zero_at_least_impl<alloc> ||
					::fast_io::details::has_allocate_aligned_zero_at_least_impl<alloc> ||
					::fast_io::details::has_allocate_impl<alloc> ||
					::fast_io::details::has_allocate_aligned_impl<alloc> ||
					::fast_io::details::has_allocate_zero_impl<alloc> ||
					::fast_io::details::has_allocate_aligned_zero_impl<alloc> ||
					::fast_io::details::has_allocate_conditional_zero_impl<alloc> ||
					::fast_io::details::has_allocate_aligned_conditional_zero_impl<alloc> ||
					::fast_io::details::has_allocate_conditional_zero_at_least_impl<alloc> ||
					::fast_io::details::has_allocate_aligned_conditional_zero_at_least_impl<alloc>)};

#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
static inline void *allocate_conditional_zero(::std::size_t n, bool zero) noexcept
	requires(!has_status)
{
#if __cpp_if_consteval >= 202106L
	if consteval
#elif __cpp_lib_is_constant_evaluated >= 201811L && __cpp_constexpr_dynamic_alloc >= 201907L
	if (__builtin_is_constant_evaluated())
#else
	if (false)
#endif
	{
		auto p{
#if FAST_IO_HAS_BUILTIN(__builtin_operator_new)
			__builtin_operator_new(n)
#else
			::operator new(n)
#endif
		};
		if (zero)
		{
			::fast_io::freestanding::bytes_clear_n(static_cast<::std::byte *>(p), n);
		}
		return p;
	}
	else
	{
		if constexpr (::fast_io::details::has_allocate_conditional_zero_impl<alloc>)
		{
			return allocator_type::allocate_conditional_zero(n, zero);
		}
		else if constexpr (::fast_io::details::has_allocate_conditional_zero_at_least_impl<alloc>)
		{
			return allocator_type::allocate_conditional_zero_at_least(n, zero).ptr;
		}
		else if constexpr (::fast_io::details::has_allocate_aligned_conditional_zero_impl<alloc>)
		{
			return allocator_type::allocate_aligned_conditional_zero(default_alignment, n, zero);
		}
		else if constexpr (::fast_io::details::has_allocate_aligned_conditional_zero_at_least_impl<alloc>)
		{
			return allocator_type::allocate_aligned_conditional_zero_at_least(default_alignment, n, zero).ptr;
		}
		else
		{
			constexpr bool has_none_zero_ops{::fast_io::details::native_allocate_has_none_zero_ops<alloc>};
			constexpr bool has_zero_ops{::fast_io::details::native_allocate_has_zero_ops<alloc>};
			if constexpr (!has_none_zero_ops && !has_zero_ops)
			{
				::fast_io::fast_terminate();
#if __has_cpp_attribute(unreachable)
				[[unreachable]];
#endif
			}
			else if constexpr (!has_none_zero_ops && has_zero_ops)
			{
				if constexpr (::fast_io::details::has_allocate_zero_impl<alloc>)
				{
					return allocator_type::allocate_zero(n);
				}
				else if constexpr (::fast_io::details::has_allocate_zero_at_least_impl<alloc>)
				{
					return allocator_type::allocate_zero_at_least(n).ptr;
				}
				else if constexpr (::fast_io::details::has_allocate_aligned_zero_impl<alloc>)
				{
					return allocator_type::allocate_aligned_zero(default_alignment, n);
				}
				else if constexpr (::fast_io::details::has_allocate_aligned_zero_at_least_impl<alloc>)
				{
					return allocator_type::allocate_aligned_zero_at_least(default_alignment, n).ptr;
				}
			}
			else if constexpr (has_none_zero_ops && !has_zero_ops)
			{
				void *ptr;
				if constexpr (::fast_io::details::has_allocate_impl<alloc>)
				{
					ptr = allocator_type::allocate(n);
				}
				else if constexpr (::fast_io::details::has_allocate_at_least_impl<alloc>)
				{
					ptr = allocator_type::allocate_at_least(n).ptr;
				}
				else if constexpr (::fast_io::details::has_allocate_aligned_impl<alloc>)
				{
					ptr = allocator_type::allocate_aligned(default_alignment, n);
				}
				else if constexpr (::fast_io::details::has_allocate_aligned_at_least_impl<alloc>)
				{
					ptr = allocator_type::allocate_aligned_at_least(default_alignment, n).ptr;
				}
				if (zero)
				{
					::fast_io::freestanding::bytes_clear_n(reinterpret_cast<::std::byte *>(ptr), n);
				}
				return ptr;
			}
			else
			{
				void *ptr;
				if (zero)
				{
					if constexpr (::fast_io::details::has_allocate_zero_impl<alloc>)
					{
						ptr = allocator_type::allocate_zero(n);
					}
					else if constexpr (::fast_io::details::has_allocate_zero_at_least_impl<alloc>)
					{
						ptr = allocator_type::allocate_zero_at_least(n).ptr;
					}
					else if constexpr (::fast_io::details::has_allocate_aligned_zero_impl<alloc>)
					{
						ptr = allocator_type::allocate_aligned_zero(default_alignment, n);
					}
					else if constexpr (::fast_io::details::has_allocate_aligned_zero_at_least_impl<alloc>)
					{
						ptr = allocator_type::allocate_aligned_zero_at_least(default_alignment, n).ptr;
					}
					else
					{
						::fast_io::fast_terminate();
					}
				}
				else
				{
					if constexpr (::fast_io::details::has_allocate_impl<alloc>)
					{
						ptr = allocator_type::allocate(n);
					}
					else if constexpr (::fast_io::details::has_allocate_at_least_impl<alloc>)
					{
						ptr = allocator_type::allocate_at_least(n).ptr;
					}
					else if constexpr (::fast_io::details::has_allocate_aligned_impl<alloc>)
					{
						ptr = allocator_type::allocate_aligned(default_alignment, n);
					}
					else if constexpr (::fast_io::details::has_allocate_aligned_at_least_impl<alloc>)
					{
						ptr = allocator_type::allocate_aligned_at_least(default_alignment, n).ptr;
					}
					else
					{
						::fast_io::fast_terminate();
					}
				}
				return ptr;
			}
		}
	}
}
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
static inline constexpr void *
allocate(::std::size_t n) noexcept
	requires(!has_status)
{
#if __cpp_constexpr_dynamic_alloc >= 201907L
#if __cpp_if_consteval >= 202106L
	if consteval
#else
	if (__builtin_is_constant_evaluated())
#endif
	{
		return generic_allocator_adapter::allocate_conditional_zero(n, false);
	}
	else
#endif
	{
		if constexpr (::fast_io::details::has_allocate_impl<alloc>)
		{
			return allocator_type::allocate(n);
		}
		else
		{
			return generic_allocator_adapter::allocate_conditional_zero(n, false);
		}
	}
}

#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
static inline void *allocate_zero(::std::size_t n) noexcept
	requires(!has_status)
{
#if __cpp_constexpr_dynamic_alloc >= 201907L
#if __cpp_if_consteval >= 202106L
	if consteval
#else
	if (__builtin_is_constant_evaluated())
#endif
	{
		return generic_allocator_adapter::allocate_conditional_zero(n, true);
	}
	else
#endif
	{
		if constexpr (::fast_io::details::has_allocate_zero_impl<alloc>)
		{
			return allocator_type::allocate_zero(n);
		}
		else
		{
			return generic_allocator_adapter::allocate_conditional_zero(n, true);
		}
	}
}

static inline constexpr bool has_reallocate = (::fast_io::details::has_reallocate_impl<alloc> ||
											   ::fast_io::details::has_reallocate_at_least_impl<alloc> ||
											   ::fast_io::details::has_reallocate_aligned_impl<alloc> ||
											   ::fast_io::details::has_reallocate_aligned_at_least_impl<alloc> ||
											   ::fast_io::details::has_reallocate_zero_impl<alloc> ||
											   ::fast_io::details::has_reallocate_zero_at_least_impl<alloc> ||
											   ::fast_io::details::has_reallocate_aligned_zero_impl<alloc> ||
											   ::fast_io::details::has_reallocate_aligned_zero_at_least_impl<alloc> ||
											   ::fast_io::details::has_reallocate_conditional_zero_impl<alloc> ||
											   ::fast_io::details::has_reallocate_aligned_conditional_zero_impl<alloc> ||
											   ::fast_io::details::has_reallocate_conditional_zero_at_least_impl<alloc> ||
											   ::fast_io::details::has_reallocate_aligned_conditional_zero_at_least_impl<alloc>);

#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
static inline void *reallocate_conditional_zero(void *p, ::std::size_t n, bool zero) noexcept
	requires(!has_status && has_reallocate)
{
	if constexpr (::fast_io::details::has_reallocate_conditional_zero_impl<alloc>)
	{
		return allocator_type::reallocate_conditional_zero(p, n, zero);
	}
	else if constexpr (::fast_io::details::has_reallocate_conditional_zero_at_least_impl<alloc>)
	{
		return allocator_type::reallocate_conditional_zero_at_least(p, n, zero).ptr;
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_conditional_zero_impl<alloc>)
	{
		return allocator_type::reallocate_aligned_conditional_zero(p, default_alignment, n, zero);
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_conditional_zero_at_least_impl<alloc>)
	{
		return allocator_type::reallocate_aligned_conditional_zero_at_least(p, default_alignment, n, zero).ptr;
	}
	else if constexpr (::fast_io::details::has_reallocate_impl<alloc> ||
					   ::fast_io::details::has_reallocate_at_least_impl<alloc> ||
					   ::fast_io::details::has_reallocate_aligned_impl<alloc> ||
					   ::fast_io::details::has_reallocate_aligned_at_least_impl<alloc>)
	{
		// Non-zero APIs exist - need runtime branch
		if (zero)
		{
			if constexpr (::fast_io::details::has_reallocate_zero_impl<alloc>)
			{
				return allocator_type::reallocate_zero(p, n);
			}
			else if constexpr (::fast_io::details::has_reallocate_zero_at_least_impl<alloc>)
			{
				return allocator_type::reallocate_zero_at_least(p, n).ptr;
			}
			else if constexpr (::fast_io::details::has_reallocate_aligned_zero_impl<alloc>)
			{
				return allocator_type::reallocate_aligned_zero(p, default_alignment, n);
			}
			else if constexpr (::fast_io::details::has_reallocate_aligned_zero_at_least_impl<alloc>)
			{
				return allocator_type::reallocate_aligned_zero_at_least(p, default_alignment, n).ptr;
			}
			else
			{
				::fast_io::fast_terminate();
			}
		}
		else
		{
			if constexpr (::fast_io::details::has_reallocate_impl<alloc>)
			{
				return allocator_type::reallocate(p, n);
			}
			else if constexpr (::fast_io::details::has_reallocate_at_least_impl<alloc>)
			{
				return allocator_type::reallocate_at_least(p, n).ptr;
			}
			else if constexpr (::fast_io::details::has_reallocate_aligned_impl<alloc>)
			{
				return allocator_type::reallocate_aligned(p, default_alignment, n);
			}
			else
			{
				return allocator_type::reallocate_aligned_at_least(p, default_alignment, n).ptr;
			}
		}
	}
	else
	{
		// Only zero APIs exist - use zero API for both cases
		if constexpr (::fast_io::details::has_reallocate_zero_impl<alloc>)
		{
			return allocator_type::reallocate_zero(p, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_zero_at_least_impl<alloc>)
		{
			return allocator_type::reallocate_zero_at_least(p, n).ptr;
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_zero_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_zero(p, default_alignment, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_zero_at_least_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_zero_at_least(p, default_alignment, n).ptr;
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
static inline void *reallocate(void *p, ::std::size_t n) noexcept
	requires(!has_status && has_reallocate)
{
	if constexpr (::fast_io::details::has_reallocate_impl<alloc>)
	{
		return allocator_type::reallocate(p, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_at_least_impl<alloc>)
	{
		return allocator_type::reallocate_at_least(p, n).ptr;
	}
	else
	{
		return generic_allocator_adapter::reallocate_conditional_zero(p, n, false);
	}
}

static inline constexpr bool has_reallocate_zero = (::fast_io::details::has_reallocate_zero_impl<alloc> ||
													::fast_io::details::has_reallocate_zero_at_least_impl<alloc> ||
													::fast_io::details::has_reallocate_aligned_zero_impl<alloc> ||
													::fast_io::details::has_reallocate_aligned_zero_at_least_impl<alloc>);


#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
static inline void *reallocate_zero(void *p, ::std::size_t n) noexcept
	requires(!has_status && has_reallocate_zero)
{
	if constexpr (::fast_io::details::has_reallocate_zero_impl<alloc>)
	{
		return allocator_type::reallocate_zero(p, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_zero_at_least_impl<alloc>)
	{
		return allocator_type::reallocate_zero_at_least(p, n).ptr;
	}
	else
	{
		return generic_allocator_adapter::reallocate_conditional_zero(p, n, true);
	}
}

#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
static inline void *reallocate_n_conditional_zero(void *p, ::std::size_t oldn, ::std::size_t n, bool zero) noexcept
	requires(!has_status)
{
	if (p != nullptr && oldn == n)
	{
		return p;
	}
#if __cpp_if_consteval >= 202106L
	if consteval
#elif __cpp_lib_is_constant_evaluated >= 201811L && __cpp_constexpr_dynamic_alloc >= 201907L
	if (__builtin_is_constant_evaluated())
#else
	if (false)
#endif
	{
		auto newptr{::operator new(n)};
		if (p != nullptr)
		{
			if (n)
			{
				::std::size_t copyn{oldn < n ? oldn : n};
				::fast_io::freestanding::nonoverlapped_bytes_copy_n(reinterpret_cast<::std::byte const *>(p), copyn, reinterpret_cast<::std::byte *>(newptr));
			}
			::operator delete(p);
		}
		if (zero && oldn < n)
		{
			::fast_io::freestanding::bytes_clear_n(reinterpret_cast<::std::byte *>(newptr) + oldn, n - oldn);
		}
		return newptr;
	}
	else
	{
		if constexpr (::fast_io::details::has_reallocate_n_conditional_zero_impl<alloc>)
		{
			return allocator_type::reallocate_n_conditional_zero(p, oldn, n, zero);
		}
		else if constexpr (::fast_io::details::has_reallocate_n_conditional_zero_at_least_impl<alloc>)
		{
			return allocator_type::reallocate_n_conditional_zero_at_least(p, oldn, n, zero).ptr;
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_n_conditional_zero_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_n_conditional_zero(p, oldn, default_alignment, n, zero);
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_n_conditional_zero_at_least_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_n_conditional_zero_at_least(p, oldn, default_alignment, n, zero).ptr;
		}
		else if (zero)
		{
			if constexpr (::fast_io::details::has_reallocate_zero_n_impl<alloc>)
			{
				return allocator_type::reallocate_zero_n(p, oldn, n);
			}
			else if constexpr (::fast_io::details::has_reallocate_zero_n_at_least_impl<alloc>)
			{
				return allocator_type::reallocate_zero_n_at_least(p, oldn, n).ptr;
			}
			else if constexpr (::fast_io::details::has_reallocate_aligned_zero_n_impl<alloc>)
			{
				return allocator_type::reallocate_aligned_zero_n(p, oldn, default_alignment, n);
			}
			else if constexpr (::fast_io::details::has_reallocate_aligned_zero_n_at_least_impl<alloc>)
			{
				return allocator_type::reallocate_aligned_zero_n_at_least(p, oldn, default_alignment, n).ptr;
			}
			else
			{
				auto newptr{generic_allocator_adapter::reallocate_n(p, oldn, n)};
				if (oldn < n)
				{
					::fast_io::freestanding::bytes_clear_n(reinterpret_cast<::std::byte *>(newptr) + oldn, n - oldn);
				}
				return newptr;
			}
		}
		else
		{
			if constexpr (::fast_io::details::has_reallocate_n_impl<alloc>)
			{
				return allocator_type::reallocate_n(p, oldn, n);
			}
			else if constexpr (::fast_io::details::has_reallocate_n_at_least_impl<alloc>)
			{
				return allocator_type::reallocate_n_at_least(p, oldn, n).ptr;
			}
			else if constexpr (::fast_io::details::has_reallocate_aligned_n_impl<alloc>)
			{
				return allocator_type::reallocate_aligned_n(p, oldn, default_alignment, n);
			}
			else if constexpr (::fast_io::details::has_reallocate_aligned_n_at_least_impl<alloc>)
			{
				return allocator_type::reallocate_aligned_n_at_least(p, oldn, default_alignment, n).ptr;
			}
			else if constexpr (::fast_io::details::has_reallocate_aligned_zero_n_impl<alloc>)
			{
				return allocator_type::reallocate_aligned_zero_n(p, oldn, default_alignment, n);
			}
			else if constexpr (::fast_io::details::has_reallocate_aligned_zero_n_at_least_impl<alloc>)
			{
				return allocator_type::reallocate_aligned_zero_n_at_least(p, oldn, default_alignment, n).ptr;
			}
			else if constexpr (::fast_io::details::has_reallocate_zero_n_impl<alloc>)
			{
				return allocator_type::reallocate_zero_n(p, oldn, n);
			}
			else if constexpr (::fast_io::details::has_reallocate_zero_n_at_least_impl<alloc>)
			{
				return allocator_type::reallocate_zero_n_at_least(p, oldn, n).ptr;
			}
			else
			{
				auto newptr{generic_allocator_adapter::allocate(n)};
				if (p != nullptr)
				{
					if (n)
					{
						::std::size_t copyn{oldn < n ? oldn : n};
						::fast_io::freestanding::nonoverlapped_bytes_copy_n(reinterpret_cast<::std::byte const *>(p), copyn, reinterpret_cast<::std::byte *>(newptr));
					}
					generic_allocator_adapter::deallocate_n(p, oldn);
				}
				return newptr;
			}
		}
	}
}

#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
static inline void *reallocate_n(void *p, ::std::size_t oldn, ::std::size_t n) noexcept
	requires(!has_status)
{
	if (p != nullptr && oldn == n)
	{
		return p;
	}
	if constexpr (::fast_io::details::has_reallocate_n_impl<alloc>)
	{
		return allocator_type::reallocate_n(p, oldn, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_n_at_least_impl<alloc>)
	{
		return allocator_type::reallocate_n_at_least(p, oldn, n).ptr;
	}
	else
	{
		return generic_allocator_adapter::reallocate_n_conditional_zero(p, oldn, n, false);
	}
}

#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
static inline void *reallocate_zero_n(void *p, ::std::size_t oldn, ::std::size_t n) noexcept
	requires(!has_status)
{
	if (p != nullptr && oldn == n)
	{
		return p;
	}
	if constexpr (::fast_io::details::has_reallocate_zero_n_impl<alloc>)
	{
		return allocator_type::reallocate_zero_n(p, oldn, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_zero_n_at_least_impl<alloc>)
	{
		return allocator_type::reallocate_zero_n_at_least(p, oldn, n).ptr;
	}
	else
	{
		return generic_allocator_adapter::reallocate_n_conditional_zero(p, oldn, n, true);
	}
}

static inline constexpr bool has_deallocate = (::fast_io::details::has_deallocate_impl<alloc> ||
											   ::fast_io::details::has_deallocate_aligned_impl<alloc>);
static inline constexpr void deallocate(void *p) noexcept
	requires(!has_status && has_deallocate)
{
#if __cpp_constexpr_dynamic_alloc >= 201907L
	if (__builtin_is_constant_evaluated())
	{
#if FAST_IO_HAS_BUILTIN(__builtin_operator_delete)
		__builtin_operator_delete(p);
#else
		::operator delete(p);
#endif
	}
	else
#endif
	{
		if constexpr (::fast_io::details::has_deallocate_impl<alloc>)
		{
			allocator_type::deallocate(p);
		}
		else if constexpr (::fast_io::details::has_deallocate_aligned_impl<alloc>)
		{
			allocator_type::deallocate_aligned(p, default_alignment);
		}
	}
}

static inline constexpr void deallocate_n(void *p, ::std::size_t n) noexcept
	requires(!has_status)
{
#if __cpp_constexpr_dynamic_alloc >= 201907L
	if (__builtin_is_constant_evaluated())
	{
#if FAST_IO_HAS_BUILTIN(__builtin_operator_delete)
		__builtin_operator_delete(p);
#else
		::operator delete(p);
#endif
	}
	else
#endif
	{
		if constexpr (::fast_io::details::has_deallocate_n_impl<alloc>)
		{
			allocator_type::deallocate_n(p, n);
		}
		else if constexpr (::fast_io::details::has_deallocate_aligned_n_impl<alloc>)
		{
			allocator_type::deallocate_aligned_n(p, default_alignment, n);
		}
		else if constexpr (::fast_io::details::has_deallocate_impl<alloc>)
		{
			allocator_type::deallocate(p);
		}
		else if constexpr (::fast_io::details::has_deallocate_aligned_impl<alloc>)
		{
			allocator_type::deallocate_aligned(p, default_alignment);
		}
		else
		{
			static_assert(::fast_io::details::has_deallocate_n_impl<alloc>);
		}
	}
}

#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
static inline constexpr void *
allocate_aligned_conditional_zero(::std::size_t alignment, ::std::size_t n, bool zero) noexcept
	requires(!has_status)
{
#if __cpp_if_consteval >= 202106L
	if consteval
#elif __cpp_lib_is_constant_evaluated >= 201811L && __cpp_constexpr_dynamic_alloc >= 201907L
	if (__builtin_is_constant_evaluated())
#else
	if (false)
#endif
	{
		auto p{
#if FAST_IO_HAS_BUILTIN(__builtin_operator_new)
			__builtin_operator_new(n)
#else
			::operator new(n)
#endif
		};
		if (zero)
		{
			::fast_io::freestanding::bytes_clear_n(static_cast<::std::byte *>(p), n);
		}
		return p;
	}
	if constexpr (::fast_io::details::has_allocate_aligned_conditional_zero_impl<alloc>)
	{
		return allocator_type::allocate_aligned_conditional_zero(alignment, n, zero);
	}
	else if constexpr (::fast_io::details::has_allocate_aligned_conditional_zero_at_least_impl<alloc>)
	{
		return allocator_type::allocate_aligned_conditional_zero_at_least(alignment, n, zero).ptr;
	}
	else if constexpr (::fast_io::details::native_allocate_aligned_has_ops<alloc>)
	{
		constexpr bool has_none_zero_ops{::fast_io::details::native_allocate_aligned_has_none_zero_ops<alloc>};
		constexpr bool has_zero_ops{::fast_io::details::native_allocate_aligned_has_zero_ops<alloc>};
		if constexpr (!has_none_zero_ops && has_zero_ops)
		{
			if constexpr (::fast_io::details::has_allocate_aligned_zero_impl<alloc>)
			{
				return allocator_type::allocate_aligned_zero(alignment, n);
			}
			else
			{
				return allocator_type::allocate_aligned_zero_at_least(alignment, n).ptr;
			}
		}
		else if constexpr (has_none_zero_ops && !has_zero_ops)
		{
			void *ptr;
			if constexpr (::fast_io::details::has_allocate_aligned_impl<alloc>)
			{
				ptr = allocator_type::allocate_aligned(alignment, n);
			}
			else
			{
				ptr = allocator_type::allocate_aligned_at_least(alignment, n).ptr;
			}
			if (zero)
			{
				::fast_io::freestanding::bytes_clear_n(reinterpret_cast<::std::byte *>(ptr), n);
			}
			return ptr;
		}
		else if constexpr (has_none_zero_ops && has_zero_ops)
		{
			void *ptr;
			if (zero)
			{
				if constexpr (::fast_io::details::has_allocate_aligned_zero_impl<alloc>)
				{
					ptr = allocator_type::allocate_aligned_zero(alignment, n);
				}
				else
				{
					ptr = allocator_type::allocate_aligned_at_least(alignment, n).ptr;
				}
			}
			else
			{
				if constexpr (::fast_io::details::has_allocate_aligned_impl<alloc>)
				{
					ptr = allocator_type::allocate_aligned_zero(alignment, n);
				}
				else
				{
					ptr = allocator_type::allocate_aligned_at_least(alignment, n).ptr;
				}
			}
			return ptr;
		}
		else
		{
			::fast_io::fast_terminate();
		}
	}
	else
	{
		return ::fast_io::details::allocator_pointer_aligned_impl<alloc>(alignment, n, zero);
	}
}

#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
static inline constexpr void *
allocate_aligned(::std::size_t alignment, ::std::size_t n) noexcept
	requires(!has_status)
{
#if __cpp_if_consteval >= 202106L
	if consteval
#elif __cpp_lib_is_constant_evaluated >= 201811L && __cpp_constexpr_dynamic_alloc >= 201907L
	if (__builtin_is_constant_evaluated())
#else
	if (false)
#endif
	{
		return
#if FAST_IO_HAS_BUILTIN(__builtin_operator_new)
			__builtin_operator_new(n)
#else
			::operator new(n)
#endif
				;
	}
	if constexpr (::fast_io::details::has_allocate_aligned_impl<alloc>)
	{
		return allocator_type::allocate_aligned(alignment, n);
	}
	else
	{
		return generic_allocator_adapter::allocate_aligned_conditional_zero(alignment, n, false);
	}
}

#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
static inline constexpr void *
allocate_aligned_zero(::std::size_t alignment, ::std::size_t n) noexcept
	requires(!has_status)
{
#if __cpp_if_consteval >= 202106L
	if consteval
#elif __cpp_lib_is_constant_evaluated >= 201811L && __cpp_constexpr_dynamic_alloc >= 201907L
	if (__builtin_is_constant_evaluated())
#else
	if (false)
#endif
	{
		return
#if FAST_IO_HAS_BUILTIN(__builtin_operator_new)
			__builtin_operator_new(n)
#else
			::operator new(n)
#endif
				;
	}
	if constexpr (::fast_io::details::has_allocate_aligned_zero_impl<alloc>)
	{
		return allocator_type::allocate_aligned_zero(alignment, n);
	}
	else
	{
		return generic_allocator_adapter::allocate_aligned_conditional_zero(alignment, n, true);
	}
}

static inline constexpr bool has_native_allocate_at_least{
	!has_status && (::fast_io::details::has_allocate_at_least_impl<alloc> ||
					::fast_io::details::has_allocate_aligned_at_least_impl<alloc> ||
					::fast_io::details::has_allocate_zero_at_least_impl<alloc> ||
					::fast_io::details::has_allocate_aligned_zero_at_least_impl<alloc>)};

static inline constexpr allocation_least_result
allocate_at_least(::std::size_t n) noexcept
	requires(!has_status)
{
#if __cpp_if_consteval >= 202106L
	if consteval
#elif __cpp_lib_is_constant_evaluated >= 201811L && __cpp_constexpr_dynamic_alloc >= 201907L
	if (__builtin_is_constant_evaluated())
#else
	if (false)
#endif
	{
		return {
#if FAST_IO_HAS_BUILTIN(__builtin_operator_new)
			__builtin_operator_new(n)
#else
			::operator new(n)
#endif
				,
			n};
	}
	else
	{
		if constexpr (::fast_io::details::has_allocate_at_least_impl<alloc>)
		{
			return allocator_type::allocate_at_least(n);
		}
		else
		{
			return {generic_allocator_adapter::allocate(n), n};
		}
	}
}

static inline constexpr allocation_least_result
allocate_zero_at_least(::std::size_t n) noexcept
	requires(!has_status)
{
#if __cpp_if_consteval >= 202106L
	if consteval
#elif __cpp_lib_is_constant_evaluated >= 201811L && __cpp_constexpr_dynamic_alloc >= 201907L
	if (__builtin_is_constant_evaluated())
#else
	if (false)
#endif
	{
		return {
#if FAST_IO_HAS_BUILTIN(__builtin_operator_new)
			__builtin_operator_new(n)
#else
			::operator new(n)
#endif
				,
			n};
	}
	else
	{
		if constexpr (::fast_io::details::has_allocate_zero_at_least_impl<alloc>)
		{
			return allocator_type::allocate_zero_at_least(n);
		}
		else
		{
			return generic_allocator_adapter::allocate_conditional_zero_at_least(n, true);
		}
	}
}

static inline constexpr allocation_least_result
allocate_conditional_zero_at_least(::std::size_t n, bool zero) noexcept
	requires(!has_status)
{
#if __cpp_if_consteval >= 202106L
	if consteval
#elif __cpp_lib_is_constant_evaluated >= 201811L && __cpp_constexpr_dynamic_alloc >= 201907L
	if (__builtin_is_constant_evaluated())
#else
	if (false)
#endif
	{
		auto p{::operator new(n)};
		if (zero)
		{
			::fast_io::freestanding::bytes_clear_n(reinterpret_cast<::std::byte *>(p), n);
		}
		return {p, n};
	}
	else
	{
		if constexpr (::fast_io::details::has_allocate_conditional_zero_at_least_impl<alloc>)
		{
			return allocator_type::allocate_conditional_zero_at_least(n, zero);
		}
		if constexpr (::fast_io::details::has_allocate_conditional_zero_impl<alloc>)
		{
			return {allocator_type::allocate_conditional_zero(n, zero), n};
		}
		else if constexpr (::fast_io::details::has_allocate_aligned_conditional_zero_at_least_impl<alloc>)
		{
			return allocator_type::allocate_aligned_conditional_zero_at_least(default_alignment, n, zero);
		}
		else if constexpr (::fast_io::details::has_allocate_aligned_conditional_zero_impl<alloc>)
		{
			return {allocator_type::allocate_aligned_conditional_zero(default_alignment, n, zero), n};
		}
		else
		{
			constexpr bool has_none_zero_ops{::fast_io::details::native_allocate_has_none_zero_ops<alloc>};
			constexpr bool has_zero_ops{::fast_io::details::native_allocate_has_zero_ops<alloc>};
			if constexpr (!has_none_zero_ops && !has_zero_ops)
			{
				::fast_io::fast_terminate();
#if __has_cpp_attribute(unreachable)
				[[unreachable]];
#endif
			}
			else if constexpr (!has_none_zero_ops && has_zero_ops)
			{
				if constexpr (::fast_io::details::has_allocate_zero_at_least_impl<alloc>)
				{
					return allocator_type::allocate_zero_at_least(n);
				}
				else if constexpr (::fast_io::details::has_allocate_zero_impl<alloc>)
				{
					return {allocator_type::allocate_zero(n), n};
				}
				else if constexpr (::fast_io::details::has_allocate_aligned_zero_at_least_impl<alloc>)
				{
					return allocator_type::allocate_aligned_zero_at_least(default_alignment, n);
				}
				else if constexpr (::fast_io::details::has_allocate_aligned_zero_impl<alloc>)
				{
					return {allocator_type::allocate_aligned_zero(default_alignment, n), n};
				}
			}
			else if constexpr (has_none_zero_ops && !has_zero_ops)
			{
				::fast_io::allocation_least_result res;
				if constexpr (::fast_io::details::has_allocate_at_least_impl<alloc>)
				{
					res = allocator_type::allocate_at_least(n);
				}
				else if constexpr (::fast_io::details::has_allocate_impl<alloc>)
				{
					res = {allocator_type::allocate(n), n};
				}
				else if constexpr (::fast_io::details::has_allocate_aligned_at_least_impl<alloc>)
				{
					res = allocator_type::allocate_aligned_at_least(default_alignment, n);
				}
				else if constexpr (::fast_io::details::has_allocate_aligned_impl<alloc>)
				{
					res = {allocator_type::allocate_aligned(default_alignment, n), n};
				}
				if (zero)
				{
					::fast_io::freestanding::bytes_clear_n(reinterpret_cast<::std::byte *>(res.ptr), res.count);
				}
				return res;
			}
			else
			{
				::fast_io::allocation_least_result res;
				if (zero)
				{
					if constexpr (::fast_io::details::has_allocate_at_least_impl<alloc>)
					{
						res = allocator_type::allocate_at_least(n);
					}
					else if constexpr (::fast_io::details::has_allocate_impl<alloc>)
					{
						res = {allocator_type::allocate(n), n};
					}
					else if constexpr (::fast_io::details::has_allocate_aligned_at_least_impl<alloc>)
					{
						res = allocator_type::allocate_aligned_at_least(default_alignment, n);
					}
					else if constexpr (::fast_io::details::has_allocate_aligned_impl<alloc>)
					{
						res = {allocator_type::allocate_aligned(default_alignment, n), n};
					}
					else
					{
						::fast_io::fast_terminate();
					}
				}
				else
				{
					if constexpr (::fast_io::details::has_allocate_zero_at_least_impl<alloc>)
					{
						res = allocator_type::allocate_zero_at_least(n);
					}
					else if constexpr (::fast_io::details::has_allocate_zero_impl<alloc>)
					{
						res = {allocator_type::allocate_zero(n), n};
					}
					else if constexpr (::fast_io::details::has_allocate_aligned_zero_at_least_impl<alloc>)
					{
						res = allocator_type::allocate_aligned_zero_at_least(default_alignment, n);
					}
					else if constexpr (::fast_io::details::has_allocate_aligned_zero_impl<alloc>)
					{
						res = {allocator_type::allocate_aligned_zero(default_alignment, n), n};
					}
					else
					{
						::fast_io::fast_terminate();
					}
				}
				return res;
			}
		}
	}
}
static inline constexpr allocation_least_result
allocate_aligned_at_least(::std::size_t alignment, ::std::size_t n) noexcept
	requires(!has_status)
{
#if __cpp_if_consteval >= 202106L
	if consteval
#elif __cpp_lib_is_constant_evaluated >= 201811L && __cpp_constexpr_dynamic_alloc >= 201907L
	if (__builtin_is_constant_evaluated())
#else
	if (false)
#endif
	{
		return generic_allocator_adapter::allocate_aligned_conditional_zero_at_least(alignment, n, false);
	}
	else
	{
		if constexpr (::fast_io::details::has_allocate_aligned_at_least_impl<alloc>)
		{
			return allocator_type::allocate_aligned_at_least(alignment, n);
		}
		else
		{
			return generic_allocator_adapter::allocate_aligned_conditional_zero_at_least(alignment, n, false);
		}
	}
}

static inline constexpr allocation_least_result
allocate_aligned_zero_at_least(::std::size_t alignment, ::std::size_t n) noexcept
	requires(!has_status)
{
#if __cpp_if_consteval >= 202106L
	if consteval
#elif __cpp_lib_is_constant_evaluated >= 201811L && __cpp_constexpr_dynamic_alloc >= 201907L
	if (__builtin_is_constant_evaluated())
#else
	if (false)
#endif
	{
		return generic_allocator_adapter::allocate_aligned_conditional_zero_at_least(alignment, n, true);
	}
	else
	{
		if constexpr (::fast_io::details::has_allocate_aligned_zero_at_least_impl<alloc>)
		{
			return allocator_type::allocate_aligned_zero_at_least(alignment, n);
		}
		else
		{
			return generic_allocator_adapter::allocate_aligned_conditional_zero_at_least(alignment, n, true);
		}
	}
}
static inline constexpr allocation_least_result
allocate_aligned_conditional_zero_at_least(::std::size_t alignment, ::std::size_t n, bool zero) noexcept
	requires(!has_status)
{
#if __cpp_if_consteval >= 202106L
	if consteval
#elif __cpp_lib_is_constant_evaluated >= 201811L && __cpp_constexpr_dynamic_alloc >= 201907L
	if (__builtin_is_constant_evaluated())
#else
	if (false)
#endif
	{
		auto p{::operator new(n)};
		if (zero)
		{
			::fast_io::freestanding::bytes_clear_n(reinterpret_cast<::std::byte *>(p), n);
		}
		return {p, n};
	}
	if constexpr (::fast_io::details::has_allocate_aligned_conditional_zero_at_least_impl<alloc>)
	{
		return allocator_type::allocate_aligned_conditional_zero_at_least(alignment, n, zero);
	}
	else if constexpr (::fast_io::details::has_allocate_aligned_conditional_zero_impl<alloc>)
	{
		return {allocator_type::allocate_aligned_conditional_zero(alignment, n, zero), n};
	}
	else if constexpr (::fast_io::details::native_allocate_aligned_has_ops<alloc>)
	{
		constexpr bool has_none_zero_ops{::fast_io::details::native_allocate_aligned_has_none_zero_ops<alloc>};
		constexpr bool has_zero_ops{::fast_io::details::native_allocate_aligned_has_zero_ops<alloc>};
		if constexpr (!has_none_zero_ops && has_zero_ops)
		{
			if constexpr (::fast_io::details::has_allocate_aligned_zero_at_least_impl<alloc>)
			{
				return allocator_type::allocate_aligned_zero_at_least(alignment, n);
			}
			else
			{
				return {allocator_type::allocate_aligned_zero(alignment, n), n};
			}
		}
		else if constexpr (has_none_zero_ops && !has_zero_ops)
		{
			::fast_io::allocation_least_result res;
			if constexpr (::fast_io::details::has_allocate_aligned_at_least_impl<alloc>)
			{
				res = allocator_type::allocate_aligned_at_least(alignment, n);
			}
			else
			{
				res = {allocator_type::allocate_aligned(alignment, n), n};
			}
			if (zero)
			{
				::fast_io::freestanding::bytes_clear_n(reinterpret_cast<::std::byte *>(res.ptr), res.count);
			}
			return res;
		}
		else if constexpr (has_none_zero_ops && has_zero_ops)
		{
			::fast_io::allocation_least_result res;
			if (zero)
			{
				if constexpr (::fast_io::details::has_allocate_aligned_zero_at_least_impl<alloc>)
				{
					res = allocator_type::allocate_aligned_zero_at_least(alignment, n);
				}
				else
				{
					res = {allocator_type::allocate_aligned_zero(alignment, n), n};
				}
			}
			else
			{
				if constexpr (::fast_io::details::has_allocate_aligned_at_least_impl<alloc>)
				{
					res = allocator_type::allocate_aligned_at_least(alignment, n);
				}
				else
				{
					res = {allocator_type::allocate_aligned(alignment, n), n};
				}
			}
			return res;
		}
		else
		{
			::fast_io::fast_terminate();
		}
	}
	else
	{
		return ::fast_io::details::allocator_pointer_aligned_at_least_impl<alloc>(default_alignment, n, zero);
	}
}

static inline constexpr bool has_reallocate_aligned = (::fast_io::details::has_reallocate_aligned_impl<alloc> ||
													   ::fast_io::details::has_reallocate_aligned_at_least_impl<alloc> ||
													   ::fast_io::details::has_reallocate_aligned_zero_impl<alloc> ||
													   ::fast_io::details::has_reallocate_aligned_zero_at_least_impl<alloc> ||
													   ::fast_io::details::has_reallocate_aligned_conditional_zero_impl<alloc> ||
													   ::fast_io::details::has_reallocate_aligned_conditional_zero_at_least_impl<alloc>);

#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
static inline void *
reallocate_aligned_conditional_zero(void *p, ::std::size_t alignment, ::std::size_t n, bool zero) noexcept
	requires(!has_status && has_reallocate_aligned)
{
	if constexpr (::fast_io::details::has_reallocate_aligned_conditional_zero_impl<alloc>)
	{
		return allocator_type::reallocate_aligned_conditional_zero(p, alignment, n, zero);
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_conditional_zero_at_least_impl<alloc>)
	{
		return allocator_type::reallocate_aligned_conditional_zero_at_least(p, alignment, n, zero).ptr;
	}
	else if (zero)
	{
		if constexpr (::fast_io::details::has_reallocate_aligned_zero_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_zero(p, alignment, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_zero_at_least_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_zero_at_least(p, alignment, n).ptr;
		}
		else
		{
			::fast_io::fast_terminate();
		}
	}
	else
	{
		if constexpr (::fast_io::details::has_reallocate_aligned_impl<alloc>)
		{
			return allocator_type::reallocate_aligned(p, alignment, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_at_least_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_at_least(p, alignment, n).ptr;
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_zero_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_zero(p, alignment, n);
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_zero_at_least_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_zero_at_least(p, alignment, n).ptr;
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
static inline void *
reallocate_aligned(void *p, ::std::size_t alignment, ::std::size_t n) noexcept
	requires(!has_status && has_reallocate_aligned)
{
	if constexpr (::fast_io::details::has_reallocate_aligned_impl<alloc>)
	{
		return allocator_type::reallocate_aligned(p, alignment, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_at_least_impl<alloc>)
	{
		return allocator_type::reallocate_aligned_at_least(p, alignment, n).ptr;
	}
	else
	{
		return generic_allocator_adapter::reallocate_aligned_conditional_zero(p, alignment, n, false);
	}
}

static inline constexpr bool has_reallocate_aligned_zero = (::fast_io::details::has_reallocate_aligned_zero_impl<alloc> ||
															::fast_io::details::has_reallocate_aligned_zero_at_least_impl<alloc>);
#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
static inline void *
reallocate_aligned_zero(void *p, ::std::size_t alignment, ::std::size_t n) noexcept
	requires(!has_status && has_reallocate_aligned_zero)
{
	if constexpr (::fast_io::details::has_reallocate_aligned_zero_impl<alloc>)
	{
		return allocator_type::reallocate_aligned_zero(p, alignment, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_zero_at_least_impl<alloc>)
	{
		return allocator_type::reallocate_aligned_zero_at_least(p, alignment, n).ptr;
	}
	else
	{
		return generic_allocator_adapter::reallocate_aligned_conditional_zero(p, alignment, n, true);
	}
}

#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
static inline void *
reallocate_aligned_n_conditional_zero(void *p, ::std::size_t oldn, ::std::size_t alignment, ::std::size_t n, bool zero) noexcept
	requires(!has_status)
{
	if (p != nullptr && oldn == n)
	{
		return p;
	}
#if __cpp_if_consteval >= 202106L
	if consteval
#elif __cpp_lib_is_constant_evaluated >= 201811L && __cpp_constexpr_dynamic_alloc >= 201907L
	if (__builtin_is_constant_evaluated())
#else
	if (false)
#endif
	{
		auto newptr{::operator new(n)};
		if (p != nullptr)
		{
			if (n)
			{
				::std::size_t copyn{oldn < n ? oldn : n};
				::fast_io::freestanding::nonoverlapped_bytes_copy_n(reinterpret_cast<::std::byte const *>(p), copyn, reinterpret_cast<::std::byte *>(newptr));
			}
			::operator delete(p);
		}
		if (zero && oldn < n)
		{
			::fast_io::freestanding::bytes_clear_n(reinterpret_cast<::std::byte *>(newptr) + oldn, n - oldn);
		}
		return newptr;
	}
	else
	{
		if constexpr (::fast_io::details::has_reallocate_aligned_n_conditional_zero_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_n_conditional_zero(p, oldn, alignment, n, zero);
		}
		else if constexpr (::fast_io::details::has_reallocate_aligned_n_conditional_zero_at_least_impl<alloc>)
		{
			return allocator_type::reallocate_aligned_n_conditional_zero_at_least(p, oldn, alignment, n, zero).ptr;
		}
		else if (zero)
		{
			if constexpr (::fast_io::details::has_reallocate_aligned_zero_n_impl<alloc>)
			{
				return allocator_type::reallocate_aligned_zero_n(p, oldn, alignment, n);
			}
			else if constexpr (::fast_io::details::has_reallocate_aligned_zero_n_at_least_impl<alloc>)
			{
				return allocator_type::reallocate_aligned_zero_n_at_least(p, oldn, alignment, n).ptr;
			}
			else
			{
				auto newptr{generic_allocator_adapter::reallocate_aligned_n(p, oldn, alignment, n)};
				if (oldn < n)
				{
					::fast_io::freestanding::bytes_clear_n(reinterpret_cast<::std::byte *>(newptr) + oldn, n - oldn);
				}
				return newptr;
			}
		}
		else
		{
			if constexpr (::fast_io::details::has_reallocate_aligned_n_impl<alloc>)
			{
				return allocator_type::reallocate_aligned_n(p, oldn, alignment, n);
			}
			else if constexpr (::fast_io::details::has_reallocate_aligned_n_at_least_impl<alloc>)
			{
				return allocator_type::reallocate_aligned_n_at_least(p, oldn, alignment, n).ptr;
			}
			else if constexpr (::fast_io::details::has_reallocate_aligned_impl<alloc>)
			{
				return allocator_type::reallocate_aligned(p, alignment, n);
			}
			else if constexpr (::fast_io::details::has_reallocate_aligned_at_least_impl<alloc>)
			{
				return allocator_type::reallocate_aligned_at_least(p, alignment, n).ptr;
			}
			else if constexpr (::fast_io::details::has_reallocate_aligned_zero_n_impl<alloc>)
			{
				return allocator_type::reallocate_aligned_zero_n(p, oldn, alignment, n);
			}
			else if constexpr (::fast_io::details::has_reallocate_aligned_zero_n_at_least_impl<alloc>)
			{
				return allocator_type::reallocate_aligned_zero_n_at_least(p, oldn, alignment, n).ptr;
			}
			else if constexpr (::fast_io::details::has_reallocate_aligned_zero_impl<alloc>)
			{
				return allocator_type::reallocate_aligned_zero(p, alignment, n);
			}
			else if constexpr (::fast_io::details::has_reallocate_aligned_zero_at_least_impl<alloc>)
			{
				return allocator_type::reallocate_aligned_zero_at_least(p, alignment, n).ptr;
			}
			else
			{
				if constexpr (::fast_io::details::has_reallocate_n_impl<alloc> ||
							  ::fast_io::details::has_reallocate_zero_n_impl<alloc> ||
							  ::fast_io::details::has_reallocate_impl<alloc> ||
							  ::fast_io::details::has_reallocate_zero_impl<alloc>)
				{
					if (alignment <= default_alignment)
					{
						return generic_allocator_adapter::reallocate_n(p, oldn, n);
					}
				}
				auto newptr{::fast_io::details::allocator_pointer_aligned_impl<alloc>(alignment, n, false)};
				if (p != nullptr)
				{
					if (n)
					{
						bool moren{oldn < n};
						::std::size_t copyn{moren ? oldn : n};
						::fast_io::freestanding::nonoverlapped_bytes_copy_n(reinterpret_cast<::std::byte const *>(p), copyn, reinterpret_cast<::std::byte *>(newptr));
					}
					generic_allocator_adapter::deallocate_aligned_n(p, alignment, oldn);
				}
				return newptr;
			}
		}
	}
}

#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
static inline void *
reallocate_aligned_n(void *p, ::std::size_t oldn, ::std::size_t alignment, ::std::size_t n) noexcept
	requires(!has_status)
{
	if (p != nullptr && oldn == n)
	{
		return p;
	}
	if constexpr (::fast_io::details::has_reallocate_aligned_n_impl<alloc>)
	{
		return allocator_type::reallocate_aligned_n(p, oldn, alignment, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_n_at_least_impl<alloc>)
	{
		return allocator_type::reallocate_aligned_n_at_least(p, oldn, alignment, n).ptr;
	}
	else
	{
		return generic_allocator_adapter::reallocate_aligned_n_conditional_zero(p, oldn, alignment, n, false);
	}
}

#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
static inline void *
reallocate_aligned_zero_n(void *p, ::std::size_t oldn, ::std::size_t alignment, ::std::size_t n) noexcept
	requires(!has_status)
{
	if (p != nullptr && oldn == n)
	{
		return p;
	}
	if constexpr (::fast_io::details::has_reallocate_aligned_zero_n_impl<alloc>)
	{
		return allocator_type::reallocate_aligned_zero_n(p, oldn, alignment, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_zero_n_at_least_impl<alloc>)
	{
		return allocator_type::reallocate_aligned_zero_n_at_least(p, oldn, alignment, n).ptr;
	}
	else
	{
		auto newptr{generic_allocator_adapter::reallocate_aligned_n(p, oldn, alignment, n)};
		if (oldn < n)
		{
			::std::size_t const to_zero_bytes{static_cast<::std::size_t>(n - oldn)};
			::fast_io::freestanding::bytes_clear_n(reinterpret_cast<::std::byte *>(newptr) + oldn, to_zero_bytes);
		}
		return newptr;
	}
}

static inline constexpr bool has_native_reallocate_at_least = (has_reallocate &&
															   (::fast_io::details::has_reallocate_aligned_at_least_impl<alloc> ||
																::fast_io::details::has_reallocate_aligned_zero_at_least_impl<alloc>));
static inline ::fast_io::allocation_least_result
reallocate_at_least(void *p, ::std::size_t n) noexcept
	requires(!has_status && has_reallocate)
{
	if constexpr (::fast_io::details::has_reallocate_at_least_impl<alloc>)
	{
		return allocator_type::reallocate_at_least(p, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_at_least_impl<alloc>)
	{
		return allocator_type::reallocate_aligned_at_least(p, default_alignment, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_zero_at_least_impl<alloc>)
	{
		return allocator_type::reallocate_zero_at_least(p, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_zero_at_least_impl<alloc>)
	{
		return allocator_type::reallocate_aligned_zero_at_least(p, default_alignment, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_impl<alloc>)
	{
		return {allocator_type::reallocate(p, n), n};
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_impl<alloc>)
	{
		return {allocator_type::reallocate_aligned(p, default_alignment, n), n};
	}
	else if constexpr (::fast_io::details::has_reallocate_zero_impl<alloc>)
	{
		return {allocator_type::reallocate_zero(p, n), n};
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_zero_impl<alloc>)
	{
		return {allocator_type::reallocate_aligned_zero(p, default_alignment, n), n};
	}
}

static inline constexpr bool has_native_reallocate_zero_at_least = (has_reallocate_zero &&
																	(::fast_io::details::has_reallocate_zero_at_least_impl<alloc> ||
																	 ::fast_io::details::has_reallocate_aligned_zero_at_least_impl<alloc>));

static inline ::fast_io::allocation_least_result
reallocate_zero_at_least(void *p, ::std::size_t n) noexcept
	requires(!has_status && has_reallocate)
{
	if constexpr (::fast_io::details::has_reallocate_zero_at_least_impl<alloc>)
	{
		return allocator_type::reallocate_zero_at_least(p, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_zero_at_least_impl<alloc>)
	{
		return allocator_type::reallocate_aligned_zero_at_least(p, default_alignment, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_zero_impl<alloc>)
	{
		return {allocator_type::reallocate_zero(p, n), n};
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_impl<alloc>)
	{
		return {allocator_type::reallocate_aligned(p, default_alignment, n), n};
	}
}

static inline ::fast_io::allocation_least_result
reallocate_n_at_least(void *p, ::std::size_t oldn, ::std::size_t n) noexcept
	requires(!has_status)
{
	if constexpr (::fast_io::details::has_reallocate_n_at_least_impl<alloc>)
	{
		return allocator_type::reallocate_n_at_least(p, oldn, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_n_at_least_impl<alloc>)
	{
		return allocator_type::reallocate_aligned_n_at_least(p, oldn, default_alignment, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_zero_n_at_least_impl<alloc>)
	{
		return allocator_type::reallocate_zero_n_at_least(p, oldn, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_zero_n_at_least_impl<alloc>)
	{
		return allocator_type::reallocate_aligned_zero_n_at_least(p, oldn, default_alignment, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_at_least_impl<alloc>)
	{
		return allocator_type::reallocate_at_least(p, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_at_least_impl<alloc>)
	{
		return allocator_type::reallocate_aligned_at_least(p, default_alignment, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_zero_at_least_impl<alloc>)
	{
		return allocator_type::reallocate_zero_at_least(p, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_zero_at_least_impl<alloc>)
	{
		return allocator_type::reallocate_aligned_zero_at_least(p, default_alignment, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_n_impl<alloc>)
	{
		return {allocator_type::reallocate_n(p, oldn, n), n};
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_n_impl<alloc>)
	{
		return {allocator_type::reallocate_aligned_n(p, oldn, default_alignment, n), n};
	}
	else if constexpr (::fast_io::details::has_reallocate_zero_n_impl<alloc>)
	{
		return {allocator_type::reallocate_zero_n(p, oldn, n), n};
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_zero_n_impl<alloc>)
	{
		return {allocator_type::reallocate_aligned_zero_n(p, oldn, default_alignment, n), n};
	}
	else if constexpr (::fast_io::details::has_reallocate_impl<alloc>)
	{
		return {allocator_type::reallocate(p, n), n};
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_impl<alloc>)
	{
		return {allocator_type::reallocate_aligned(p, default_alignment, n), n};
	}
	else if constexpr (::fast_io::details::has_reallocate_zero_impl<alloc>)
	{
		return {allocator_type::reallocate_zero(p, n), n};
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_zero_impl<alloc>)
	{
		return {allocator_type::reallocate_aligned_zero(p, default_alignment, n), n};
	}
	else
	{
		auto newres{generic_allocator_adapter::allocate_at_least(n)};
		auto newptr{newres.ptr};
		if (p != nullptr)
		{
			if (n)
			{
				if (oldn < n)
				{
					n = oldn;
				}
				::fast_io::freestanding::nonoverlapped_bytes_copy_n(reinterpret_cast<::std::byte const *>(p), n, reinterpret_cast<::std::byte *>(newptr));
			}
			generic_allocator_adapter::deallocate_n(p, oldn);
		}
		return newres;
	}
}

static inline ::fast_io::allocation_least_result
reallocate_zero_n_at_least(void *p, ::std::size_t oldn, ::std::size_t n) noexcept
	requires(!has_status)
{
	if constexpr (::fast_io::details::has_reallocate_zero_n_at_least_impl<alloc>)
	{
		return allocator_type::reallocate_zero_n_at_least(p, oldn, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_zero_n_at_least_impl<alloc>)
	{
		return allocator_type::reallocate_aligned_zero_n_at_least(p, oldn, default_alignment, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_zero_at_least_impl<alloc>)
	{
		return allocator_type::reallocate_zero_at_least(p, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_zero_at_least_impl<alloc>)
	{
		return allocator_type::reallocate_aligned_zero_at_least(p, default_alignment, n);
	}
	else
	{
		auto newres{generic_allocator_adapter::reallocate_n_at_least(p, oldn, n)};
		auto newptr{newres.ptr};
		n = newres.count;
		if (oldn < n)
		{
			::std::size_t const to_zero_bytes{static_cast<::std::size_t>(n - oldn)};
			::fast_io::freestanding::bytes_clear_n(reinterpret_cast<::std::byte *>(newptr) + oldn, to_zero_bytes);
		}
		return newres;
	}
}

static inline constexpr bool has_native_reallocate_aligned_at_least = (has_reallocate_aligned &&
																	   (::fast_io::details::has_reallocate_aligned_zero_at_least_impl<alloc> ||
																		::fast_io::details::has_reallocate_aligned_at_least_impl<alloc>));

static inline ::fast_io::allocation_least_result
reallocate_aligned_at_least(void *p, ::std::size_t alignment, ::std::size_t n) noexcept
	requires(!has_status && has_reallocate_aligned_zero)
{
	if constexpr (::fast_io::details::has_reallocate_aligned_at_least_impl<alloc>)
	{
		return allocator_type::reallocate_aligned_at_least(p, alignment, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_zero_at_least_impl<alloc>)
	{
		return allocator_type::reallocate_aligned_zero_at_least(p, alignment, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_impl<alloc>)
	{
		return {allocator_type::reallocate_aligned(p, alignment, n), n};
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_zero_impl<alloc>)
	{
		return {allocator_type::reallocate_aligned_zero(p, alignment, n), n};
	}
}

static inline constexpr bool has_native_reallocate_aligned_zero_at_least = (has_reallocate_aligned_zero &&
																			::fast_io::details::has_reallocate_aligned_zero_at_least_impl<alloc>);

static inline ::fast_io::allocation_least_result
reallocate_aligned_zero_at_least(void *p, ::std::size_t alignment, ::std::size_t n) noexcept
	requires(!has_status && has_reallocate_aligned_zero)
{
	if constexpr (::fast_io::details::has_reallocate_aligned_zero_at_least_impl<alloc>)
	{
		return allocator_type::reallocate_aligned_zero_at_least(p, alignment, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_zero_impl<alloc>)
	{
		return {allocator_type::reallocate_aligned_zero(p, alignment, n), n};
	}
}

static inline constexpr bool has_native_reallocate_aligned_n_at_least = (has_reallocate_aligned &&
																		 (::fast_io::details::has_reallocate_aligned_n_at_least_impl<alloc> ||
																		  ::fast_io::details::has_reallocate_aligned_zero_n_at_least_impl<alloc>));
static inline ::fast_io::allocation_least_result
reallocate_aligned_n_at_least(void *p, ::std::size_t oldn, ::std::size_t alignment, ::std::size_t n) noexcept
	requires(!has_status)
{
	if constexpr (::fast_io::details::has_reallocate_aligned_n_at_least_impl<alloc>)
	{
		return allocator_type::reallocate_aligned_n_at_least(p, oldn, alignment, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_at_least_impl<alloc>)
	{
		return allocator_type::reallocate_aligned_at_least(p, alignment, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_zero_n_at_least_impl<alloc>)
	{
		return allocator_type::reallocate_aligned_zero_n_at_least(p, oldn, alignment, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_zero_at_least_impl<alloc>)
	{
		return allocator_type::reallocate_aligned_zero_at_least(p, alignment, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_impl<alloc>)
	{
		return {allocator_type::reallocate_aligned_n(p, oldn, alignment, n), n};
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_impl<alloc>)
	{
		return {allocator_type::reallocate_aligned(p, alignment, n), n};
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_zero_n_impl<alloc>)
	{
		return {allocator_type::reallocate_aligned_zero_n(p, oldn, alignment, n), n};
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_zero_impl<alloc>)
	{
		return {allocator_type::reallocate_aligned_zero(p, alignment, n), n};
	}
	else
	{
		if constexpr (
			::fast_io::details::has_reallocate_at_least_impl<alloc> ||
			::fast_io::details::has_reallocate_zero_at_least_impl<alloc> ||
			::fast_io::details::has_reallocate_n_at_least_impl<alloc> ||
			::fast_io::details::has_reallocate_zero_n_at_least_impl<alloc>)
		{
			if (alignment <= default_alignment)
			{
				return generic_allocator_adapter::reallocate_n_at_least(p, oldn, n);
			}
		}
		auto newres{::fast_io::details::allocator_pointer_aligned_at_least_impl<alloc>(alignment, n, false)};
		auto newptr{newres.ptr};
		if (p != nullptr)
		{
			if (n)
			{
				if (oldn < n)
				{
					n = oldn;
				}
				::fast_io::freestanding::nonoverlapped_bytes_copy_n(reinterpret_cast<::std::byte const *>(p), n, reinterpret_cast<::std::byte *>(newptr));
			}
			generic_allocator_adapter::deallocate_aligned_n(p, alignment, oldn);
		}
		return newres;
	}
}

static inline constexpr bool has_native_reallocate_aligned_zero_n_at_least = (has_reallocate_aligned_zero && ::fast_io::details::has_reallocate_aligned_zero_at_least_impl<alloc>);

static inline ::fast_io::allocation_least_result reallocate_aligned_zero_n_at_least(void *p, ::std::size_t oldn, ::std::size_t alignment, ::std::size_t n) noexcept
	requires(!has_status)
{
	if constexpr (::fast_io::details::has_reallocate_aligned_zero_n_at_least_impl<alloc>)
	{
		return allocator_type::reallocate_aligned_zero_n_at_least(p, oldn, alignment, n);
	}
	else if constexpr (::fast_io::details::has_reallocate_aligned_zero_n_impl<alloc>)
	{
		return {allocator_type::reallocate_aligned_zero_n(p, oldn, alignment, n), n};
	}
	else
	{
		auto newres = generic_allocator_adapter::reallocate_aligned_n_at_least(p, oldn, alignment, n);
		auto newptr{newres.ptr};
		n = newres.count;
		if (oldn < n)
		{
			::std::size_t const to_zero_bytes{static_cast<::std::size_t>(n - oldn)};
			::fast_io::freestanding::bytes_clear_n(reinterpret_cast<::std::byte *>(newptr) + oldn, to_zero_bytes);
		}
		return newres;
	}
}

static inline constexpr bool has_deallocate_aligned = (::fast_io::details::has_deallocate_aligned_impl<alloc> ||
													   ::fast_io::details::has_deallocate_impl<alloc>);
static inline void deallocate_aligned(void *p, ::std::size_t alignment) noexcept
	requires(!has_status && has_deallocate_aligned)
{
	if constexpr (::fast_io::details::has_deallocate_aligned_impl<alloc>)
	{
		allocator_type::deallocate_aligned(p, alignment);
	}
	else
	{
		if (p == nullptr)
		{
			return;
		}
		if (default_alignment < alignment)
		{
			p = reinterpret_cast<void **>(p)[-1];
		}
		allocator_type::deallocate(p);
	}
}

static inline void deallocate_aligned_n(void *p, ::std::size_t alignment, ::std::size_t n) noexcept
	requires(!has_status)
{
	if constexpr (::fast_io::details::has_deallocate_aligned_n_impl<alloc>)
	{
		allocator_type::deallocate_aligned_n(p, alignment, n);
	}
	else if constexpr (::fast_io::details::has_deallocate_aligned_impl<alloc>)
	{
		allocator_type::deallocate_aligned(p, alignment);
	}
	else
	{
		if (p == nullptr)
		{
			return;
		}
		if (default_alignment < alignment)
		{
			auto start{reinterpret_cast<void **>(p)[-1]};
			n += static_cast<::std::size_t>(reinterpret_cast<char unsigned *>(p) - reinterpret_cast<char unsigned *>(start));
			p = start;
		}
		if constexpr (::fast_io::details::has_deallocate_impl<alloc>)
		{
			allocator_type::deallocate(p);
		}
		else
		{
			allocator_type::deallocate_n(p, n);
		}
	}
}

// Handle-based allocation functions (for allocators with non-empty handle_type)
static inline constexpr bool has_native_handle_allocate{
	has_status && (::fast_io::details::has_handle_allocate_impl<alloc> ||
				   ::fast_io::details::has_handle_allocate_aligned_impl<alloc> ||
				   ::fast_io::details::has_handle_allocate_zero_impl<alloc> ||
				   ::fast_io::details::has_handle_allocate_aligned_zero_impl<alloc> ||
				   ::fast_io::details::has_handle_allocate_at_least_impl<alloc> ||
				   ::fast_io::details::has_handle_allocate_aligned_at_least_impl<alloc> ||
				   ::fast_io::details::has_handle_allocate_zero_at_least_impl<alloc> ||
				   ::fast_io::details::has_handle_allocate_aligned_zero_at_least_impl<alloc> ||
				   ::fast_io::details::has_handle_allocate_conditional_zero_impl<alloc> ||
				   ::fast_io::details::has_handle_allocate_aligned_conditional_zero_impl<alloc> ||
				   ::fast_io::details::has_handle_allocate_conditional_zero_at_least_impl<alloc> ||
				   ::fast_io::details::has_handle_allocate_aligned_conditional_zero_at_least_impl<alloc>)};

#if __has_cpp_attribute(__gnu__::__returns_nonnull__)
[[__gnu__::__returns_nonnull__]]
#endif
static inline void *handle_allocate_conditional_zero(handle_type handle, ::std::size_t n, bool zero) noexcept
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
		auto p{::operator new(n)};
		if (zero)
		{
			::fast_io::freestanding::bytes_clear_n(reinterpret_cast<::std::byte *>(p), n);
		}
		return p;
	}
	else
	{
		if constexpr (::fast_io::details::has_handle_allocate_conditional_zero_impl<alloc>)
		{
			return allocator_type::handle_allocate_conditional_zero(handle, n, zero);
		}
		else if constexpr (::fast_io::details::has_handle_allocate_conditional_zero_at_least_impl<alloc>)
		{
			return allocator_type::handle_allocate_conditional_zero_at_least(handle, n, zero).ptr;
		}
		else if constexpr (::fast_io::details::has_handle_allocate_aligned_conditional_zero_impl<alloc>)
		{
			return allocator_type::handle_allocate_aligned_conditional_zero(handle, default_alignment, n, zero);
		}
		else if constexpr (::fast_io::details::has_handle_allocate_aligned_conditional_zero_at_least_impl<alloc>)
		{
			return allocator_type::handle_allocate_aligned_conditional_zero_at_least(handle, default_alignment, n, zero).ptr;
		}
		else if constexpr (::fast_io::details::has_handle_allocate_impl<alloc> ||
						   ::fast_io::details::has_handle_allocate_at_least_impl<alloc> ||
						   ::fast_io::details::has_handle_allocate_aligned_impl<alloc> ||
						   ::fast_io::details::has_handle_allocate_aligned_at_least_impl<alloc>)
		{
			if (zero)
			{
				if constexpr (::fast_io::details::has_handle_allocate_zero_impl<alloc>)
				{
					return allocator_type::handle_allocate_zero(handle, n);
				}
				else if constexpr (::fast_io::details::has_handle_allocate_zero_at_least_impl<alloc>)
				{
					return allocator_type::handle_allocate_zero_at_least(handle, n).ptr;
				}
				else if constexpr (::fast_io::details::has_handle_allocate_aligned_zero_impl<alloc>)
				{
					return allocator_type::handle_allocate_aligned_zero(handle, default_alignment, n);
				}
				else if constexpr (::fast_io::details::has_handle_allocate_aligned_zero_at_least_impl<alloc>)
				{
					return allocator_type::handle_allocate_aligned_zero_at_least(handle, default_alignment, n).ptr;
				}
				else
				{
					auto p{generic_allocator_adapter::handle_allocate(handle, n)};
					::fast_io::freestanding::bytes_clear_n(reinterpret_cast<::std::byte *>(p), n);
					return p;
				}
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
				else if constexpr (::fast_io::details::has_handle_allocate_aligned_impl<alloc>)
				{
					return allocator_type::handle_allocate_aligned(handle, default_alignment, n);
				}
				else
				{
					return allocator_type::handle_allocate_aligned_at_least(handle, default_alignment, n).ptr;
				}
			}
		}
		else
		{
			if constexpr (::fast_io::details::has_handle_allocate_zero_impl<alloc>)
			{
				return allocator_type::handle_allocate_zero(handle, n);
			}
			else if constexpr (::fast_io::details::has_handle_allocate_zero_at_least_impl<alloc>)
			{
				return allocator_type::handle_allocate_zero_at_least(handle, n).ptr;
			}
			else if constexpr (::fast_io::details::has_handle_allocate_aligned_zero_impl<alloc>)
			{
				return allocator_type::handle_allocate_aligned_zero(handle, default_alignment, n);
			}
			else if constexpr (::fast_io::details::has_handle_allocate_aligned_zero_at_least_impl<alloc>)
			{
				return allocator_type::handle_allocate_aligned_zero_at_least(handle, default_alignment, n).ptr;
			}
			else
			{
				::fast_io::fast_terminate();
			}
		}
	}
}
