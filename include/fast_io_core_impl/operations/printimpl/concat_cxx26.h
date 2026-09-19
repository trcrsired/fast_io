#pragma once

namespace fast_io::details::decay
{

template <typename ch_type, typename T>
concept strlike_construct_define_nothrow =
	::std::integral<ch_type> &&
	noexcept(strlike_construct_define(io_strlike_type<ch_type, T>, ::std::declval<ch_type const *>(),
									  ::std::declval<ch_type const *>()));

template <typename ch_type, typename T>
concept strlike_construct_single_character_define_nothrow =
	::std::integral<ch_type> &&
	noexcept(strlike_construct_single_character_define(io_strlike_type<ch_type, T>, ::std::declval<ch_type>()));

template <::std::integral char_type, typename T>
inline constexpr ::std::size_t calculate_scatter_reserve_size_unit()
	FAST_IO_HERBCEPTIONS_THROWS
{
	using real_type = ::std::remove_cvref_t<T>;
	if constexpr (reserve_printable<char_type, real_type>)
	{
		constexpr ::std::size_t sz{print_reserve_size(io_reserve_type<char_type, real_type>)};
		return sz;
	}
	else
	{
		return 0;
	}
}

template <::std::integral char_type, typename... Args>
inline constexpr ::std::size_t calculate_scatter_reserve_size()
	FAST_IO_HERBCEPTIONS_THROWS
{
	::std::size_t total{};
	template for (constexpr auto i : ::fast_io::details::index_array_range<0zu, sizeof...(Args)>)
	{
		total = ::fast_io::details::intrinsics::add_or_overflow_die(
			total, calculate_scatter_reserve_size_unit<char_type, Args...[i]>());
	}
	return total;
}

template <::std::integral char_type, typename... Args>
inline constexpr ::std::size_t calculate_scatter_dynamic_reserve_size_with_scatter(Args... args)
	FAST_IO_HERBCEPTIONS_THROWS_IF((::fast_io::details::has_any_print_define_operations_not_noexcept<char_type, Args> ||
								  ...))
{
	::std::size_t total{};
	template for (constexpr auto i : ::fast_io::details::index_array_range<0zu, sizeof...(Args)>)
	{
		using argtype = ::std::remove_cvref_t<Args...[i]>;
		if constexpr (dynamic_reserve_printable<char_type, argtype>)
		{
			total = ::fast_io::details::intrinsics::add_or_overflow_die(
				total, print_reserve_size(io_reserve_type<char_type, argtype>, args...[i]));
		}
		else if constexpr (scatter_printable<char_type, argtype>)
		{
			total = ::fast_io::details::intrinsics::add_or_overflow_die(
				total, print_scatter_define(io_reserve_type<char_type, argtype>, args...[i]).len);
		}
	}
	return total;
}

template <bool line, ::std::integral char_type, typename... Args>
inline constexpr char_type *print_reserve_define_chain_impl(char_type *p, Args... args)
	FAST_IO_HERBCEPTIONS_THROWS_IF((::fast_io::details::has_any_print_define_operations_not_noexcept<char_type, Args> ||
								  ...))
{
	template for (constexpr auto i : ::fast_io::details::index_array_range<0zu, sizeof...(Args)>)
	{
		p = print_reserve_define(io_reserve_type<char_type, ::std::remove_cvref_t<Args...[i]>>, p, args...[i]);
	}
	if constexpr (line)
	{
		*p = char_literal_v<u8'\n', char_type>;
		++p;
	}
	return p;
}

template <bool line, ::std::integral char_type, typename... Args>
inline constexpr char_type *print_reserve_define_chain_scatter_impl(char_type *p, Args... args)
	FAST_IO_HERBCEPTIONS_THROWS_IF((::fast_io::details::has_any_print_define_operations_not_noexcept<char_type, Args> ||
								  ...))
{
	template for (constexpr auto i : ::fast_io::details::index_array_range<0zu, sizeof...(Args)>)
	{
		using argtype = ::std::remove_cvref_t<Args...[i]>;
		if constexpr (dynamic_reserve_printable<char_type, argtype> || reserve_printable<char_type, argtype>)
		{
			p = print_reserve_define(io_reserve_type<char_type, argtype>, p, args...[i]);
		}
		else
		{
			auto sc{print_scatter_define(io_reserve_type<char_type, argtype>, args...[i])};
			p = non_overlapped_copy_n(sc.base, sc.len, p);
		}
	}
	if constexpr (line)
	{
		*p = char_literal_v<u8'\n', char_type>;
		++p;
	}
	return p;
}

template <bool line, ::std::integral ch_type, typename T, typename... Args>
consteval bool basic_general_concat_phase1_may_throw() noexcept
{
	if constexpr (sizeof...(Args) == 0)
	{
		if constexpr (line)
		{
			if constexpr (single_character_constructible_strlike<ch_type, T>)
			{
				return !strlike_construct_single_character_define_nothrow<ch_type, T>;
			}
			else
			{
				return !strlike_construct_define_nothrow<ch_type, T>;
			}
		}
		else
		{
			return false;
		}
	}
	else if constexpr (buffer_strlike<ch_type, T>)
	{
		return ::fast_io::details::decayed_output_stream_print_may_throw<
			decltype(io_strlike_ref(::fast_io::io_alias, ::std::declval<T &>())), Args...>;
	}
	else if constexpr ((!line) && sizeof...(Args) == 1 && (scatter_printable<ch_type, Args> && ...))
	{
		return !strlike_construct_define_nothrow<ch_type, T> ||
			   (::fast_io::details::has_any_print_define_operations_not_noexcept<ch_type, Args> || ...);
	}
	else if constexpr ((reserve_printable<ch_type, Args> && ...))
	{
		return !strlike_construct_define_nothrow<ch_type, T> ||
			   (::fast_io::details::has_any_print_define_operations_not_noexcept<ch_type, Args> || ...);
	}
	else
	{
		return !strlike_construct_define_nothrow<ch_type, T> ||
			   ::fast_io::details::decayed_output_stream_print_may_throw<
				   decltype(io_strlike_ref(
					   ::fast_io::io_alias,
					   ::std::declval<::fast_io::details::basic_concat_buffer<ch_type> &>())),
				   Args...>;
	}
}

template <bool line, ::std::integral ch_type, typename T, typename... Args>
inline constexpr T basic_general_concat_phase1_decay_impl(Args... args)
	FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::details::decay::basic_general_concat_phase1_may_throw<line, ch_type, T, Args...>())
{
	if constexpr (sizeof...(Args) == 0)
	{
		if constexpr (line)
		{
			if constexpr (single_character_constructible_strlike<ch_type, T>)
			{
				return strlike_construct_single_character_define(io_strlike_type<ch_type, T>,
																 char_literal_v<u8'\n', ch_type>);
			}
			else
			{
				return strlike_construct_define(io_strlike_type<ch_type, T>,
												__builtin_addressof(char_literal_v<u8'\n', ch_type>),
												__builtin_addressof(char_literal_v<u8'\n', ch_type>) + 1);
			}
		}
		else
		{
			return {};
		}
	}
	else if constexpr (buffer_strlike<ch_type, T>)
	{
		T str;
		::fast_io::operations::decay::print_freestanding_decay<line>(
			io_strlike_ref(::fast_io::io_alias, str), args...);
		return str;
	}
	else if constexpr ((!line) && sizeof...(Args) == 1 && (scatter_printable<ch_type, Args> && ...))
	{
		basic_io_scatter_t<ch_type> scatter{
			print_scatter_define(io_reserve_type<ch_type, ::std::remove_cvref_t<Args...[0]>>, args...[0])};
		return strlike_construct_define(io_strlike_type<ch_type, T>, scatter.base, scatter.base + scatter.len);
	}
	else if constexpr ((reserve_printable<ch_type, Args> && ...))
	{
		constexpr ::std::size_t sz{calculate_scatter_reserve_size<ch_type, Args...>()};
		if constexpr (line)
		{
			static_assert(sz != SIZE_MAX, "overflow\n");
		}
		constexpr ::std::size_t sz_with_line{sz + static_cast<::std::size_t>(line)};
		ch_type buffer[sz_with_line];
		auto p{print_reserve_define_chain_impl<line>(buffer, args...)};
		return strlike_construct_define(io_strlike_type<ch_type, T>, buffer, p);
	}
	else
	{
		::fast_io::details::basic_concat_buffer<ch_type> buffer;
		::fast_io::operations::decay::print_freestanding_decay<line>(
			io_strlike_ref(::fast_io::io_alias, buffer), args...);
		return strlike_construct_define(io_strlike_type<ch_type, T>, buffer.buffer_begin, buffer.buffer_curr);
	}
}

} // namespace fast_io::details::decay

namespace fast_io
{

template <bool line, ::std::integral char_type, typename T, typename... Args>
	requires strlike<char_type, T>
inline constexpr T basic_general_concat(Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS_IF(::fast_io::details::decay::basic_general_concat_phase1_may_throw<line, char_type, T,
																								  decltype(io_print_forward<char_type>(io_print_alias(args)))...>())
{
	return ::fast_io::details::decay::basic_general_concat_phase1_decay_impl<line, char_type, T>(
		io_print_forward<char_type>(io_print_alias(args))...);
}

} // namespace fast_io
