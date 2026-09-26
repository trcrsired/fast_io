#pragma once

/*
Generic class wrapper over the ed25519 API.
The free functions live in ::fast_io::curve25519::details; this class
exposes the same surface as static members for template code written
generically over signature algorithms.
*/

namespace fast_io
{

class ed25519
{
public:
	using blinding_context = ::fast_io::curve25519::edp_blinding_context;
	using verify_context = ::fast_io::curve25519::ed25519_verify_context;

	static inline constexpr ::std::size_t secret_key_size{32};
	static inline constexpr ::std::size_t public_key_size{32};
	static inline constexpr ::std::size_t private_key_size{64};
	static inline constexpr ::std::size_t signature_size{64};

	static inline constexpr ::fast_io::curve25519::edp_blinding_context const &default_blinding{
		::fast_io::curve25519::default_blinding};

	static inline
#if __cpp_lib_bit_cast >= 201806L && __cpp_lib_is_constant_evaluated >= 201811L
		constexpr
#endif
		void create_key_pair(::std::byte *public_key, ::std::byte *private_key, ::std::byte const *secret_key) noexcept
	{
		::fast_io::curve25519::details::ed25519_create_key_pair(public_key, private_key, secret_key);
	}

	static inline
#if __cpp_lib_bit_cast >= 201806L && __cpp_lib_is_constant_evaluated >= 201811L
		constexpr
#endif
		void create_key_pair(::std::byte *public_key, ::std::byte *private_key, blinding_context const &blinding, ::std::byte const *secret_key) noexcept
	{
		::fast_io::curve25519::details::ed25519_create_key_pair(public_key, private_key, blinding, secret_key);
	}

	static inline
#if __cpp_lib_bit_cast >= 201806L && __cpp_lib_is_constant_evaluated >= 201811L
		constexpr
#endif
		void sign_message(::std::byte *signature, ::std::byte const *private_key, ::std::byte const *msg, ::std::size_t msg_size) noexcept
	{
		::fast_io::curve25519::details::ed25519_sign_message(signature, private_key, msg, msg_size);
	}

	static inline
#if __cpp_lib_bit_cast >= 201806L && __cpp_lib_is_constant_evaluated >= 201811L
		constexpr
#endif
		void sign_message(::std::byte *signature, ::std::byte const *private_key, blinding_context const &blinding, ::std::byte const *msg, ::std::size_t msg_size) noexcept
	{
		::fast_io::curve25519::details::ed25519_sign_message(signature, private_key, blinding, msg, msg_size);
	}

	static inline
#if __cpp_lib_bit_cast >= 201806L && __cpp_lib_is_constant_evaluated >= 201811L
		constexpr
#endif
		void verify_init(verify_context &ctx, ::std::byte const *public_key) noexcept
	{
		::fast_io::curve25519::details::ed25519_verify_init(ctx, public_key);
	}

	static inline
#if __cpp_lib_bit_cast >= 201806L && __cpp_lib_is_constant_evaluated >= 201811L
		constexpr
#endif
		bool verify_check(verify_context const &ctx, ::std::byte const *signature, ::std::byte const *msg, ::std::size_t msg_size) noexcept
	{
		return ::fast_io::curve25519::details::ed25519_verify_check(ctx, signature, msg, msg_size);
	}

	static inline
#if __cpp_lib_bit_cast >= 201806L && __cpp_lib_is_constant_evaluated >= 201811L
		constexpr
#endif
		bool verify_signature(::std::byte const *signature, ::std::byte const *public_key, ::std::byte const *msg, ::std::size_t msg_size) noexcept
	{
		return ::fast_io::curve25519::details::ed25519_verify_signature(signature, public_key, msg, msg_size);
	}

	static inline
#if __cpp_lib_bit_cast >= 201806L && __cpp_lib_is_constant_evaluated >= 201811L
		constexpr
#endif
		blinding_context &blinding_init(blinding_context &ctx, ::std::byte const *seed, ::std::size_t seed_size) noexcept
	{
		return ::fast_io::curve25519::details::ed25519_blinding_init(ctx, seed, seed_size);
	}

	template <typename instmtype>
	static inline blinding_context &blinding_init(blinding_context &ctx, instmtype &&instm)
		FAST_IO_HERBCEPTIONS_THROWS_IF(!::fast_io::operations::defines::input_stream_operations_nothrow<instmtype>)
	{
		return ::fast_io::curve25519::details::ed25519_blinding_init(ctx, ::fast_io::freestanding::forward<instmtype>(instm));
	}
};

} // namespace fast_io
