#pragma once

/*
gnutls_crypto_backend: a fast_io::tls crypto backend whose primitives
are the gnutls low-level crypto API (gnutls_aead_cipher_*) plus nettle
curve25519_mul -- nettle is gnutls's own backend, already linked with
-lgnutls. The TLS protocol layer stays in fast_io; the libraries only
supply the math. Link with -lgnutls -lnettle.

The transcript hash and HKDF keep the fast_io implementations, same
reasoning as the openssl backend: they are tiny constexpr routines and
the provider handles are heap objects that cannot satisfy the key
schedule's noexcept machinery. Certificate signature verify likewise
stays on the fast_io parser for now.
*/

#if !defined(FAST_IO_TLS_HAS_GNUTLS_CRYPTO)
#if __has_include(<gnutls/crypto.h>) && __has_include(<nettle/curve25519.h>)
#define FAST_IO_TLS_HAS_GNUTLS_CRYPTO 1
#else
#define FAST_IO_TLS_HAS_GNUTLS_CRYPTO 0
#endif
#endif

#if FAST_IO_TLS_HAS_GNUTLS_CRYPTO

#include <gnutls/crypto.h>
#include <nettle/curve25519.h>

namespace fast_io::tls
{

struct gnutls_crypto_backend
{
	using sha256 = ::fast_io::sha256_context;
	using sha384 = ::fast_io::sha384_context;

	static inline void x25519_keypair(::std::byte *pk, ::std::byte *sk) noexcept
	{
		curve25519_mul_g(reinterpret_cast<uint8_t *>(pk),
						 reinterpret_cast<uint8_t const *>(sk));
	}

	static inline void x25519_shared_secret(::std::byte *out, ::std::byte const *peer_pk,
											::std::byte *sk) noexcept
	{
		curve25519_mul(reinterpret_cast<uint8_t *>(out),
					   reinterpret_cast<uint8_t const *>(sk),
					   reinterpret_cast<uint8_t const *>(peer_pk));
	}

private:
	static inline gnutls_cipher_algorithm_t gnutls_tls_cipher(cipher_suite suite) noexcept
	{
		switch (suite)
		{
		case cipher_suite::aes_128_gcm_sha256:
			return GNUTLS_CIPHER_AES_128_GCM;
		case cipher_suite::aes_256_gcm_sha384:
			return GNUTLS_CIPHER_AES_256_GCM;
		case cipher_suite::chacha20_poly1305_sha256:
			return GNUTLS_CIPHER_CHACHA20_POLY1305;
		default:
			return GNUTLS_CIPHER_UNKNOWN;
		}
	}

public:
	static inline bool record_open(::std::byte *out, ::std::size_t &out_size,
								   content_type &inner_type, ::std::byte const *hdr5,
								   ::std::byte const *ct, ::std::size_t ct_size,
								   cipher_suite suite, ::std::byte const *key,
								   ::std::byte const *iv, ::std::uint_least64_t seq) noexcept
	{
		if (ct_size < 17) /* tag(16) + inner type byte */
		{
			return false;
		}
		::std::byte nonce[12];
		details::tls13_nonce_to_ptr(nonce, iv, seq);
		::std::size_t const inner_size{ct_size - 16};

		gnutls_aead_cipher_hd_t h{};
		gnutls_datum_t const kd{const_cast<char unsigned *>(reinterpret_cast<char unsigned const *>(key)),
								static_cast<unsigned>(details::cipher_suite_key_size(suite))};
		if (gnutls_aead_cipher_init(__builtin_addressof(h), gnutls_tls_cipher(suite),
									__builtin_addressof(kd)) != 0)
		{
			::fast_io::fast_terminate();
		}
		::std::size_t inner{inner_size};
		int const ret{gnutls_aead_cipher_decrypt(
			h, nonce, 12, hdr5, 5, 16, ct, ct_size, out, __builtin_addressof(inner))};
		gnutls_aead_cipher_deinit(h);
		if (ret != 0)
		{
			return false;
		}
		/* strip zero padding, then the inner content type byte */
		while (inner != 0 && out[inner - 1] == ::std::byte{})
		{
			--inner;
		}
		if (inner == 0)
		{
			return false;
		}
		inner_type = static_cast<content_type>(::std::to_underlying(out[--inner]));
		out_size = inner;
		return true;
	}

	static inline ::std::size_t
	record_seal_inner(::std::byte *out, ::std::byte const *inner, ::std::size_t inner_size,
					  cipher_suite suite, ::std::byte const *key, ::std::byte const *iv,
					  ::std::uint_least64_t seq) noexcept
	{
		::std::byte nonce[12];
		details::tls13_nonce_to_ptr(nonce, iv, seq);
		::std::byte *p{details::record_header_write(out, content_type::application_data,
													static_cast<::std::uint_least16_t>(inner_size + 16))};
		::std::byte *const hdr{out};

		gnutls_aead_cipher_hd_t h{};
		gnutls_datum_t const kd{const_cast<char unsigned *>(reinterpret_cast<char unsigned const *>(key)),
								static_cast<unsigned>(details::cipher_suite_key_size(suite))};
		if (gnutls_aead_cipher_init(__builtin_addressof(h), gnutls_tls_cipher(suite),
									__builtin_addressof(kd)) != 0)
		{
			::fast_io::fast_terminate();
		}
		::std::size_t sealed{static_cast<::std::size_t>(out + 16640 + 16 - p)};
		if (gnutls_aead_cipher_encrypt(h, nonce, 12, hdr, 5, 16, inner, inner_size,
									   p, __builtin_addressof(sealed)) != 0)
		{
			::fast_io::fast_terminate();
		}
		gnutls_aead_cipher_deinit(h);
		return static_cast<::std::size_t>(p - out) + sealed;
	}

	static inline ::std::size_t
	record_seal(::std::byte *out, content_type inner_type, ::std::byte const *pt,
				::std::size_t pt_size, cipher_suite suite, ::std::byte const *key,
				::std::byte const *iv, ::std::uint_least64_t seq) noexcept
	{
		if (pt_size >= 16641)
		{
			::fast_io::fast_terminate();
		}
		::std::byte inner[16641];
		::fast_io::freestanding::non_overlapped_copy_n(pt, pt_size, inner);
		inner[pt_size] = static_cast<::std::byte>(inner_type);
		return record_seal_inner(out, inner, pt_size + 1, suite, key, iv, seq);
	}

	/* DER parsing, chain walk and SAN checks stay in fast_io -- the
	   signature math forwards to the fast_io verifier until a provider
	   verify op is wired in */
	static inline details::x509_verify_result
	cert_sig_verify(details::x509_certificate const &cert,
					details::algorithm_identifier const &issuer_alg,
					::std::byte const *issuer_public_key,
					::std::size_t issuer_public_key_size) noexcept
	{
		return details::x509_verify_signature(cert, issuer_alg, issuer_public_key,
											  issuer_public_key_size);
	}

	static inline details::x509_verify_result
	cert_cv_verify(signature_scheme scheme, ::std::byte const *covered,
				   ::std::size_t covered_size, ::std::byte const *signature,
				   ::std::size_t signature_size,
				   details::algorithm_identifier const &leaf_alg,
				   ::std::byte const *leaf_key, ::std::size_t leaf_key_size) noexcept
	{
		return details::tls_certificate_verify(scheme, covered, covered_size, signature,
											   signature_size, leaf_alg, leaf_key,
											   leaf_key_size);
	}
};

} // namespace fast_io::tls

#endif /* FAST_IO_TLS_HAS_GNUTLS_CRYPTO */
