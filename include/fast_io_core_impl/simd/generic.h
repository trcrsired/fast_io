#pragma once

namespace fast_io
{

namespace details
{

namespace cpu_flags
{

inline constexpr bool avx2_supported
{
#if defined(_M_X64)
#if defined(__AVX2__)
true
#endif
#endif
};

inline constexpr bool avx512bw_supported
{
#if defined(_M_X64)
#if defined(__AVX512BW__)
true
#endif
#endif
};

inline constexpr bool avx512f_supported
{
#if defined(_M_X64)
#if defined(__AVX512F__)
true
#endif
#endif
};

}

template<typename T,typename Func>
inline constexpr void generic_simd_operation_impl(T& selfval,T const& other,Func func) noexcept
{
	constexpr std::size_t N{std::remove_cvref_t<T>::size()};
	for(std::size_t i{};i!=N;++i)
	{
		func(selfval.value[i],other.value[i]);
	}
}

template<typename T,typename Func>
inline constexpr void generic_simd_self_op_impl(T& selfval,Func func) noexcept
{
	constexpr std::size_t N{std::remove_cvref_t<T>::size()};
	for(std::size_t i{};i!=N;++i)
	{
		func(selfval.value[i]);
	}
}

template<typename T,typename Func>
inline constexpr void generic_simd_self_create_op_impl(T const& selfval,Func func) noexcept
{
	std::remove_cvref_t<T> newval;
	constexpr std::size_t N{std::remove_cvref_t<T>::size()};
	for(std::size_t i{};i!=N;++i)
	{
		newval.value[i]=func(selfval.value[i]);
	}
	return newval;
}

template<typename T,typename Func>
inline constexpr std::remove_cvref_t<T> generic_simd_create_op_impl(T const& a,T const& b,Func func) noexcept
{
	std::remove_cvref_t<T> newval;
	constexpr std::size_t N{std::remove_cvref_t<T>::size()};
	for(std::size_t i{};i!=N;++i)
	{
		newval.value[i]=func(a.value[i],b.value[i]);
	}
	return newval;
}

template<typename T,std::size_t N>
struct
#if defined(_MSC_VER) && !defined(__clang__)
__declspec(intrin_type) __declspec(align(sizeof(T)*N/2))
#endif
generic_simd_vector
{
	using value_type = T;
	using vec_type = value_type[N];
	vec_type value;
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#endif
	inline void load(void const* address) noexcept
	{
#if defined(__has_builtin)
#if __has_builtin(__builtin_memcpy)
		__builtin_memcpy(__builtin_addressof(value),address,sizeof(value));
#else
		::std::memcpy(__builtin_addressof(value),address,sizeof(value));
#endif
#else
		::std::memcpy(__builtin_addressof(value),address,sizeof(value));
#endif
	}
	inline static constexpr std::size_t size() noexcept
	{
		return N;
	}
#if __has_cpp_attribute(__gnu__::__always_inline__)
	[[__gnu__::__always_inline__]]
#endif
	inline void store(void* address) noexcept
	{
#if defined(__has_builtin)
#if __has_builtin(__builtin_memcpy)
		__builtin_memcpy(address,__builtin_addressof(value),sizeof(value));
#else
		::std::memcpy(address,__builtin_addressof(value),sizeof(value));
#endif
#else
		::std::memcpy(address,__builtin_addressof(value),sizeof(value));
#endif
	}
	inline constexpr value_type front() const noexcept
	{
		return value[0];
	}
	inline constexpr value_type back() const noexcept
	{
		constexpr std::size_t nm1{N-1};
		return value[nm1];
	}

	inline static constexpr bool empty() noexcept
	{
		return !N;
	}
	inline static constexpr std::size_t max_size() noexcept
	{
		constexpr std::size_t v{static_cast<std::size_t>(-1)/sizeof(value_type)};
		return v;
	}
	inline constexpr value_type operator[](std::size_t n) const noexcept
	{
		return value[n];
	}

	inline constexpr generic_simd_vector<T,N>& operator+=(generic_simd_vector<T,N> const& other) noexcept
	{
		generic_simd_operation_impl(*this,other,[](T& a,T t) noexcept
		{
			a+=t;
		});
		return *this;
	}
	inline constexpr generic_simd_vector<T,N>& operator-=(generic_simd_vector<T,N> const& other) noexcept
	{
		generic_simd_operation_impl(*this,other,[](T& a,T t) noexcept
		{
			a-=t;
		});
		return *this;
	}
	inline constexpr generic_simd_vector<T,N>& operator*=(generic_simd_vector<T,N> const& other) noexcept
	{
		generic_simd_operation_impl(*this,other,[](T& a,T t) noexcept
		{
			a*=t;
		});
		return *this;
	}
	inline constexpr generic_simd_vector<T,N>& operator/=(generic_simd_vector<T,N> const& other) noexcept
	{
		generic_simd_operation_impl(*this,other,[](T& a,T t) noexcept
		{
			a/=t;
		});
		return *this;
	}
	inline constexpr generic_simd_vector<T,N> operator-() const noexcept
	{
		return generic_simd_self_create_op_impl(*this,[](T v)
		{
			return -v;
		});
	}
	inline constexpr generic_simd_vector<T,N>& operator&=(generic_simd_vector<T,N> const& other) noexcept
	{
		generic_simd_operation_impl(*this,other,[](T& a,T t) noexcept
		{
			a&=t;
		});
		return *this;
	}
	inline constexpr generic_simd_vector<T,N>& operator^=(generic_simd_vector<T,N> const& other) noexcept
	{
		generic_simd_operation_impl(*this,other,[](T& a,T t) noexcept
		{
			a^=t;
		});
		return *this;
	}
	inline constexpr generic_simd_vector<T,N>& operator|=(generic_simd_vector<T,N> const& other) noexcept
	{
		generic_simd_operation_impl(*this,other,[](T& a,T t) noexcept
		{
			a|=t;
		});
		return *this;
	}
	inline constexpr generic_simd_vector<T,N>& operator<<=(generic_simd_vector<T,N> const& other) noexcept
	{
		generic_simd_operation_impl(*this,other,[](T& a,T t) noexcept
		{
			a<<=t;
		});
		return *this;
	}
	inline constexpr generic_simd_vector<T,N>& operator>>=(generic_simd_vector<T,N> const& other) noexcept
	{
		generic_simd_operation_impl(*this,other,[](T& a,T t) noexcept
		{
			a>>=t;
		});
		return *this;
	}
	inline constexpr generic_simd_vector<T,N> operator~() const noexcept
	{
		return generic_simd_self_create_op_impl(*this,[](T v) noexcept
		{
			return ~v;
		});
	}
	inline constexpr void swap_endian() noexcept requires(::std::integral<value_type>)
	{
		generic_simd_self_op_impl(*this,[](T& t)
		{
			t=::fast_io::byte_swap(t);
		});
	}
};

template<typename T>
inline constexpr auto create_value_mx() noexcept
{
	::fast_io::freestanding::array<std::uint_least8_t,sizeof(T)> arr;
	for(auto& e : arr)
	{
		e=std::numeric_limits<std::uint_least8_t>::max();
	}
	return std::bit_cast<T>(arr);
}

template<typename T,typename Func>
inline constexpr T generic_simd_comparision_common_impl(T const& a,
	T const& b,Func func) noexcept
{
	using value_type = std::remove_cvref_t<T>::value_type;
	return generic_simd_create_op_impl(a,b,[&](value_type va,value_type vb)->value_type
	{
		bool t{func(va,vb)};
		if(t)
		{
			return {};
		}
		constexpr auto mx{create_value_mx<value_type>()};
		return mx;		
	});
}

template<typename T,std::size_t N>
inline constexpr generic_simd_vector<T,N> operator+(generic_simd_vector<T,N> const& a,generic_simd_vector<T,N> const& b) noexcept
{
#if defined(_MSC_VER) && !defined(__clang__) && defined(_M_X64)
#if __cpp_if_consteval >= 202106L
	if !consteval
#else
	if (!__builtin_is_constant_evaluated())
#endif
	{
		using vec_type = generic_simd_vector<T,N>;
		if constexpr(sizeof(vec_type)==16)
		{
			if constexpr(std::integral<T>)
			{
				__m128i amm = __builtin_bit_cast(__m128i,a);
				__m128i bmm = __builtin_bit_cast(__m128i,b);
				if constexpr(sizeof(T)==1)
				{
					return __builtin_bit_cast(vec_type,_mm_add_epi8(amm,bmm));
				}
				else if constexpr(sizeof(T)==2)
				{
					return __builtin_bit_cast(vec_type,_mm_add_epi16(amm,bmm));
				}
				else if constexpr(sizeof(T)==4)
				{
					return __builtin_bit_cast(vec_type,_mm_add_epi32(amm,bmm));
				}
				else if constexpr(sizeof(T)==8)
				{
					return __builtin_bit_cast(vec_type,_mm_add_epi64(amm,bmm));
				}
			}
			else if constexpr(std::floating_point<T>)
			{
				if constexpr(sizeof(T)==sizeof(float)/2)
				{
					__m128h amm = __builtin_bit_cast(__m128h,a);
					__m128h bmm = __builtin_bit_cast(__m128h,b);
					return __builtin_bit_cast(vec_type,_mm_add_ph(amm,bmm));
				}
				else if constexpr(sizeof(T)==sizeof(float))
				{
					__m128 amm = __builtin_bit_cast(__m128,a);
					__m128 bmm = __builtin_bit_cast(__m128,b);
					return __builtin_bit_cast(vec_type,_mm_add_ps(amm,bmm));
				}
				else if constexpr(sizeof(T)==sizeof(double))
				{
					__m128d amm = __builtin_bit_cast(__m128d,a);
					__m128d bmm = __builtin_bit_cast(__m128d,b);
					return __builtin_bit_cast(vec_type,_mm_add_pd(amm,bmm));
				}
			}
		}
		else if constexpr(sizeof(vec_type)==32&&::fast_io::details::cpu_flags::avx2_supported)
		{
			if constexpr(std::integral<T>)
			{
				__m256i amm = __builtin_bit_cast(__m256i,a);
				__m256i bmm = __builtin_bit_cast(__m256i,b);
				if constexpr(sizeof(T)==1)
				{
					return __builtin_bit_cast(vec_type,_mm256_add_epi8(amm,bmm));
				}
				else if constexpr(sizeof(T)==2)
				{
					return __builtin_bit_cast(vec_type,_mm256_add_epi16(amm,bmm));
				}
				else if constexpr(sizeof(T)==4)
				{
					return __builtin_bit_cast(vec_type,_mm256_add_epi32(amm,bmm));
				}
				else if constexpr(sizeof(T)==8)
				{
					return __builtin_bit_cast(vec_type,_mm256_add_epi64(amm,bmm));
				}
			}
			else if constexpr(std::floating_point<T>)
			{
				if constexpr(sizeof(T)==sizeof(float)/2)
				{
					__m256h amm = __builtin_bit_cast(__m256h,a);
					__m256h bmm = __builtin_bit_cast(__m256h,b);
					return __builtin_bit_cast(vec_type,_mm256_add_ph(amm,bmm));
				}
				else if constexpr(sizeof(T)==sizeof(float))
				{
					__m256 amm = __builtin_bit_cast(__m256,a);
					__m256 bmm = __builtin_bit_cast(__m256,b);
					return __builtin_bit_cast(vec_type,_mm256_add_ps(amm,bmm));
				}
				else if constexpr(sizeof(T)==sizeof(double))
				{
					__m256d amm = __builtin_bit_cast(__m256d,a);
					__m256d bmm = __builtin_bit_cast(__m256d,b);
					return __builtin_bit_cast(vec_type,_mm256_add_pd(amm,bmm));
				}
			}
		}
		else if constexpr(sizeof(vec_type)==64)
		{
			if constexpr(std::integral<T>&&::fast_io::details::cpu_flags::avx512bw_supported)
			{
				__m512i amm = __builtin_bit_cast(__m512i,a);
				__m512i bmm = __builtin_bit_cast(__m512i,b);
				if constexpr(sizeof(T)==1)
				{
					return __builtin_bit_cast(vec_type,_mm512_add_epi8(amm,bmm));
				}
				else if constexpr(sizeof(T)==2)
				{
					return __builtin_bit_cast(vec_type,_mm512_add_epi16(amm,bmm));
				}
				else if constexpr(sizeof(T)==4)
				{
					return __builtin_bit_cast(vec_type,_mm512_add_epi32(amm,bmm));
				}
				else if constexpr(sizeof(T)==8)
				{
					return __builtin_bit_cast(vec_type,_mm512_add_epi64(amm,bmm));
				}
			}
			else if constexpr(std::floating_point<T>&&::fast_io::details::cpu_flags::avx512f_supported)
			{
				if constexpr(sizeof(T)==sizeof(float)/2)
				{
					__m512h amm = __builtin_bit_cast(__m512h,a);
					__m512h bmm = __builtin_bit_cast(__m512h,b);
					return __builtin_bit_cast(vec_type,_mm512_add_ph(amm,bmm));
				}
				else if constexpr(sizeof(T)==sizeof(float))
				{
					__m512 amm = __builtin_bit_cast(__m512,a);
					__m512 bmm = __builtin_bit_cast(__m512,b);
					return __builtin_bit_cast(vec_type,_mm512_add_ps(amm,bmm));
				}
				else if constexpr(sizeof(T)==sizeof(double))
				{
					__m512d amm = __builtin_bit_cast(__m512d,a);
					__m512d bmm = __builtin_bit_cast(__m512d,b);
					return __builtin_bit_cast(vec_type,_mm512_add_pd(amm,bmm));
				}
			}
		}
	}
#endif
	return generic_simd_create_op_impl(a,b,[](T va,T vb)
	{
		return va+vb;
	});
}

template<typename T,std::size_t N>
inline constexpr generic_simd_vector<T,N> operator-(generic_simd_vector<T,N> const& a,generic_simd_vector<T,N> const& b) noexcept
{
#if defined(_MSC_VER) && !defined(__clang__) && defined(_M_X64)
#if __cpp_if_consteval >= 202106L
	if !consteval
#else
	if (!__builtin_is_constant_evaluated())
#endif
	{
		using vec_type = generic_simd_vector<T,N>;
		if constexpr(sizeof(vec_type)==16)
		{
			if constexpr(std::integral<T>)
			{
				__m128i amm = __builtin_bit_cast(__m128i,a);
				__m128i bmm = __builtin_bit_cast(__m128i,b);
				if constexpr(sizeof(T)==1)
				{
					return __builtin_bit_cast(vec_type,_mm_sub_epi8(amm,bmm));
				}
				else if constexpr(sizeof(T)==2)
				{
					return __builtin_bit_cast(vec_type,_mm_sub_epi16(amm,bmm));
				}
				else if constexpr(sizeof(T)==4)
				{
					return __builtin_bit_cast(vec_type,_mm_sub_epi32(amm,bmm));
				}
				else if constexpr(sizeof(T)==8)
				{
					return __builtin_bit_cast(vec_type,_mm_sub_epi64(amm,bmm));
				}
			}
			else if constexpr(std::floating_point<T>)
			{
				if constexpr(sizeof(T)==sizeof(float)/2)
				{
					__m128h amm = __builtin_bit_cast(__m128h,a);
					__m128h bmm = __builtin_bit_cast(__m128h,b);
					return __builtin_bit_cast(vec_type,_mm_sub_ph(amm,bmm));
				}
				else if constexpr(sizeof(T)==sizeof(float))
				{
					__m128 amm = __builtin_bit_cast(__m128,a);
					__m128 bmm = __builtin_bit_cast(__m128,b);
					return __builtin_bit_cast(vec_type,_mm_sub_ps(amm,bmm));
				}
				else if constexpr(sizeof(T)==sizeof(double))
				{
					__m128d amm = __builtin_bit_cast(__m128d,a);
					__m128d bmm = __builtin_bit_cast(__m128d,b);
					return __builtin_bit_cast(vec_type,_mm_sub_pd(amm,bmm));
				}
			}
		}
		else if constexpr(sizeof(vec_type)==32&&::fast_io::details::cpu_flags::avx2_supported)
		{
			if constexpr(std::integral<T>)
			{
				__m256i amm = __builtin_bit_cast(__m256i,a);
				__m256i bmm = __builtin_bit_cast(__m256i,b);
				if constexpr(sizeof(T)==1)
				{
					return __builtin_bit_cast(vec_type,_mm256_sub_epi8(amm,bmm));
				}
				else if constexpr(sizeof(T)==2)
				{
					return __builtin_bit_cast(vec_type,_mm256_sub_epi16(amm,bmm));
				}
				else if constexpr(sizeof(T)==4)
				{
					return __builtin_bit_cast(vec_type,_mm256_sub_epi32(amm,bmm));
				}
				else if constexpr(sizeof(T)==8)
				{
					return __builtin_bit_cast(vec_type,_mm256_sub_epi64(amm,bmm));
				}
			}
			else if constexpr(std::floating_point<T>)
			{
				if constexpr(sizeof(T)==sizeof(float)/2)
				{
					__m256h amm = __builtin_bit_cast(__m256h,a);
					__m256h bmm = __builtin_bit_cast(__m256h,b);
					return __builtin_bit_cast(vec_type,_mm256_sub_ph(amm,bmm));
				}
				else if constexpr(sizeof(T)==sizeof(float))
				{
					__m256 amm = __builtin_bit_cast(__m256,a);
					__m256 bmm = __builtin_bit_cast(__m256,b);
					return __builtin_bit_cast(vec_type,_mm256_sub_ps(amm,bmm));
				}
				else if constexpr(sizeof(T)==sizeof(double))
				{
					__m256d amm = __builtin_bit_cast(__m256d,a);
					__m256d bmm = __builtin_bit_cast(__m256d,b);
					return __builtin_bit_cast(vec_type,_mm256_sub_pd(amm,bmm));
				}
			}
		}
		else if constexpr(sizeof(vec_type)==64)
		{
			if constexpr(std::integral<T>&&::fast_io::details::cpu_flags::avx512bw_supported)
			{
				__m512i amm = __builtin_bit_cast(__m512i,a);
				__m512i bmm = __builtin_bit_cast(__m512i,b);
				if constexpr(sizeof(T)==1)
				{
					return __builtin_bit_cast(vec_type,_mm512_sub_epi8(amm,bmm));
				}
				else if constexpr(sizeof(T)==2)
				{
					return __builtin_bit_cast(vec_type,_mm512_sub_epi16(amm,bmm));
				}
				else if constexpr(sizeof(T)==4)
				{
					return __builtin_bit_cast(vec_type,_mm512_sub_epi32(amm,bmm));
				}
				else if constexpr(sizeof(T)==8)
				{
					return __builtin_bit_cast(vec_type,_mm512_sub_epi64(amm,bmm));
				}
			}
			else if constexpr(std::floating_point<T>&&::fast_io::details::cpu_flags::avx512f_supported)
			{
				if constexpr(sizeof(T)==sizeof(float)/2)
				{
					__m512h amm = __builtin_bit_cast(__m512h,a);
					__m512h bmm = __builtin_bit_cast(__m512h,b);
					return __builtin_bit_cast(vec_type,_mm512_sub_ph(amm,bmm));
				}
				else if constexpr(sizeof(T)==sizeof(float))
				{
					__m512 amm = __builtin_bit_cast(__m512,a);
					__m512 bmm = __builtin_bit_cast(__m512,b);
					return __builtin_bit_cast(vec_type,_mm512_sub_ps(amm,bmm));
				}
				else if constexpr(sizeof(T)==sizeof(double))
				{
					__m512d amm = __builtin_bit_cast(__m512d,a);
					__m512d bmm = __builtin_bit_cast(__m512d,b);
					return __builtin_bit_cast(vec_type,_mm512_sub_pd(amm,bmm));
				}
			}
		}
	}
#endif
	return generic_simd_create_op_impl(a,b,[](T va,T vb)
	{
		return va-vb;
	});
}

template<typename T,std::size_t N>
inline constexpr generic_simd_vector<T,N> operator*(generic_simd_vector<T,N> const& a,generic_simd_vector<T,N> const& b) noexcept
{
#if defined(_MSC_VER) && !defined(__clang__) && defined(_M_X64)
#if __cpp_if_consteval >= 202106L
	if !consteval
#else
	if (!__builtin_is_constant_evaluated())
#endif
	{
		using vec_type = generic_simd_vector<T,N>;
		if constexpr(sizeof(vec_type)==16)
		{
			if constexpr(std::integral<T>)
			{
				__m128i amm = __builtin_bit_cast(__m128i,a);
				__m128i bmm = __builtin_bit_cast(__m128i,b);
				if constexpr(std::signed_integral<T>)
				{
					if constexpr(sizeof(T)==4)
					{
						return __builtin_bit_cast(vec_type,_mm_mul_epi32(amm,bmm));
					}
				}
				else
				{
					if constexpr(sizeof(T)==4)
					{
						return __builtin_bit_cast(vec_type,_mm_mul_epu32(amm,bmm));
					}
				}
			}
			else if constexpr(std::floating_point<T>)
			{
				if constexpr(sizeof(T)==sizeof(float)/2)
				{
					__m128h amm = __builtin_bit_cast(__m128h,a);
					__m128h bmm = __builtin_bit_cast(__m128h,b);
					return __builtin_bit_cast(vec_type,_mm_mul_ph(amm,bmm));
				}
				else if constexpr(sizeof(T)==sizeof(float))
				{
					__m128 amm = __builtin_bit_cast(__m128,a);
					__m128 bmm = __builtin_bit_cast(__m128,b);
					return __builtin_bit_cast(vec_type,_mm_mul_ps(amm,bmm));
				}
				else if constexpr(sizeof(T)==sizeof(double))
				{
					__m128d amm = __builtin_bit_cast(__m128d,a);
					__m128d bmm = __builtin_bit_cast(__m128d,b);
					return __builtin_bit_cast(vec_type,_mm_mul_pd(amm,bmm));
				}
			}
		}
		else if constexpr(sizeof(vec_type)==32&&::fast_io::details::cpu_flags::avx2_supported)
		{
			if constexpr(std::integral<T>)
			{
				__m256i amm = __builtin_bit_cast(__m256i,a);
				__m256i bmm = __builtin_bit_cast(__m256i,b);
				if constexpr(std::signed_integral<T>)
				{
					if constexpr(sizeof(T)==4)
					{
						return __builtin_bit_cast(vec_type,_mm256_mul_epi32(amm,bmm));
					}
				}
				else
				{
					if constexpr(sizeof(T)==4)
					{
						return __builtin_bit_cast(vec_type,_mm256_mul_epu32(amm,bmm));
					}
				}
			}
			else if constexpr(std::floating_point<T>)
			{
				if constexpr(sizeof(T)==sizeof(float)/2)
				{
					__m256h amm = __builtin_bit_cast(__m256h,a);
					__m256h bmm = __builtin_bit_cast(__m256h,b);
					return __builtin_bit_cast(vec_type,_mm256_mul_ph(amm,bmm));
				}
				else if constexpr(sizeof(T)==sizeof(float))
				{
					__m256 amm = __builtin_bit_cast(__m256,a);
					__m256 bmm = __builtin_bit_cast(__m256,b);
					return __builtin_bit_cast(vec_type,_mm256_mul_ps(amm,bmm));
				}
				else if constexpr(sizeof(T)==sizeof(double))
				{
					__m256d amm = __builtin_bit_cast(__m256d,a);
					__m256d bmm = __builtin_bit_cast(__m256d,b);
					return __builtin_bit_cast(vec_type,_mm256_mul_pd(amm,bmm));
				}
			}
		}
		else if constexpr(sizeof(vec_type)==64)
		{
			if constexpr(std::integral<T>&&::fast_io::details::cpu_flags::avx512bw_supported)
			{
				__m512i amm = __builtin_bit_cast(__m512i,a);
				__m512i bmm = __builtin_bit_cast(__m512i,b);
				if constexpr(std::signed_integral<T>)
				{
					if constexpr(sizeof(T)==4)
					{
						return __builtin_bit_cast(vec_type,_mm512_mul_epi32(amm,bmm));
					}
				}
				else
				{
					if constexpr(sizeof(T)==4)
					{
						return __builtin_bit_cast(vec_type,_mm512_mul_epu32(amm,bmm));
					}
				}
			}
			else if constexpr(std::floating_point<T>&&::fast_io::details::cpu_flags::avx512f_supported)
			{
				if constexpr(sizeof(T)==sizeof(float)/2)
				{
					__m512h amm = __builtin_bit_cast(__m512h,a);
					__m512h bmm = __builtin_bit_cast(__m512h,b);
					return __builtin_bit_cast(vec_type,_mm512_mul_ph(amm,bmm));
				}
				else if constexpr(sizeof(T)==sizeof(float))
				{
					__m512 amm = __builtin_bit_cast(__m512,a);
					__m512 bmm = __builtin_bit_cast(__m512,b);
					return __builtin_bit_cast(vec_type,_mm512_mul_ps(amm,bmm));
				}
				else if constexpr(sizeof(T)==sizeof(double))
				{
					__m512d amm = __builtin_bit_cast(__m512d,a);
					__m512d bmm = __builtin_bit_cast(__m512d,b);
					return __builtin_bit_cast(vec_type,_mm512_mul_pd(amm,bmm));
				}
			}
		}
	}
#endif
	return generic_simd_create_op_impl(a,b,[](T va,T vb)
	{
		return va*vb;
	});
}

template<typename T,std::size_t N>
inline constexpr generic_simd_vector<T,N> operator/(generic_simd_vector<T,N> const& a,generic_simd_vector<T,N> const& b) noexcept
{
#if defined(_MSC_VER) && !defined(__clang__) && defined(_M_X64)
#if __cpp_if_consteval >= 202106L
	if !consteval
#else
	if (!__builtin_is_constant_evaluated())
#endif
	{
		using vec_type = generic_simd_vector<T,N>;
		if constexpr(sizeof(vec_type)==16)
		{
			if constexpr(std::integral<T>)
			{
				__m128i amm = __builtin_bit_cast(__m128i,a);
				__m128i bmm = __builtin_bit_cast(__m128i,b);
				if constexpr(sizeof(T)==1)
				{
					return __builtin_bit_cast(vec_type,_mm_div_epi8(amm,bmm));
				}
				else if constexpr(sizeof(T)==2)
				{
					return __builtin_bit_cast(vec_type,_mm_div_epi16(amm,bmm));
				}
				else if constexpr(sizeof(T)==4)
				{
					return __builtin_bit_cast(vec_type,_mm_div_epi32(amm,bmm));
				}
				else if constexpr(sizeof(T)==8)
				{
					return __builtin_bit_cast(vec_type,_mm_div_epi64(amm,bmm));
				}
			}
			else if constexpr(std::floating_point<T>)
			{
				if constexpr(sizeof(T)==sizeof(float)/2)
				{
					__m128h amm = __builtin_bit_cast(__m128h,a);
					__m128h bmm = __builtin_bit_cast(__m128h,b);
					return __builtin_bit_cast(vec_type,_mm_div_ph(amm,bmm));
				}
				else if constexpr(sizeof(T)==sizeof(float))
				{
					__m128 amm = __builtin_bit_cast(__m128,a);
					__m128 bmm = __builtin_bit_cast(__m128,b);
					return __builtin_bit_cast(vec_type,_mm_div_ps(amm,bmm));
				}
				else if constexpr(sizeof(T)==sizeof(double))
				{
					__m128d amm = __builtin_bit_cast(__m128d,a);
					__m128d bmm = __builtin_bit_cast(__m128d,b);
					return __builtin_bit_cast(vec_type,_mm_div_pd(amm,bmm));
				}
			}
		}
		else if constexpr(sizeof(vec_type)==32&&::fast_io::details::cpu_flags::avx2_supported)
		{
			if constexpr(std::integral<T>)
			{
				__m256i amm = __builtin_bit_cast(__m256i,a);
				__m256i bmm = __builtin_bit_cast(__m256i,b);
				if constexpr(sizeof(T)==1)
				{
					return __builtin_bit_cast(vec_type,_mm256_div_epi8(amm,bmm));
				}
				else if constexpr(sizeof(T)==2)
				{
					return __builtin_bit_cast(vec_type,_mm256_div_epi16(amm,bmm));
				}
				else if constexpr(sizeof(T)==4)
				{
					return __builtin_bit_cast(vec_type,_mm256_div_epi32(amm,bmm));
				}
				else if constexpr(sizeof(T)==8)
				{
					return __builtin_bit_cast(vec_type,_mm256_div_epi64(amm,bmm));
				}
			}
			else if constexpr(std::floating_point<T>)
			{
				if constexpr(sizeof(T)==sizeof(float)/2)
				{
					__m256h amm = __builtin_bit_cast(__m256h,a);
					__m256h bmm = __builtin_bit_cast(__m256h,b);
					return __builtin_bit_cast(vec_type,_mm256_div_ph(amm,bmm));
				}
				else if constexpr(sizeof(T)==sizeof(float))
				{
					__m256 amm = __builtin_bit_cast(__m256,a);
					__m256 bmm = __builtin_bit_cast(__m256,b);
					return __builtin_bit_cast(vec_type,_mm256_div_ps(amm,bmm));
				}
				else if constexpr(sizeof(T)==sizeof(double))
				{
					__m256d amm = __builtin_bit_cast(__m256d,a);
					__m256d bmm = __builtin_bit_cast(__m256d,b);
					return __builtin_bit_cast(vec_type,_mm256_div_pd(amm,bmm));
				}
			}
		}
		else if constexpr(sizeof(vec_type)==64)
		{
			if constexpr(std::integral<T>&&::fast_io::details::cpu_flags::avx512bw_supported)
			{
				__m512i amm = __builtin_bit_cast(__m512i,a);
				__m512i bmm = __builtin_bit_cast(__m512i,b);
				if constexpr(sizeof(T)==1)
				{
					return __builtin_bit_cast(vec_type,_mm512_div_epi8(amm,bmm));
				}
				else if constexpr(sizeof(T)==2)
				{
					return __builtin_bit_cast(vec_type,_mm512_div_epi16(amm,bmm));
				}
				else if constexpr(sizeof(T)==4)
				{
					return __builtin_bit_cast(vec_type,_mm512_div_epi32(amm,bmm));
				}
				else if constexpr(sizeof(T)==8)
				{
					return __builtin_bit_cast(vec_type,_mm512_div_epi64(amm,bmm));
				}
			}
			else if constexpr(std::floating_point<T>&&::fast_io::details::cpu_flags::avx512f_supported)
			{
				if constexpr(sizeof(T)==sizeof(float)/2)
				{
					__m512h amm = __builtin_bit_cast(__m512h,a);
					__m512h bmm = __builtin_bit_cast(__m512h,b);
					return __builtin_bit_cast(vec_type,_mm512_div_ph(amm,bmm));
				}
				else if constexpr(sizeof(T)==sizeof(float))
				{
					__m512 amm = __builtin_bit_cast(__m512,a);
					__m512 bmm = __builtin_bit_cast(__m512,b);
					return __builtin_bit_cast(vec_type,_mm512_div_ps(amm,bmm));
				}
				else if constexpr(sizeof(T)==sizeof(double))
				{
					__m512d amm = __builtin_bit_cast(__m512d,a);
					__m512d bmm = __builtin_bit_cast(__m512d,b);
					return __builtin_bit_cast(vec_type,_mm512_div_pd(amm,bmm));
				}
			}
		}
	}
#endif
	return generic_simd_create_op_impl(a,b,[](T va,T vb)
	{
		return va/vb;
	});
}

template<typename T,std::size_t N>
inline constexpr generic_simd_vector<T,N> operator&(generic_simd_vector<T,N> const& a,generic_simd_vector<T,N> const& b) noexcept
{
	return generic_simd_create_op_impl(a,b,[](T va,T vb)
	{
		return va&vb;
	});
}

template<typename T,std::size_t N>
inline constexpr generic_simd_vector<T,N> operator|(generic_simd_vector<T,N> const& a,generic_simd_vector<T,N> const& b) noexcept
{
	return generic_simd_create_op_impl(a,b,[](T va,T vb)
	{
		return va|vb;
	});
}

template<typename T,std::size_t N>
inline constexpr generic_simd_vector<T,N> operator^(generic_simd_vector<T,N> const& a,generic_simd_vector<T,N> const& b) noexcept
{
	return generic_simd_create_op_impl(a,b,[](T va,T vb)
	{
		return va^vb;
	});
}

template<typename T,std::size_t N>
inline constexpr generic_simd_vector<T,N> operator<<(generic_simd_vector<T,N> const& a,generic_simd_vector<T,N> const& b) noexcept
{
	return generic_simd_create_op_impl(a,b,[](T va,T vb)
	{
		return va<<vb;
	});
}

template<typename T,std::size_t N>
inline constexpr generic_simd_vector<T,N> operator>>(generic_simd_vector<T,N> const& a,generic_simd_vector<T,N> const& b) noexcept
{
	return generic_simd_create_op_impl(a,b,[](T va,T vb)
	{
		return va>>vb;
	});
}

template<typename T,std::size_t N,std::integral I>
inline constexpr generic_simd_vector<T,N> operator<<(generic_simd_vector<T,N> const& a,I i) noexcept
{
	return generic_simd_self_create_op_impl(a,[i](T va)
	{
		return va<<i;
	});
}

template<typename T,std::size_t N,std::integral I>
inline constexpr generic_simd_vector<T,N> operator>>(generic_simd_vector<T,N> const& a,I i) noexcept
{
	return generic_simd_self_create_op_impl(a,[i](T va)
	{
		return va>>i;
	});
}

template<typename T,std::size_t N>
inline constexpr generic_simd_vector<T,N> operator<(generic_simd_vector<T,N> const& a,generic_simd_vector<T,N> const& b) noexcept
{
	return generic_simd_comparision_common_impl(a,b,[](T va,T vb) noexcept
	{
		return va<vb;
	});
}

template<typename T,std::size_t N>
inline constexpr generic_simd_vector<T,N> operator>(generic_simd_vector<T,N> const& a,generic_simd_vector<T,N> const& b) noexcept
{
	return generic_simd_comparision_common_impl(a,b,[](T va,T vb) noexcept
	{
		return va>vb;
	});
}

template<typename T,std::size_t N>
inline constexpr bool test_zero(generic_simd_vector<T,N> const& a) noexcept
{
	for(auto const e : a.value)
	{
		if(e)
		{
			return false;
		}
	}
	return true;
}

template<typename T,std::size_t N>
inline constexpr generic_simd_vector<T,N> operator<=(generic_simd_vector<T,N> const& a,generic_simd_vector<T,N> const& b) noexcept
{
	return generic_simd_comparision_common_impl(a,b,[](T va,T vb) noexcept
	{
		return va<=vb;
	});
}

template<typename T,std::size_t N>
inline constexpr generic_simd_vector<T,N> operator>=(generic_simd_vector<T,N> const& a,generic_simd_vector<T,N> const& b) noexcept
{
	return generic_simd_comparision_common_impl(a,b,[](T va,T vb) noexcept
	{
		return va>=vb;
	});
}

template<typename T,std::size_t N>
inline constexpr generic_simd_vector<T,N> operator==(generic_simd_vector<T,N> const& a,generic_simd_vector<T,N> const& b) noexcept
{
	return generic_simd_comparision_common_impl(a,b,[](T va,T vb) noexcept
	{
		return va==vb;
	});
}

template<typename T,std::size_t N>
inline constexpr generic_simd_vector<T,N> operator!=(generic_simd_vector<T,N> const& a,generic_simd_vector<T,N> const& b) noexcept
{
	return generic_simd_comparision_common_impl(a,b,[](T va,T vb) noexcept
	{
		return va!=vb;
	});
}

}

#if !defined(__clang__)
namespace intrinsics
{
template<typename T,std::size_t N>
using simd_vector = ::fast_io::details::generic_simd_vector<T,N>;
}
#endif

}
