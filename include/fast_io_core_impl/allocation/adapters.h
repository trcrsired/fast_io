#pragma once

// To make a constexpr allocator, we need ::std::allocator. Because only new expression and
// ::std::allocator<T>::allocate are allowed in constexpr functions. See https://github.com/microsoft/STL/issues/1532
// https://github.com/microsoft/STL/issues/4002 gcc and clang provide constexpr new, but still won't compile.
// ::std::allocator<T> is NOT freestanding.

namespace fast_io
{

namespace details
{

#include "has_methods_detect.h"

template <typename alloc>
concept has_default_alignment_impl = requires(::std::size_t n) { alloc::default_alignment; };

template <typename alloc>
inline constexpr ::std::size_t calculate_default_alignment() noexcept
{
	if constexpr (has_default_alignment_impl<alloc>)
	{
		return alloc::default_alignment;
	}
	else
	{
		return __STDCPP_DEFAULT_NEW_ALIGNMENT__;
	}
}

inline constexpr ::std::size_t allocator_compute_aligned_total_size_impl(::std::size_t alignment, ::std::size_t n) noexcept
{
	constexpr ::std::size_t mxn{::std::numeric_limits<::std::size_t>::max()},
		sizeofptr{sizeof(void *)},
		mxmptr{mxn - sizeofptr};
	if (alignment < sizeofptr)
	{
		alignment = sizeofptr;
	}
	if (alignment > mxmptr)
	{
		::fast_io::fast_terminate();
	}
	::std::size_t total_extra_space{alignment + sizeofptr};
	::std::size_t upperlimit{static_cast<::std::size_t>(mxn - total_extra_space)};
	if (n > upperlimit)
	{
		::fast_io::fast_terminate();
	}
	return n + total_extra_space;
}

inline void *allocator_adjust_ptr_to_aligned_impl(void *p, ::std::size_t alignment) noexcept
{
	void *aligned_ptr{reinterpret_cast<void *>((reinterpret_cast<::std::uintptr_t>(p) + alignment) & (0 - alignment))};
	reinterpret_cast<void **>(aligned_ptr)[-1] = p;
	return aligned_ptr;
}

template <typename>
inline constexpr void *allocator_pointer_aligned_impl(::std::size_t, ::std::size_t, bool) noexcept;

template <typename>
inline constexpr ::fast_io::allocation_least_result allocator_pointer_aligned_at_least_impl(::std::size_t, ::std::size_t, bool) noexcept;

template <typename alloc>
inline constexpr void *status_allocator_pointer_aligned_impl(typename alloc::handle_type, ::std::size_t, ::std::size_t, bool) noexcept;

template <typename alloc>
inline constexpr ::fast_io::allocation_least_result status_allocator_pointer_aligned_at_least_impl(typename alloc::handle_type, ::std::size_t, ::std::size_t, bool) noexcept;


template <typename alloc>
concept native_allocate_aligned_has_none_zero_ops =
	::fast_io::details::has_allocate_aligned_impl<alloc> ||
	::fast_io::details::has_allocate_aligned_at_least_impl<alloc>;
template <typename alloc>
concept native_allocate_aligned_has_zero_ops =
	::fast_io::details::has_allocate_aligned_zero_impl<alloc> ||
	::fast_io::details::has_allocate_aligned_zero_at_least_impl<alloc>;

template <typename alloc>
concept native_allocate_aligned_has_ops =
	::fast_io::details::native_allocate_aligned_has_none_zero_ops<alloc> ||
	::fast_io::details::native_allocate_aligned_has_zero_ops<alloc>;

template <typename alloc>
concept native_allocate_has_none_zero_ops = ::fast_io::details::has_allocate_impl<alloc> ||
											::fast_io::details::has_allocate_at_least_impl<alloc> ||
											::fast_io::details::native_allocate_aligned_has_none_zero_ops<alloc>;
template <typename alloc>
concept native_allocate_has_zero_ops =
	::fast_io::details::has_allocate_zero_impl<alloc> ||
	::fast_io::details::has_allocate_zero_at_least_impl<alloc> || ::fast_io::details::native_allocate_aligned_has_zero_ops<alloc>;
} // namespace details

#if 0
#include "allocator_adapter_flags.h"
#endif

template <typename alloc>
class generic_allocator_adapter
{
public:
	using allocator_type = alloc;
	static inline constexpr bool has_status{::fast_io::details::has_non_empty_handle_type<allocator_type>};
	template <typename T, bool = false>
	struct has
	{
		using type = allocator_type; // any meaningless type other than void
	};
	template <typename T>
	struct has<T, true>
	{
		using type = typename T::handle_type;
	};
	using handle_type = typename has<allocator_type, has_status>::type;
	static inline constexpr ::std::size_t default_alignment{::fast_io::details::calculate_default_alignment<allocator_type>()};

#include "adapters_no_status.h"
#include "adapters_status.h"
};

namespace details
{

template <typename alloc>
inline constexpr void *allocator_pointer_aligned_impl(::std::size_t alignment, ::std::size_t n, bool zero) noexcept
{
	static_assert(::fast_io::generic_allocator_adapter<alloc>::has_native_allocate);

	constexpr ::std::size_t defaultalignment{::fast_io::details::calculate_default_alignment<alloc>()};
	bool const alignedadjustment{defaultalignment < alignment};
	if (alignedadjustment)
	{
		n = ::fast_io::details::allocator_compute_aligned_total_size_impl(alignment, n);
	}
	void *p = ::fast_io::generic_allocator_adapter<alloc>::allocate_conditional_zero(n, zero);
	if (alignedadjustment)
	{
		p = ::fast_io::details::allocator_adjust_ptr_to_aligned_impl(p, alignment);
	}
	return p;
}

template <typename alloc>
inline constexpr ::fast_io::allocation_least_result allocator_pointer_aligned_at_least_impl(::std::size_t alignment, ::std::size_t n, bool zero) noexcept
{
	static_assert(::fast_io::generic_allocator_adapter<alloc>::has_native_allocate);

	constexpr ::std::size_t defaultalignment{::fast_io::details::calculate_default_alignment<alloc>()};
	bool const alignedadjustment{defaultalignment < alignment};
	if (alignedadjustment)
	{
		n = ::fast_io::details::allocator_compute_aligned_total_size_impl(alignment, n);
	}
	::fast_io::allocation_least_result res = ::fast_io::generic_allocator_adapter<alloc>::allocate_conditional_zero_at_least(n, zero);
	if (alignedadjustment)
	{
		auto resptr{res.ptr};
		auto aligned_ptr = ::fast_io::details::allocator_adjust_ptr_to_aligned_impl(resptr, alignment);
		res = {aligned_ptr, res.count - static_cast<::std::size_t>(reinterpret_cast<char unsigned *>(aligned_ptr) - reinterpret_cast<char unsigned *>(resptr))};
	}
	return res;
}

#if 0
template <typename alloc, bool zero>
inline constexpr void *status_allocator_pointer_aligned_impl(typename alloc::handle_type handle, ::std::size_t alignment, ::std::size_t n) noexcept
{
	static_assert(::fast_io::generic_allocator_adapter<alloc>::has_native_handle_allocate);

	constexpr ::std::size_t defaultalignment{::fast_io::details::calculate_default_alignment<alloc>()};
	bool const alignedadjustment{defaultalignment < alignment};
	if (alignedadjustment)
	{
		n = ::fast_io::details::allocator_compute_aligned_total_size_impl(alignment, n);
	}
	void *p;
	if constexpr (zero)
	{
		p = ::fast_io::generic_allocator_adapter<alloc>::handle_allocate_zero(handle, n);
	}
	else
	{
		p = ::fast_io::generic_allocator_adapter<alloc>::handle_allocate(handle, n);
	}
	if (alignedadjustment)
	{
		p = ::fast_io::details::allocator_adjust_ptr_to_aligned_impl(p,alignment);
	}
	return p;
}

template <typename alloc, bool zero>
inline constexpr ::fast_io::allocation_least_result status_allocator_pointer_aligned_impl(typename alloc::handle_type handle, ::std::size_t alignment, ::std::size_t n) noexcept
{
	static_assert(::fast_io::generic_allocator_adapter<alloc>::has_native_handle_allocate);

	constexpr ::std::size_t defaultalignment{::fast_io::details::calculate_default_alignment<alloc>()};
	bool const alignedadjustment{defaultalignment < alignment};
	if (alignedadjustment)
	{
		n = ::fast_io::details::allocator_compute_aligned_total_size_impl(alignment, n);
	}
	::fast_io::allocation_least_result res;
	if constexpr (zero)
	{
		res = ::fast_io::generic_allocator_adapter<alloc>::handle_allocate_zero_at_least(handle, n);
	}
	else
	{
		res = ::fast_io::generic_allocator_adapter<alloc>::handle_allocate_at_least(handle, n);
	}
	if (alignedadjustment)
	{
		auto resptr{res.ptr};
		auto aligned_ptr = ::fast_io::details::allocator_adjust_ptr_to_aligned_impl(resptr,alignment);
		res = {aligned_ptr,res.count-static_cast<::std::size_t>(reinterpret_cast<char unsigned*>(aligned_ptr)-reinterpret_cast<char unsigned*>(resptr))};
	}
	return res;
}
#endif
} // namespace details

} // namespace fast_io
