#pragma once

#include "../../../fast_io_dsal/impl/misc/push_macros.h"

namespace fast_io
{

template <::fast_io::wine_family family>
struct wine_family_file_factory FAST_IO_TRIVIALLY_RELOCATABLE_IF_ELIGIBLE
{
	using native_handle_type = ::fast_io::wine_host_fd_t;
	native_handle_type host_fd{};

	inline explicit constexpr wine_family_file_factory(native_handle_type v) noexcept
		: host_fd(v) {};
	inline wine_family_file_factory(wine_family_file_factory const &) = delete;
	inline wine_family_file_factory &operator=(wine_family_file_factory const &) = delete;
	inline ~wine_family_file_factory()
	{
		if (host_fd) [[likely]]
		{
			__wine_unix_close_returns_status(host_fd);
		}
	}
};

using wine_file_factory = wine_family_file_factory<::fast_io::wine_family::native>;

template <::fast_io::wine_family family, ::std::integral ch_type>
class basic_wine_family_file : public basic_wine_family_io_observer<family, ch_type>
{
public:
	using typename basic_wine_family_io_observer<family, ch_type>::char_type;
	using typename basic_wine_family_io_observer<family, ch_type>::input_char_type;
	using typename basic_wine_family_io_observer<family, ch_type>::output_char_type;
	using typename basic_wine_family_io_observer<family, ch_type>::native_handle_type;
	using file_factory_type = wine_family_file_factory<family>;
	using basic_wine_family_io_observer<family, ch_type>::native_handle;
	inline constexpr basic_wine_family_file() noexcept = default;
	template <typename native_hd>
		requires ::std::same_as<native_handle_type, ::std::remove_cvref_t<native_hd>>
	inline explicit constexpr basic_wine_family_file(native_hd host_fd1) noexcept
		: basic_wine_family_io_observer<family, ch_type>{host_fd1}
	{
	}
	inline explicit constexpr basic_wine_family_file(wine_family_file_factory<family> &&factory) noexcept
		: basic_wine_family_io_observer<family, ch_type>{factory.host_fd}
	{
		factory.host_fd = 0;
	}

	inline explicit basic_wine_family_file(char const *filename, open_mode om,
										   perms pm = static_cast<perms>(436)) FAST_IO_HERBCEPTIONS_THROWS
		: basic_wine_family_io_observer<family, ch_type>{::fast_io::details::wine_open_file_impl(
			  0, filename, ::fast_io::cstr_len(filename), om, pm)}
	{
	}

	inline explicit basic_wine_family_file(wine_at_entry wate, char const *filename, open_mode om,
										   perms pm = static_cast<perms>(436)) FAST_IO_HERBCEPTIONS_THROWS
		: basic_wine_family_io_observer<family, ch_type>{::fast_io::details::wine_open_file_impl(
			  wate.host_fd, filename, ::fast_io::cstr_len(filename), om, pm)}
	{
	}

	inline constexpr basic_wine_family_file(basic_wine_family_file const &) = delete;
	inline constexpr basic_wine_family_file &operator=(basic_wine_family_file const &) = delete;

	inline constexpr basic_wine_family_file(basic_wine_family_file &&__restrict other) noexcept
		: basic_wine_family_io_observer<family, ch_type>{other.release()}
	{
	}
	inline basic_wine_family_file &operator=(basic_wine_family_file &&__restrict other) noexcept
	{
		if (this->host_fd) [[likely]]
		{
			__wine_unix_close_returns_status(this->host_fd);
		}
		this->host_fd = other.release();
		return *this;
	}
	inline constexpr basic_wine_family_file(decltype(nullptr)) noexcept = delete;
	inline constexpr basic_wine_family_file(basic_wine_family_io_observer<family, ch_type>) noexcept = delete;
	inline constexpr basic_wine_family_file &operator=(basic_wine_family_io_observer<family, ch_type>) noexcept = delete;
	inline ~basic_wine_family_file()
	{
		if (this->host_fd) [[likely]]
		{
			__wine_unix_close_returns_status(this->host_fd);
		}
	}
};

template <::std::integral ch_type>
using basic_wine_io_observer = basic_wine_family_io_observer<::fast_io::wine_family::native, ch_type>;

template <::std::integral ch_type>
using basic_wine_file = basic_wine_family_file<::fast_io::wine_family::native, ch_type>;

using wine_io_observer = basic_wine_io_observer<char>;
using wine_file = basic_wine_file<char>;

namespace details
{
template <int which>
inline ::fast_io::wine_host_fd_t wine_get_std_host_fd() noexcept
{
	return __wine_unix_get_std_host_fd_returns_status(which).host_fd;
}
} // namespace details

/*
std streams through whichever wineunix.dll is loaded: unixcall impl returns
unix fd + 1, nt impl returns the process's Standard{Input,Output,Error}
handle — both wrapped as a host_fd.
*/
template <::std::integral char_type = char>
inline basic_wine_io_observer<char_type> wine_stdin() noexcept
{
	return {::fast_io::details::wine_get_std_host_fd<0>()};
}

template <::std::integral char_type = char>
inline basic_wine_io_observer<char_type> wine_stdout() noexcept
{
	return {::fast_io::details::wine_get_std_host_fd<1>()};
}

template <::std::integral char_type = char>
inline basic_wine_io_observer<char_type> wine_stderr() noexcept
{
	return {::fast_io::details::wine_get_std_host_fd<2>()};
}

namespace freestanding
{
template <::fast_io::wine_family family>
struct is_trivially_copyable_or_relocatable<wine_family_file_factory<family>>
{
	inline static constexpr bool value = true;
};

template <::fast_io::wine_family family, ::std::integral char_type>
struct is_zero_default_constructible<basic_wine_family_io_observer<family, char_type>>
{
	inline static constexpr bool value = true;
};

template <::fast_io::wine_family family, ::std::integral char_type>
struct is_zero_default_constructible<basic_wine_family_file<family, char_type>>
{
	inline static constexpr bool value = true;
};

} // namespace freestanding
} // namespace fast_io

#include "../../../fast_io_dsal/impl/misc/pop_macros.h"
