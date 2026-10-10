#pragma once

/*
crypto primitive provider for basic_tls_client. The protocol machinery
-- record framing, sequence numbers, key schedule, X.509 parse, chain
and SAN checks -- always stays in fast_io; the backend only supplies the
math primitives. Substituting a provider (OpenSSL EVP, GnuTLS, Windows
CNG/bcrypt) swaps the algorithm implementations while the protocol,
kTLS offload and async paths are untouched; the provider's library is
already linked, so fast_io's AES/SHA/X25519/RSA instantiations drop out
of the binary.

A backend supplies:

	sha256 / sha384        context types (digest_size, update,
						   do_final, digest_to_byte_ptr) -- the
						   transcript and HKDF run over these
	x25519_keypair         clamp sk[32], write pk[32]
	x25519_shared_secret   X25519(sk, peer_pk) -> out[32]; the caller
						   checks the all-zero result
	record_open/seal       suite-dispatched AEAD on TLS 1.3 records
	cert_sig_verify        cert.signature over cert.tbs under the
						   issuer SPKI -- DER parsing stays ours,
						   only the signature math delegates
	cert_cv_verify         the CertificateVerify signature over the
						   transcript
*/

namespace fast_io::tls
{

struct fast_io_crypto_backend
{
	using sha256 = ::fast_io::sha256_context;
	using sha384 = ::fast_io::sha384_context;

	static inline void x25519_keypair(::std::byte *pk, ::std::byte *sk) noexcept
	{
		::fast_io::diffie_hellman::x25519::trim_secret_key(
			::fast_io::containers::index_span<::std::byte, 32>{
				::fast_io::containers::index_unchecked, sk});
		::fast_io::diffie_hellman::x25519::calculate_public_key_to_ptr(pk, sk);
	}

	static inline void x25519_shared_secret(::std::byte *out, ::std::byte const *peer_pk,
											::std::byte *sk) noexcept
	{
		::fast_io::diffie_hellman::x25519::create_shared_key_to_ptr(out, peer_pk, sk);
	}

	static inline bool record_open(::std::byte *out, ::std::size_t &out_size,
								   content_type &inner_type, ::std::byte const *hdr5,
								   ::std::byte const *ct, ::std::size_t ct_size,
								   cipher_suite suite, ::std::byte const *key,
								   ::std::byte const *iv, ::std::uint_least64_t seq) noexcept
	{
		return details::tls13_record_open(out, out_size, inner_type, hdr5, ct,
										  ct_size, suite, key, iv, seq);
	}

	static inline ::std::size_t
	record_seal_inner(::std::byte *out, ::std::byte const *inner, ::std::size_t inner_size,
					  cipher_suite suite, ::std::byte const *key, ::std::byte const *iv,
					  ::std::uint_least64_t seq) noexcept
	{
		return details::tls13_record_seal_inner(out, inner, inner_size, suite, key, iv, seq);
	}

	static inline ::std::size_t
	record_seal(::std::byte *out, content_type inner_type, ::std::byte const *pt,
				::std::size_t pt_size, cipher_suite suite, ::std::byte const *key,
				::std::byte const *iv, ::std::uint_least64_t seq) noexcept
	{
		return details::tls13_record_seal(out, inner_type, pt, pt_size, suite, key, iv, seq);
	}

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
