#pragma once

namespace fast_io::freestanding
{
#ifdef __HERBCEPTIONS__

#if defined(_WIN32) || defined(__CYGWIN__)
using ::std::win32_errc;
using ::std::nt_errc;
using ::std::wine_errc;
using ::std::com_errc;
#endif

using ::std::cmath_errc;
using ::std::parse_errc;
using ::std::http_errc;

using ::std::cxx_std_error;

#else
#if defined(_WIN32) || defined(__CYGWIN__)
// placeholders
enum class win32_errc : ::std::uint_least32_t
{
};
enum class nt_errc : ::std::uint_least32_t
{
};
enum class wine_errc : ::std::uint_least32_t
{
};
enum class com_errc : ::std::uint_least32_t
{
};
#endif

enum class cmath_errc : ::std::uint_least32_t
{
};
enum class parse_errc : ::std::uint_least32_t
{
	ok = 0,
	end_of_file = 1,
	partial = 2,
	invalid = 3,
	overflow = 4
};

enum class http_errc : ::std::uint_least32_t
{
};

struct cxx_std_error
{
	void const *domain{};
	::std::size_t code{};
};

#endif
#if 0
struct cxx_std_error_guard
{
public:
	::fast_io::freestanding::cxx_std_error err{};
	constexpr cxx_std_error_guard() noexcept = default;
	explicit constexpr cxx_std_error_guard(::fast_io::freeestanding::cxx_std_error other) noexcept:
		err(other)
	{
	}
private:
	constexpr void destroy() noexcept
	{
		auto domain{err.domain};
		if(domain)
		{
#ifdef __HERBCEPTIONS__
			try
			{
				throw throws err;
			}
			catch throws(::std::error)
			{}
#else
			using do_cleanup_function_pointer_type = 
			void (*)(::std::size_t cd) noexcept;
			do_cleanup_function_pointer_type do_cleanup{*static_cast<do_cleanup_function_pointer_type*>(domain)};
			do_cleanup(err.code);
#endif
		}
	}
public:
	constexpr cxx_std_error_guard(cxx_std_error_guard const&) = delete;
	constexpr cxx_std_error_guard& operator=(cxx_std_error_guard const&) = delete;
	constexpr cxx_std_error_guard(cxx_std_error_guard&& other) noexcept : err(other.err)
	{
		other.err = {};
	}
	constexpr cxx_std_error_guard& operator=(cxx_std_error_guard&& other) noexcept
	{
		if(__builtin_addressof(other) == this) [[unlikely]]
		{
			return *this;
		}
		this->destroy();
		this->err = other.err;
		other.err = {};
		return *this;
	}
#ifdef __HERBCEPTIONS__
	constexpr void rethrow_if_cxx_std_error() throws
	{
		if(this->err.domain)
		{
			auto tmp{this->err};
			this->err = {};
			throw throws tmp;
		}
	}
#endif
	constexpr ::fast_io::freestanding::cxx_std_error release() noexcept
	{
		auto tmp = this->err;
		this->err = {};
		return tmp;
	}
	constexpr ~cxx_std_error_guard()
	{
		this->destroy();
	}
};
#endif
} // namespace fast_io::freestanding
