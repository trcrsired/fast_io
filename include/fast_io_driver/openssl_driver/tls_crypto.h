#pragma once

/*
ossl_crypto_backend: a fast_io::tls crypto backend whose primitives are
OpenSSL EVP calls. The TLS protocol layer -- record framing, sequence
numbers, key schedule, X.509 parse/chain/SAN -- stays in fast_io;
OpenSSL only supplies the math. Since -lssl -lcrypto is already linked
the fast_io AEAD and X25519 instantiations drop out of the binary.

The transcript hash and HKDF keep the fast_io implementations: they are
tiny constexpr table-free routines, while EVP digest contexts are heap
objects that cannot satisfy the constexpr noexcept shape the key
schedule requires. Certificate signature verify likewise stays on the
fast_io parser for now -- the ops that matter for throughput (record
AEAD) and state size (X25519) are the EVP ones.
*/

#include <openssl/evp.h>

namespace fast_io::tls
{

struct ossl_crypto_backend
{
	/* small constexpr contexts -- EVP digest handles cannot live in the
	   key schedule's noexcept machinery */
	using sha256 = ::fast_io::sha256_context;
	using sha384 = ::fast_io::sha384_context;

	static inline void x25519_keypair(::std::byte *pk, ::std::byte *sk) noexcept
	{
		EVP_PKEY *skp{EVP_PKEY_new_raw_private_key(EVP_PKEY_X25519, nullptr,
												   reinterpret_cast<char unsigned const *>(sk), 32)};
		if (skp == nullptr)
		{
			::fast_io::fast_terminate();
		}
		::std::size_t n{32};
		if (EVP_PKEY_get_raw_public_key(skp, reinterpret_cast<char unsigned *>(pk), __builtin_addressof(n)) != 1 || n != 32)
		{
			::fast_io::fast_terminate();
		}
		EVP_PKEY_free(skp);
	}

	static inline void x25519_shared_secret(::std::byte *out, ::std::byte const *peer_pk,
											::std::byte *sk) noexcept
	{
		EVP_PKEY *skp{EVP_PKEY_new_raw_private_key(EVP_PKEY_X25519, nullptr,
												   reinterpret_cast<char unsigned const *>(sk), 32)};
		EVP_PKEY *pkp{EVP_PKEY_new_raw_public_key(EVP_PKEY_X25519, nullptr,
												  reinterpret_cast<char unsigned const *>(peer_pk), 32)};
		EVP_PKEY_CTX *d{skp ? EVP_PKEY_CTX_new(skp, nullptr) : nullptr};
		if (d == nullptr || pkp == nullptr)
		{
			::fast_io::fast_terminate();
		}
		::std::size_t n{32};
		if (EVP_PKEY_derive_init(d) != 1 || EVP_PKEY_derive_set_peer(d, pkp) != 1 ||
			EVP_PKEY_derive(d, reinterpret_cast<char unsigned *>(out), __builtin_addressof(n)) != 1 ||
			n != 32)
		{
			::fast_io::fast_terminate();
		}
		EVP_PKEY_CTX_free(d);
		EVP_PKEY_free(pkp);
		EVP_PKEY_free(skp);
	}

private:
	static inline EVP_CIPHER const *ossl_tls_cipher(cipher_suite suite) noexcept
	{
		switch (suite)
		{
		case cipher_suite::aes_128_gcm_sha256:
			return EVP_aes_128_gcm();
		case cipher_suite::aes_256_gcm_sha384:
			return EVP_aes_256_gcm();
		case cipher_suite::chacha20_poly1305_sha256:
			return EVP_chacha20_poly1305();
		default:
			return nullptr;
		}
	}

	static inline ::std::size_t ossl_tls_key_size(cipher_suite suite) noexcept
	{
		return suite == cipher_suite::aes_256_gcm_sha384 || suite == cipher_suite::chacha20_poly1305_sha256
				   ? 32
				   : 16;
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

		EVP_CIPHER_CTX *x{EVP_CIPHER_CTX_new()};
		if (x == nullptr)
		{
			::fast_io::fast_terminate();
		}
		int l{}, l2{};
		bool ok{EVP_DecryptInit_ex(x, ossl_tls_cipher(suite), nullptr, nullptr, nullptr) == 1 &&
				EVP_CIPHER_CTX_ctrl(x, EVP_CTRL_AEAD_SET_IVLEN, 12, nullptr) == 1 &&
				EVP_DecryptInit_ex(x, nullptr, nullptr,
								   reinterpret_cast<char unsigned const *>(key),
								   reinterpret_cast<char unsigned const *>(nonce)) == 1 &&
				EVP_DecryptUpdate(x, nullptr, __builtin_addressof(l),
								  reinterpret_cast<char unsigned const *>(hdr5), 5) == 1 &&
				EVP_CIPHER_CTX_ctrl(x, EVP_CTRL_AEAD_SET_TAG, 16,
									const_cast<::std::byte *>(ct + inner_size)) == 1 &&
				EVP_DecryptUpdate(x, reinterpret_cast<char unsigned *>(out), __builtin_addressof(l),
								  reinterpret_cast<char unsigned const *>(ct),
								  static_cast<int>(inner_size)) == 1 &&
				EVP_DecryptFinal_ex(x, reinterpret_cast<char unsigned *>(out) + l,
									__builtin_addressof(l2)) == 1};
		EVP_CIPHER_CTX_free(x);
		if (!ok)
		{
			/* EVP reports tag mismatch and ciphertext errors the same
			   way -- either is a bad_record_mac */
			return false;
		}
		::std::size_t inner{static_cast<::std::size_t>(l)};
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

		EVP_CIPHER_CTX *x{EVP_CIPHER_CTX_new()};
		if (x == nullptr)
		{
			::fast_io::fast_terminate();
		}
		int l{}, l2{};
		::std::byte tag[16];
		bool ok{EVP_EncryptInit_ex(x, ossl_tls_cipher(suite), nullptr, nullptr, nullptr) == 1 &&
				EVP_CIPHER_CTX_ctrl(x, EVP_CTRL_AEAD_SET_IVLEN, 12, nullptr) == 1 &&
				EVP_EncryptInit_ex(x, nullptr, nullptr,
								   reinterpret_cast<char unsigned const *>(key),
								   reinterpret_cast<char unsigned const *>(nonce)) == 1 &&
				EVP_EncryptUpdate(x, nullptr, __builtin_addressof(l),
								  reinterpret_cast<char unsigned const *>(hdr), 5) == 1 &&
				EVP_EncryptUpdate(x, reinterpret_cast<char unsigned *>(p), __builtin_addressof(l),
								  reinterpret_cast<char unsigned const *>(inner),
								  static_cast<int>(inner_size)) == 1 &&
				EVP_EncryptFinal_ex(x, reinterpret_cast<char unsigned *>(p) + l,
									__builtin_addressof(l2)) == 1 &&
				EVP_CIPHER_CTX_ctrl(x, EVP_CTRL_AEAD_GET_TAG, 16,
									reinterpret_cast<char unsigned *>(tag)) == 1};
		EVP_CIPHER_CTX_free(x);
		if (!ok)
		{
			::fast_io::fast_terminate();
		}
		::std::byte *tag_out{p + l + l2};
		::fast_io::freestanding::non_overlapped_copy_n(tag, 16, tag_out);
		return static_cast<::std::size_t>(tag_out - out) + 16;
	}

	static inline ::std::size_t
	record_seal(::std::byte *out, content_type inner_type, ::std::byte const *pt,
				::std::size_t pt_size, cipher_suite suite, ::std::byte const *key,
				::std::byte const *iv, ::std::uint_least64_t seq) noexcept
	{
		::std::byte inner[16641];
		for (::std::size_t i{}; i != pt_size; ++i)
		{
			inner[i] = pt[i];
		}
		inner[pt_size] = static_cast<::std::byte>(inner_type);
		return record_seal_inner(out, inner, pt_size + 1, suite, key, iv, seq);
	}

	/* DER parsing, chain walk and SAN checks stay in fast_io -- the
	   signature math forwards to the fast_io verifier until the EVP
	   verify ops are wired in */
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
