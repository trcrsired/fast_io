#pragma once

namespace fast_io::curve25519
{


template <std::endian end, typename D>
inline
#if __cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L
	constexpr
#endif
	void hash_digest_to_ptr_common_impl(std::byte const *ptr, std::size_t n, D &digest) noexcept
{
	using U = typename D::value_type;
#if __cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L
#if __cpp_if_consteval >= 202106L
	if consteval
#else
	if (__builtin_is_constant_evaluated())
#endif
	{
		for (std::size_t i{}; i != n; ++i)
		{
			U t;
			ptr = ::fast_io::freestanding::type_punning_from_bytes(ptr, t);
			if constexpr (::std::endian::native != end)
			{
				t = ::fast_io::byte_swap(t);
			}
			digest.index_unchecked(i) = t;
		}
	}
	else
#endif
	{
		if constexpr (::std::endian::native == end)
		{
			::fast_io::freestanding::nonoverlapped_bytes_copy_n(ptr, n * sizeof(U), reinterpret_cast<std::byte *>(digest.data()));
		}
		else
		{
			for (std::size_t i{}; i != n; ++i)
			{
				U t;
				ptr = ::fast_io::freestanding::type_punning_from_bytes(ptr, t);
				digest.index_unchecked(i) = ::fast_io::byte_swap(t);
			}
		}
	}
}

template <std::endian end, typename D>
inline
#if __cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L
	constexpr
#endif
	void hash_digest_from_ptr_common_impl(D const &digest, std::size_t n, std::byte *ptr) noexcept
{
	using U = typename D::value_type;
#if __cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L
#if __cpp_if_consteval >= 202106L
	if consteval
#else
	if (__builtin_is_constant_evaluated())
#endif
	{
		for (std::size_t i{}; i != n; ++i)
		{
			U t{digest.index_unchecked(i)};
			if constexpr (::std::endian::native != end)
			{
				t = ::fast_io::byte_swap(t);
			}
			ptr = ::fast_io::freestanding::type_punning_to_bytes(t, ptr);
		}
	}
	else
#endif
	{
		if constexpr (::std::endian::native == end)
		{
			::fast_io::freestanding::nonoverlapped_bytes_copy_n(reinterpret_cast<std::byte const *>(digest.data()), n * sizeof(U), ptr);
		}
		else
		{
			for (std::size_t i{}; i != n; ++i)
			{
				U t{digest.index_unchecked(i)};
				t = ::fast_io::byte_swap(t);
				ptr = ::fast_io::freestanding::type_punning_to_bytes(t, ptr);
			}
		}
	}
}


#if (!defined(_MSC_VER) || defined(__clang__)) && (defined(__SSE4_2__) || defined(__wasm_simd128__))

template <std::endian end, typename D>
inline constexpr void hash_digest_to_ptr_simd16_impl(std::byte const *ptr, std::size_t n, D &digest) noexcept
{
	using U = typename D::value_type;
	constexpr std::size_t usz{sizeof(U)};
#if __cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L
#if __cpp_if_consteval >= 202106L
	if consteval
#else
	if (__builtin_is_constant_evaluated())
#endif
	{
		hash_digest_to_ptr_common_impl<end>(ptr, n, digest);
	}
	else
#endif
	{
		if constexpr (::std::endian::native == end)
		{
			::fast_io::freestanding::nonoverlapped_bytes_copy_n(ptr, n * sizeof(U), reinterpret_cast<std::byte *>(digest.data()));
		}
		else
		{
			constexpr std::size_t sixteen{16u};
			constexpr std::size_t factor{sixteen / usz};
			static_assert(sixteen % usz == 0 && usz != sixteen);
			::fast_io::intrinsics::simd_vector<U, factor> s;
			U *d{digest.data()};
			std::byte const *i{ptr};
			std::byte const *e{ptr + n * sizeof(U)};
			for (; i != e; i += sixteen, d += factor)
			{
				s.load(i);
				s.swap_endian();
				s.store(d);
			}
		}
	}
}

template <std::endian end, typename D>
inline constexpr void hash_digest_from_ptr_simd16_impl(D const &digest, std::size_t n, std::byte *ptr) noexcept
{
	using U = typename D::value_type;
	constexpr std::size_t usz{sizeof(U)};
#if __cpp_if_consteval >= 202106L || __cpp_lib_is_constant_evaluated >= 201811L
#if __cpp_if_consteval >= 202106L
	if consteval
#else
	if (__builtin_is_constant_evaluated())
#endif
	{
		hash_digest_from_ptr_common_impl<end>(digest, n, ptr);
	}
	else
#endif
	{
		if constexpr (::std::endian::native == end)
		{
			::fast_io::freestanding::nonoverlapped_bytes_copy_n(reinterpret_cast<std::byte const *>(digest.data()), n * sizeof(U), ptr);
		}
		else
		{
			constexpr std::size_t sixteen{16u};
			constexpr std::size_t factor{sixteen / usz};
			static_assert(sixteen % usz == 0 && usz != sixteen);
			::fast_io::intrinsics::simd_vector<U, factor> s;
			U const *d{digest.data()};
			std::byte *i{ptr};
			std::byte *e{ptr + n * sizeof(U)};
			for (; i != e; i += sixteen, d += factor)
			{
				s.load(d);
				s.swap_endian();
				s.store(i);
			}
		}
	}
}

#endif

template <std::endian end, typename D>
	requires(sizeof(typename D::value_type) != 0)
inline constexpr void hash_digest_to_byte_ptr_common(std::byte const *ptr, D &digest) noexcept
{
	using U = typename D::value_type;
	constexpr std::size_t sz{D::size() * sizeof(U)};
	static_assert(sz != 0);
#if (!defined(_MSC_VER) || defined(__clang__)) && (defined(__SSE4_2__) || defined(__wasm_simd128__))
	if constexpr (sz % 16u == 0)
	{
		hash_digest_to_ptr_simd16_impl<end>(ptr, D::size(), digest);
	}
	else
#endif
	{
		hash_digest_to_ptr_common_impl<end>(ptr, D::size(), digest);
	}
}

template <std::endian end, typename D>
	requires(sizeof(typename D::value_type) != 0)
inline constexpr void hash_digest_from_byte_ptr_common(D const &digest, std::byte *ptr) noexcept
{
	using U = typename D::value_type;
	constexpr std::size_t sz{D::size() * sizeof(U)};
	static_assert(sz != 0);
#if (!defined(_MSC_VER) || defined(__clang__)) && (defined(__SSE4_2__) || defined(__wasm_simd128__))
	if constexpr (sz % 16u == 0)
	{
		hash_digest_from_ptr_simd16_impl<end>(digest, D::size(), ptr);
	}
	else
#endif
	{
		hash_digest_from_ptr_common_impl<end>(digest, D::size(), ptr);
	}
}

namespace details
{

/*
Encode a field element y-coord as little-endian bytes, top bit = x parity.
*/
inline constexpr void ed25519_encode(::std::byte *y, field_number const &x, ::std::byte parity) noexcept
{
	field_number t{x};
	t.back_unchecked() = (t.back_unchecked() & 0x7fffffffffffffffULL) |
						 (static_cast<std::uint_least64_t>(parity & std::byte{1}) << 63u);
	hash_digest_from_byte_ptr_common<std::endian::little>(t, y);
}

inline constexpr void ed25519_packpoint(::std::byte *y, field_number const &x, ::std::byte parity) noexcept
{
	ed25519_encode(y, x, parity & std::byte{1});
}

/*
Decode a little-endian encoded point: returns the x-parity bit, y words to out.
*/
inline constexpr std::uint_least64_t ed25519_decode_int(field_number &y, ::std::byte const *x) noexcept
{
	y.front_unchecked() = bytes_to_u64_little_endian(x);
	y.index_unchecked(1) = bytes_to_u64_little_endian(x + 8);
	y.index_unchecked(2) = bytes_to_u64_little_endian(x + 16);
	y.back_unchecked() = bytes_to_u64_little_endian(x + 24) & 0x7FFFFFFFFFFFFFFFULL;
	return static_cast<std::uint_least64_t>(x[31] >> 7);
}

inline
#if __cpp_lib_bit_cast >= 201806L && __cpp_lib_is_constant_evaluated >= 201811L
	constexpr
#endif
	void ed25519_create_key_pair(std::byte *public_key, std::byte *private_key, std::byte const *secret_key) noexcept
{
	constexpr std::size_t keylength{32};
	::fast_io::sha512_context sha;
	sha.update(secret_key, secret_key + keylength);
	sha.do_final();
	constexpr std::size_t digest_size{::fast_io::sha512_context::digest_size};
	::fast_io::containers::array<::std::byte, digest_size> md;
	sha.digest_to_byte_ptr(md.data());

	x25519_trim_secret_key(::fast_io::containers::index_span<::std::byte, 32>{::fast_io::containers::index_unchecked, md.data()});
	field_number t;
	hash_digest_to_byte_ptr_common<::std::endian::little>(md.data(), t);

	affine_point q;
	base_point_multiply(q, t);
	ed25519_packpoint(public_key, q.y, static_cast<std::byte>(q.x.front_unchecked()));

	::fast_io::freestanding::nonoverlapped_bytes_copy_n(secret_key, keylength, private_key);
	::fast_io::freestanding::nonoverlapped_bytes_copy_n(public_key, keylength, private_key + keylength);
}

inline
#if __cpp_lib_bit_cast >= 201806L && __cpp_lib_is_constant_evaluated >= 201811L
	constexpr
#endif
	void ed25519_create_key_pair_with_blinding(std::byte *public_key, std::byte *private_key, std::byte const *secret_key, edp_blinding_context &blinding) noexcept
{
	constexpr std::size_t keylength{32};
	::fast_io::sha512_context sha;
	sha.update(secret_key, secret_key + keylength);
	sha.do_final();
	constexpr std::size_t digest_size{::fast_io::sha512_context::digest_size};
	::fast_io::containers::array<::std::byte, digest_size> md;
	sha.digest_to_byte_ptr(md.data());

	x25519_trim_secret_key(::fast_io::containers::index_span<::std::byte, 32>{::fast_io::containers::index_unchecked, md.data()});
	field_number t;
	hash_digest_to_byte_ptr_common<::std::endian::little>(md.data(), t);

	affine_point q;
	base_point_multiply(q, t, blinding);
	ed25519_packpoint(public_key, q.y, static_cast<std::byte>(q.x.front_unchecked()));

	::fast_io::freestanding::nonoverlapped_bytes_copy_n(secret_key, keylength, private_key);
	::fast_io::freestanding::nonoverlapped_bytes_copy_n(public_key, keylength, private_key + keylength);
}

inline
#if __cpp_lib_bit_cast >= 201806L && __cpp_lib_is_constant_evaluated >= 201811L
	constexpr
#endif
	void ed25519_sign_message_impl(::std::byte *signature, ::std::byte const *private_key, edp_blinding_context const *blinding, ::std::byte const *msg, std::size_t msg_size) noexcept
{
	affine_point R;
	field_number a, t, r;
	::fast_io::sha512_context H;

	constexpr std::size_t digest_size{::fast_io::sha512_context::digest_size};
	::fast_io::containers::array<::std::byte, digest_size> md;

	/* [a:b] = H(sk), a = clamped first half */
	H.update(private_key, private_key + 32);
	H.do_final();
	H.digest_to_byte_ptr(md.data());
	x25519_trim_secret_key(::fast_io::containers::index_span<::std::byte, 32>{::fast_io::containers::index_unchecked, md.data()});
	hash_digest_to_byte_ptr_common<::std::endian::little>(md.data(), a);

	/* r = H(b || m) mod BPO */
	H.reset();
	H.update(md.data() + 32, md.data() + 64);
	H.update(msg, msg + msg_size);
	H.do_final();
	H.digest_to_byte_ptr(md.data());
	eco_digest_to_words(r, md.data());
	eco_mod(r);

	/* R = r*B */
	if (blinding)
	{
		base_point_multiply(R, r, *blinding);
	}
	else
	{
		base_point_multiply(R, r);
	}
	ed25519_packpoint(signature, R.y, static_cast<std::byte>(R.x.front_unchecked()));

	/* S = r + H(R || pk || m)*a mod BPO */
	H.reset();
	H.update(signature, signature + 32);
	H.update(private_key + 32, private_key + 64);
	H.update(msg, msg + msg_size);
	H.do_final();
	H.digest_to_byte_ptr(md.data());
	eco_digest_to_words(t, md.data());
	eco_mul_reduce(t, t, a);
	eco_add_reduce(t, t, r);
	eco_mod(t);
	for (::std::uint_fast8_t i{}; i != 4; ++i)
	{
		u64_to_bytes_little_endian(signature + 32 + i * 8, t.index_unchecked(i));
	}

// Clear sensitive data. Not needed (nor possible) during constant evaluation.
#if __cpp_if_consteval >= 202106L
	if !consteval
#else
	if (!__builtin_is_constant_evaluated())
#endif
	{
		::fast_io::secure_clear(__builtin_addressof(a), sizeof(a));
		::fast_io::secure_clear(__builtin_addressof(r), sizeof(r));
	}
}

/*
signature = R(32 bytes) || S(32 bytes)
R = r*B  where r = H(sk_hash[32:64] || msg) mod BPO
S = (r + H(R || pk || msg)*a) mod BPO
*/
inline
#if __cpp_lib_bit_cast >= 201806L && __cpp_lib_is_constant_evaluated >= 201811L
	constexpr
#endif
	void ed25519_sign_message(::std::byte *signature, ::std::byte const *private_key, ::std::byte const *msg, std::size_t msg_size) noexcept
{
	ed25519_sign_message_impl(signature, private_key, nullptr, msg, msg_size);
}

inline
#if __cpp_lib_bit_cast >= 201806L && __cpp_lib_is_constant_evaluated >= 201811L
	constexpr
#endif
	void ed25519_sign_message_with_blinding(::std::byte *signature, ::std::byte const *private_key, ::std::byte const *msg, std::size_t msg_size, edp_blinding_context &blinding) noexcept
{
	ed25519_sign_message_impl(signature, private_key, __builtin_addressof(blinding), msg, msg_size);
}

} // namespace details

} // namespace fast_io::curve25519
