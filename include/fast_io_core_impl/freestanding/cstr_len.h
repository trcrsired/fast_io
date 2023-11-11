#pragma once

namespace fast_io
{

namespace details
{

template<::std::integral char_type>
inline constexpr ::std::size_t dummy_cstr_len(char_type const* cstr) noexcept
{
	::std::size_t n{};
	for(;*cstr;++cstr)
		++n;
	return n;
}

template<::std::integral char_type>
inline constexpr ::std::size_t dummy_cstr_nlen(char_type const* cstr, ::std::size_t maxn) noexcept
{
	::std::size_t n{};
	for(;n!=maxn&&cstr[n];++n);
	return n;
}

}

template<::std::integral char_type>
inline constexpr ::std::size_t cstr_len(char_type const* cstr) noexcept
{
#if __cpp_consteval >= 202106L
	if consteval
	{
		return details::dummy_cstr_len(cstr);
	}
#else
	if(::std::is_constant_evaluated())
	{
		return details::dummy_cstr_len(cstr);
	}
#endif
	else
	{
		if constexpr (sizeof(char_type) == sizeof(char))
		{
#if defined(__has_builtin)
#if __has_builtin(__builtin_strlen)
			return __builtin_strlen(reinterpret_cast<char const*>(cstr));
#else
			return ::std::strlen(reinterpret_cast<char const*>(cstr));
#endif
#else
			return ::std::strlen(reinterpret_cast<char const*>(cstr));
#endif
		} 
		else if constexpr (sizeof(char_type) == sizeof(wchar_t)) 
		{
#if defined(__has_builtin)
#if __has_builtin(__builtin_strlen)
			return __builtin_wcslen(reinterpret_cast<wchar_t const*>(cstr));
#else
			return ::std::wcslen(reinterpret_cast<wchar_t const*>(cstr));
#endif
#else
			return ::std::wcslen(reinterpret_cast<wchar_t const*>(cstr));
#endif
		}
		else
		{
			return details::dummy_cstr_len(cstr);
		}

	}
}

template<::std::integral char_type>
inline constexpr ::std::size_t cstr_nlen(char_type const* cstr, ::std::size_t n) noexcept
{
#if __cpp_consteval >= 202106L
	if consteval
	{
		return details::dummy_cstr_nlen(cstr, n);
	}
#else
	if(::std::is_constant_evaluated())
	{
		return details::dummy_cstr_nlen(cstr, n);
	}
#endif
	else
	{
		if constexpr (sizeof(char_type) == sizeof(char))
		{
#if defined(__has_builtin)
#if __has_builtin(__builtin_strnlen)
			return __builtin_strnlen(reinterpret_cast<char const*>(cstr), n);
#else
			return details::dummy_cstr_nlen(reinterpret_cast<char const*>(cstr), n);
#endif
#else
			return details::dummy_cstr_nlen(reinterpret_cast<char const*>(cstr), n);
#endif
		}
		else if constexpr (sizeof(char_type) == sizeof(wchar_t)) 
		{
#if defined(__has_builtin)
#if __has_builtin(__builtin_strlen)
			return __builtin_wcsnlen(reinterpret_cast<wchar_t const*>(cstr), n);
#else
			return details::dummy_cstr_nlen(reinterpret_cast<wchar_t const*>(cstr), n);
#endif
#else
			return details::dummy_cstr_nlen(reinterpret_cast<wchar_t const*>(cstr), n);
#endif
		}
		else
		{
			return details::dummy_cstr_nlen(cstr, n);
		}

	}
}

}
