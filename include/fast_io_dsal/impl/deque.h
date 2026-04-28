#pragma once

namespace fast_io
{
namespace containers
{
namespace details
{

struct
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	deque_control_block_common
{
	::std::byte *begin_ptr, *curr_ptr;
	::std::byte **controller_ptr;
};

template <typename T>
struct deque_control_block
{
	T *begin_ptr, *curr_ptr;
	T **controller_ptr;
};

inline constexpr ::std::size_t deque_block_size_shift{12};

inline constexpr ::std::size_t deque_block_size_common{static_cast<::std::size_t>(1) << deque_block_size_shift};

template <::std::size_t sz>
inline constexpr ::std::size_t deque_block_size{sz <= (deque_block_size_common / 16u) ? ::std::bit_ceil(static_cast<::std::size_t>(deque_block_size_common / sz)) : static_cast<::std::size_t>(16u)};

template <::std::size_t sz>
inline constexpr ::std::size_t deque_block_bytes{sz * ::fast_io::containers::details::deque_block_size<sz>};

struct
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	deque_controller_block_common
{
	using replacetype = ::std::byte;
	::std::byte **controller_start_ptr;
	::std::byte **controller_start_reserved_ptr;
	::std::byte **controller_after_reserved_ptr;
	::std::byte **controller_after_ptr;
};

template <typename T>
struct deque_controller_block
{
	using replacetype = T;
	T **controller_start_ptr;
	T **controller_start_reserved_ptr;
	T **controller_after_reserved_ptr;
	T **controller_after_ptr;
};

template <typename T>
struct deque_controller
{
	using replacetype = T;
	using controlreplacetype = T *;
	::fast_io::containers::details::deque_control_block<T> front_block;
	controlreplacetype front_end_ptr;
	::fast_io::containers::details::deque_control_block<T> back_block;
	controlreplacetype back_end_ptr;
	::fast_io::containers::details::deque_controller_block<T> controller_block;
};

struct
#if __has_cpp_attribute(__gnu__::__may_alias__)
	[[__gnu__::__may_alias__]]
#endif
	deque_controller_common
{
	using replacetype = ::std::byte;
	using controlreplacetype = ::std::byte *;
	::fast_io::containers::details::deque_control_block_common front_block;
	controlreplacetype front_end_ptr;
	::fast_io::containers::details::deque_control_block_common back_block;
	controlreplacetype back_end_ptr;
	::fast_io::containers::details::deque_controller_block_common controller_block;
};

template <typename T>
inline constexpr void deque_add_assign_signed_impl(
	::fast_io::containers::details::deque_control_block<T> &itercontent,
	::std::ptrdiff_t pos) noexcept
{
	constexpr ::std::size_t blocksize{::fast_io::containers::details::deque_block_size<sizeof(T)>};
	constexpr ::std::ptrdiff_t signedblocksize{static_cast<::std::ptrdiff_t>(blocksize)};

	auto curr_ptr{itercontent.curr_ptr};
	// Signed offset inside current block
	::std::ptrdiff_t offset{pos + (curr_ptr - itercontent.begin_ptr)};

	// Fast path: stays inside the same block
	if (static_cast<::std::size_t>(offset) < blocksize)
	{
		itercontent.curr_ptr = curr_ptr + pos;
	}
	else
	{
		// Compute block jump with correct signed semantics
		::std::ptrdiff_t node_offset;
		if (offset < 0)
		{
			node_offset = (offset + 1) / signedblocksize - 1;
		}
		else
		{
			node_offset = static_cast<::std::ptrdiff_t>(static_cast<::std::size_t>(offset) / blocksize);
		}

		// Move to the correct block
		auto begin_ptr{*(itercontent.controller_ptr += node_offset)};
		itercontent.begin_ptr = begin_ptr;

		// Normalize pointer inside the block
		itercontent.curr_ptr = begin_ptr + (offset - node_offset * signedblocksize);
	}
}

template <typename T>
inline constexpr void deque_sub_assign_signed_impl(::fast_io::containers::details::deque_control_block<T> &it, ::std::ptrdiff_t pos) noexcept
{
	::fast_io::containers::details::deque_add_assign_signed_impl(it, -pos);
}

template <typename T>
inline constexpr void deque_add_assign_unsigned_impl(::fast_io::containers::details::deque_control_block<T> &itercontent, ::std::size_t unsignedpos) noexcept
{
	using size_type = ::std::size_t;
	constexpr size_type blocksize{::fast_io::containers::details::deque_block_size<sizeof(T)>};

	auto begin_ptr{itercontent.begin_ptr};
	auto curr_ptr{itercontent.curr_ptr};
	size_type diff{static_cast<size_type>(curr_ptr - begin_ptr) + unsignedpos};
	if (diff < blocksize)
	{
		itercontent.curr_ptr = curr_ptr + unsignedpos;
	}
	else
	{
		begin_ptr = *(itercontent.controller_ptr += diff / blocksize);
		itercontent.begin_ptr = begin_ptr;
		itercontent.curr_ptr = begin_ptr + diff % blocksize;
	}
}

template <typename T>
inline constexpr void deque_sub_assign_unsigned_impl(::fast_io::containers::details::deque_control_block<T> &itercontent, ::std::size_t unsignedpos) noexcept
{
	using size_type = ::std::size_t;
	constexpr size_type blocksize{::fast_io::containers::details::deque_block_size<sizeof(T)>};
	constexpr size_type blocksizem1{blocksize - 1u};
	auto begin_ptr{itercontent.begin_ptr};
	auto curr_ptr{itercontent.curr_ptr};
	size_type offset{static_cast<size_type>(curr_ptr - begin_ptr)};
	if (unsignedpos <= offset)
	{
		itercontent.curr_ptr = curr_ptr - unsignedpos;
	}
	else
	{
		size_type diff{blocksizem1 + unsignedpos - offset};
		begin_ptr = (*(itercontent.controller_ptr -= diff / blocksize));
		itercontent.begin_ptr = begin_ptr;
		itercontent.curr_ptr = begin_ptr + (blocksizem1 - diff % blocksize);
	}
}

template <typename T>
inline constexpr T &deque_index_signed(::fast_io::containers::details::deque_control_block<T> const &itercontent, ::std::ptrdiff_t pos) noexcept
{
	using size_type = ::std::size_t;
	constexpr size_type blocksize{::fast_io::containers::details::deque_block_size<sizeof(T)>};
	constexpr ::std::ptrdiff_t signedblocksize{static_cast<::std::ptrdiff_t>(blocksize)};

	auto curr_ptr{itercontent.curr_ptr};
	// Signed offset inside current block
	::std::ptrdiff_t offset{pos + (curr_ptr - itercontent.begin_ptr)};

	// Fast path: stays inside the same block
	if (static_cast<::std::size_t>(offset) < blocksize)
	{
		return curr_ptr[pos];
	}
	else
	{
		// Compute block jump with correct signed semantics
		::std::ptrdiff_t node_offset;
		if (offset < 0)
		{
			node_offset = (offset + 1) / signedblocksize - 1;
		}
		else
		{
			node_offset = static_cast<::std::ptrdiff_t>(static_cast<::std::size_t>(offset) / blocksize);
		}
		return itercontent.controller_ptr[node_offset][offset - node_offset * signedblocksize];
	}
}

template <typename T>
inline constexpr T &deque_index_unsigned(::fast_io::containers::details::deque_control_block<T> const &itercontent, ::std::size_t unsignedpos) noexcept
{
	using size_type = ::std::size_t;
	constexpr size_type blocksize{::fast_io::containers::details::deque_block_size<sizeof(T)>};
	auto curr_ptr{itercontent.curr_ptr};
	size_type const diff{static_cast<size_type>(curr_ptr - itercontent.begin_ptr) + unsignedpos};
	if (diff < blocksize)
	{
		// Fast path: stays inside the same block
		return curr_ptr[unsignedpos];
	}
	else
	{
		return itercontent.controller_ptr[diff / blocksize][diff % blocksize];
	}
}

template <typename T>
inline constexpr T &deque_index_container_unsigned(::fast_io::containers::details::deque_control_block<T> const &itercontent, ::std::size_t unsignedpos) noexcept
{
	using size_type = ::std::size_t;
	constexpr size_type blocksize{::fast_io::containers::details::deque_block_size<sizeof(T)>};
	auto curr_ptr{itercontent.curr_ptr};
	size_type const diff{static_cast<size_type>(curr_ptr - itercontent.begin_ptr) + unsignedpos};
	// Container should not have fast path because container cannot have locality like iterator[] does
	return itercontent.controller_ptr[diff / blocksize][diff % blocksize];
}

template <typename T, bool isconst>
struct deque_iterator
{
	using value_type = T;
	using pointer = ::std::conditional_t<isconst, value_type const *, value_type *>;
	using const_pointer = value_type const *;

	using reference = ::std::conditional_t<isconst, value_type const &, value_type &>;
	using const_reference = value_type const &;

	using size_type = ::std::size_t;
	using difference_type = ::std::ptrdiff_t;

	deque_control_block<T> itercontent;

	inline constexpr deque_iterator &operator++() noexcept
	{
		if ((itercontent.begin_ptr + ::fast_io::containers::details::deque_block_size<sizeof(value_type)>) == ++itercontent.curr_ptr) [[unlikely]]
		{
			itercontent.curr_ptr = itercontent.begin_ptr = (*++itercontent.controller_ptr);
		}
		return *this;
	}

	inline constexpr deque_iterator &operator--() noexcept
	{
		if (itercontent.begin_ptr == itercontent.curr_ptr) [[unlikely]]
		{
			itercontent.curr_ptr = (itercontent.begin_ptr = (*--itercontent.controller_ptr)) + ::fast_io::containers::details::deque_block_size<sizeof(value_type)>;
		}
		--itercontent.curr_ptr;
		return *this;
	}

	inline constexpr deque_iterator operator++(int) noexcept
	{
		auto temp(*this);
		++*this;
		return temp;
	}

	inline constexpr deque_iterator operator--(int) noexcept
	{
		auto temp(*this);
		--*this;
		return temp;
	}

	inline constexpr reference operator*() const noexcept
	{
		return *this->itercontent.curr_ptr;
	}

	inline constexpr pointer operator->() const noexcept
	{
		return this->itercontent.curr_ptr;
	}
	template <::std::integral postype>
	inline constexpr deque_iterator &operator+=(postype pos) noexcept
	{
		if constexpr (::std::signed_integral<postype>)
		{
			::fast_io::containers::details::deque_add_assign_signed_impl(
				this->itercontent, static_cast<difference_type>(pos));
		}
		else
		{
			::fast_io::containers::details::deque_add_assign_unsigned_impl(
				this->itercontent, static_cast<size_type>(pos));
		}
		return *this;
	}
	template <::std::integral postype>
	inline constexpr deque_iterator &operator-=(postype pos) noexcept
	{
		if constexpr (::std::signed_integral<postype>)
		{
			::fast_io::containers::details::deque_sub_assign_signed_impl(
				this->itercontent, static_cast<difference_type>(pos));
		}
		else
		{
			::fast_io::containers::details::deque_sub_assign_unsigned_impl(
				this->itercontent, static_cast<size_type>(pos));
		}
		return *this;
	}
	template <::std::integral postype>
	inline constexpr reference operator[](postype pos) const noexcept
	{
		if constexpr (::std::signed_integral<postype>)
		{
			return ::fast_io::containers::details::deque_index_signed(this->itercontent,
																	  static_cast<difference_type>(pos));
		}
		else
		{
			return ::fast_io::containers::details::deque_index_unsigned(this->itercontent,
																		static_cast<size_type>(pos));
		}
	}

	inline constexpr operator deque_iterator<T, true>() const noexcept
		requires(!isconst)
	{
		return {this->itercontent};
	}
};

template <typename T, bool isconst, ::std::integral postype>
inline constexpr ::fast_io::containers::details::deque_iterator<T, isconst> operator+(::fast_io::containers::details::deque_iterator<T, isconst> a, postype pos) noexcept
{
	if constexpr (::std::signed_integral<postype>)
	{
		::fast_io::containers::details::deque_add_assign_signed_impl(
			a.itercontent, static_cast<::std::ptrdiff_t>(pos));
	}
	else
	{
		::fast_io::containers::details::deque_add_assign_unsigned_impl(
			a.itercontent, static_cast<::std::size_t>(pos));
	}
	return a;
}

template <typename T, bool isconst, ::std::integral postype>
inline constexpr ::fast_io::containers::details::deque_iterator<T, isconst> operator+(postype pos, ::fast_io::containers::details::deque_iterator<T, isconst> a) noexcept
{
	if constexpr (::std::signed_integral<postype>)
	{
		::fast_io::containers::details::deque_add_assign_signed_impl(
			a.itercontent, static_cast<::std::ptrdiff_t>(pos));
	}
	else
	{
		::fast_io::containers::details::deque_add_assign_unsigned_impl(
			a.itercontent, static_cast<::std::size_t>(pos));
	}
	return a;
}

template <typename T, bool isconst, ::std::integral postype>
inline constexpr ::fast_io::containers::details::deque_iterator<T, isconst> operator-(::fast_io::containers::details::deque_iterator<T, isconst> a, postype pos) noexcept
{
	if constexpr (::std::signed_integral<postype>)
	{
		::fast_io::containers::details::deque_sub_assign_signed_impl(
			a.itercontent, static_cast<::std::ptrdiff_t>(pos));
	}
	else
	{
		::fast_io::containers::details::deque_sub_assign_unsigned_impl(
			a.itercontent, static_cast<::std::size_t>(pos));
	}
	return a;
}

template <typename T>
inline constexpr ::std::ptrdiff_t deque_iter_difference_common(::fast_io::containers::details::deque_control_block<T> const &a, ::fast_io::containers::details::deque_control_block<T> const &b) noexcept
{
	::std::ptrdiff_t controllerdiff{a.controller_ptr - b.controller_ptr};
	constexpr ::std::ptrdiff_t blocksizedf{static_cast<::std::ptrdiff_t>(::fast_io::containers::details::deque_block_size<sizeof(T)>)};
	return controllerdiff * blocksizedf + (a.curr_ptr - a.begin_ptr) + (b.begin_ptr - b.curr_ptr);
}

template <typename T>
inline constexpr ::std::size_t deque_iter_difference_unsigned_common(::fast_io::containers::details::deque_control_block<T> const &a, ::fast_io::containers::details::deque_control_block<T> const &b) noexcept
{
	::std::size_t controllerdiff{static_cast<::std::size_t>(a.controller_ptr - b.controller_ptr)};
	constexpr ::std::size_t blocksizedf{::fast_io::containers::details::deque_block_size<sizeof(T)>};
	return controllerdiff * blocksizedf + static_cast<::std::size_t>((a.curr_ptr - a.begin_ptr) + (b.begin_ptr - b.curr_ptr));
}

template <typename T, bool isconst1, bool isconst2>
inline constexpr ::std::ptrdiff_t operator-(::fast_io::containers::details::deque_iterator<T, isconst1> const &a, ::fast_io::containers::details::deque_iterator<T, isconst2> const &b) noexcept
{
	return ::fast_io::containers::details::deque_iter_difference_common(a.itercontent, b.itercontent);
}

template <typename T, bool isconst1, bool isconst2>
inline constexpr bool operator==(::fast_io::containers::details::deque_iterator<T, isconst1> const &a, ::fast_io::containers::details::deque_iterator<T, isconst2> const &b) noexcept
{
	return a.itercontent.curr_ptr == b.itercontent.curr_ptr;
}

template <typename T, bool isconst1, bool isconst2>
inline constexpr auto operator<=>(::fast_io::containers::details::deque_iterator<T, isconst1> const &a, ::fast_io::containers::details::deque_iterator<T, isconst2> const &b) noexcept
{
	auto block3way{a.itercontent.controller_ptr <=> b.itercontent.controller_ptr};
	if (block3way == 0)
	{
		return a.itercontent.curr_ptr <=> b.itercontent.curr_ptr;
	}
	return block3way;
}

template <typename allocator, typename controllerblocktype>
inline constexpr void deque_destroy_trivial_common_align(controllerblocktype &controller, ::std::size_t aligns, ::std::size_t totalsz) noexcept
{
	for (auto i{controller.controller_start_reserved_ptr}, e{controller.controller_after_reserved_ptr}; i != e; ++i)
	{
		allocator::deallocate_aligned_n(*i, aligns, totalsz);
	}
	::std::size_t const n{static_cast<::std::size_t>(controller.controller_after_ptr - controller.controller_start_ptr + 1) * sizeof(void *)};
	allocator::deallocate_n(controller.controller_start_ptr, n);
}

template <typename allocator, ::std::size_t align, ::std::size_t sz, typename controllerblocktype>
inline constexpr void deque_destroy_trivial_common(controllerblocktype &controller) noexcept
{
	constexpr ::std::size_t totalsz{sz * ::fast_io::containers::details::deque_block_size<sz>};
	if consteval
	{
		::fast_io::containers::details::deque_destroy_trivial_common_align<allocator>(controller, align, totalsz);
	}
	else
	{
		::fast_io::containers::details::deque_destroy_trivial_common_align<allocator>(
			*reinterpret_cast<::fast_io::containers::details::deque_controller_block_common *>(__builtin_addressof(controller)), align, totalsz);
	}
}

template <typename allocator, typename dequecontroltype>
inline constexpr void deque_grow_to_new_blocks_count_impl(dequecontroltype &controller, ::std::size_t new_blocks_count_least) noexcept
{
	auto old_start_ptr{controller.controller_block.controller_start_ptr};

	auto old_start_reserved_ptr{controller.controller_block.controller_start_reserved_ptr};
	auto old_after_reserved_ptr{controller.controller_block.controller_after_reserved_ptr};

	::std::size_t const old_start_reserved_ptr_pos{static_cast<::std::size_t>(old_start_reserved_ptr - old_start_ptr)};
	::std::size_t const old_after_ptr_pos{static_cast<::std::size_t>(controller.controller_block.controller_after_ptr - old_start_ptr)};
	::std::size_t const old_front_block_ptr_pos{static_cast<::std::size_t>(controller.front_block.controller_ptr - old_start_ptr)};
	::std::size_t const old_back_block_ptr_pos{static_cast<::std::size_t>(controller.back_block.controller_ptr - old_start_ptr)};

	using block_typed_allocator = ::fast_io::typed_generic_allocator_adapter<allocator, typename dequecontroltype::controlreplacetype>;

#if (defined(__GNUC__) || defined(__clang__))
	::std::size_t new_blocks_count_least_p1;
	if (__builtin_add_overflow(new_blocks_count_least, 1zu, __builtin_addressof(new_blocks_count_least_p1))) [[unlikely]]
	{
		::fast_io::fast_terminate();
	}
#else
	constexpr ::std::size_t mx{::std::numeric_limits<::std::size_t>::max()};
	::std::size_t new_blocks_count_least_p1{new_blocks_count_least};
	if (mx == new_blocks_count_least)
	{
		::fast_io::fast_terminate();
	}
	++new_blocks_count_least_p1;
#endif
	auto [new_start_ptr, new_blocks_count] = block_typed_allocator::allocate_at_least(new_blocks_count_least_p1);

	auto const old_reserved_blocks_count{
		static_cast<::std::size_t>(old_after_reserved_ptr - old_start_reserved_ptr)};
	auto const old_half_reserved_blocks_count{
		static_cast<::std::size_t>(old_reserved_blocks_count >> 1u)};
	auto old_reserved_pivot{old_start_reserved_ptr + old_half_reserved_blocks_count};
	auto const old_used_blocks_count{
		static_cast<::std::size_t>(controller.back_block.controller_ptr - controller.front_block.controller_ptr) + 1zu};
	auto const old_half_used_blocks_count{
		static_cast<::std::size_t>(old_used_blocks_count >> 1u)};
	auto old_used_blocks_pivot{controller.front_block.controller_ptr + old_half_used_blocks_count};

	::std::ptrdiff_t pivot_diff{old_reserved_pivot - old_used_blocks_pivot};

	::std::size_t const new_blocks_offset{static_cast<::std::size_t>(new_blocks_count - old_reserved_blocks_count) >> 1u};
	--new_blocks_count;

	auto new_start_reserved_ptr{new_start_ptr + new_blocks_offset};
	auto new_after_reserved_ptr{new_start_reserved_ptr + old_reserved_blocks_count};

	decltype(old_start_reserved_ptr) old_pivot, new_pivot;
	if (pivot_diff < 0)
	{
		old_pivot = old_start_reserved_ptr;
		new_pivot = new_after_reserved_ptr;
	}
	else
	{
		old_pivot = old_after_reserved_ptr;
		new_pivot = new_start_reserved_ptr;
	}
	old_pivot -= pivot_diff;
	new_pivot += pivot_diff;

	::fast_io::freestanding::non_overlapped_copy(old_pivot,
												 old_after_reserved_ptr, new_start_reserved_ptr);
	::fast_io::freestanding::non_overlapped_copy(old_start_reserved_ptr,
												 old_pivot, new_pivot);

	*new_after_reserved_ptr = 0;
	block_typed_allocator::deallocate_n(old_start_ptr, static_cast<::std::size_t>(old_after_ptr_pos + 1u));

	controller.controller_block.controller_start_ptr = new_start_ptr;
	controller.controller_block.controller_start_reserved_ptr = new_start_reserved_ptr;
	controller.controller_block.controller_after_reserved_ptr = new_after_reserved_ptr;
	controller.controller_block.controller_after_ptr = new_start_ptr + new_blocks_count;

	controller.front_block.controller_ptr = new_start_ptr + (new_blocks_offset + (old_front_block_ptr_pos - old_start_reserved_ptr_pos)) + pivot_diff;
	controller.back_block.controller_ptr = new_start_ptr + (new_blocks_offset + (old_back_block_ptr_pos - old_start_reserved_ptr_pos)) + pivot_diff;
}

template <typename allocator, typename dequecontroltype>
inline constexpr void deque_rebalance_or_grow_2x_after_blocks_impl(dequecontroltype &controller) noexcept
{
	auto const used_blocks_count{
		static_cast<::std::size_t>(controller.back_block.controller_ptr - controller.front_block.controller_ptr) + 1zu};
	auto const total_slots_count{
		static_cast<::std::size_t>(controller.controller_block.controller_after_ptr - controller.controller_block.controller_start_ptr)};
	auto const half_slots_count{static_cast<::std::size_t>(total_slots_count >> 1u)};
	if (half_slots_count < used_blocks_count) // grow blocks
	{
		constexpr ::std::size_t mx{::std::numeric_limits<::std::size_t>::max()};
		constexpr ::std::size_t mxdv2m1{(mx >> 1u) - 1u};
		if (mxdv2m1 < total_slots_count)
		{
			::fast_io::fast_terminate();
		}
		::fast_io::containers::details::deque_grow_to_new_blocks_count_impl<allocator>(controller,
																					   static_cast<::std::size_t>((total_slots_count << 1u) + 1u));
	}
	else
	{
		// balance blocks
		auto start_reserved_ptr{controller.controller_block.controller_start_reserved_ptr};
		auto after_reserved_ptr{controller.controller_block.controller_after_reserved_ptr};
		auto const reserved_blocks_count{
			static_cast<::std::size_t>(after_reserved_ptr - start_reserved_ptr)};
		auto const half_reserved_blocks_count{
			static_cast<::std::size_t>(reserved_blocks_count >> 1u)};
		auto reserved_pivot{start_reserved_ptr + half_reserved_blocks_count};
		auto const half_used_blocks_count{
			static_cast<::std::size_t>(used_blocks_count >> 1u)};
		auto used_blocks_pivot{controller.front_block.controller_ptr + half_used_blocks_count};
		if (used_blocks_pivot != reserved_pivot)
		{
			::std::ptrdiff_t diff{reserved_pivot - used_blocks_pivot};
			auto rotate_pivot{diff < 0 ? start_reserved_ptr : after_reserved_ptr};
			rotate_pivot -= diff;
			::std::rotate(start_reserved_ptr, rotate_pivot, after_reserved_ptr);
			controller.front_block.controller_ptr += diff;
			controller.back_block.controller_ptr += diff;
		}

		auto slots_pivot{controller.controller_block.controller_start_ptr + half_slots_count};
		if (slots_pivot != reserved_pivot)
		{
			::std::ptrdiff_t diff{slots_pivot - reserved_pivot};
			::fast_io::freestanding::overlapped_copy(start_reserved_ptr,
													 after_reserved_ptr, start_reserved_ptr + diff);
			controller.front_block.controller_ptr += diff;
			controller.back_block.controller_ptr += diff;
			controller.controller_block.controller_start_reserved_ptr += diff;
			*(controller.controller_block.controller_after_reserved_ptr += diff) = nullptr;
		}
	}
}

template <typename allocator, typename dequecontroltype>
inline constexpr void deque_allocate_on_empty_common_with_n_impl(dequecontroltype &controller, ::std::size_t align, ::std::size_t bytes,
																 ::std::size_t initial_allocated_block_counts) noexcept
{
#if (defined(__GNUC__) || defined(__clang__))
	::std::size_t initial_allocated_block_counts_with_sentinel;
	if (__builtin_add_overflow(initial_allocated_block_counts, 1u,
							   __builtin_addressof(initial_allocated_block_counts_with_sentinel)))
	{
		::fast_io::fast_terminate();
	}
#else
	constexpr ::std::size_t maxval{::std::numeric_limits<::std::size_t>::max()};
	if (initial_allocated_block_counts == maxval) [[unlikely]]
	{
		::fast_io::fast_terminate();
	}
	::std::size_t initial_allocated_block_counts_with_sentinel{initial_allocated_block_counts + 1u};
#endif
	using block_typed_allocator = ::fast_io::typed_generic_allocator_adapter<allocator, typename dequecontroltype::controlreplacetype>;
	auto [allocated_blocks_ptr, allocated_blocks_count] = block_typed_allocator::allocate_at_least(initial_allocated_block_counts_with_sentinel);

	// we need a null terminator as sentinel like c style string does
	--allocated_blocks_count;
	auto &controller_block{controller.controller_block};
	auto &front_block{controller.front_block};
	auto &back_block{controller.back_block};

	using begin_ptrtype = typename dequecontroltype::replacetype *;

	auto begin_ptr{static_cast<begin_ptrtype>(allocator::allocate_aligned(align, bytes))};

	controller_block.controller_start_ptr = allocated_blocks_ptr;
	auto allocated_mid_block{allocated_blocks_ptr + (allocated_blocks_count >> 1u)};
	back_block.controller_ptr = front_block.controller_ptr = controller_block.controller_start_reserved_ptr = ::std::construct_at(allocated_mid_block, begin_ptr);

	*(controller_block.controller_after_reserved_ptr = allocated_mid_block + 1) = nullptr; // set nullptr as a sentinel

	controller_block.controller_after_ptr = allocated_blocks_ptr + allocated_blocks_count;
	::std::size_t halfsize{bytes >> 1u};

	back_block.begin_ptr = front_block.begin_ptr = begin_ptr;
	controller.back_end_ptr = controller.front_end_ptr = (begin_ptr + bytes);
	auto halfposptr{begin_ptr + halfsize};
	front_block.curr_ptr = halfposptr;
	back_block.curr_ptr = halfposptr;
}

template <typename allocator, typename dequecontroltype>
inline constexpr void deque_clear_common_impl(dequecontroltype &controller, ::std::size_t blockbytes)
{
	auto start_reserved_ptr{controller.controller_block.controller_start_reserved_ptr};
	auto after_reserved_ptr{controller.controller_block.controller_after_reserved_ptr};
	if (start_reserved_ptr == after_reserved_ptr)
	{
		return;
	}
	auto const reserved_blocks_count{
		static_cast<::std::size_t>(after_reserved_ptr - start_reserved_ptr)};
	auto const half_reserved_blocks_count{
		static_cast<::std::size_t>(reserved_blocks_count >> 1u)};
	auto reserved_pivot{start_reserved_ptr + half_reserved_blocks_count};
	using replacetype = typename dequecontroltype::replacetype;
	using begin_ptrtype = replacetype *;
	auto begin_ptr{static_cast<begin_ptrtype>(*reserved_pivot)};
	auto end_ptr{begin_ptr + blockbytes};
	auto mid_ptr{begin_ptr + static_cast<::std::size_t>(blockbytes >> 1u)};
	controller.back_block.controller_ptr = controller.front_block.controller_ptr = reserved_pivot;
	controller.back_block.begin_ptr = controller.front_block.begin_ptr = begin_ptr;
	controller.back_block.curr_ptr = controller.front_block.curr_ptr = mid_ptr;
	controller.back_end_ptr = controller.front_end_ptr = end_ptr;
}

template <typename allocator, ::std::size_t sz, ::std::size_t block_size, typename dequecontroltype>
inline constexpr void deque_clear_common(dequecontroltype &controller) noexcept
{
	constexpr ::std::size_t blockbytes{sz * block_size};
	::fast_io::containers::details::deque_clear_common_impl<allocator>(controller, blockbytes);
}

template <typename allocator, typename dequecontroltype>
inline constexpr void deque_allocate_init_blocks_dezeroing_impl(dequecontroltype &controller, ::std::size_t align, ::std::size_t blockbytes, ::std::size_t blocks_count_least, bool zeroing) noexcept
{
	if (!blocks_count_least)
	{
		controller = {};
		return;
	}
	constexpr ::std::size_t mx{::std::numeric_limits<::std::size_t>::max()};
	if (blocks_count_least == mx)
	{
		::fast_io::fast_terminate();
	}
	using block_typed_allocator = ::fast_io::typed_generic_allocator_adapter<allocator, typename dequecontroltype::controlreplacetype>;
	auto [start_ptr, blocks_count] = block_typed_allocator::allocate_at_least(blocks_count_least + 1u);
	--blocks_count;
	::std::size_t const half_blocks_count{blocks_count >> 1u};
	::std::size_t const half_blocks_count_least{blocks_count_least >> 1u};
	::std::size_t const offset{half_blocks_count - half_blocks_count_least};
	auto reserve_start{start_ptr + offset}, reserve_after{reserve_start + blocks_count_least};
	using begin_ptrtype = typename dequecontroltype::replacetype *;
	for (auto it{reserve_start}, ed{reserve_after}; it != ed; ++it)
	{
		if (zeroing)
		{
			::std::construct_at(it, static_cast<begin_ptrtype>(allocator::allocate_aligned_zero(align, blockbytes)));
		}
		else
		{
			::std::construct_at(it, static_cast<begin_ptrtype>(allocator::allocate_aligned(align, blockbytes)));
		}
	}
	::std::construct_at(reserve_after, nullptr);
	using replacetype = typename dequecontroltype::replacetype;
	using begin_ptrtype = replacetype *;
	begin_ptrtype reserve_start_block{static_cast<begin_ptrtype>(*reserve_start)};
	controller.front_block = {reserve_start_block, reserve_start_block, reserve_start};
	controller.front_end_ptr = reserve_start_block + blockbytes;
	begin_ptrtype reserve_back_block{static_cast<begin_ptrtype>(reserve_after[-1])};
	controller.back_block = {reserve_back_block, reserve_back_block, reserve_after - 1};
	controller.back_end_ptr = reserve_back_block + blockbytes;
	controller.controller_block = {
		start_ptr, reserve_start, reserve_after, start_ptr + blocks_count};
}
template <typename allocator, bool zeroing, typename dequecontroltype>
inline constexpr void deque_allocate_init_blocks_impl(dequecontroltype &controller, ::std::size_t align, ::std::size_t blockbytes, ::std::size_t blocks_count_least) noexcept
{
	::fast_io::containers::details::deque_allocate_init_blocks_dezeroing_impl<allocator>(controller, align, blockbytes, blocks_count_least, zeroing);
}

template <typename allocator, ::std::size_t align, ::std::size_t sz, ::std::size_t block_size, bool zeroing, typename dequecontroltype>
inline constexpr void deque_init_space_common(dequecontroltype &controller, ::std::size_t n) noexcept
{
	constexpr ::std::size_t blockbytes{sz * block_size};
	::std::size_t const ndivsz{n / block_size};
	::std::size_t const nmodsz{n % block_size};
	::std::size_t const counts{ndivsz + static_cast<::std::size_t>(nmodsz != 0u)};

	::fast_io::containers::details::deque_allocate_init_blocks_impl<allocator, zeroing>(controller, align, blockbytes, counts);
	if (!n)
	{
		return;
	}
	auto &back_curr_ptr{controller.back_block.curr_ptr};
	::std::size_t offset_for_back{blockbytes};
	if (nmodsz)
	{
		offset_for_back = nmodsz * sz;
	}
	back_curr_ptr += offset_for_back;
}

template <typename ToIter>
struct uninitialized_copy_n_for_deque_guard
{
	bool torecover{true};
	ToIter d_first, &current;
	constexpr explicit uninitialized_copy_n_for_deque_guard(ToIter &toiter) noexcept
		: d_first(toiter), current(toiter)
	{}
	uninitialized_copy_n_for_deque_guard(uninitialized_copy_n_for_deque_guard const &) = delete;
	uninitialized_copy_n_for_deque_guard &operator=(uninitialized_copy_n_for_deque_guard const &) = delete;
	constexpr ~uninitialized_copy_n_for_deque_guard()
	{
		if (torecover)
		{
			::std::destroy(d_first, current);
		}
	}
};

template <typename FromIter, typename ToIter>
struct uninitialized_copy_n_for_deque_in_out_result
{
	FromIter from;
	ToIter to;
};
template <typename FromIter, typename ToIter>
inline constexpr ::fast_io::containers::details::uninitialized_copy_n_for_deque_in_out_result<FromIter, ToIter> uninitialized_copy_n_for_deque(FromIter fromiter, ::std::size_t count, ToIter toiter)
{
#if 0
	using fromvaluet = ::std::iter_value_t<FromIter>;
	using tovaluet = ::std::iter_value_t<ToIter>;
	if constexpr(::std::is_trivially_copyable_v<FromIter>&&
		::std::is_trivially_copyable_v<ToIter>)
	{
		
	}
#endif
	{
		uninitialized_copy_n_for_deque_guard g(toiter);
		for (; count; --count)
		{
			::std::construct_at(::std::addressof(*toiter), *fromiter);
			++fromiter;
			++toiter;
		}
		g.torecover = false;
	}
	return {fromiter, toiter};
}

template <typename allocator, typename dequecontroltype>
inline constexpr void deque_clone_trivial_impl(dequecontroltype &controller, dequecontroltype const &fromcontroller,
											   ::std::size_t align, ::std::size_t blockbytes) noexcept
{
	if (fromcontroller.front_block.curr_ptr == fromcontroller.back_block.curr_ptr)
	{
		controller = {};
		return;
	}
	auto front_controller_ptr{fromcontroller.front_block.controller_ptr};
	auto back_controller_ptr{fromcontroller.back_block.controller_ptr};
	::std::size_t blocks_required{static_cast<::std::size_t>(back_controller_ptr -
															 front_controller_ptr + 1)};

	::fast_io::containers::details::deque_allocate_init_blocks_dezeroing_impl<allocator>(controller, align, blockbytes, blocks_required, false);
	using replacetype = typename dequecontroltype::replacetype;
	using begin_ptrtype = replacetype *;

	begin_ptrtype lastblockbegin;
	if (front_controller_ptr == back_controller_ptr)
	{
		lastblockbegin = fromcontroller.front_block.curr_ptr;
	}
	else
	{
		auto destit{controller.front_block.controller_ptr};
		auto pos{fromcontroller.front_block.curr_ptr - fromcontroller.front_block.begin_ptr};
		controller.front_end_ptr =
			::fast_io::freestanding::non_overlapped_copy(fromcontroller.front_block.curr_ptr,
														 fromcontroller.front_end_ptr,
														 (controller.front_block.curr_ptr =
															  pos + controller.front_block.begin_ptr));
		++destit;
		for (begin_ptrtype *it{front_controller_ptr + 1}, *ed{back_controller_ptr}; it != ed; ++it)
		{
			begin_ptrtype blockptr{*it};
			::fast_io::freestanding::non_overlapped_copy_n(blockptr, blockbytes, *destit);
			++destit;
		}
		lastblockbegin = fromcontroller.back_block.begin_ptr;
	}
	controller.back_block.curr_ptr =
		::fast_io::freestanding::non_overlapped_copy(lastblockbegin,
													 fromcontroller.back_block.curr_ptr, controller.back_block.begin_ptr);
}

template <typename allocator, ::std::size_t align, ::std::size_t sz, ::std::size_t block_size, typename dequecontroltype>
inline constexpr void deque_clone_trivial_common(dequecontroltype &controller, dequecontroltype const &fromcontroller) noexcept
{
	constexpr ::std::size_t blockbytes{sz * block_size};
	::fast_io::containers::details::deque_clone_trivial_impl<allocator>(controller, fromcontroller, align, blockbytes);
}

template <typename Itercontent>
inline constexpr Itercontent
deque_copy_impl(Itercontent first, Itercontent last,
				Itercontent dest, ::std::size_t blocksize)
{
	if (first.curr_ptr == last.curr_ptr)
	{
		return dest;
	}
	for (;;)
	{
		decltype(first.begin_ptr) firstend;
		if (first.controller_ptr == last.controller_ptr)
		{
			firstend = last.curr_ptr;
		}
		else
		{
			firstend = first.begin_ptr + blocksize;
		}
		auto firstcurrptr{first.curr_ptr};
		::std::size_t const curr_block_to_end{
			static_cast<::std::size_t>(firstend - firstcurrptr)};
		auto destcurrptr{dest.curr_ptr};
		::std::size_t const dest_block_to_end{
			static_cast<::std::size_t>(dest.begin_ptr + blocksize - destcurrptr)};
		auto cmp{curr_block_to_end <=> dest_block_to_end};
		::std::size_t tocopy;
		if (cmp < 0)
		{
			tocopy = curr_block_to_end;
		}
		else
		{
			tocopy = dest_block_to_end;
		}
		::fast_io::freestanding::copy_n(firstcurrptr, tocopy, destcurrptr);
		if (0 < cmp)
		{
			dest.curr_ptr = (dest.begin_ptr = (*++dest.controller_ptr));
			first.curr_ptr += tocopy;
		}
		else
		{
			if (cmp < 0)
			{
				dest.curr_ptr += tocopy;
			}
			else if (cmp == 0)
			{
				dest.curr_ptr = (dest.begin_ptr = (*++dest.controller_ptr));
			}
			if (first.controller_ptr == last.controller_ptr)
			{
				break;
			}
			first.curr_ptr = (first.begin_ptr = (*++first.controller_ptr));
		}
	}
	return dest;
}

template <typename Itercontent>
inline constexpr Itercontent
deque_copy_backward_impl(Itercontent first, Itercontent last,
						 Itercontent dest, ::std::size_t blocksize)
{
	if (first.curr_ptr == last.curr_ptr)
	{
		return dest;
	}
	for (;;)
	{
		decltype(first.begin_ptr) lastbegin;
		if (first.controller_ptr == last.controller_ptr)
		{
			lastbegin = first.curr_ptr;
		}
		else
		{
			lastbegin = last.begin_ptr;
		}
		auto lastcurrptr{last.curr_ptr};
		::std::size_t const curr_block_to_begin{
			static_cast<::std::size_t>(lastcurrptr - lastbegin)};
		auto destcurrptr{dest.curr_ptr};
		::std::size_t const dest_block_to_begin{
			static_cast<::std::size_t>(destcurrptr - dest.begin_ptr)};
		auto cmp{curr_block_to_begin <=> dest_block_to_begin};
		::std::size_t tocopy;
		if (cmp < 0)
		{
			tocopy = curr_block_to_begin;
		}
		else
		{
			tocopy = dest_block_to_begin;
		}
		::fast_io::freestanding::copy_backward(lastcurrptr - tocopy, lastcurrptr,
											   destcurrptr);
		if (0 < cmp)
		{
			dest.curr_ptr = (dest.begin_ptr = (*--dest.controller_ptr)) + blocksize;
			last.curr_ptr -= tocopy;
		}
		else
		{
			if (cmp == 0)
			{
				if (last.controller_ptr == first.controller_ptr)
				{
					dest.curr_ptr = dest.begin_ptr;
					break;
				}
				dest.curr_ptr = (dest.begin_ptr = (*--dest.controller_ptr)) + blocksize;
			}
			else
			{
				dest.curr_ptr -= tocopy;
				if (last.controller_ptr == first.controller_ptr)
				{
					break;
				}
			}
			last.curr_ptr = (last.begin_ptr = (*--last.controller_ptr)) + blocksize;
		}
	}
	return dest;
}


template <typename Itercontent>
inline constexpr Itercontent
deque_relocate_impl(Itercontent first, Itercontent last,
					Itercontent dest, ::std::size_t blocksize)
{
	if (first.curr_ptr == last.curr_ptr)
	{
		return dest;
	}
	for (;;)
	{
		decltype(first.begin_ptr) firstend;
		if (first.controller_ptr == last.controller_ptr)
		{
			firstend = last.curr_ptr;
		}
		else
		{
			firstend = first.begin_ptr + blocksize;
		}
		auto firstcurrptr{first.curr_ptr};
		::std::size_t const curr_block_to_end{
			static_cast<::std::size_t>(firstend - firstcurrptr)};
		auto destcurrptr{dest.curr_ptr};
		::std::size_t const dest_block_to_end{
			static_cast<::std::size_t>(dest.begin_ptr + blocksize - destcurrptr)};
		auto cmp{curr_block_to_end <=> dest_block_to_end};
		::std::size_t tocopy;
		if (cmp < 0)
		{
			tocopy = curr_block_to_end;
		}
		else
		{
			tocopy = dest_block_to_end;
		}
		::fast_io::freestanding::uninitialized_relocate_ignore_define(firstcurrptr, firstcurrptr + tocopy, destcurrptr);
		if (0 < cmp)
		{
			dest.curr_ptr = (dest.begin_ptr = (*++dest.controller_ptr));
			first.curr_ptr += tocopy;
		}
		else
		{
			if (cmp < 0)
			{
				dest.curr_ptr += tocopy;
			}
			else if (cmp == 0)
			{
				dest.curr_ptr = (dest.begin_ptr = (*++dest.controller_ptr));
			}
			if (first.controller_ptr == last.controller_ptr)
			{
				break;
			}
			first.curr_ptr = (first.begin_ptr = (*++first.controller_ptr));
		}
	}
	return dest;
}

template <typename Itercontent>
inline constexpr Itercontent
deque_relocate_backward_impl(Itercontent first, Itercontent last,
							 Itercontent dest, ::std::size_t blocksize)
{
	if (first.curr_ptr == last.curr_ptr)
	{
		return dest;
	}
	for (;;)
	{
		decltype(first.begin_ptr) lastbegin;
		if (first.controller_ptr == last.controller_ptr)
		{
			lastbegin = first.curr_ptr;
		}
		else
		{
			lastbegin = last.begin_ptr;
		}
		auto lastcurrptr{last.curr_ptr};
		::std::size_t const curr_block_to_begin{
			static_cast<::std::size_t>(lastcurrptr - lastbegin)};
		auto destcurrptr{dest.curr_ptr};
		::std::size_t const dest_block_to_begin{
			static_cast<::std::size_t>(destcurrptr - dest.begin_ptr)};
		auto cmp{curr_block_to_begin <=> dest_block_to_begin};
		::std::size_t tocopy;
		if (cmp < 0)
		{
			tocopy = curr_block_to_begin;
		}
		else
		{
			tocopy = dest_block_to_begin;
		}
		::fast_io::freestanding::uninitialized_relocate_backward_ignore_define(lastcurrptr - tocopy, lastcurrptr,
																			   destcurrptr);
		if (0 < cmp)
		{
			dest.curr_ptr = (dest.begin_ptr = (*--dest.controller_ptr)) + blocksize;
			last.curr_ptr -= tocopy;
		}
		else
		{
			if (cmp == 0)
			{
				if (last.controller_ptr == first.controller_ptr)
				{
					dest.curr_ptr = dest.begin_ptr;
					break;
				}
				dest.curr_ptr = (dest.begin_ptr = (*--dest.controller_ptr)) + blocksize;
			}
			else
			{
				dest.curr_ptr -= tocopy;
				if (last.controller_ptr == first.controller_ptr)
				{
					break;
				}
			}
			last.curr_ptr = (last.begin_ptr = (*--last.controller_ptr)) + blocksize;
		}
	}
	return dest;
}

inline ::fast_io::containers::details::deque_control_block_common
deque_erase_common_trivial_impl(::fast_io::containers::details::deque_controller_common &controller,
								::fast_io::containers::details::deque_control_block_common first,
								::fast_io::containers::details::deque_control_block_common last,
								bool moveleft,
								::std::size_t blockbytes) noexcept
{
	::fast_io::containers::details::deque_control_block_common back_block{controller.back_block};
	if (moveleft)
	{
		controller.front_block = ::fast_io::containers::details::deque_copy_backward_impl(controller.front_block, first, last, blockbytes);
		first = last;
		if (controller.front_block.curr_ptr == back_block.curr_ptr && back_block.curr_ptr == controller.back_end_ptr) [[unlikely]]
		{
			auto back_begin_ptr{back_block.begin_ptr};
			if (back_begin_ptr) [[likely]]
			{
				// erase after empty we should reset to middle
				controller.front_block.curr_ptr = back_block.curr_ptr = back_begin_ptr + (blockbytes >> 1u);
			}
		}
	}
	else
	{
		if (back_block.curr_ptr == controller.back_end_ptr) [[unlikely]]
		{
			if (back_block.controller_ptr) [[likely]]
			{
				back_block.curr_ptr = back_block.begin_ptr = (*++back_block.controller_ptr);
			}
		}
		back_block = ::fast_io::containers::details::deque_copy_impl(last, back_block, first, blockbytes);
	}
	if (back_block.begin_ptr == back_block.curr_ptr)
	{
		if (controller.front_block.controller_ptr != back_block.controller_ptr)
		{
			back_block.curr_ptr = ((back_block.begin_ptr = (*--back_block.controller_ptr)) + blockbytes);
		}
	}
	controller.back_block = back_block;
	controller.back_end_ptr = back_block.begin_ptr + blockbytes;
	return first;
}

template <typename allocator, typename dequecontroltype>
inline constexpr void deque_rebalance_or_grow_insertation_impl(dequecontroltype &controller, ::std::size_t extrablocks) noexcept
{
	// ignore overchecked first
	auto const used_blocks_count{
		static_cast<::std::size_t>(controller.back_block.controller_ptr - controller.front_block.controller_ptr) + 1zu};
	auto const total_slots_count{
		static_cast<::std::size_t>(controller.controller_block.controller_after_ptr - controller.controller_block.controller_start_ptr)};
	auto const half_slots_count{static_cast<::std::size_t>(total_slots_count >> 1u)};
#if defined(__GNUC__) || defined(__clang__)
	::std::size_t new_used_blocks_count;
	if (__builtin_add_overflow(used_blocks_count, extrablocks, __builtin_addressof(new_used_blocks_count))) [[unlikely]]
	{
		::fast_io::fast_terminate();
	}
#else
	constexpr ::std::size_t mx{::std::numeric_limits<::std::size_t>::max()};
	::std::size_t const mx_sub_extrablocks{mx - extrablocks};
	if (mx_sub_extrablocks < used_blocks_count)
	{
		::fast_io::fast_terminate();
	}

	auto const new_used_blocks_count{used_blocks_count + extrablocks};
#endif

	if (half_slots_count < new_used_blocks_count) // grow blocks
	{
#if defined(__GNUC__) || defined(__clang__)
		::std::size_t doubleslotsextra;
		if (__builtin_add_overflow(total_slots_count, extrablocks, __builtin_addressof(doubleslotsextra)))
		{
			::fast_io::fast_terminate();
		}
		if (__builtin_add_overflow(doubleslotsextra, doubleslotsextra, __builtin_addressof(doubleslotsextra)))
		{
			::fast_io::fast_terminate();
		}
#else
		::std::size_t mx_total_slots{mx - extrablocks};
		if (mx_total_slots < total_slots_count)
		{
			::fast_io::fast_terminate();
		}
		::std::size_t doubleslotsextra{extrablocks + total_slots_count};
		constexpr ::std::size_t mxdv2m1{(mx >> 1u)};
		if (mxdv2m1 < doubleslotsextra)
		{
			::fast_io::fast_terminate();
		}
		doubleslotsextra <<= 1u;
#endif

		::fast_io::containers::details::deque_grow_to_new_blocks_count_impl<allocator>(controller, doubleslotsextra);
	}
	else
	{
		// balance blocks
		auto start_reserved_ptr{controller.controller_block.controller_start_reserved_ptr};
		auto after_reserved_ptr{controller.controller_block.controller_after_reserved_ptr};
		auto const reserved_blocks_count{
			static_cast<::std::size_t>(after_reserved_ptr - start_reserved_ptr)};
		auto const half_reserved_blocks_count{
			static_cast<::std::size_t>(reserved_blocks_count >> 1u)};
		auto reserved_pivot{start_reserved_ptr + half_reserved_blocks_count};
		auto const half_used_blocks_count{
			static_cast<::std::size_t>(used_blocks_count >> 1u)}; // this place needs to deal with extra block
		auto used_blocks_pivot{controller.front_block.controller_ptr + half_used_blocks_count};
		if (used_blocks_pivot != reserved_pivot)
		{
			::std::ptrdiff_t diff{reserved_pivot - used_blocks_pivot};
			auto rotate_pivot{diff < 0 ? start_reserved_ptr : after_reserved_ptr};
			rotate_pivot -= diff;
			::std::rotate(start_reserved_ptr, rotate_pivot, after_reserved_ptr);
			controller.front_block.controller_ptr += diff;
			controller.back_block.controller_ptr += diff;
		}
		auto const half_slotsextra_count{static_cast<::std::size_t>((total_slots_count + extrablocks) >> 1u)};
		auto slots_pivot{controller.controller_block.controller_start_ptr + half_slotsextra_count};
		if (slots_pivot != reserved_pivot)
		{
			::std::ptrdiff_t diff{slots_pivot - reserved_pivot};
			::fast_io::freestanding::overlapped_copy(start_reserved_ptr,
													 after_reserved_ptr, start_reserved_ptr + diff);
			controller.front_block.controller_ptr += diff;
			controller.back_block.controller_ptr += diff;
			controller.controller_block.controller_start_reserved_ptr += diff;
			*(controller.controller_block.controller_after_reserved_ptr += diff) = nullptr;
		}
	}
}

template <typename allocator, typename dequecontroltype>
inline constexpr bool deque_reserve_back_blocks_impl(dequecontroltype &controller, ::std::size_t nb, ::std::size_t align, ::std::size_t blockbytes) noexcept
{
	if (controller.controller_block.controller_start_ptr == nullptr)
	{
		::fast_io::containers::details::deque_allocate_on_empty_common_with_n_impl<allocator>(
			controller, align, blockbytes, nb);
		return false;
	}
	using replacetype = typename dequecontroltype::replacetype;
	using begin_ptrtype = replacetype *;

	::std::size_t diff_to_after_ptr =
		static_cast<::std::size_t>(
			controller.controller_block.controller_after_reserved_ptr -
			controller.back_block.controller_ptr);
	if (diff_to_after_ptr <= nb)
	{
		::std::size_t distance_back_to_after{
			static_cast<::std::size_t>(controller.controller_block.controller_after_ptr -
									   controller.back_block.controller_ptr)};
		if (distance_back_to_after <= nb)
		{
			::fast_io::containers::details::deque_rebalance_or_grow_insertation_impl<allocator>(controller, nb);
		}
		::std::size_t diff_to_after_ptr2 =
			static_cast<::std::size_t>(
				controller.controller_block.controller_after_reserved_ptr -
				controller.back_block.controller_ptr);
		if (diff_to_after_ptr2 <= nb)
		{
			::std::size_t front_reserved_blocks{
				static_cast<::std::size_t>(controller.front_block.controller_ptr - controller.controller_block.controller_start_reserved_ptr)};

			::std::size_t front_borrowed_blocks_count{front_reserved_blocks};
			::std::size_t to_allocate_blocks{nb};
			if (nb < front_reserved_blocks)
			{
				front_borrowed_blocks_count = nb;
				to_allocate_blocks = 0u;
			}
			else
			{
				to_allocate_blocks -= front_borrowed_blocks_count;
			}
			auto controller_start_reserved_ptr{
				controller.controller_block.controller_start_reserved_ptr};

			auto pos{
				controller.controller_block.controller_after_reserved_ptr};
			pos = ::fast_io::freestanding::non_overlapped_copy_n(controller_start_reserved_ptr,
																 front_borrowed_blocks_count,
																 pos);
			controller.controller_block.controller_start_reserved_ptr =
				controller_start_reserved_ptr + front_borrowed_blocks_count;
			for (auto e{pos + to_allocate_blocks}; pos != e; ++pos)
			{
				::std::construct_at(pos, static_cast<begin_ptrtype>(allocator::allocate_aligned(align, blockbytes)));
			}
			*pos = nullptr;
			controller.controller_block.controller_after_reserved_ptr = pos;
		}
	}

	if (controller.back_block.controller_ptr == controller.front_block.controller_ptr && controller.front_block.curr_ptr == controller.front_end_ptr)
	{
		auto front_block_controller_ptr{controller.front_block.controller_ptr + 1};
		controller.front_block.controller_ptr = front_block_controller_ptr;
		auto front_begin_ptr = static_cast<begin_ptrtype>(*front_block_controller_ptr);
		controller.front_block.curr_ptr = controller.front_block.begin_ptr = front_begin_ptr;
		controller.front_end_ptr = front_begin_ptr + blockbytes;
	}

	return true;
}

template <typename allocator, typename dequecontroltype>
inline constexpr void deque_grow_back_common_impl(
	dequecontroltype &controller,
	std::size_t align,
	std::size_t bytes) noexcept
{
	if (!::fast_io::containers::details::deque_reserve_back_blocks_impl<allocator>(controller, 1zu, align, bytes))
	{
		return;
	}
	++controller.back_block.controller_ptr;
	auto begin_ptr{*controller.back_block.controller_ptr};
	controller.back_block.begin_ptr = begin_ptr;
	controller.back_block.curr_ptr = begin_ptr;
	controller.back_end_ptr = begin_ptr + bytes;
}

template <typename allocator, ::std::size_t align, ::std::size_t sz, ::std::size_t block_size, typename dequecontroltype>
inline constexpr void deque_grow_back_common(dequecontroltype &controller) noexcept
{
	constexpr ::std::size_t blockbytes{sz * block_size};
	::fast_io::containers::details::deque_grow_back_common_impl<allocator>(controller, align, blockbytes);
}

template <typename allocator, ::std::size_t align, ::std::size_t sz, ::std::size_t block_size, typename dequecontroltype>
inline constexpr void deque_reserve_back_spaces(dequecontroltype &controller, ::std::size_t n)
{
	if (!n)
	{
		return;
	}
	auto back_curr_ptr{controller.back_block.curr_ptr};
	::std::size_t blocksn{static_cast<::std::size_t>(controller.back_end_ptr - back_curr_ptr)};
	if (n <= blocksn)
	{
		return;
	}
	::std::size_t nmblocksn{n - blocksn};
	::std::size_t back_more_blocks{nmblocksn / block_size};
	::std::size_t const back_more_blocks_mod{nmblocksn % block_size};
	::std::size_t toallocate{back_more_blocks};
	if (back_more_blocks_mod)
	{
		++toallocate;
	}
	if consteval
	{
		::fast_io::containers::details::deque_reserve_back_blocks_impl<allocator>(controller,
																				  toallocate, align, block_size);
	}
	else
	{
		constexpr ::std::size_t block_bytes{block_size * sz};
		::fast_io::containers::details::deque_reserve_back_blocks_impl<allocator>(*reinterpret_cast<::fast_io::containers::details::deque_controller_common *>(
																					  __builtin_addressof(controller)),
																				  toallocate, align, block_bytes);
	}
}

template <typename allocator, typename dequecontroltype>
inline constexpr bool deque_reserve_front_blocks_impl(dequecontroltype &controller, ::std::size_t nb, ::std::size_t align, ::std::size_t blockbytes) noexcept
{
	if (controller.controller_block.controller_start_ptr == nullptr)
	{
		::fast_io::containers::details::deque_allocate_on_empty_common_with_n_impl<allocator>(
			controller, align, blockbytes, nb);
		return false;
	}
	using replacetype = typename dequecontroltype::replacetype;
	using begin_ptrtype = replacetype *;

	::std::size_t diff_to_start_ptr =
		static_cast<::std::size_t>(
			controller.front_block.controller_ptr -
			controller.controller_block.controller_start_reserved_ptr);
	if (diff_to_start_ptr < nb)
	{
		::std::size_t distance_front_to_start{
			static_cast<::std::size_t>(controller.front_block.controller_ptr -
									   controller.controller_block.controller_start_ptr)};
		if (distance_front_to_start < nb)
		{
			::fast_io::containers::details::deque_rebalance_or_grow_insertation_impl<allocator>(controller, nb);
		}
		::std::size_t diff_to_start_ptr2 =
			static_cast<::std::size_t>(
				controller.front_block.controller_ptr -
				controller.controller_block.controller_start_reserved_ptr);
		if (diff_to_start_ptr2 < nb)
		{
			::std::size_t back_reserved_blocks{
				static_cast<::std::size_t>(controller.controller_block.controller_after_reserved_ptr -
										   controller.back_block.controller_ptr - 1)};

			::std::size_t back_borrowed_blocks_count{back_reserved_blocks};
			::std::size_t to_allocate_blocks{nb};
			if (nb < back_reserved_blocks)
			{
				back_borrowed_blocks_count = nb;
				to_allocate_blocks = 0u;
			}
			else
			{
				to_allocate_blocks -= back_borrowed_blocks_count;
			}

			auto controller_after_reserved_ptr{
				controller.controller_block.controller_after_reserved_ptr};

			auto controller_start_reserved_ptr{
				controller.controller_block.controller_start_reserved_ptr};

			auto new_controller_start_reserved_ptr{
				controller_start_reserved_ptr - back_borrowed_blocks_count};
			auto new_controller_after_reserved_ptr{
				controller_after_reserved_ptr - back_borrowed_blocks_count};
			::fast_io::freestanding::non_overlapped_copy_n(new_controller_after_reserved_ptr,
														   back_borrowed_blocks_count,
														   new_controller_start_reserved_ptr);
			*(controller.controller_block.controller_after_reserved_ptr = new_controller_after_reserved_ptr) = nullptr;
			// after this line Todo

			auto ed{new_controller_start_reserved_ptr};
			new_controller_start_reserved_ptr -= to_allocate_blocks;
			for (auto i{new_controller_start_reserved_ptr}; i != ed; ++i)
			{
				::std::construct_at(i, static_cast<begin_ptrtype>(allocator::allocate_aligned(align, blockbytes)));
			}
			controller.controller_block.controller_start_reserved_ptr = new_controller_start_reserved_ptr;
		}
	}
	return true;
}

template <typename allocator, ::std::size_t align, ::std::size_t sz, ::std::size_t block_size, typename dequecontroltype>
inline constexpr void deque_reserve_front_spaces(dequecontroltype &controller, ::std::size_t n)
{
	if (!n)
	{
		return;
	}
	auto front_curr_ptr{controller.front_block.curr_ptr};
	::std::size_t blocksn{static_cast<::std::size_t>(front_curr_ptr - controller.front_block.begin_ptr)};
	if (n <= blocksn)
	{
		return;
	}
	::std::size_t nmblocksn{n - blocksn};
	::std::size_t front_more_blocks{nmblocksn / block_size};
	::std::size_t const front_more_blocks_mod{nmblocksn % block_size};
	::std::size_t toallocate{front_more_blocks};
	if (front_more_blocks_mod)
	{
		++toallocate;
	}
	if consteval
	{
		::fast_io::containers::details::deque_reserve_front_blocks_impl<allocator>(controller,
																				   toallocate, align, block_size);
	}
	else
	{
		constexpr ::std::size_t block_bytes{block_size * sz};
		::fast_io::containers::details::deque_reserve_front_blocks_impl<allocator>(*reinterpret_cast<::fast_io::containers::details::deque_controller_common *>(
																					   __builtin_addressof(controller)),
																				   toallocate, align, block_bytes);
	}
}

template <typename allocator, typename dequecontroltype>
inline constexpr void deque_grow_front_common_impl(
	dequecontroltype &controller,
	std::size_t align,
	std::size_t bytes) noexcept
{
	if (!::fast_io::containers::details::deque_reserve_front_blocks_impl<allocator>(controller, 1zu, align, bytes))
	{
		return;
	}
	auto begin_ptr{*(--controller.front_block.controller_ptr)};
	controller.front_block.begin_ptr = begin_ptr;
	auto end_ptr{begin_ptr + bytes};
	controller.front_block.curr_ptr = end_ptr;
	controller.front_end_ptr = end_ptr;
}

template <typename allocator, ::std::size_t align, ::std::size_t sz, ::std::size_t block_size, typename dequecontroltype>
inline constexpr void deque_grow_front_common(dequecontroltype &controller) noexcept
{
	constexpr ::std::size_t blockbytes{sz * block_size};

	::fast_io::containers::details::deque_grow_front_common_impl<allocator>(controller, align, blockbytes);
}

template <typename>
struct is_fast_io_deque_iterator_impl : ::std::false_type
{};

template <typename U>
struct is_fast_io_deque_iterator_impl<
	::fast_io::containers::details::deque_iterator<U, false>> : ::std::true_type
{};

template <typename U>
struct is_fast_io_deque_iterator_impl<
	::fast_io::containers::details::deque_iterator<U, true>> : ::std::true_type
{};

template <typename T>
concept is_fast_io_deque_iterator_v =
	::fast_io::containers::details::is_fast_io_deque_iterator_impl<std::remove_cvref_t<T>>::value;

template <typename Iter>
	requires(::fast_io::containers::details::is_fast_io_deque_iterator_v<Iter>)
inline constexpr Iter uninitialized_relocate_define(
	::fast_io::operations::defines::memory_algorithm_define_type<Iter, Iter, Iter>,
	Iter first, Iter last, Iter dest)
{
	using valtype = ::std::iter_value_t<Iter>;
	if constexpr (::fast_io::freestanding::is_trivially_copyable_or_relocatable_v<valtype>)
	{
		if !consteval
		{
			return ::std::bit_cast<Iter>(
				::fast_io::containers::details::deque_copy_impl(
					::std::bit_cast<::fast_io::containers::details::deque_control_block_common>(first.itercontent),
					::std::bit_cast<::fast_io::containers::details::deque_control_block_common>(last.itercontent),
					::std::bit_cast<::fast_io::containers::details::deque_control_block_common>(dest.itercontent),
					::fast_io::containers::details::deque_block_bytes<sizeof(valtype)>));
		}
	}
	return ::std::bit_cast<Iter>(
		::fast_io::containers::details::deque_relocate_impl(first.itercontent, last.itercontent, dest.itercontent,
															::fast_io::containers::details::deque_block_size<sizeof(valtype)>));
}

template <typename Iter>
	requires(::fast_io::containers::details::is_fast_io_deque_iterator_v<Iter>)
inline constexpr Iter uninitialized_relocate_backward_define(
	::fast_io::operations::defines::memory_algorithm_define_type<Iter, Iter, Iter>,
	Iter first, Iter last, Iter dest)
{
	using valtype = ::std::iter_value_t<Iter>;
	if constexpr (::fast_io::freestanding::is_trivially_copyable_or_relocatable_v<valtype>)
	{
		if !consteval
		{
			return ::std::bit_cast<Iter>(
				::fast_io::containers::details::deque_copy_backward_impl(
					::std::bit_cast<::fast_io::containers::details::deque_control_block_common>(first.itercontent),
					::std::bit_cast<::fast_io::containers::details::deque_control_block_common>(last.itercontent),
					::std::bit_cast<::fast_io::containers::details::deque_control_block_common>(dest.itercontent),
					::fast_io::containers::details::deque_block_bytes<sizeof(valtype)>));
		}
	}
	return ::std::bit_cast<Iter>(
		::fast_io::containers::details::deque_relocate_backward_impl(first.itercontent, last.itercontent, dest.itercontent,
																	 ::fast_io::containers::details::deque_block_size<sizeof(valtype)>));
}

} // namespace details

template <::std::forward_iterator ForwardIt>
inline constexpr ForwardIt rotate_for_fast_io_deque(ForwardIt first, ForwardIt middle, ForwardIt last) noexcept
{
	return ::std::rotate(first, middle, last);
}

template <typename T, typename allocator>
class deque FAST_IO_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE
{
public:
	using value_type = T;
	using pointer = value_type *;
	using reference = value_type &;
	using const_reference = value_type const &;
	using size_type = ::std::size_t;
	using difference_type = ::std::ptrdiff_t;
	using iterator = ::fast_io::containers::details::deque_iterator<T, false>;
	using const_iterator = ::fast_io::containers::details::deque_iterator<T, true>;
	using reverse_iterator = ::std::reverse_iterator<iterator>;
	using const_reverse_iterator = ::std::reverse_iterator<const_iterator>;

private:
	using controller_type = ::fast_io::containers::details::deque_controller<T>;

public:
	controller_type controller;
	static inline constexpr size_type block_size{::fast_io::containers::details::deque_block_size<sizeof(value_type)>};
	inline explicit constexpr deque() noexcept
		: controller{}
	{}

	inline constexpr deque(deque const &other) noexcept(::std::is_nothrow_copy_constructible_v<value_type>)
	{
		this->copy_construct_impl(other.controller);
	}
	inline constexpr deque &operator=(deque const &other) noexcept(::std::is_nothrow_copy_constructible_v<value_type>)
	{
		if (__builtin_addressof(other) == this)
		{
			return *this;
		}
		deque temp(other);
		destroy_deque_controller(this->controller);
		this->controller = temp.controller;
		temp.controller = {};
		return *this;
	}

	inline constexpr deque(deque &&other) noexcept : controller(other.controller)
	{
		other.controller = {};
	}
	inline constexpr deque &operator=(deque &&other) noexcept
	{
		if (__builtin_addressof(other) == this)
		{
			return *this;
		}
		destroy_deque_controller(this->controller);
		this->controller = other.controller;
		other.controller = {};
		return *this;
	}

private:
	struct run_destroy
	{
		controller_type *thiscontroller{};
		inline constexpr run_destroy() noexcept = default;
		inline explicit constexpr run_destroy(controller_type *p) noexcept
			: thiscontroller(p)
		{}
		inline run_destroy(run_destroy const &) = delete;
		inline run_destroy &operator=(run_destroy const &) = delete;
		inline constexpr ~run_destroy()
		{
			if (thiscontroller)
			{
				destroy_deque_controller(*thiscontroller);
			}
		}
	};
	inline constexpr void copy_construct_impl(controller_type const &fromcontroller)
	{
		if constexpr (::std::is_trivially_copyable_v<value_type>)
		{
			if (__builtin_is_constant_evaluated())
			{
				::fast_io::containers::details::deque_clone_trivial_common<allocator, alignof(value_type), 1u, block_size>(controller, fromcontroller);
			}
			else
			{
				::fast_io::containers::details::deque_clone_trivial_common<allocator, alignof(value_type), sizeof(value_type), block_size>(*reinterpret_cast<::fast_io::containers::details::deque_controller_common *>(__builtin_addressof(controller)),
																																		   *reinterpret_cast<::fast_io::containers::details::deque_controller_common const *>(__builtin_addressof(fromcontroller)));
			}
			return;
		}
		else
		{
			if (fromcontroller.front_block.curr_ptr == fromcontroller.back_block.curr_ptr)
			{
				this->controller = {};
				return;
			}

			auto front_controller_ptr{fromcontroller.front_block.controller_ptr};
			auto back_controller_ptr{fromcontroller.back_block.controller_ptr};
			::std::size_t blocks_required{static_cast<::std::size_t>(back_controller_ptr -
																	 front_controller_ptr + 1)};
			constexpr ::std::size_t block_bytes{block_size * sizeof(value_type)};
			::fast_io::containers::details::deque_allocate_init_blocks_dezeroing_impl<allocator>(controller, alignof(value_type), block_bytes, blocks_required, false);

			run_destroy destroyer(__builtin_addressof(this->controller));
			auto dq_back_backup{this->controller.back_block};
			this->controller.back_block = this->controller.front_block;
			auto dq_back_end_ptr_backup{this->controller.back_end_ptr};
			this->controller.back_end_ptr = this->controller.front_end_ptr;
			pointer lastblockbegin;
			if (front_controller_ptr == back_controller_ptr)
			{
				lastblockbegin = controller.front_block.curr_ptr;
			}
			else
			{
				auto destit{controller.front_block.controller_ptr};
				auto pos{fromcontroller.front_block.curr_ptr - fromcontroller.front_block.begin_ptr};
				::fast_io::freestanding::uninitialized_copy(
					fromcontroller.front_block.curr_ptr,
					fromcontroller.front_end_ptr,
					(controller.front_block.curr_ptr =
						 pos + controller.front_block.begin_ptr));
				this->controller.back_block.curr_ptr = controller.front_end_ptr =
					controller.front_block.begin_ptr + block_size;
				++destit;
				for (pointer *it{front_controller_ptr + 1}, *ed{back_controller_ptr}; it != ed; ++it)
				{
					pointer blockptr{*it};
					::fast_io::freestanding::uninitialized_copy_n(blockptr, block_size, *destit);
					auto new_curr_ptr{blockptr + block_size};
					this->controller.back_block = {blockptr, new_curr_ptr, destit};
					this->controller.back_end_ptr = new_curr_ptr;
					++destit;
				}
				lastblockbegin = fromcontroller.back_block.begin_ptr;
			}

			dq_back_backup.curr_ptr =
				::fast_io::freestanding::uninitialized_copy(lastblockbegin,
															fromcontroller.back_block.curr_ptr, dq_back_backup.begin_ptr);

			this->controller.back_block = dq_back_backup;
			this->controller.back_end_ptr = dq_back_end_ptr_backup;
			destroyer.thiscontroller = nullptr;
		}
	}
	inline constexpr void default_construct_impl()
	{
		run_destroy des(__builtin_addressof(this->controller));

		auto front_controller_ptr{controller.front_block.controller_ptr};
		auto back_controller_ptr{controller.back_block.controller_ptr};

		auto dq_back_backup{controller.back_block};
		controller.back_block = controller.front_block;
		auto dq_back_end_ptr_backup{controller.back_end_ptr};
		controller.back_end_ptr = controller.back_begin_ptr;

		T *lastblockbegin;
		if (front_controller_ptr == back_controller_ptr)
		{
			lastblockbegin = controller.front_block.curr_ptr;
		}
		else
		{
			::fast_io::freestanding::uninitialized_default_construct(controller.front_block.curr_ptr, controller.front_end_ptr);
			this->controller.back_block.curr_ptr = this->controller.back_end_ptr;
			for (T **it{front_controller_ptr + 1}, **ed{back_controller_ptr}; it != ed; ++it)
			{
				T *blockptr{*it};
				::fast_io::freestanding::uninitialized_default_construct(blockptr, blockptr + block_size);
				auto new_curr_ptr{blockptr + block_size};
				this->controller.back_block = {blockptr, new_curr_ptr, it};
				this->controller.back_end_ptr = new_curr_ptr;
			}
			lastblockbegin = dq_back_backup.begin_ptr;
		}
		::fast_io::freestanding::uninitialized_default_construct(lastblockbegin, dq_back_backup.curr_ptr);
		this->controller.back_block = dq_back_backup;
		this->controller.back_end_ptr = dq_back_end_ptr_backup;
		des.thiscontroller = nullptr;
	}

public:
	inline explicit constexpr deque(size_type n) noexcept(::fast_io::freestanding::is_zero_default_constructible_v<value_type> ||
														  ::std::is_nothrow_default_constructible_v<value_type>)
	{
		constexpr bool iszeroconstr{::fast_io::freestanding::is_zero_default_constructible_v<value_type>};
		this->init_blocks_common<iszeroconstr>(n);
		if constexpr (!iszeroconstr)
		{
			this->default_construct_impl();
		}
	}

	inline explicit constexpr deque(size_type n, ::fast_io::for_overwrite_t) noexcept(::fast_io::freestanding::is_zero_default_constructible_v<value_type> ||
																					  ::std::is_nothrow_default_constructible_v<value_type>)
	{
		if constexpr (::std::is_trivially_default_constructible_v<value_type>)
		{
			this->init_blocks_common<false>(n);
		}
		else if constexpr (::fast_io::freestanding::is_zero_default_constructible_v<value_type>)
		{
			this->init_blocks_common<true>(n);
		}
		else
		{
			this->init_blocks_common<false>(n);
			this->default_construct_impl();
		}
	}

	template <::std::ranges::range R>
	inline explicit constexpr deque(::fast_io::freestanding::from_range_t, R &&rg)
	{
		this->construct_deque_common_impl(::std::ranges::begin(rg), ::std::ranges::end(rg));
	}

	inline explicit constexpr deque(::std::initializer_list<value_type> ilist) noexcept(::std::is_nothrow_copy_constructible_v<value_type>)
	{
		this->construct_deque_common_impl(ilist.begin(), ilist.end());
	}

private:
	template <typename Iter, typename Sentinel>
	inline constexpr void construct_deque_common_impl(Iter first, Sentinel last)
	{
		if (first == last)
		{
			controller = {};
			return;
		}
		run_destroy des(__builtin_addressof(this->controller));
		if constexpr (::std::sized_sentinel_for<Sentinel, Iter>)
		{
			auto const dist{::std::ranges::distance(first, last)};

			this->init_blocks_common<false>(static_cast<::std::size_t>(dist));
			auto front_controller_ptr{controller.front_block.controller_ptr};
			auto back_controller_ptr{controller.back_block.controller_ptr};
			auto dq_back_backup{this->controller.back_block};
			this->controller.back_block = this->controller.front_block;
			auto dq_back_end_ptr_backup{controller.back_end_ptr};
			controller.back_end_ptr = controller.front_end_ptr;

			T *lastblockbegin;
			if (front_controller_ptr == back_controller_ptr)
			{
				lastblockbegin = controller.front_block.curr_ptr;
			}
			else
			{
				for (T **it{front_controller_ptr}, **ed{back_controller_ptr}; it != ed; ++it)
				{
					T *blockptr{*it};
					first = ::fast_io::containers::details::uninitialized_copy_n_for_deque(first, block_size, blockptr).from;
					auto new_curr_ptr{blockptr + block_size};
					this->controller.back_block = {blockptr, new_curr_ptr, it};
					this->controller.back_end_ptr = new_curr_ptr;
				}
				lastblockbegin = dq_back_backup.begin_ptr;
			}
			::fast_io::containers::details::uninitialized_copy_n_for_deque(
				first,
				static_cast<::std::size_t>(dq_back_backup.curr_ptr - lastblockbegin),
				lastblockbegin);
			this->controller.back_block = dq_back_backup;
			this->controller.back_end_ptr = dq_back_end_ptr_backup;
		}
		else
		{
			this->controller = {};
			for (; first != last; ++first)
			{
				this->push_back(*first);
			}
		}
		des.thiscontroller = nullptr;
	}

	template <bool iszeroconstr>
	inline constexpr void init_blocks_common(::std::size_t n) noexcept
	{
		if (__builtin_is_constant_evaluated())
		{
			::fast_io::containers::details::deque_init_space_common<allocator, alignof(value_type), 1u, block_size, iszeroconstr>(controller, n);
		}
		else
		{
			::fast_io::containers::details::deque_init_space_common<allocator, alignof(value_type), sizeof(value_type), block_size, iszeroconstr>(*reinterpret_cast<::fast_io::containers::details::deque_controller_common *>(__builtin_addressof(controller)), n);
		}
	}
	inline static constexpr void destroy_elements_range(
		::fast_io::containers::details::deque_control_block<value_type> const &first,
		::fast_io::containers::details::deque_control_block<value_type> const &last) noexcept
	{
		if constexpr (!::std::is_trivially_destructible_v<value_type>)
		{
			auto front_controller_ptr{first.controller_ptr};
			auto back_controller_ptr{last.controller_ptr};
			T *lastblockbegin;
			if (front_controller_ptr == back_controller_ptr)
			{
				lastblockbegin = first.curr_ptr;
			}
			else
			{
				::std::destroy(first.curr_ptr, first.begin_ptr + block_size);
				for (T **it{front_controller_ptr + 1}, **ed{back_controller_ptr}; it != ed; ++it)
				{
					T *blockptr{*it};
					::std::destroy(blockptr, blockptr + block_size);
				}
				lastblockbegin = last.begin_ptr;
			}
			::std::destroy(lastblockbegin, last.curr_ptr);
		}
	}

	inline static constexpr void destroy_all_elements(controller_type &controller) noexcept
	{
		auto front_controller_ptr{controller.front_block.controller_ptr};
		auto back_controller_ptr{controller.back_block.controller_ptr};
		T *lastblockbegin;
		if (front_controller_ptr == back_controller_ptr)
		{
			lastblockbegin = controller.front_block.curr_ptr;
		}
		else
		{
			::std::destroy(controller.front_block.curr_ptr, controller.front_end_ptr);
			for (T **it{front_controller_ptr + 1}, **ed{back_controller_ptr}; it != ed; ++it)
			{
				T *blockptr{*it};
				::std::destroy(blockptr, blockptr + block_size);
			}
			lastblockbegin = controller.back_block.begin_ptr;
		}
		::std::destroy(lastblockbegin, controller.back_block.curr_ptr);
	}

	inline static constexpr void destroy_deque_controller(controller_type &controller) noexcept
	{
		if constexpr (!::std::is_trivially_destructible_v<value_type>)
		{
			destroy_all_elements(controller);
		}
		::fast_io::containers::details::deque_destroy_trivial_common<allocator, alignof(value_type), sizeof(value_type)>(controller.controller_block);
	}

#if __has_cpp_attribute(__gnu__::__cold__)
	[[__gnu__::__cold__]]
#endif
	inline constexpr void grow_front() noexcept
	{
		if (__builtin_is_constant_evaluated())
		{
			::fast_io::containers::details::deque_grow_front_common<allocator, alignof(value_type), 1u, block_size>(controller);
		}
		else
		{
			::fast_io::containers::details::deque_grow_front_common<allocator, alignof(value_type), sizeof(value_type), block_size>(*reinterpret_cast<::fast_io::containers::details::deque_controller_common *>(__builtin_addressof(controller)));
		}
	}

#if __has_cpp_attribute(__gnu__::__cold__)
	[[__gnu__::__cold__]]
#endif
	inline constexpr void grow_back() noexcept
	{
		if (__builtin_is_constant_evaluated())
		{
			::fast_io::containers::details::deque_grow_back_common<allocator, alignof(value_type), 1u, block_size>(controller);
		}
		else
		{
			::fast_io::containers::details::deque_grow_back_common<allocator, alignof(value_type), sizeof(value_type), block_size>(*reinterpret_cast<::fast_io::containers::details::deque_controller_common *>(__builtin_addressof(controller)));
		}
	}

	inline constexpr void front_backspace() noexcept
	{
		auto front_controller_ptr{controller.front_block.controller_ptr};
		if (front_controller_ptr == controller.back_block.controller_ptr) [[unlikely]]
		{
			auto begin_ptr{controller.front_block.begin_ptr};
			constexpr ::std::size_t half_blocks_size{block_size >> 1u};
			controller.back_block.curr_ptr = (controller.front_block.curr_ptr = begin_ptr + half_blocks_size);
			return;
		}
		controller.front_end_ptr = (controller.front_block.curr_ptr = controller.front_block.begin_ptr = *(controller.front_block.controller_ptr = front_controller_ptr + 1)) + block_size;
	}

	inline constexpr void back_backspace() noexcept
	{
		auto back_controller_ptr{controller.back_block.controller_ptr};
		if (back_controller_ptr == controller.front_block.controller_ptr) [[unlikely]]
		{
			auto begin_ptr{controller.back_block.begin_ptr};
			constexpr ::std::size_t half_blocks_size{block_size >> 1u};
			controller.front_block.curr_ptr = (controller.back_block.curr_ptr = begin_ptr + half_blocks_size);
			return;
		}
		controller.back_block.curr_ptr = (controller.back_end_ptr = ((controller.back_block.begin_ptr = *(controller.back_block.controller_ptr = back_controller_ptr - 1)) + block_size));
	}

public:
	inline constexpr void clear() noexcept
	{
		if constexpr (!::std::is_trivially_destructible_v<value_type>)
		{
			destroy_all_elements(this->controller);
		}
		if (__builtin_is_constant_evaluated())
		{
			::fast_io::containers::details::deque_clear_common<allocator, 1u, block_size>(controller);
		}
		else
		{
			::fast_io::containers::details::deque_clear_common<allocator, sizeof(value_type), block_size>(*reinterpret_cast<::fast_io::containers::details::deque_controller_common *>(__builtin_addressof(controller)));
		}
	}
	template <typename... Args>
		requires ::std::constructible_from<value_type, Args...>
	inline constexpr reference emplace_back(Args &&...args) noexcept(::std::is_nothrow_constructible_v<value_type, Args...>)
	{
		if (controller.back_block.curr_ptr == controller.back_end_ptr) [[unlikely]]
		{
			grow_back();
		}
		auto currptr{controller.back_block.curr_ptr};
		::std::construct_at(currptr, ::std::forward<Args>(args)...);
		controller.back_block.curr_ptr = currptr + 1;
		return *currptr;
	}

	inline constexpr void push_back(value_type const &value) noexcept(::std::is_nothrow_copy_constructible_v<value_type>)
	{
		this->emplace_back(value);
	}

	inline constexpr void push_back(value_type &&value) noexcept
	{
		this->emplace_back(::std::move(value));
	}
	inline constexpr void pop_back() noexcept
	{
		if (controller.front_block.curr_ptr == controller.back_block.curr_ptr) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		pop_back_unchecked();
	}

	inline constexpr void pop_back_unchecked() noexcept
	{
		if constexpr (!::std::is_trivially_destructible_v<value_type>)
		{
			::std::destroy_at(controller.back_block.curr_ptr - 1);
		}
		if (--controller.back_block.curr_ptr == controller.back_block.begin_ptr) [[unlikely]]
		{
			this->back_backspace();
		}
	}

	inline constexpr reference back_unchecked() noexcept
	{
		return controller.back_block.curr_ptr[-1];
	}

	inline constexpr const_reference back_unchecked() const noexcept
	{
		return controller.back_block.curr_ptr[-1];
	}

	inline constexpr reference back() noexcept
	{
		if (controller.front_block.curr_ptr == controller.back_block.curr_ptr) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}

		return controller.back_block.curr_ptr[-1];
	}

	inline constexpr const_reference back() const noexcept
	{
		if (controller.front_block.curr_ptr == controller.back_block.curr_ptr) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}

		return controller.back_block.curr_ptr[-1];
	}

private:
	struct emplace_front_guard
	{
		using handletype = ::fast_io::containers::details::deque_controller<value_type>;
		handletype *thisdeq;
		explicit constexpr emplace_front_guard(handletype *other) noexcept : thisdeq{other}
		{
		}
		emplace_front_guard(emplace_front_guard const &) = delete;
		emplace_front_guard &operator=(emplace_front_guard const &) = delete;
		constexpr ~emplace_front_guard()
		{
			if (this->thisdeq)
			{
				auto &frontblock{this->thisdeq->front_block};
				if (frontblock.curr_ptr ==
						this->thisdeq->front_end_ptr &&
					frontblock.controller_ptr !=
						this->thisdeq->back_block.controller_ptr)
				{
					this->thisdeq->front_end_ptr = ((frontblock.curr_ptr = frontblock.begin_ptr = *(++frontblock.controller_ptr)) + block_size);
				}
			}
		}
	};

public:
	template <typename... Args>
		requires ::std::constructible_from<value_type, Args...>
	inline constexpr reference emplace_front(Args &&...args) noexcept(::std::is_nothrow_constructible_v<value_type, Args...>)
	{
		if (controller.front_block.curr_ptr == controller.front_block.begin_ptr) [[unlikely]]
		{
			grow_front();
		}
		auto front_curr_ptr{controller.front_block.curr_ptr};
		if constexpr (::std::is_nothrow_constructible_v<value_type, Args...>)
		{
			return *(controller.front_block.curr_ptr = ::std::construct_at(front_curr_ptr - 1, ::std::forward<Args>(args)...));
		}
		else
		{
			emplace_front_guard guard(__builtin_addressof(this->controller));
			front_curr_ptr = ::std::construct_at(front_curr_ptr - 1, ::std::forward<Args>(args)...);
			guard.thisdeq = nullptr;
			return *(controller.front_block.curr_ptr = front_curr_ptr);
		}
	}

	inline constexpr void push_front(value_type const &value)
	{
		this->emplace_front(value);
	}

	inline constexpr void push_front(value_type &&value) noexcept
	{
		this->emplace_front(::std::move(value));
	}

	inline constexpr void pop_front() noexcept
	{
		if (controller.front_block.curr_ptr == controller.back_block.curr_ptr) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}

		pop_front_unchecked();
	}

	inline constexpr void pop_front_unchecked() noexcept
	{
		if constexpr (!::std::is_trivially_destructible_v<value_type>)
		{
			::std::destroy_at(controller.front_block.curr_ptr);
		}

		if (++controller.front_block.curr_ptr == controller.front_end_ptr) [[unlikely]]
		{
			this->front_backspace();
		}
	}

	inline constexpr reference front_unchecked() noexcept
	{
		return *controller.front_block.curr_ptr;
	}

	inline constexpr const_reference front_unchecked() const noexcept
	{
		return *controller.front_block.curr_ptr;
	}

	inline constexpr reference front() noexcept
	{
		if (controller.front_block.curr_ptr == controller.back_block.curr_ptr) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}

		return *controller.front_block.curr_ptr;
	}

	inline constexpr const_reference front() const noexcept
	{
		if (controller.front_block.curr_ptr == controller.back_block.curr_ptr) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}

		return *controller.front_block.curr_ptr;
	}

	inline constexpr reference operator[](size_type index) noexcept
	{
		if (size() <= index) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		return ::fast_io::containers::details::deque_index_container_unsigned(controller.front_block, index);
	}

	inline constexpr const_reference operator[](size_type index) const noexcept
	{
		if (size() <= index) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		return ::fast_io::containers::details::deque_index_container_unsigned(controller.front_block, index);
	}

	inline constexpr reference index_unchecked(size_type index) noexcept
	{
		return ::fast_io::containers::details::deque_index_container_unsigned(controller.front_block, index);
	}

	inline constexpr const_reference index_unchecked(size_type index) const noexcept
	{
		return ::fast_io::containers::details::deque_index_container_unsigned(controller.front_block, index);
	}

	static inline constexpr size_type max_size() noexcept
	{
		constexpr size_type mxval{::std::numeric_limits<::std::size_t>::max() / sizeof(value_type)};
		return mxval;
	}

	static inline constexpr size_type max_size_bytes() noexcept
	{
		constexpr size_type mxval{::std::numeric_limits<::std::size_t>::max() / sizeof(value_type) * sizeof(value_type)};
		return mxval;
	}

	inline constexpr size_type size() const noexcept
	{
		return block_size * static_cast<size_type>(controller.back_block.controller_ptr - controller.front_block.controller_ptr) + static_cast<size_type>((controller.back_block.curr_ptr - controller.back_block.begin_ptr) + (controller.front_block.begin_ptr - controller.front_block.curr_ptr));
	}

	inline constexpr size_type size_bytes() const noexcept
	{
		return size() * sizeof(value_type);
	}

	inline constexpr iterator begin() noexcept
	{
		return {this->controller.front_block};
	}

	inline constexpr const_iterator begin() const noexcept
	{
		return {this->controller.front_block};
	}

	inline constexpr const_iterator cbegin() const noexcept
	{
		return {this->controller.front_block};
	}

	inline constexpr reverse_iterator rend() noexcept
	{
		return reverse_iterator({this->controller.front_block});
	}

	inline constexpr const_reverse_iterator rend() const noexcept
	{
		return const_reverse_iterator({this->controller.front_block});
	}

	inline constexpr const_reverse_iterator crend() const noexcept
	{
		return const_reverse_iterator({this->controller.front_block});
	}

private:
	inline constexpr ::fast_io::containers::details::deque_control_block<value_type> end_common() const noexcept
	{
		::fast_io::containers::details::deque_control_block<value_type> backblock{this->controller.back_block};
		if (backblock.curr_ptr == this->controller.back_end_ptr) [[unlikely]]
		{
			if (backblock.controller_ptr) [[likely]]
			{
				backblock.curr_ptr = backblock.begin_ptr = (*++backblock.controller_ptr);
			}
		}
		return {backblock};
	}

public:
	inline constexpr iterator end() noexcept
	{
		return {this->end_common()};
	}

	inline constexpr const_iterator end() const noexcept
	{
		return {this->end_common()};
	}

	inline constexpr const_iterator cend() const noexcept
	{
		return {this->end_common()};
	}

	inline constexpr reverse_iterator rbegin() noexcept
	{
		return reverse_iterator({this->end_common()});
	}

	inline constexpr const_reverse_iterator rbegin() const noexcept
	{
		return const_reverse_iterator({this->end_common()});
	}

	inline constexpr const_reverse_iterator crbegin() const noexcept
	{
		return const_reverse_iterator({this->end_common()});
	}

	inline constexpr bool empty() const noexcept
	{
		return controller.front_block.curr_ptr == controller.back_block.curr_ptr;
	}

	inline constexpr bool is_empty() const noexcept
	{
		return controller.front_block.curr_ptr == controller.back_block.curr_ptr;
	}

	inline constexpr void clear_destroy() noexcept
	{
		destroy_deque_controller(this->controller);
		this->controller = {};
	}

private:
	struct insert_range_result
	{
		size_type pos;
		iterator it;
	};
	inline constexpr insert_range_result insert_n_front_common_impl(size_type pos, size_type rgsize) noexcept
	{
		::fast_io::containers::details::deque_reserve_front_spaces<allocator,
																   alignof(value_type), sizeof(value_type), block_size>(this->controller, rgsize);
		auto thisbg{this->begin()};
		auto posit{thisbg + pos};
		auto thisbgrgsize{thisbg - rgsize};
		auto thisbgrgsizenew{::fast_io::freestanding::uninitialized_relocate(thisbg,
																			 posit, thisbgrgsize)};
		this->controller.front_block = thisbgrgsize.itercontent;
		this->controller.front_end_ptr = thisbgrgsize.itercontent.begin_ptr + block_size;
		return {pos, thisbgrgsizenew};
	}
	template <::std::ranges::range R>
		requires ::std::constructible_from<value_type, ::std::ranges::range_value_t<R>>
	inline constexpr insert_range_result insert_range_front_impl(size_type pos, R &&rg, size_type rgsize) noexcept(::std::is_nothrow_constructible_v<value_type, ::std::ranges::range_value_t<R>>)
	{
		auto ret{this->insert_n_front_common_impl(pos, rgsize)};
		::fast_io::freestanding::uninitialized_copy_n(::std::ranges::cbegin(rg), rgsize, ret.it);
		return ret;
	}
	inline constexpr insert_range_result insert_n_back_common_impl(size_type pos, size_type rgsize) noexcept
	{
		::fast_io::containers::details::deque_reserve_back_spaces<allocator,
																  alignof(value_type), sizeof(value_type), block_size>(this->controller, rgsize);
		auto posit{this->begin() + pos};
		auto thisend{this->end()};
		auto thisendrgsize{thisend + rgsize};
		::fast_io::freestanding::uninitialized_relocate_backward(posit,
																 thisend, thisendrgsize);
		if (thisendrgsize.itercontent.begin_ptr == thisendrgsize.itercontent.curr_ptr)
		{
			thisendrgsize.itercontent.curr_ptr =
				(thisendrgsize.itercontent.begin_ptr = *--thisendrgsize.itercontent.controller_ptr) + block_size;
		}
		this->controller.back_block = thisendrgsize.itercontent;
		this->controller.back_end_ptr = thisendrgsize.itercontent.begin_ptr + block_size;
		return {pos, posit};
	}
	template <::std::ranges::range R>
		requires ::std::constructible_from<value_type, ::std::ranges::range_value_t<R>>
	inline constexpr insert_range_result insert_range_back_impl(size_type pos, R &&rg, size_type rgsize) noexcept(::std::is_nothrow_constructible_v<value_type, ::std::ranges::range_value_t<R>>)
	{
		auto ret{this->insert_n_back_common_impl(pos, rgsize)};
		::fast_io::freestanding::uninitialized_copy_n(::std::ranges::cbegin(rg), rgsize, ret.it);
		return ret;
	}
	template <::std::ranges::range R>
		requires ::std::constructible_from<value_type, ::std::ranges::range_value_t<R>>
	inline constexpr insert_range_result insert_range_impl(size_type pos, R &&rg, size_type old_size) noexcept(::std::is_nothrow_constructible_v<value_type, ::std::ranges::range_value_t<R>>)
	{
		if constexpr (::std::ranges::sized_range<R>)
		{
			size_type const rgsize{::std::ranges::size(rg)};
			if (!rgsize)
			{
				return {pos, this->begin() + pos};
			}
			size_type const half_size{old_size >> 1u};
			insert_range_result ret;
			if (pos < half_size)
			{
				ret = this->insert_n_front_common_impl(pos, rgsize);
			}
			else
			{
				ret = this->insert_n_back_common_impl(pos, rgsize);
			}
			::fast_io::freestanding::uninitialized_copy_n(::std::ranges::cbegin(rg), rgsize, ret.it);
			return ret;
		}
		else
		{
			size_type retpos;
			iterator retit, rotfirst, rotmid, rotlast;
			size_type const quarterold_size{old_size >> 2u};
			if (pos < quarterold_size)
			{
				this->prepend_range(rg);
				size_type const new_size{this->size()};
				size_type const inserted{new_size - old_size};
				auto bg{this->begin()};
				size_type newpos{pos + inserted};
				rotfirst = bg;
				rotmid = bg + inserted;
				retpos = newpos;
				retit = rotlast = bg + newpos;
			}
			else
			{
				this->append_range(rg);
				auto bg{this->begin()};
				rotfirst = retit = bg + pos;
				rotmid = bg + old_size;
				rotlast = this->end();
				retpos = pos;
			}
			::fast_io::containers::rotate_for_fast_io_deque(rotfirst, rotmid, rotlast);
			return {retpos, retit};
		}
	}

public:
	template <::std::ranges::range R>
		requires ::std::constructible_from<value_type, ::std::ranges::range_value_t<R>>
	inline constexpr iterator insert_range(const_iterator pos, R &&rg) noexcept(::std::is_nothrow_constructible_v<value_type, ::std::ranges::range_value_t<R>>)
	{
		return this->insert_range_impl(
					   ::fast_io::containers::details::deque_iter_difference_unsigned_common(pos.itercontent, this->controller.front_block), ::std::forward<R>(rg), this->size())
			.it;
	}

	template <::std::ranges::range R>
		requires ::std::constructible_from<value_type, ::std::ranges::range_value_t<R>>
	inline constexpr size_type insert_range_index(size_type pos, R &&rg) noexcept(::std::is_nothrow_constructible_v<value_type, ::std::ranges::range_value_t<R>>)
	{
		size_type const n{this->size()};
		if (n < pos) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		return this->insert_range_impl(pos, ::std::forward<R>(rg), n).pos;
	}

private:
	template <bool isprepend>
	struct prepend_or_append_range_guard
	{
		deque *thisdeq{};
		size_type oldn{};
		constexpr ~prepend_or_append_range_guard()
		{
			if (thisdeq)
			{
				if constexpr (isprepend)
				{
					thisdeq->erase(thisdeq->cbegin(), thisdeq->cend() - oldn);
				}
				else
				{
					thisdeq->erase(thisdeq->cbegin() + oldn, thisdeq->cend());
				}
			}
		}
	};
	using append_range_guard = prepend_or_append_range_guard<false>;
	using prepend_range_guard = prepend_or_append_range_guard<true>;

public:
	template <::std::ranges::range R>
		requires ::std::constructible_from<value_type, ::std::ranges::range_value_t<R>>
	inline constexpr void append_range(R &&rg) noexcept(::std::is_nothrow_constructible_v<value_type, ::std::ranges::range_value_t<R>>)
	{
		if constexpr (::std::ranges::sized_range<R>)
		{
			size_type const rgsize{::std::ranges::size(rg)};
			if (!rgsize)
			{
				return;
			}
			// To do write append specific code without using insert_range
			this->insert_range_back_impl(this->size(), ::std::forward<R>(rg), rgsize);
		}
		else
		{
			// To do: cleanup code
			if constexpr (::std::is_nothrow_constructible_v<value_type, ::std::ranges::range_value_t<R>>)
			{
				for (auto &e : rg)
				{
					this->push_back(e);
				}
			}
			else
			{
				append_range_guard guard{this, this->size()};
				for (auto &e : rg)
				{
					this->push_back(e);
				}
				guard.thisdeq = nullptr;
			}
		}
	}
#if 0
private:
	template <::std::ranges::range R>
		requires ::std::constructible_from<value_type, ::std::ranges::range_value_t<R>>
	inline constexpr size_type prepend_range_impl(R &&rg) noexcept(::std::is_nothrow_constructible_v<value_type, ::std::ranges::range_value_t<R>>)
	{
		size_type const old_size{this->size()};
		for (auto &e : rg)
		{
			this->push_front(e);
		}
		return this->size() - old_size;
	}
#endif
public:
	template <::std::ranges::range R>
		requires ::std::constructible_from<value_type, ::std::ranges::range_value_t<R>>
	inline constexpr void prepend_range(R &&rg) noexcept(
		::std::is_nothrow_constructible_v<value_type, ::std::ranges::range_value_t<R>> &&
		::std::is_nothrow_swappable_v<value_type>)
	{
		if constexpr (::std::ranges::sized_range<R>)
		{
			size_type const rgsize{::std::ranges::size(rg)};
			if (!rgsize)
			{
				return;
			}
			this->insert_range_front_impl(0, ::std::forward<R>(rg), rgsize);
		}
		else
		{
			// To do: cleanup code
			size_type oldn{this->size()};
			if constexpr (
				::std::is_nothrow_constructible_v<value_type, ::std::ranges::range_value_t<R>> &&
				::std::is_nothrow_swappable_v<value_type>)
			{
				for (auto &e : rg)
				{
					this->push_front(e);
				}
				::std::reverse(this->begin(), this->end() - oldn);
			}
			else
			{
				prepend_range_guard guard{this, oldn};
				for (auto &e : rg)
				{
					this->push_front(e);
				}
				::std::reverse(this->begin(), this->end() - oldn);
				guard.thisdeq = nullptr;
			}
		}
	}

private:
	inline constexpr insert_range_result emplace_index_n_impl(size_type pos, size_type n, size_type old_size) noexcept
	{
		size_type const half_size{old_size >> 1u};
		insert_range_result ret;
		if (pos < half_size)
		{
			ret = this->insert_n_front_common_impl(pos, n);
		}
		else
		{
			ret = this->insert_n_back_common_impl(pos, n);
		}
		return ret;
	}
	inline constexpr insert_range_result emplace_index_impl(size_type pos, size_type n) noexcept
	{
		return this->emplace_index_n_impl(pos, 1u, n);
	}
	inline constexpr insert_range_result emplace_impl(size_type pos) noexcept
	{
		return this->emplace_index_impl(pos, this->size());
	}

	struct emplace_decision
	{
		::std::size_t pos;
		::std::int_fast8_t decision;
	};
	template <bool isnothrow>
	inline constexpr ::std::conditional_t<isnothrow,
										  iterator, emplace_decision>
	emplace_decision_common(const_iterator iter) noexcept
	{
		// eh safety for being and end.
		auto iter_curr_ptr{iter.itercontent.curr_ptr};
		if (iter_curr_ptr == this->controller.back_block.curr_ptr ||
			this->controller.back_block.controller_ptr < iter.itercontent.controller_ptr)
		{
			if (this->controller.back_block.curr_ptr != controller.back_end_ptr) [[likely]]
			{
				iterator retit{this->controller.back_block.begin_ptr,
							   this->controller.back_block.curr_ptr++,
							   this->controller.back_block.controller_ptr};
				if constexpr (isnothrow)
				{
					return retit;
				}
				else
				{
					return emplace_decision{retit, 1};
				}
			}
		}
		else if (iter_curr_ptr == this->controller.front_block.curr_ptr)
		{
			if (this->controller.front_block.curr_ptr != this->controller.front_block.begin_ptr) [[likely]]
			{
				iterator retit{this->controller.front_block.begin_ptr,
							   --this->controller.front_block.curr_ptr,
							   this->controller.front_block.controller_ptr};
				if constexpr (isnothrow)
				{
					return retit;
				}
				else
				{
					return emplace_decision{retit, -1};
				}
			}
		}
		iterator retit{this->emplace_impl(::fast_io::containers::details::deque_iter_difference_unsigned_common(iter.itercontent, this->controller.front_block)).it};
		if constexpr (isnothrow)
		{
			return retit;
		}
		else
		{
			return emplace_decision{retit, 0};
		}
	}
	struct emplace_guard
	{
		deque *thisdeq;
		iterator retit;
		::std::int_fast8_t decision;
		constexpr ~emplace_guard()
		{
			if (thisdeq) [[unlikely]]
			{
				auto &thiscontroller{thisdeq->controller};
				if (decision < 0)
				{
					++thiscontroller.front_block.curr_ptr;
				}
				else if (0 < decision)
				{
					--thiscontroller.back_block.curr_ptr;
				}
				else
				{
					::std::size_t const distofront{
						::fast_io::containers::details::deque_iter_difference_unsigned_common(retit.itercontent, thisdeq->controller.front_block)};
					::std::size_t const deqsize{thisdeq->size()};
					thisdeq->erase_unchecked_single_nodestroy_impl(retit, static_cast<::std::size_t>(deqsize - distofront) < distofront);
				}
			}
		}
	};
	struct emplace_index_decision
	{
		pointer retptr;
		::std::int_fast8_t decision;
	};
	template <bool isnothrow>
	inline constexpr ::std::conditional_t<isnothrow,
										  pointer, emplace_index_decision>
	emplace_index_decision_common(::std::size_t idx) noexcept
	{

		auto oldsize{this->size()};
		if (oldsize < idx) [[unlikely]]
		{
			::fast_io::fast_terminate();
#if __has_cpp_attribute(unreachable)
			[[unreachable]];
#endif
		}
#if 1
		else if (idx == oldsize)
		{
			if (this->controller.back_block.curr_ptr != controller.back_end_ptr) [[likely]]
			{
				pointer retptr{this->controller.back_block.curr_ptr++};
				if constexpr (isnothrow)
				{
					return retptr;
				}
				else
				{
					return emplace_index_decision{retptr, 1};
				}
			}
		}
		else if (!idx)
		{
			if (this->controller.front_block.curr_ptr != this->controller.front_block.begin_ptr) [[likely]]
			{
				pointer retptr{--this->controller.front_block.curr_ptr};
				if constexpr (isnothrow)
				{
					return retptr;
				}
				else
				{
					return emplace_index_decision{retptr, -1};
				}
			}
		}
#endif
		auto result{this->emplace_index_impl(idx, oldsize)};
		pointer retptr{result.it.itercontent.curr_ptr};
		if constexpr (isnothrow)
		{
			return retptr;
		}
		else
		{
			return emplace_index_decision{retptr, 0};
		}
	}

	struct emplace_index_guard
	{
		deque *thisdeq;
		::std::size_t pos;
		::std::size_t oldsize;
		::std::int_fast8_t decision;
		constexpr ~emplace_index_guard()
		{
			if (thisdeq) [[unlikely]]
			{
				auto &thiscontroller{thisdeq->controller};
				if (decision < 0)
				{
					++thiscontroller.front_block.curr_ptr;
				}
				else if (0 < decision)
				{
					--thiscontroller.back_block.curr_ptr;
				}
				else
				{
					thisdeq->erase_unchecked_single_nodestroy_impl(
						thisdeq->begin() + static_cast<::std::ptrdiff_t>(pos),
						static_cast<::std::size_t>(oldsize - pos) <= pos);
				}
			}
		}
	};

public:
	template <typename... Args>
	inline constexpr iterator emplace(const_iterator iter, Args &&...args) noexcept(::std::is_nothrow_constructible_v<value_type, Args...>)
	{
		if constexpr (::std::is_nothrow_constructible_v<value_type, Args...>)
		{
			auto retit = this->emplace_decision_common<true>(iter);
			::std::construct_at(retit.itercontent.curr_ptr, ::std::forward<Args>(args)...);
			return retit;
		}
		else
		{
			auto [retit, decision] = this->emplace_decision_common<false>(iter);
			emplace_guard guard{this, retit, decision};
			::std::construct_at(retit.itercontent.curr_ptr, ::std::forward<Args>(args)...);
			guard.thisdeq = nullptr;
			return retit;
		}
	}
	template <typename... Args>
	inline constexpr reference emplace_index(size_type idx, Args &&...args) noexcept(::std::is_nothrow_constructible_v<value_type, Args...>)
	{
		// bounds checking && eh safety for being and end.
		auto oldsize{this->size()};
		if (oldsize < idx) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		if constexpr (::std::is_nothrow_constructible_v<value_type, Args...>)
		{
			auto retptr = this->emplace_index_decision_common<true>(idx);
			::std::construct_at(retptr, ::std::forward<Args>(args)...);
			return *retptr;
		}
		else
		{
			auto [retptr, decision] = this->emplace_index_decision_common<false>(idx);
			emplace_index_guard guard{this, idx, oldsize, decision};
			::std::construct_at(retptr, ::std::forward<Args>(args)...);
			guard.thisdeq = nullptr;
			return *retptr;
		}
	}

	inline constexpr iterator insert(const_iterator iter, const_reference val) noexcept(::std::is_nothrow_copy_constructible_v<value_type>)
	{
		return this->emplace(iter, val);
	}
	inline constexpr iterator insert(const_iterator iter, value_type &&val) noexcept(::std::is_nothrow_move_constructible_v<value_type>)
	{
		return this->emplace(iter, ::std::move(val));
	}
	inline constexpr reference insert_index(size_type idx, const_reference val) noexcept(::std::is_nothrow_copy_constructible_v<value_type>)
	{
		return this->emplace_index(idx, val);
	}
	inline constexpr reference insert_index(size_type idx, value_type &&val) noexcept(::std::is_nothrow_move_constructible_v<value_type>)
	{
		return this->emplace_index(idx, ::std::move(val));
	}

#if 0
	inline constexpr iterator insert(const_iterator iter, size_type count, const_reference val) noexcept(::std::is_nothrow_copy_constructible_v<value_type>)
	{
	}
	inline constexpr iterator insert_index(size_type idx, size_type count, const_reference val) noexcept(::std::is_nothrow_copy_constructible_v<value_type>)
	{
		
	}
#endif
private:
	inline constexpr iterator erase_no_destroy_common_impl(iterator first, iterator last, bool moveleft) noexcept
	{
		::fast_io::containers::details::deque_control_block<value_type> back_block;
		if (moveleft)
		{
			this->controller.front_block = ::fast_io::freestanding::uninitialized_relocate_backward(this->begin(), first, last).itercontent;
			first = last;
			back_block = this->controller.back_block;
		}
		else
		{
			back_block = ::fast_io::freestanding::uninitialized_relocate(last, this->end(), first).itercontent;
		}
		if (back_block.begin_ptr == back_block.curr_ptr)
		{
			if (this->controller.front_block.controller_ptr != back_block.controller_ptr)
			{
				back_block.curr_ptr = ((back_block.begin_ptr = (*--back_block.controller_ptr)) + block_size);
			}
		}
		this->controller.back_block = back_block;
		this->controller.back_end_ptr = back_block.begin_ptr + block_size;
		return first;
	}
	inline constexpr iterator erase_unchecked_impl(iterator first, iterator last, bool moveleft) noexcept
	{
		if (first == last)
		{
			return first;
		}
		if constexpr (!::std::is_trivially_destructible_v<value_type>)
		{
			this->destroy_elements_range(first, last);
		}
		if constexpr (::fast_io::freestanding::is_trivially_copyable_or_relocatable_v<value_type>)
		{
			if !consteval
			{
				constexpr size_type blockbytes{block_size * sizeof(value_type)};
				return ::std::bit_cast<iterator>(
					::fast_io::containers::details::deque_erase_common_trivial_impl(
						*reinterpret_cast<::fast_io::containers::details::deque_controller_common *>(__builtin_addressof(
							this->controller)),
						::std::bit_cast<::fast_io::containers::details::deque_control_block_common>(first.itercontent),
						::std::bit_cast<::fast_io::containers::details::deque_control_block_common>(last.itercontent), moveleft, blockbytes));
			}
		}
		return this->erase_no_destroy_common_impl(first, last, moveleft);
	}
	inline constexpr iterator erase_unchecked_single_nodestroy_impl(iterator pos, bool moveleft) noexcept
	{
		if constexpr (::fast_io::freestanding::is_trivially_copyable_or_relocatable_v<value_type>)
		{
			if !consteval
			{
				auto posp1{pos.itercontent};
				if (++posp1.curr_ptr == posp1.begin_ptr + block_size)
				{
					if (posp1.curr_ptr != this->controller.back_end_ptr)
					{
						posp1.curr_ptr = posp1.begin_ptr = *(++posp1.controller_ptr);
					}
				}
				constexpr size_type blockbytes{block_size * sizeof(value_type)};
				return ::std::bit_cast<iterator>(
					::fast_io::containers::details::deque_erase_common_trivial_impl(
						*reinterpret_cast<::fast_io::containers::details::deque_controller_common *>(__builtin_addressof(
							this->controller)),
						::std::bit_cast<::fast_io::containers::details::deque_control_block_common>(pos.itercontent),
						::std::bit_cast<::fast_io::containers::details::deque_control_block_common>(posp1), moveleft, blockbytes));
			}
		}
		auto posp1{pos};
		++posp1;
		return this->erase_no_destroy_common_impl(pos, posp1, moveleft);
	}
	inline constexpr iterator erase_unchecked_single_impl(iterator pos, bool moveleft) noexcept
	{
		if constexpr (!::std::is_trivially_destructible_v<value_type>)
		{
			::std::destroy(pos.itercontent.curr_ptr);
		}
		return this->erase_unchecked_single_nodestroy_impl(pos, moveleft);
	}

public:
	inline constexpr iterator erase(const_iterator first, const_iterator last) noexcept
	{
		return this->erase_unchecked_impl(iterator{first.itercontent},
										  iterator{last.itercontent},
										  static_cast<size_type>(first - this->cbegin()) <
											  static_cast<size_type>(this->cend() - last));
	}

	inline constexpr size_type erase_index(size_type firstidx, size_type lastidx) noexcept
	{
		size_type const n{this->size()};
		if (n < firstidx || n < lastidx) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		auto bg{this->begin()};
		this->erase_unchecked_impl(bg + firstidx, bg + lastidx, firstidx < (n - lastidx));
		return firstidx;
	}

	inline constexpr iterator erase(const_iterator first) noexcept
	{
		::std::size_t const n{this->size()};
		::std::size_t const distofront{
			::fast_io::containers::details::deque_iter_difference_unsigned_common(first.itercontent, this->controller.front_block)};
		return this->erase_unchecked_single_impl(iterator{first.itercontent},
												 distofront < static_cast<::std::size_t>(n - distofront));
	}

	inline constexpr size_type erase_index(size_type firstidx) noexcept
	{
		size_type const n{this->size()};
		if (n <= firstidx) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		this->erase_unchecked_single_impl(this->begin() + firstidx,
										  firstidx < static_cast<size_type>(n - firstidx));
		return firstidx;
	}

	inline constexpr ~deque()
	{
		destroy_deque_controller(this->controller);
	}
};

template <typename T, typename allocator1, typename allocator2>
	requires ::std::equality_comparable<T>
inline constexpr bool operator==(::fast_io::containers::deque<T, allocator1> const &lhs, ::fast_io::containers::deque<T, allocator2> const &rhs) noexcept
{
	return ::std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
}

#if defined(__cpp_lib_three_way_comparison)

template <typename T, typename allocator1, typename allocator2>
	requires ::std::three_way_comparable<T>
inline constexpr auto operator<=>(::fast_io::containers::deque<T, allocator1> const &lhs, ::fast_io::containers::deque<T, allocator2> const &rhs) noexcept
{
	return ::fast_io::freestanding::lexicographical_compare_three_way(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend(), ::std::compare_three_way{});
}

#endif

template <typename T, typename Alloc, typename U = T>
constexpr typename ::fast_io::containers::deque<T, Alloc>::size_type
erase(::fast_io::containers::deque<T, Alloc> &c, U const &value)
{
	auto ed{c.end()};
	auto it = ::std::remove(c.begin(), ed, value);
	auto r = ::fast_io::containers::details::deque_iter_difference_unsigned_common(ed.itercontent, it.itercontent);
	c.erase(it, ed);
	return r;
}

template <typename T, typename Alloc, typename Pred>
constexpr typename ::fast_io::containers::deque<T, Alloc>::size_type
erase_if(::fast_io::containers::deque<T, Alloc> &c, Pred pred)
{
	auto ed{c.end()};
	auto it = ::std::remove_if(c.begin(), ed, pred);
	auto r = ::fast_io::containers::details::deque_iter_difference_unsigned_common(ed.itercontent, it.itercontent);
	c.erase(it, ed);
	return r;
}

} // namespace containers

namespace freestanding
{

template <typename T, typename allocator>
struct is_trivially_copyable_or_relocatable<::fast_io::containers::deque<T, allocator>>
{
	inline static constexpr bool value = true;
};

template <typename T, typename allocator>
struct is_zero_default_constructible<::fast_io::containers::deque<T, allocator>>
{
	inline static constexpr bool value = true;
};

} // namespace freestanding

} // namespace fast_io
