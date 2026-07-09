#pragma once


/*
https://discourse.llvm.org/t/adding-a-size-based-vector-to-libc-s-unstable-abi/86306
*/
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
	vector_model
{
	::std::byte *begin_ptr;
	::std::size_t sz;
	::std::size_t cap;
};

namespace vector
{

namespace detemplate
{

template <typename allocator>
inline void *grow_to_byte_size_iter_impl(vector_model &imp, void *iter, ::std::size_t newcap, ::std::size_t gap, ::std::size_t size, ::std::size_t alignment) noexcept
{
	::std::byte *old_begin_ptr{imp.begin_ptr};
	::std::size_t const old_size{imp.sz};
	::std::size_t const old_capacity{imp.cap};
	::std::size_t const iter_offset{static_cast<::std::size_t>(static_cast<::std::byte *>(iter) - old_begin_ptr)};

	auto newres = allocator::allocate_aligned_at_least(alignment, newcap);
	auto begin_ptr = reinterpret_cast<::std::byte *>(newres.ptr);
	auto newiter = ::fast_io::freestanding::nonoverlapped_bytes_copy(reinterpret_cast<::std::byte const *>(old_begin_ptr), reinterpret_cast<::std::byte const *>(old_begin_ptr) + iter_offset,
																	 reinterpret_cast<::std::byte *>(begin_ptr));
	::fast_io::freestanding::nonoverlapped_bytes_copy(reinterpret_cast<::std::byte const *>(old_begin_ptr) + iter_offset, reinterpret_cast<::std::byte const *>(old_begin_ptr) + old_size,
													  reinterpret_cast<::std::byte *>(newiter + gap));
	allocator::deallocate_aligned_n(old_begin_ptr, alignment, old_capacity);
	imp.begin_ptr = begin_ptr;
	imp.sz = old_size;
	imp.cap = newres.count / size * size;
	return newiter;
}

template <typename allocator>
inline void *grow_to_size_iter_impl(vector_model &imp, void *iter, ::std::size_t newcap, ::std::size_t size, ::std::size_t alignment) noexcept
{
#if defined(_MSC_VER) && !defined(__clang__)
	::std::size_t mx{SIZE_MAX / size};
	if (newcap > mx) [[unlikely]]
	{
		::fast_io::fast_terminate();
	}
	newcap *= size;
#else
	if (__builtin_mul_overflow(size, newcap, __builtin_addressof(newcap))) [[unlikely]]
	{
		__builtin_trap();
	}
#endif
	return grow_to_byte_size_iter_impl<allocator>(imp, iter, newcap, size, size, alignment);
}

template <typename allocator>
inline void *grow_to_size_impl(vector_model &imp, ::std::size_t newcap, ::std::size_t size, ::std::size_t alignment) noexcept
{
	void *iter{imp.begin_ptr + imp.sz};
	return grow_to_size_iter_impl<allocator>(imp, iter, newcap, size, alignment);
}

template <typename allocator>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr void *grow_twice_iter_impl(vector_model &imp, void *iter, ::std::size_t size, ::std::size_t alignment) noexcept
{
	::std::size_t toallocate{size};
	::std::size_t const diff{imp.cap};
	if (diff) [[likely]]
	{
#if defined(_MSC_VER) && !defined(__clang__)
		constexpr ::std::size_t mx{SIZE_MAX / 2};
		if (diff > mx) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		toallocate = (diff << 1u);
#else
		if (__builtin_mul_overflow(diff, 2u, __builtin_addressof(toallocate))) [[unlikely]]
		{
			__builtin_trap();
		}
#endif
	}
	return grow_to_byte_size_iter_impl<allocator>(imp, iter, toallocate, size, size, alignment);
}

template <typename allocator>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr void *grow_twice_impl(vector_model &imp, ::std::size_t size, ::std::size_t alignment) noexcept
{
	void *iter{imp.begin_ptr + imp.sz};
	return grow_twice_iter_impl<allocator>(imp, iter, size, alignment);
}

template <typename allocator>
inline constexpr void *move_backward_impl(vector_model &imp, void *iter, ::std::size_t size, ::std::size_t alignment) noexcept
{
	if (imp.sz == imp.cap) [[unlikely]]
	{
		return grow_twice_iter_impl<allocator>(imp, iter, size, alignment);
	}
	::std::byte *curr_ptr{imp.begin_ptr + imp.sz};
	::std::byte *curr_ptr_p1{curr_ptr + size};
	::fast_io::freestanding::uninitialized_move_backward(reinterpret_cast<::std::byte *>(iter), curr_ptr, curr_ptr_p1);
	return iter;
}

} // namespace detemplate

template <typename allocator, ::std::size_t size, ::std::size_t alignment>
inline constexpr void *grow_to_size_iter_impl(vector_model &imp, void *iter, ::std::size_t newcap) noexcept
{
	if constexpr (alignment < allocator::default_alignment)
	{
		return ::fast_io::containers::details::vector::grow_to_size_iter_impl<allocator, size, allocator::default_alignment>(imp, iter, newcap);
	}
	else
	{
		return ::fast_io::containers::details::vector::detemplate::grow_to_size_iter_impl<allocator>(imp, iter, newcap, size, alignment);
	}
}

template <typename allocator, ::std::size_t size, ::std::size_t alignment>
inline constexpr void *grow_to_size_impl(vector_model &imp, ::std::size_t newcap) noexcept
{
	if constexpr (alignment < allocator::default_alignment)
	{
		return ::fast_io::containers::details::vector::grow_to_size_impl<allocator, size, allocator::default_alignment>(imp, newcap);
	}
	else
	{
		return ::fast_io::containers::details::vector::detemplate::grow_to_size_impl<allocator>(imp, newcap, size, alignment);
	}
}

template <typename allocator, ::std::size_t size, ::std::size_t alignment>
inline constexpr void *grow_twice_impl(vector_model &imp) noexcept
{
	if constexpr (alignment < allocator::default_alignment)
	{
		return ::fast_io::containers::details::vector::grow_twice_impl<allocator, size, allocator::default_alignment>(imp);
	}
	else
	{
		return ::fast_io::containers::details::vector::detemplate::grow_twice_impl<allocator>(imp, size, alignment);
	}
}

template <typename allocator, ::std::size_t size, ::std::size_t alignment>
inline constexpr void *grow_twice_iter_impl(vector_model &imp, void *iter) noexcept
{
	if constexpr (alignment < allocator::default_alignment)
	{
		return ::fast_io::containers::details::vector::grow_twice_iter_impl<allocator, size, allocator::default_alignment>(imp, iter);
	}
	else
	{
		return ::fast_io::containers::details::vector::detemplate::grow_twice_iter_impl<allocator>(imp, iter, size, alignment);
	}
}

template <typename allocator, ::std::size_t size, ::std::size_t alignment>
inline constexpr void *move_backward_impl(vector_model &imp, void *iter) noexcept
{
	if constexpr (alignment < allocator::default_alignment)
	{
		return ::fast_io::containers::details::vector::move_backward_impl<allocator, size, allocator::default_alignment>(imp, iter);
	}
	else
	{
		return ::fast_io::containers::details::vector::detemplate::move_backward_impl<allocator>(imp, iter, size, alignment);
	}
}

} // namespace vector

template <typename T>
struct vector_internal
{
	T *begin_ptr{};
	::std::size_t sz{};
	::std::size_t cap{};
};

} // namespace details

template <typename T, typename allocator>
class vector FAST_IO_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE
{
public:
	using allocator_type = allocator;
	using value_type = T;

private:
	using typed_allocator_type = typed_generic_allocator_adapter<allocator_type, value_type>;

public:
	using pointer = value_type *;
	using const_pointer = value_type const *;

	using reference = value_type &;
	using const_reference = value_type const &;

	using iterator = value_type *;
	using const_iterator = value_type const *;

	using reverse_iterator = ::std::reverse_iterator<iterator>;
	using const_reverse_iterator = ::std::reverse_iterator<const_iterator>;

	using size_type = ::std::size_t;
	using difference_type = ::std::ptrdiff_t;
	::fast_io::containers::details::vector_internal<T> imp;

	inline constexpr vector() noexcept = default;

private:
	inline constexpr void destroy() noexcept
	{
		clear();
		if constexpr (!typed_allocator_type::has_deallocate)
		{
			typed_allocator_type::deallocate(imp.begin_ptr);
		}
		else
		{
			typed_allocator_type::deallocate_n(imp.begin_ptr, imp.cap);
		}
	}
	struct run_destroy
	{
		vector *thisvec{};
		inline constexpr run_destroy() noexcept = default;
		inline explicit constexpr run_destroy(vector *p) noexcept
			: thisvec(p)
		{}
		inline run_destroy(run_destroy const &) = delete;
		inline run_destroy &operator=(run_destroy const &) = delete;
		inline constexpr ~run_destroy()
		{
			if (thisvec)
			{
				thisvec->destroy();
			}
		}
	};

	inline constexpr void default_construct_impl(size_type n)
	{
		this->imp.begin_ptr = typed_allocator_type::allocate(n);
		this->imp.sz = 0;
		this->imp.cap = n;
		run_destroy des(this);
		auto e{this->imp.begin_ptr + n};
		for (auto p{this->imp.begin_ptr}; p != e; ++p)
		{
			::std::construct_at(p);
		}
		this->imp.sz = n;
		des.thisvec = nullptr;
	}

	template <typename Iter, typename Sentinel>
	inline constexpr void construct_vector_common_impl(Iter first, Sentinel last)
	{
		using rvaluetype = ::std::iter_value_t<Iter>;
		if constexpr (::std::same_as<Iter, Sentinel> && ::std::contiguous_iterator<Iter> && !::std::is_pointer_v<Iter>)
		{
			this->construct_vector_common_impl(::std::to_address(first), ::std::to_address(last));
		}
		else
		{
			if constexpr (::std::forward_iterator<Iter>)
			{
				size_type n{static_cast<size_type>(::std::ranges::distance(first, last))};
				this->imp.begin_ptr = typed_allocator_type::allocate(n);
				this->imp.sz = 0;
				this->imp.cap = n;
				if constexpr (
					::std::is_pointer_v<Iter> &&
					::std::is_trivially_constructible_v<value_type, rvaluetype> &&
					::std::same_as<::std::remove_cvref_t<rvaluetype>, ::std::remove_cvref_t<value_type>>)
				{
					if (n) [[likely]]
					{
#if defined(_MSC_VER) && !defined(__clang__)
						::std::memcpy
#else
						__builtin_memcpy
#endif
							(this->imp.begin_ptr, first, n * sizeof(value_type));
					}
					this->imp.sz = n;
				}
				else if constexpr (::std::is_nothrow_constructible_v<value_type, rvaluetype>)
				{
					auto e{this->imp.begin_ptr + n};
					for (auto curr{this->imp.begin_ptr}; curr != e; ++curr)
					{
						::std::construct_at(curr, *first);
						++first;
					}
					this->imp.sz = n;
				}
				else
				{
					run_destroy des(this);
					auto e{this->imp.begin_ptr + n};
					for (auto p{this->imp.begin_ptr}; p != e; ++p)
					{
						::std::construct_at(p, *first);
						++first;
					}
					this->imp.sz = n;
					des.thisvec = nullptr;
				}
			}
			else
			{
				run_destroy des(this);
				for (; first != last; ++first)
				{
					this->emplace_back(*first);
				}
				des.thisvec = nullptr;
			}
		}
	}

public:
	inline explicit constexpr vector(size_type n) noexcept(::fast_io::freestanding::is_zero_default_constructible_v<value_type> || noexcept(value_type()))
	{
		if constexpr (::fast_io::freestanding::is_zero_default_constructible_v<value_type>)
		{
			imp.begin_ptr = typed_allocator_type::allocate_zero(n);
			imp.sz = n;
			imp.cap = n;
		}
		else
		{
			this->default_construct_impl(n);
		}
	}

	inline explicit constexpr vector(size_type n, ::fast_io::for_overwrite_t) noexcept(::std::is_trivially_default_constructible_v<value_type> || ::fast_io::freestanding::is_zero_default_constructible_v<value_type> || noexcept(value_type()))
	{
		if constexpr (::std::is_trivially_default_constructible_v<value_type>)
		{
			imp.begin_ptr = typed_allocator_type::allocate(n);
			imp.sz = n;
			imp.cap = n;
		}
		else if constexpr (::fast_io::freestanding::is_zero_default_constructible_v<value_type>)
		{
			imp.begin_ptr = typed_allocator_type::allocate_zero(n);
			imp.sz = n;
			imp.cap = n;
		}
		else
		{
			this->default_construct_impl(n);
		}
	}

	inline explicit constexpr vector(size_type n, const_reference val) noexcept(::std::is_nothrow_copy_constructible_v<value_type>)
	{
		this->imp.begin_ptr = typed_allocator_type::allocate(n);
		this->imp.sz = 0;
		this->imp.cap = n;
		run_destroy des(this);
		auto e{this->imp.begin_ptr + n};
		for (auto p{this->imp.begin_ptr}; p != e; ++p)
		{
			::std::construct_at(p, val);
		}
		this->imp.sz = n;
		des.thisvec = nullptr;
	}

	template <::std::ranges::range R>
	inline explicit constexpr vector(::fast_io::freestanding::from_range_t, R &&rg)
	{
		this->construct_vector_common_impl(::std::ranges::begin(rg), ::std::ranges::end(rg));
	}

	inline explicit constexpr vector(::std::initializer_list<value_type> ilist) noexcept(::std::is_nothrow_copy_constructible_v<value_type>)
	{
		this->construct_vector_common_impl(ilist.begin(), ilist.end());
	}

	inline constexpr vector(vector const &vec)
	{
		static_assert(::std::is_copy_constructible_v<value_type>, "vector's value type must be copy constructible to use copy constructor");
		size_type const vecsize{vec.imp.sz};
		if (vecsize == 0)
		{
			return;
		}
		imp.begin_ptr = typed_allocator_type::allocate(vecsize);
		imp.sz = 0;
		imp.cap = vecsize;
		if constexpr (::std::is_trivially_copyable_v<value_type>)
		{
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_constexpr_dynamic_alloc >= 201907L
#if __cpp_if_consteval >= 202106L
			if !consteval
#else
			if (!__builtin_is_constant_evaluated())
#endif
#endif
			{
				::fast_io::freestanding::nonoverlapped_bytes_copy(
					reinterpret_cast<::std::byte const *>(vec.imp.begin_ptr),
					reinterpret_cast<::std::byte const *>(vec.imp.begin_ptr + vecsize),
					reinterpret_cast<::std::byte *>(this->imp.begin_ptr));
				this->imp.sz = vecsize;
				return;
			}
		}
		run_destroy des(this);
		for (size_type i{}; i != vecsize; ++i)
		{
			::std::construct_at(this->imp.begin_ptr + i, vec.imp.begin_ptr[i]);
		}
		this->imp.sz = vecsize;
		des.thisvec = nullptr;
	}

	inline constexpr vector &operator=(vector const &vec)
	{
		static_assert(::std::copyable<value_type>, "vector's value type must be copyable to use copy assignment operator");
		if (__builtin_addressof(vec) == this) [[unlikely]]
		{
			return *this;
		}
		vector newvec(vec);
		this->operator=(::std::move(newvec));
		return *this;
	}

	inline constexpr vector(vector &&vec) noexcept
		: imp(vec.imp)
	{
		vec.imp = {};
	}
	inline constexpr vector &operator=(vector &&vec) noexcept
	{
		if (__builtin_addressof(vec) == this) [[unlikely]]
		{
			return *this;
		}
		this->destroy();
		this->imp = vec.imp;
		vec.imp = {};
		return *this;
	}
	inline constexpr ~vector()
	{
		destroy();
	}

	template <typename... Args>
		requires std::constructible_from<value_type, Args...>
	inline constexpr reference emplace_back_unchecked(Args &&...args) noexcept(::std::is_nothrow_constructible_v<value_type, Args...>)
	{
		auto p{::std::construct_at(imp.begin_ptr + imp.sz, ::std::forward<Args>(args)...)};
		++imp.sz;
		return *p;
	}

private:
	inline constexpr pointer grow_to_size_iter_impl(size_type newcap, pointer iter, size_type /*n*/) noexcept
	{
		if constexpr (::fast_io::freestanding::is_trivially_copyable_or_relocatable_v<value_type>)
		{
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_constexpr_dynamic_alloc >= 201907L
#if __cpp_if_consteval >= 202106L
			if !consteval
#else
			if (!__builtin_is_constant_evaluated())
#endif
#endif
			{
				auto vm = to_vector_model(imp);
				auto result = ::fast_io::containers::details::vector::grow_to_size_iter_impl<allocator_type, sizeof(value_type), alignof(value_type)>(
					*reinterpret_cast<::fast_io::containers::details::vector_model *>(__builtin_addressof(vm)),
					iter, newcap);
				sync_from_vector_model(imp, vm);
				return reinterpret_cast<pointer>(result);
			}
		}
		auto newres = typed_allocator_type::allocate_at_least(newcap);
		auto new_begin_ptr = newres.ptr;
		auto old_begin_ptr{imp.begin_ptr};
		size_type const old_size{imp.sz};
		size_type const old_cap{imp.cap};
		size_type const iter_offset{static_cast<size_type>(iter - old_begin_ptr)};
		auto newiter{::fast_io::freestanding::uninitialized_relocate(old_begin_ptr, iter, new_begin_ptr)};
		::fast_io::freestanding::uninitialized_relocate(iter, old_begin_ptr + old_size, newiter + 1);
		if constexpr (typed_allocator_type::has_deallocate)
		{
			typed_allocator_type::deallocate(old_begin_ptr);
		}
		else
		{
			typed_allocator_type::deallocate_n(old_begin_ptr, old_cap);
		}
		imp.begin_ptr = new_begin_ptr;
		imp.sz = old_size;
		imp.cap = newres.count;
		return newiter;
	}

#if __has_cpp_attribute(__gnu__::__cold__)
	[[__gnu__::__cold__]]
#endif
	inline constexpr pointer grow_twice_iter_impl(pointer iter) noexcept
	{
		if constexpr (::fast_io::freestanding::is_trivially_copyable_or_relocatable_v<value_type>)
		{
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_constexpr_dynamic_alloc >= 201811L
#if __cpp_if_consteval >= 202106L
			if !consteval
#else
			if (!__builtin_is_constant_evaluated())
#endif
#endif
			{
				auto vm = to_vector_model(imp);
				auto result = ::fast_io::containers::details::vector::grow_twice_iter_impl<allocator_type, sizeof(value_type), alignof(value_type)>(
					*reinterpret_cast<::fast_io::containers::details::vector_model *>(__builtin_addressof(vm)),
					iter);
				sync_from_vector_model(imp, vm);
				return reinterpret_cast<pointer>(result);
			}
		}
		size_type const cap{imp.cap};
		return this->grow_to_size_iter_impl(::fast_io::containers::details::cal_grow_twice_size<sizeof(value_type), false>(cap), iter, 1);
	}
	inline constexpr pointer move_backward_common_impl(pointer iter) noexcept
	{
		if constexpr (::fast_io::freestanding::is_trivially_copyable_or_relocatable_v<value_type>)
		{
#ifdef __cpp_if_consteval
			if !consteval
#else
			if (!__builtin_is_constant_evaluated())
#endif
			{
				auto vm = to_vector_model(imp);
				auto result = ::fast_io::containers::details::vector::move_backward_impl<allocator, sizeof(value_type), alignof(value_type)>(
					*reinterpret_cast<::fast_io::containers::details::vector_model *>(__builtin_addressof(vm)),
					iter);
				sync_from_vector_model(imp, vm);
				return reinterpret_cast<pointer>(result);
			}
		}
		if (imp.sz == imp.cap) [[unlikely]]
		{
			return this->grow_twice_iter_impl(iter);
		}
		auto currptr{imp.begin_ptr + imp.sz};
		auto currptrp1{currptr + 1};
		::fast_io::freestanding::uninitialized_move_backward(iter, currptr, currptrp1);
		return iter;
	}

	inline constexpr void grow_to_size_impl(size_type newcap) noexcept
	{
		if constexpr (::fast_io::freestanding::is_trivially_copyable_or_relocatable_v<value_type>)
		{
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_constexpr_dynamic_alloc >= 201907L
#if __cpp_if_consteval >= 202106L
			if !consteval
#else
			if (!__builtin_is_constant_evaluated())
#endif
#endif
			{
				auto vm = to_vector_model(imp);
				::fast_io::containers::details::vector::grow_to_size_impl<allocator_type, sizeof(value_type), alignof(value_type)>(
					*reinterpret_cast<::fast_io::containers::details::vector_model *>(__builtin_addressof(vm)), newcap);
				sync_from_vector_model(imp, vm);
				return;
			}
		}
		this->grow_to_size_iter_impl(newcap, imp.begin_ptr + imp.sz, 1);
	}
#if __has_cpp_attribute(__gnu__::__cold__)
	[[__gnu__::__cold__]]
#endif
	inline constexpr void grow_twice_impl() noexcept
	{
		if constexpr (::fast_io::freestanding::is_trivially_copyable_or_relocatable_v<value_type>)
		{
#if (__cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L) && __cpp_constexpr_dynamic_alloc >= 201907L
#if __cpp_if_consteval >= 202106L
			if !consteval
#else
			if (!__builtin_is_constant_evaluated())
#endif
#endif
			{
				auto vm = to_vector_model(imp);
				::fast_io::containers::details::vector::grow_twice_impl<allocator_type, sizeof(value_type), alignof(value_type)>(
					*reinterpret_cast<::fast_io::containers::details::vector_model *>(__builtin_addressof(vm)));
				sync_from_vector_model(imp, vm);
				return;
			}
		}
		size_type const cap{imp.cap};
		grow_to_size_impl(::fast_io::containers::details::cal_grow_twice_size<sizeof(value_type), false>(cap));
	}

public:
	inline constexpr void reserve(size_type n) noexcept
	{
		if (n <= imp.cap)
		{
			return;
		}
		grow_to_size_impl(n);
	}

	inline constexpr void shrink_to_fit() noexcept
	{
		if (imp.sz == imp.cap)
		{
			return;
		}
		grow_to_size_impl(imp.sz);
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	inline constexpr void pop_back() noexcept
	{
		if (imp.sz == 0) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		--imp.sz;
		(imp.begin_ptr + imp.sz)->~value_type();
	}

	inline constexpr void pop_back_unchecked() noexcept
	{
		--imp.sz;
		(imp.begin_ptr + imp.sz)->~value_type();
	}

	inline constexpr void push_back(T const &value) noexcept(::std::is_nothrow_copy_constructible_v<value_type>)
	{
		this->emplace_back(value);
	}
	inline constexpr void push_back(T &&value) noexcept(::std::is_nothrow_move_constructible_v<value_type>)
	{
		this->emplace_back(::std::move(value));
	}
	inline constexpr void push_back_unchecked(T const &value) noexcept(::std::is_nothrow_copy_constructible_v<value_type>)
	{
		this->emplace_back_unchecked(value);
	}
	inline constexpr void push_back_unchecked(T &&value) noexcept(::std::is_nothrow_move_constructible_v<value_type>)
	{
		this->emplace_back_unchecked(::std::move(value));
	}

	[[nodiscard]] inline constexpr pointer data() noexcept
	{
		return imp.begin_ptr;
	}
	[[nodiscard]] inline constexpr const_pointer data() const noexcept
	{
		return imp.begin_ptr;
	}
	[[nodiscard]] inline constexpr bool is_empty() const noexcept
	{
		return imp.sz == 0;
	}

	[[nodiscard]] inline constexpr bool empty() const noexcept
	{
		return imp.sz == 0;
	}
	inline constexpr void clear() noexcept
	{
		if constexpr (!::std::is_trivially_destructible_v<value_type>)
		{
			::std::destroy(imp.begin_ptr, imp.begin_ptr + imp.sz);
		}
		imp.sz = 0;
	}
	[[nodiscard]] inline constexpr size_type size() const noexcept
	{
		return imp.sz;
	}
	[[nodiscard]] inline constexpr size_type size_bytes() const noexcept
	{
		return imp.sz * sizeof(value_type);
	}
	[[nodiscard]] inline constexpr size_type capacity() const noexcept
	{
		return imp.cap;
	}
	[[nodiscard]] inline constexpr size_type capacity_bytes() const noexcept
	{
		return imp.cap * sizeof(value_type);
	}
	[[nodiscard]] static inline constexpr size_type max_size() noexcept
	{
		constexpr size_type mx{::std::numeric_limits<size_type>::max() / sizeof(value_type)};
		return mx;
	}
	[[nodiscard]] static inline constexpr size_type max_size_bytes() noexcept
	{
		constexpr size_type mx{::std::numeric_limits<size_type>::max() / sizeof(value_type) * sizeof(value_type)};
		return mx;
	}
	[[nodiscard]] inline constexpr const_reference index_unchecked(size_type pos) const noexcept
	{
		return imp.begin_ptr[pos];
	}
	[[nodiscard]] inline constexpr reference index_unchecked(size_type pos) noexcept
	{
		return imp.begin_ptr[pos];
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	[[nodiscard]] inline constexpr const_reference
	operator[](size_type pos) const noexcept
	{
		if (imp.sz <= pos) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		return imp.begin_ptr[pos];
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	[[nodiscard]] inline constexpr reference
	operator[](size_type pos) noexcept
	{
		if (imp.sz <= pos) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		return imp.begin_ptr[pos];
	}

#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	[[nodiscard]] inline constexpr const_reference
	front() const noexcept
	{
		if (imp.sz == 0) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		return *imp.begin_ptr;
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	[[nodiscard]] inline constexpr reference
	front() noexcept
	{
		if (imp.sz == 0) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		return *imp.begin_ptr;
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	[[nodiscard]] inline constexpr const_reference
	back() const noexcept
	{
		if (imp.sz == 0) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		return imp.begin_ptr[imp.sz - 1];
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#elif __has_cpp_attribute(msvc::forceinline)
	[[msvc::forceinline]]
#endif
	[[nodiscard]] inline constexpr reference
	back() noexcept
	{
		if (imp.sz == 0) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		return imp.begin_ptr[imp.sz - 1];
	}

	[[nodiscard]] inline constexpr const_reference front_unchecked() const noexcept
	{
		return *imp.begin_ptr;
	}
	[[nodiscard]] inline constexpr reference front_unchecked() noexcept
	{
		return *imp.begin_ptr;
	}
	[[nodiscard]] inline constexpr const_reference back_unchecked() const noexcept
	{
		return imp.begin_ptr[imp.sz - 1];
	}
	[[nodiscard]] inline constexpr reference back_unchecked() noexcept
	{
		return imp.begin_ptr[imp.sz - 1];
	}

	[[nodiscard]] inline constexpr iterator begin() noexcept
	{
		return imp.begin_ptr;
	}
	[[nodiscard]] inline constexpr iterator end() noexcept
	{
		return imp.begin_ptr + imp.sz;
	}
	[[nodiscard]] inline constexpr const_iterator begin() const noexcept
	{
		return imp.begin_ptr;
	}
	[[nodiscard]] inline constexpr const_iterator end() const noexcept
	{
		return imp.begin_ptr + imp.sz;
	}
	[[nodiscard]] inline constexpr const_iterator cbegin() const noexcept
	{
		return imp.begin_ptr;
	}
	[[nodiscard]] inline constexpr const_iterator cend() const noexcept
	{
		return imp.begin_ptr + imp.sz;
	}

	[[nodiscard]] inline constexpr reverse_iterator rbegin() noexcept
	{
		return reverse_iterator{imp.begin_ptr + imp.sz};
	}
	[[nodiscard]] inline constexpr reverse_iterator rend() noexcept
	{
		return reverse_iterator{imp.begin_ptr};
	}
	[[nodiscard]] inline constexpr const_reverse_iterator rbegin() const noexcept
	{
		return const_reverse_iterator{imp.begin_ptr + imp.sz};
	}
	[[nodiscard]] inline constexpr const_reverse_iterator rend() const noexcept
	{
		return const_reverse_iterator{imp.begin_ptr};
	}
	[[nodiscard]] inline constexpr const_reverse_iterator crbegin() const noexcept
	{
		return const_reverse_iterator{imp.begin_ptr + imp.sz};
	}
	[[nodiscard]] inline constexpr const_reverse_iterator crend() const noexcept
	{
		return const_reverse_iterator{imp.begin_ptr};
	}

	inline constexpr void clear_destroy() noexcept
	{
		this->destroy();
		imp = {};
	}

	template <typename... Args>
		requires std::constructible_from<value_type, Args...>
	inline constexpr reference emplace_back(Args &&...args) noexcept(::std::is_nothrow_constructible_v<value_type, Args...>)
	{
		if (imp.sz == imp.cap)
			[[unlikely]]
		{
			grow_twice_impl();
		}
		auto p{::std::construct_at(imp.begin_ptr + imp.sz, ::std::forward<Args>(args)...)};
		++imp.sz;
		return *p;
	}

private:
	struct append_range_guard
	{
		vector *thisvec{};
		size_type oldn{};
		constexpr ~append_range_guard()
		{
			if (thisvec)
			{
				thisvec->erase(thisvec->cbegin() + oldn, thisvec->cend());
			}
		}
	};

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
			size_type const old_size{imp.sz};
			size_type const new_size{old_size + rgsize};
			if (new_size > imp.cap)
			{
				this->grow_to_size_impl(new_size);
			}
			if constexpr (::std::is_nothrow_constructible_v<value_type, ::std::ranges::range_value_t<R>>)
			{
				for (auto &e : rg)
				{
					::std::construct_at(imp.begin_ptr + imp.sz, ::std::forward<decltype(e)>(e));
					++imp.sz;
				}
			}
			else
			{
				append_range_guard guard{this, old_size};
				for (auto &e : rg)
				{
					::std::construct_at(imp.begin_ptr + imp.sz, ::std::forward<decltype(e)>(e));
					++imp.sz;
				}
				guard.thisvec = nullptr;
			}
		}
		else
		{
			if constexpr (::std::is_nothrow_constructible_v<value_type, ::std::ranges::range_value_t<R>>)
			{
				for (auto &e : rg)
				{
					this->emplace_back(::std::forward<decltype(e)>(e));
				}
			}
			else
			{
				append_range_guard guard{this, this->size()};
				for (auto &e : rg)
				{
					this->emplace_back(::std::forward<decltype(e)>(e));
				}
				guard.thisvec = nullptr;
			}
		}
	}

private:
	inline constexpr iterator insert_impl(pointer iter, value_type &&tmp) noexcept
	{
		auto ret = ::std::construct_at(this->move_backward_common_impl(iter), ::std::move(tmp));
		++imp.sz;
		return ret;
	}

public:
	template <typename... Args>
		requires std::constructible_from<value_type, Args...>
	inline constexpr iterator emplace(const_iterator iter, Args &&...args) noexcept(::std::is_nothrow_constructible_v<value_type, Args...>)
	{
		if constexpr (::std::is_nothrow_constructible_v<value_type, Args...>)
		{
			pointer ret;
			if (__builtin_is_constant_evaluated())
			{
				auto beginptr{imp.begin_ptr};
				ret = ::std::construct_at(this->move_backward_common_impl(iter - beginptr + beginptr), ::std::forward<Args>(args)...);
			}
			else
			{
				ret = ::std::construct_at(this->move_backward_common_impl(const_cast<pointer>(iter)), ::std::forward<Args>(args)...);
			}
			++imp.sz;
			return ret;
		}
		else
		{
			if (__builtin_is_constant_evaluated())
			{
				auto beginptr{imp.begin_ptr};
				return this->insert_impl(iter - beginptr + beginptr, value_type(::std::forward<Args>(args)...));
			}
			else
			{
				return this->insert_impl(const_cast<pointer>(iter), value_type(::std::forward<Args>(args)...));
			}
		}
	}

	template <typename... Args>
		requires std::constructible_from<value_type, Args...>
	inline constexpr reference emplace_index(size_type idx, Args &&...args) noexcept(::std::is_nothrow_constructible_v<value_type, Args...>)
	{
		auto beginptr{imp.begin_ptr};
		size_type sz{imp.sz};
		if (sz < idx)
		{
			::fast_io::fast_terminate();
		}
		return *this->emplace(beginptr + idx, ::std::forward<Args>(args)...);
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

private:
	inline constexpr pointer erase_common(pointer it) noexcept
	{
		auto lastele{imp.begin_ptr + imp.sz};
		if constexpr (!::std::is_trivially_destructible_v<value_type>)
		{
			::std::destroy_at(it);
		}
		::fast_io::freestanding::uninitialized_relocate(it + 1, lastele, it);
		--imp.sz;
		return it;
	}

	inline constexpr pointer erase_iters_common(pointer first, pointer last) noexcept
	{
		if constexpr (!::std::is_trivially_destructible_v<value_type>)
		{
			::std::destroy(first, last);
		}
		auto new_end = ::fast_io::freestanding::uninitialized_relocate(last, imp.begin_ptr + imp.sz, first);
		imp.sz = static_cast<size_type>(new_end - imp.begin_ptr);
		return first;
	}

public:
	inline constexpr iterator erase(const_iterator it) noexcept
	{
		if (__builtin_is_constant_evaluated())
		{
			return this->erase_common(it - imp.begin_ptr + imp.begin_ptr);
		}
		else
		{
			return this->erase_common(const_cast<pointer>(it));
		}
	}

	inline constexpr size_type erase_index(size_type idx) noexcept
	{
		auto beginptr{imp.begin_ptr};
		size_type sz{imp.sz};
		if (sz <= idx)
		{
			::fast_io::fast_terminate();
		}
		this->erase_common(beginptr + idx);
		return idx;
	}

	inline constexpr iterator erase(const_iterator first, const_iterator last) noexcept
	{
		if (__builtin_is_constant_evaluated())
		{
			return this->erase_iters_common(first - imp.begin_ptr + imp.begin_ptr, last - imp.begin_ptr + imp.begin_ptr);
		}
		else
		{
			return this->erase_iters_common(const_cast<pointer>(first), const_cast<pointer>(last));
		}
	}

	inline constexpr size_type erase_index(size_type firstidx, size_type lastidx) noexcept
	{
		auto beginptr{imp.begin_ptr};
		size_type sz{imp.sz};
		if (lastidx < firstidx || sz < lastidx)
		{
			::fast_io::fast_terminate();
		}
		this->erase_iters_common(beginptr + firstidx, beginptr + lastidx);
		return firstidx;
	}

	inline constexpr void resize(size_type n) noexcept(::std::is_nothrow_default_constructible_v<value_type>)
	{
		size_type sz{imp.sz};
		if (sz < n)
		{
			this->reserve(n);
			::fast_io::freestanding::uninitialized_default_construct(imp.begin_ptr + sz, imp.begin_ptr + n);
			imp.sz = n;
		}
		else if (n < sz)
		{
			if constexpr (!::std::is_trivially_destructible_v<value_type>)
			{
				::std::destroy(imp.begin_ptr + n, imp.begin_ptr + sz);
			}
			imp.sz = n;
		}
	}

	inline constexpr void resize(size_type n, const_reference val) noexcept(::std::is_nothrow_copy_constructible_v<value_type>)
	{
		size_type sz{imp.sz};
		if (sz < n)
		{
			this->reserve(n);
			::fast_io::freestanding::uninitialized_fill(imp.begin_ptr + sz, imp.begin_ptr + n, val);
			imp.sz = n;
		}
		else if (n < sz)
		{
			if constexpr (!::std::is_trivially_destructible_v<value_type>)
			{
				::std::destroy(imp.begin_ptr + n, imp.begin_ptr + sz);
			}
			imp.sz = n;
		}
	}

	inline constexpr void assign(size_type n, const_reference val) noexcept(::std::is_nothrow_copy_constructible_v<value_type>)
	{
		this->clear();
		this->reserve(n);
		::fast_io::freestanding::uninitialized_fill(imp.begin_ptr, imp.begin_ptr + n, val);
		imp.sz = n;
	}

	inline constexpr void swap(vector &other) noexcept
	{
		::std::swap(imp, other.imp);
	}
};

template <typename T, typename allocator1, typename allocator2>
	requires ::std::equality_comparable<T>
inline constexpr bool operator==(vector<T, allocator1> const &lhs, vector<T, allocator2> const &rhs) noexcept
{
	return ::std::equal(lhs.imp.begin_ptr, lhs.imp.begin_ptr + lhs.imp.sz, rhs.imp.begin_ptr, rhs.imp.begin_ptr + rhs.imp.sz);
}

#if __cpp_impl_three_way_comparison >= 201907L
template <typename T, typename allocator1, typename allocator2>
	requires ::std::three_way_comparable<T>
inline constexpr auto operator<=>(vector<T, allocator1> const &lhs, vector<T, allocator2> const &rhs) noexcept
{
	return ::fast_io::freestanding::lexicographical_compare_three_way(
		lhs.imp.begin_ptr, lhs.imp.begin_ptr + lhs.imp.sz,
		rhs.imp.begin_ptr, rhs.imp.begin_ptr + rhs.imp.sz,
		::std::compare_three_way{});
}
#endif

template <typename T, typename allocator>
inline constexpr void swap(vector<T, allocator> &lhs, vector<T, allocator> &rhs) noexcept
{
	lhs.swap(rhs);
}

template <typename ValueType, typename Alloc, typename U>
inline constexpr ::fast_io::containers::vector<ValueType, Alloc>::size_type erase(::fast_io::containers::vector<ValueType, Alloc> &c, U const &value)
{
	auto it = ::std::remove(c.begin(), c.end(), value);
	auto r = c.end() - it;
	c.erase(it, c.end());
	return static_cast<::fast_io::containers::vector<ValueType, Alloc>::size_type>(r);
}

template <typename ValueType, typename Alloc, typename Pred>
inline constexpr ::fast_io::containers::vector<ValueType, Alloc>::size_type erase_if(::fast_io::containers::vector<ValueType, Alloc> &c, Pred pred)
{
	auto it = ::std::remove_if(c.begin(), c.end(), pred);
	auto r = c.end() - it;
	c.erase(it, c.end());
	return static_cast<::fast_io::containers::vector<ValueType, Alloc>::size_type>(r);
}

} // namespace containers

namespace freestanding
{

template <typename T, typename Alloc>
struct is_trivially_copyable_or_relocatable<::fast_io::containers::vector<T, Alloc>>
{
	inline static constexpr bool value = true;
};

template <typename T, typename Alloc>
struct is_zero_default_constructible<::fast_io::containers::vector<T, Alloc>>
{
	inline static constexpr bool value = true;
};

} // namespace freestanding
} // namespace fast_io
