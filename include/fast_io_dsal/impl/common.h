#pragma once

namespace fast_io
{

namespace containers
{

inline constexpr ::std::size_t npos{::std::numeric_limits<::std::size_t>::max()};

}

using ::fast_io::containers::npos;

} // namespace fast_io

namespace fast_io::containers::details
{
template <typename handle>
concept is_trivally_stored_allocator_handle = ::fast_io::freestanding::is_zero_default_constructible_v<handle> &&
											  ::fast_io::freestanding::is_trivially_copyable_or_relocatable_v<handle> &&
											  ::std::is_trivially_copy_constructible_v<handle> &&
											  sizeof(handle) <= sizeof(handle *) && alignof(handle) <= alignof(handle *);

template <typename handle>
struct handle_holder
{
	using handle_type = handle;
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
	[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address)
	[[no_unique_address]]
#endif
#endif
	::std::conditional_t<is_trivally_stored_allocator_handle<handle>, handle, handle *> value;
	inline constexpr handle_holder() noexcept
		: value{}
	{}
	inline constexpr handle_holder(decltype(nullptr)) noexcept = delete;
	inline constexpr handle_holder(handle small_object) noexcept
		requires(is_trivally_stored_allocator_handle<handle>)
		: value(small_object)
	{}
	inline constexpr handle_holder(handle *small_object) noexcept
		requires is_trivally_stored_allocator_handle<handle>
		: value(*small_object)
	{}
	template <typename A>
	inline constexpr handle_holder(A &&large_object) noexcept
		requires(::std::same_as<::std::remove_cvref_t<A>, handle> && !is_trivally_stored_allocator_handle<handle>)
	{
		value = ::fast_io::typed_generic_allocator_adapter<handle, handle>::handle_allocate(large_object, 1);
		::std::construct_at(value, ::std::forward<handle>(large_object));
	}
	inline constexpr handle_holder(handle *large_object) noexcept
		requires(!is_trivally_stored_allocator_handle<handle>)
		: value(large_object)
	{}
	inline constexpr handle_holder(handle_holder const &) noexcept = default;
	inline constexpr handle_holder &operator=(handle_holder const &) noexcept = default;
	inline constexpr handle_holder(handle_holder &&) noexcept = default;
	inline constexpr handle_holder &operator=(handle_holder &&) noexcept = default;
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr handle const &get() const noexcept
	{
		if constexpr (is_trivally_stored_allocator_handle<handle>)
		{
			return value;
		}
		else
		{
			return *value;
		}
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr handle &get() noexcept
	{
		if constexpr (is_trivally_stored_allocator_handle<handle>)
		{
			return value;
		}
		else
		{
			return *value;
		}
	}
};
template <::std::equality_comparable handle>
inline constexpr auto operator==(handle_holder<handle> left, handle_holder<handle> right) noexcept
{
	return left.get() == right.get();
}

template <typename sztype>
inline constexpr sztype cal_grow_twice_size_size_based(sztype cap) noexcept
{
	constexpr sztype mx_value2{::std::numeric_limits<sztype>::max()};
	constexpr sztype mx_value{mx_value2};
	constexpr sztype mx_half_value{mx_value >> 1u};
	if (cap == mx_value)
	{
		::fast_io::fast_terminate();
	}
	else if (mx_half_value < cap)
	{
		return mx_value;
	}
	else if (!cap)
	{
		return 1u;
	}
	return static_cast<sztype>(cap << 1u);
}

template <::std::size_t size, bool trivial>
inline constexpr ::std::size_t cal_grow_twice_size(::std::size_t cap) noexcept
{
	constexpr ::std::size_t mx_value2{::std::numeric_limits<::std::size_t>::max() / size};
	constexpr ::std::size_t mx_value{trivial ? mx_value2 * size : mx_value2};
	constexpr ::std::size_t mx_half_value{mx_value >> 1u};
	if (cap == mx_value)
	{
		::fast_io::fast_terminate();
	}
	else if (mx_half_value < cap)
	{
		return mx_value;
	}
	else if (!cap)
	{
		return size;
	}
	return static_cast<::std::size_t>(cap << 1);
}

template <::std::unsigned_integral T>
inline constexpr T reduce_sizet_to_small_sizetype(::std::size_t count) noexcept
{
	if constexpr (sizeof(::std::size_t) <= sizeof(T))
	{
		return static_cast<T>(count);
	}
	else
	{
		constexpr T mx{::std::numeric_limits<T>::max()};
		if (mx < count) [[unlikely]]
		{
			return mx;
		}
		return static_cast<T>(count);
	}
}

} // namespace fast_io::containers::details
