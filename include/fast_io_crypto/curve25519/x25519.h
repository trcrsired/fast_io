#pragma once

/*
Generic class wrapper over the x25519 API.
The free functions live in ::fast_io::diffie_hellman::details and
::fast_io::curve25519::details; this class exposes the same surface as
static members for template code written generically over key-exchange
algorithms.
*/

namespace fast_io::diffie_hellman
{

class x25519
{
public:
	using blinding_context = ::fast_io::curve25519::edp_blinding_context;
	using field_number = ::fast_io::curve25519::field_number;

	static inline constexpr ::std::size_t key_size{::fast_io::diffie_hellman::details::key_size};
	static inline constexpr ::fast_io::containers::array<::std::byte,32> const& base_point{
		::fast_io::diffie_hellman::details::base_point};

	static inline constexpr void trim_secret_key(::fast_io::containers::index_span<::std::byte,32> secret_key) noexcept
	{
		::fast_io::curve25519::details::x25519_trim_secret_key(secret_key);
	}

	static inline constexpr void calculate_public_key_to_ptr(::std::byte* public_key,::std::byte* secret_key) noexcept
	{
		::fast_io::diffie_hellman::details::calculate_public_key_to_ptr(public_key,secret_key);
	}

	static inline constexpr void calculate_public_key_to_ptr(::std::byte* public_key,::std::byte* secret_key,field_number const& zr) noexcept
	{
		::fast_io::diffie_hellman::details::calculate_public_key_to_ptr(public_key,secret_key,zr);
	}

	static inline constexpr void create_shared_key_to_ptr(::std::byte* shared_key,::std::byte const* public_key,::std::byte* secret_key) noexcept
	{
		::fast_io::diffie_hellman::details::create_shared_key_to_ptr(shared_key,public_key,secret_key);
	}

	static inline constexpr void create_shared_key_to_ptr(::std::byte* shared_key,::std::byte const* public_key,::std::byte* secret_key,field_number const& zr) noexcept
	{
		::fast_io::diffie_hellman::details::create_shared_key_to_ptr(shared_key,public_key,secret_key,zr);
	}

	static inline void calculate_public_key_fast_to_ptr(::std::byte* public_key,::std::byte* secret_key) noexcept
	{
		::fast_io::diffie_hellman::details::calculate_public_key_fast_to_ptr(public_key,secret_key);
	}

	static inline void calculate_public_key_fast_to_ptr(::std::byte* public_key,::std::byte* secret_key,blinding_context const& blinding) noexcept
	{
		::fast_io::diffie_hellman::details::calculate_public_key_fast_to_ptr(public_key,secret_key,blinding);
	}

	static inline constexpr void base_point_multiply(::std::byte* r,field_number const& sk) noexcept
	{
		::fast_io::curve25519::details::x25519_base_point_multiply(r,sk);
	}

	static inline constexpr void base_point_multiply(::std::byte* r,field_number const& sk,blinding_context const& blinding) noexcept
	{
		::fast_io::curve25519::details::x25519_base_point_multiply(r,sk,blinding);
	}

#if __cpp_lib_span >= 202002L && (defined(_GLIBCXX_SPAN) || defined(_LIBCPP_SPAN) || defined(_SPAN_))
	static inline constexpr void calculate_public_key(::std::span<::std::byte,32> public_key,::std::span<::std::byte,32> secret_key) noexcept
	{
		::fast_io::diffie_hellman::details::calculate_public_key(public_key,secret_key);
	}
#endif
};

}
