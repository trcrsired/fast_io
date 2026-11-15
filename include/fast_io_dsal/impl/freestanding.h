#pragma once

namespace fast_io::operations::defines
{

template <typename Iter1, typename Snt, typename Iter2>
struct memory_algorithm_define_type
{
	explicit constexpr memory_algorithm_define_type() noexcept = default;
};

template <typename Iter1, typename Snt, typename Iter2>
inline constexpr memory_algorithm_define_type<Iter1, Snt, Iter2> memory_algorithm_define{};

template <typename Iter1, typename Snt, typename Iter2>
concept has_uninitialized_relocate_define = ::std::sentinel_for<Snt, Iter1> && requires(Iter1 first, Snt last, Iter2 dest) {
	{ uninitialized_relocate_define(::fast_io::operations::defines::memory_algorithm_define<Iter1, Snt, Iter2>, first, last, dest) } -> ::std::same_as<Iter2>;
};

template <typename Iter1, typename Snt, typename Iter2>
concept has_uninitialized_relocate_backward_define = ::std::sentinel_for<Snt, Iter1> && requires(Iter1 first, Snt last, Iter2 dest) {
	{ uninitialized_relocate_backward_define(::fast_io::operations::defines::memory_algorithm_define<Iter1, Snt, Iter2>, first, last, dest) } -> ::std::same_as<Iter2>;
};

template <typename Iter1, typename Snt, typename Iter2>
concept has_uninitialized_move_define = ::std::sentinel_for<Snt, Iter1> && requires(Iter1 first, Snt last, Iter2 dest) {
	{ uninitialized_move_define(::fast_io::operations::defines::memory_algorithm_define<Iter1, Snt, Iter2>, first, last, dest) } -> ::std::same_as<Iter2>;
};

template <typename Iter1, typename Snt, typename Iter2>
concept has_uninitialized_move_backward_define = ::std::sentinel_for<Snt, Iter1> && requires(Iter1 first, Snt last, Iter2 dest) {
	{ uninitialized_move_backward_define(::fast_io::operations::defines::memory_algorithm_define<Iter1, Snt, Iter2>, first, last, dest) } -> ::std::same_as<Iter2>;
};

} // namespace fast_io::operations::defines

namespace fast_io::details
{

template <::std::input_or_output_iterator Iter1, ::std::sentinel_for<Iter1> Sent, ::std::input_or_output_iterator Iter2>
inline constexpr Iter2 uninitialized_relocate_ignore(Iter1 first, Sent last, Iter2 dest)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::std::is_nothrow_constructible_v<::std::iter_value_t<Iter2>, decltype(::std::move(*first))> ||
								   !::std::is_nothrow_destructible_v<::std::iter_value_t<Iter1>>)
{
	if constexpr (::std::contiguous_iterator<Iter1> && !::std::is_pointer_v<Iter1> && ::std::contiguous_iterator<Iter2> && !::std::is_pointer_v<Iter2>)
	{
		return uninitialized_relocate_ignore(::std::to_address(first), ::std::to_address(last),
													::std::to_address(dest)) -
			   ::std::to_address(dest) + dest;
	}
	else if constexpr (::std::contiguous_iterator<Iter1> && !::std::is_pointer_v<Iter1>)
	{
		return uninitialized_relocate_ignore(::std::to_address(first), ::std::to_address(last),
													dest);
	}
	else if constexpr (::std::contiguous_iterator<Iter2> && !::std::is_pointer_v<Iter2>)
	{
		return uninitialized_relocate_ignore(first, last, ::std::to_address(dest)) -
			   ::std::to_address(dest) + dest;
	}
	else
	{
		using iter1valuetype = ::std::iter_value_t<Iter1>;
		using iter2valuetype = ::std::iter_value_t<Iter2>;
		if constexpr (::std::is_pointer_v<Iter1> && ::std::is_pointer_v<Iter2> &&
					  (::fast_io::freestanding::is_trivially_copyable_or_relocatable_v<iter1valuetype> &&
					   ::fast_io::freestanding::is_trivially_copyable_or_relocatable_v<iter2valuetype> &&
					   (::std::same_as<iter1valuetype, iter2valuetype> ||
						((::std::integral<iter1valuetype> || ::std::same_as<iter1valuetype, ::std::byte>) &&
						 (::std::integral<iter2valuetype> || ::std::same_as<iter2valuetype, ::std::byte>) &&
						 sizeof(iter1valuetype) == sizeof(iter2valuetype)))))
		{
			FAST_IO_IF_NOT_CONSTEVAL
			{
				return reinterpret_cast<Iter2>(::fast_io::freestanding::bytes_copy(reinterpret_cast<::std::byte const *>(first), reinterpret_cast<::std::byte const *>(last), reinterpret_cast<::std::byte *>(dest)));
			}
		}
		// we do not allow move constructor to throw EH.
		while (first != last)
		{
			::new (static_cast<void *>(::std::addressof(*dest))) iter2valuetype(::std::move(*first));

			if constexpr (!::std::is_trivially_destructible_v<iter1valuetype>)
			{
				if constexpr (::std::is_pointer_v<Iter1>)
				{
					first->~iter1valuetype();
				}
				else
				{
					__builtin_addressof(*first)->~iter1valuetype();
				}
			}
			++first;
			++dest;
		}
		return dest;
	}
}

template <::std::bidirectional_iterator Iter1, ::std::sentinel_for<Iter1> Snt, ::std::bidirectional_iterator Iter2>
inline constexpr Iter2 uninitialized_relocate_backward_ignore(Iter1 first, Snt last, Iter2 dest)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!::std::is_nothrow_constructible_v<::std::iter_value_t<Iter2>, decltype(::std::move(*first))> ||
								   !::std::is_nothrow_destructible_v<::std::iter_value_t<Iter1>>)
{
	// Semantics:
	//   Relocate the range [first, last) into the uninitialized memory ending at `dest`.
	//   `dest` is treated as the end iterator (one past the last element) of the destination range.
	//   The function returns the begin iterator of the destination range:
	//       dest - (last - first)

	if constexpr (::std::contiguous_iterator<Iter1> && !::std::is_pointer_v<Iter1> &&
				  ::std::contiguous_iterator<Iter2> && !::std::is_pointer_v<Iter2>)
	{
		return uninitialized_relocate_backward_ignore(::std::to_address(first),
															 ::std::to_address(last),
															 ::std::to_address(dest)) -
			   ::std::to_address(dest) + dest;
	}
	else if constexpr (::std::contiguous_iterator<Iter1> && !::std::is_pointer_v<Iter1>)
	{
		return uninitialized_relocate_backward_ignore(::std::to_address(first),
															 ::std::to_address(last),
															 dest);
	}
	else if constexpr (::std::contiguous_iterator<Iter2> && !::std::is_pointer_v<Iter2>)
	{
		return uninitialized_relocate_backward_ignore(first,
															 last,
															 ::std::to_address(dest)) -
			   ::std::to_address(dest) + dest;
	}
	else
	{
		using iter1valuetype = ::std::iter_value_t<Iter1>;
		using iter2valuetype = ::std::iter_value_t<Iter2>;

		// Fast path: trivial relocation via byte copy
		if constexpr (::std::is_pointer_v<Iter1> && ::std::is_pointer_v<Iter2> &&
					  (::fast_io::freestanding::is_trivially_copyable_or_relocatable_v<iter1valuetype> &&
					   ::fast_io::freestanding::is_trivially_copyable_or_relocatable_v<iter2valuetype> &&
					   (::std::same_as<iter1valuetype, iter2valuetype> ||
						((::std::integral<iter1valuetype> || ::std::same_as<iter1valuetype, ::std::byte>) &&
						 (::std::integral<iter2valuetype> || ::std::same_as<iter2valuetype, ::std::byte>) &&
						 sizeof(iter1valuetype) == sizeof(iter2valuetype)))))
		{
			FAST_IO_IF_NOT_CONSTEVAL
			{
				auto const firstbyteptr{reinterpret_cast<::std::byte const *>(first)};
				auto const lastbyteptr{reinterpret_cast<::std::byte const *>(last)};
				auto const n{lastbyteptr - firstbyteptr};

				// Compute the beginning of the destination range
				auto const destfirst{reinterpret_cast<::std::byte *>(dest) - n};

				// Perform the raw byte copy
				::fast_io::freestanding::bytes_copy(firstbyteptr,
													lastbyteptr,
													destfirst);

				return reinterpret_cast<Iter2>(destfirst);
			}
		}
		// Generic slow path:
		//   Move-construct elements in reverse order into uninitialized memory,
		//   then destroy the original elements.
		while (first != last)
		{
			--last;
			--dest;
			::new (static_cast<void *>(::std::addressof(*dest))) iter2valuetype(::std::move(*last));

			if constexpr (!::std::is_trivially_destructible_v<iter1valuetype>)
			{
				if constexpr (::std::is_pointer_v<Iter1>)
				{
					last->~iter1valuetype();
				}
				else
				{
					__builtin_addressof(*last)->~iter1valuetype();
				}
			}
		}

		// Return the begin iterator of the relocated range
		return dest;
	}
}

} // namespace fast_io::details
