#pragma once

namespace fast_io
{

namespace details
{

template <typename T>
inline constexpr void close_basic_io_buffer(T &)
	FAST_IO_HERBCEPTIONS_THROWS_IF(
		(T::traits_type::mode & ::fast_io::buffer_mode::out) == ::fast_io::buffer_mode::out &&
		!::fast_io::operations::decay::defines::output_stream_operations_nothrow<
			decltype(::fast_io::operations::output_stream_ref(::std::declval<typename T::handle_type &>()))>);
template <typename T>
inline constexpr void clear_basic_io_buffer_pointers(T &) noexcept;
template <typename T>
inline constexpr void destroy_basic_io_buffer(T &) noexcept;

template <typename T, typename... Args>
concept has_reopen_impl = requires(T &&t, Args &&...args) { t.reopen(::std::forward<Args>(args)...); };

template <typename T>
concept has_close_impl = requires(T &&t) { t.close(); };

} // namespace details

template <typename handletype, typename iobuffertraits>
class basic_io_buffer
{
public:
	using handle_type = handletype;
	using traits_type = iobuffertraits;
	using input_char_type = typename traits_type::input_char_type;
	using output_char_type = typename traits_type::output_char_type;

	using input_buffer_type = ::std::conditional_t<(traits_type::mode & buffer_mode::in) == buffer_mode::in,
												   basic_io_buffer_pointers<input_char_type>, empty_buffer_pointers>;
	using output_buffer_type = ::std::conditional_t<(traits_type::mode & buffer_mode::out) == buffer_mode::out,
													basic_io_buffer_pointers<output_char_type>, empty_buffer_pointers>;

#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
	[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address) >= 201803
	[[no_unique_address]]
#endif
#endif
	input_buffer_type input_buffer;
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
	[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address) >= 201803
	[[no_unique_address]]
#endif
#endif
	output_buffer_type output_buffer;
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
	[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address) >= 201803
	[[no_unique_address]]
#endif
#endif
	handle_type handle;

	inline explicit constexpr basic_io_buffer() = default;
	template <typename... Args>
		requires(::std::constructible_from<handle_type, Args...>)
	inline explicit constexpr basic_io_buffer(Args &&...args)
		FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(handle_type(::std::forward<Args>(args)...))
		: handle(::std::forward<Args>(args)...)
	{
	}
	inline basic_io_buffer &operator=(basic_io_buffer const &) = delete;
	inline basic_io_buffer(basic_io_buffer const &) = delete;
	inline constexpr basic_io_buffer(basic_io_buffer &&__restrict other) noexcept
		: input_buffer(::std::move(other.input_buffer)), output_buffer(::std::move(other.output_buffer)),
		  handle(::std::move(other.handle))
	{
		other.input_buffer = {};
		other.output_buffer = {};
	}
	template <typename... Args>
		requires(::std::constructible_from<handle_type, Args...>)
	inline constexpr void reopen(Args &&...args)
		FAST_IO_HERBCEPTIONS_THROWS_IF(
			!noexcept(handle_type(::std::declval<Args>()...)) ||
			requires { requires !noexcept(::std::declval<handle_type &>().reopen(::std::declval<Args>()...)); } ||
			requires { requires !noexcept(::std::declval<handle_type &>() = ::std::declval<handle_type>()); } ||
			requires { requires !noexcept(::std::declval<handle_type &>().close()); } ||
			((traits_type::mode & ::fast_io::buffer_mode::out) == ::fast_io::buffer_mode::out &&
			 requires { ::fast_io::operations::output_stream_ref(::std::declval<handle_type &>()); } &&
			 !::fast_io::operations::decay::defines::output_stream_operations_nothrow<
				 decltype(::fast_io::operations::output_stream_ref(::std::declval<handle_type &>()))>))
	{
		::fast_io::details::close_basic_io_buffer(*this);
		::fast_io::details::clear_basic_io_buffer_pointers(*this);
		if constexpr (::fast_io::details::has_reopen_impl<handle_type, Args...>)
		{
			handle.reopen(::std::forward<Args>(args)...);
		}
		else
		{
			handle = handle_type(::std::forward<Args>(args)...);
		}
	}

	inline constexpr void close()
		FAST_IO_HERBCEPTIONS_THROWS_IF(
			requires { requires !noexcept(::std::declval<handle_type &>().close()); } ||
			((traits_type::mode & ::fast_io::buffer_mode::out) == ::fast_io::buffer_mode::out &&
			 requires { ::fast_io::operations::output_stream_ref(::std::declval<handle_type &>()); } &&
			 !::fast_io::operations::decay::defines::output_stream_operations_nothrow<
				 decltype(::fast_io::operations::output_stream_ref(::std::declval<handle_type &>()))>))
	{
		::fast_io::details::close_basic_io_buffer(*this);
		::fast_io::details::clear_basic_io_buffer_pointers(*this);
		if constexpr (::fast_io::details::has_close_impl<handle_type>)
		{
			handle.close();
		}
		else
		{
			handle = handle_type();
		}
	}
	inline constexpr basic_io_buffer &operator=(basic_io_buffer &&__restrict other) noexcept
	{
		if (__builtin_addressof(other) == this) [[unlikely]]
		{
			return *this;
		}
		::fast_io::details::destroy_basic_io_buffer(*this);
		input_buffer = ::std::move(other.input_buffer);
		output_buffer = ::std::move(other.output_buffer);
		handle = ::std::move(other.handle);
		other.input_buffer = {};
		other.output_buffer = {};
		return *this;
	}

	inline constexpr ~basic_io_buffer()
	{
		::fast_io::details::destroy_basic_io_buffer(*this);
	}
};

} // namespace fast_io
