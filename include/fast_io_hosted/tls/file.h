#pragma once

/*
basic_tls_file<ch_type> -- owns a tls_client and satisfies the io stream
concepts. The socket fd inside is BORROWED: destruction does not close it
(the caller's socket object owns it) and does not send close_notify --
call send_close_notify() explicitly when the peer cares about truncation.
*/

#if defined(__linux__)

namespace fast_io::tls
{

template <::std::integral ch_type>
class basic_tls_file : public basic_tls_io_observer<ch_type>
{
	tls_client client_{};

public:
	using typename basic_tls_io_observer<ch_type>::char_type;
	using typename basic_tls_io_observer<ch_type>::input_char_type;
	using typename basic_tls_io_observer<ch_type>::output_char_type;
	using typename basic_tls_io_observer<ch_type>::native_handle_type;
	using basic_tls_io_observer<ch_type>::native_handle;

	inline constexpr basic_tls_file() noexcept
	{
		this->handle = __builtin_addressof(client_);
	}
	/* borrow an established TCP fd (kernel ULP+keys are installed by handshake) */
	inline explicit constexpr basic_tls_file(int fd) noexcept : client_{fd}
	{
		this->handle = __builtin_addressof(client_);
	}
	inline constexpr basic_tls_file(decltype(nullptr)) noexcept = delete;

	basic_tls_file(basic_tls_file const &) = delete;
	basic_tls_file &operator=(basic_tls_file const &) = delete;

	inline basic_tls_file(basic_tls_file &&other) noexcept
		: client_{other.client_}
	{
		this->handle = __builtin_addressof(client_);
		/* the moved-from object keeps no key copies */
		::fast_io::secure_clear(__builtin_addressof(other.client_), sizeof(other.client_));
	}
	inline basic_tls_file &operator=(basic_tls_file &&other) noexcept
	{
		if (__builtin_addressof(other) == this)
		{
			return *this;
		}
		::fast_io::secure_clear(__builtin_addressof(client_), sizeof(client_));
		client_ = other.client_;
		this->handle = __builtin_addressof(client_);
		::fast_io::secure_clear(__builtin_addressof(other.client_), sizeof(other.client_));
		return *this;
	}

	inline constexpr tls_client &client() noexcept
	{
		return client_;
	}
	inline constexpr tls_client const &client() const noexcept
	{
		return client_;
	}

	inline void handshake(tls13_client_config const &cfg) FAST_IO_HERBCEPTIONS_THROWS
	{
		client_.handshake(cfg);
	}
	/* system trust bundle + hostname check; defined in roots.h */
	inline void handshake(::fast_io::u8cstring_view hostname) FAST_IO_HERBCEPTIONS_THROWS
	{
		client_.handshake(hostname);
	}
	inline void send_close_notify() noexcept
	{
		client_.send_close_notify();
	}
};

using tls_io_observer = basic_tls_io_observer<char>;
using wtls_io_observer = basic_tls_io_observer<wchar_t>;
using u8tls_io_observer = basic_tls_io_observer<char8_t>;
using u16tls_io_observer = basic_tls_io_observer<char16_t>;
using u32tls_io_observer = basic_tls_io_observer<char32_t>;

using tls_file = basic_tls_file<char>;
using wtls_file = basic_tls_file<wchar_t>;
using u8tls_file = basic_tls_file<char8_t>;
using u16tls_file = basic_tls_file<char16_t>;
using u32tls_file = basic_tls_file<char32_t>;

} // namespace fast_io::tls

#endif
