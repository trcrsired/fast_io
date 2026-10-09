#pragma once

/*
TLS 1.3 handshake wire messages: ClientHello builder and parsers for the
server flight. Only TLS 1.3 is negotiated -- supported_versions carries
0x0304 alone and a ServerHello without a supported_versions=0x0304
extension is rejected (no downgrade).

All hashes feed the transcript over the raw handshake octets INCLUDING
the 4-byte handshake header.
*/

namespace fast_io::tls::details
{

/* record header: u8 type || u16 legacy_version || u16 length */
inline constexpr ::std::size_t record_header_size{5};

inline constexpr ::std::byte *record_header_write(::std::byte *p, content_type type, ::std::uint_least16_t length) noexcept
{
	*p++ = static_cast<::std::byte>(type);
	/* legacy_record_version: 0x0303 for everything except the first CH (0x0301 ok too; 0x0303 is fine) */
	p = wire_put_u16(p, 0x0303);
	return wire_put_u16(p, length);
}

/* handshake header: u8 msg_type || u24 length */
inline constexpr ::std::size_t handshake_header_size{4};

inline constexpr ::std::byte *handshake_header_write(::std::byte *p, handshake_type type, ::std::uint_least32_t length) noexcept
{
	*p++ = static_cast<::std::byte>(type);
	return wire_put_u24(p, length);
}

/*
HelloRetryRequest magic: a ServerHello whose random is exactly this is
an HRR, not a real SH (rfc8446 4.1.4).
*/
inline constexpr ::std::byte hello_retry_request_random[32]{
	::std::byte{0xCF}, ::std::byte{0x21}, ::std::byte{0xAD}, ::std::byte{0x74}, ::std::byte{0xE5}, ::std::byte{0x9A}, ::std::byte{0x61}, ::std::byte{0x11},
	::std::byte{0xBE}, ::std::byte{0x1D}, ::std::byte{0x8C}, ::std::byte{0x02}, ::std::byte{0x1E}, ::std::byte{0x65}, ::std::byte{0xB8}, ::std::byte{0x91},
	::std::byte{0xC2}, ::std::byte{0xA2}, ::std::byte{0x11}, ::std::byte{0x16}, ::std::byte{0x7A}, ::std::byte{0xBB}, ::std::byte{0x8C}, ::std::byte{0x5E},
	::std::byte{0x07}, ::std::byte{0x9E}, ::std::byte{0x09}, ::std::byte{0xE2}, ::std::byte{0xC8}, ::std::byte{0xA8}, ::std::byte{0x33}, ::std::byte{0x9C}};

/*
downgrade sentinels a TLS<=1.2-negotiating server puts in random[24..32]
(rfc8446 4.1.3). We never accept <1.3 so this is belt-and-braces: the
supported_versions check below is the real gate.
*/
inline constexpr ::std::byte downgrade_sentinel_tls12[8]{
	::std::byte{0x44}, ::std::byte{0x4F}, ::std::byte{0x57}, ::std::byte{0x4E},
	::std::byte{0x47}, ::std::byte{0x52}, ::std::byte{0x44}, ::std::byte{0x01}};
inline constexpr ::std::byte downgrade_sentinel_tls11_or_below[8]{
	::std::byte{0x44}, ::std::byte{0x4F}, ::std::byte{0x57}, ::std::byte{0x4E},
	::std::byte{0x47}, ::std::byte{0x52}, ::std::byte{0x44}, ::std::byte{0x00}};

struct client_hello_params
{
	char8_t const *hostname{};
	::std::size_t hostname_size{};
	::std::byte const *session_id{};
	::std::size_t session_id_size{32};      /* 32 bytes; middlebox compat echo */
	::std::byte const *random{};            /* 32 bytes */
	::std::byte const *x25519_public_key{}; /* 32 bytes */
	bool offer_aes_128_gcm{true};
	bool offer_aes_256_gcm{true};
	bool offer_chacha20_poly1305{true};
};

/*
signature schemes the client can verify in CertificateVerify /
certificates. ecdsa_secp256r1_sha256 is intentionally absent until the
P-256 verifier lands -- do not advertise what we cannot verify.
*/
inline constexpr ::std::uint_least16_t supported_signature_schemes[]{
	0x0804 /* rsa_pss_rsae_sha256 */, 0x0805 /* rsa_pss_rsae_sha384 */,
	0x0806 /* rsa_pss_rsae_sha512 */, 0x0807 /* ed25519 */,
	0x0809 /* rsa_pss_pss_sha256 */, 0x080a /* rsa_pss_pss_sha384 */,
	0x080b /* rsa_pss_pss_sha512 */,
	0x0401 /* rsa_pkcs1_sha256 (certs only) */, 0x0501 /* rsa_pkcs1_sha384 (certs only) */,
	0x0601 /* rsa_pkcs1_sha512 (certs only) */};

inline constexpr ::std::size_t client_hello_size(client_hello_params const &params) noexcept
{
	::std::size_t const n_suites{static_cast<::std::size_t>(params.offer_aes_128_gcm) +
								 static_cast<::std::size_t>(params.offer_aes_256_gcm) +
								 static_cast<::std::size_t>(params.offer_chacha20_poly1305)};
	::std::size_t const sni_bytes{params.hostname_size ? 4 + (2 + 1 + 2 + params.hostname_size) : 0};
	return 2 /* legacy_version */ + 32 /* random */ + 1 + params.session_id_size /* session id */ +
		   2 + n_suites * 2 /* cipher suites */ + 2 /* compression */ +
		   2 /* ext list len */ +
		   sni_bytes +
		   (4 + 3) /* supported_versions: u8 vec { 0x0304 } */ +
		   (4 + 4) /* supported_groups: u16 vec { x25519 } */ +
		   (4 + 2 + 2 + 2 + 32) /* key_share: vec of {group, u16 vec} */ +
		   (4 + 2 + sizeof(supported_signature_schemes)); /* signature_algorithms */
}

/* writes the ClientHello BODY (after the 4-byte handshake header) */
inline constexpr ::std::byte *client_hello_write_body(::std::byte *p, client_hello_params const &params) noexcept
{
	p = wire_put_u16(p, 0x0303); /* legacy_version */
	p = wire_put_bytes(p, params.random, 32);
	*p++ = static_cast<::std::byte>(params.session_id_size);
	p = wire_put_bytes(p, params.session_id, params.session_id_size);
	::std::byte *const suites_len{p};
	p += 2;
	::std::uint_least16_t suites_bytes{};
	auto offer{[&](cipher_suite cs) noexcept {
		p = wire_put_u16(p, static_cast<::std::uint_least16_t>(cs));
		suites_bytes = static_cast<::std::uint_least16_t>(suites_bytes + 2u);
	}};
	if (params.offer_aes_128_gcm)
	{
		offer(cipher_suite::aes_128_gcm_sha256);
	}
	if (params.offer_aes_256_gcm)
	{
		offer(cipher_suite::aes_256_gcm_sha384);
	}
	if (params.offer_chacha20_poly1305)
	{
		offer(cipher_suite::chacha20_poly1305_sha256);
	}
	wire_put_u16(suites_len, suites_bytes);
	*p++ = ::std::byte{1};
	*p++ = ::std::byte{0}; /* null compression only */

	::std::byte *const ext_len{p};
	p += 2;
	::std::byte *const ext_begin{p};
	auto ext{[&](extension_type et, ::std::size_t body_size) noexcept -> ::std::byte * {
		p = wire_put_u16(p, static_cast<::std::uint_least16_t>(et));
		p = wire_put_u16(p, static_cast<::std::uint_least16_t>(body_size));
		return p;
	}};

	if (params.hostname_size)
	{
		::std::byte *q{ext(extension_type::server_name, 2 + 1 + 2 + params.hostname_size)};
		q = wire_put_u16(q, static_cast<::std::uint_least16_t>(1 + 2 + params.hostname_size));
		*q++ = ::std::byte{0}; /* host_name */
		q = wire_put_u16(q, static_cast<::std::uint_least16_t>(params.hostname_size));
		::fast_io::details::non_overlapped_copy_n(params.hostname, params.hostname_size,
												  reinterpret_cast<char8_t *>(q));
		p = q + params.hostname_size;
	}
	{
		::std::byte *q{ext(extension_type::supported_versions, 3)};
		/* TLS 1.3 only -- offering anything else would permit downgrade */
		*q++ = ::std::byte{2};
		q = wire_put_u16(q, protocol_version_tls13);
		p = q;
	}
	{
		::std::byte *q{ext(extension_type::supported_groups, 4)};
		q = wire_put_u16(q, 2);
		q = wire_put_u16(q, static_cast<::std::uint_least16_t>(named_group::x25519));
		p = q;
	}
	{
		::std::byte *q{ext(extension_type::key_share, 2 + 2 + 2 + 32)};
		q = wire_put_u16(q, 2 + 2 + 32); /* client_shares length */
		q = wire_put_u16(q, static_cast<::std::uint_least16_t>(named_group::x25519));
		q = wire_put_u16(q, 32);
		q = wire_put_bytes(q, params.x25519_public_key, 32);
		p = q;
	}
	{
		constexpr ::std::size_t sigalg_bytes{2 + sizeof(supported_signature_schemes)};
		::std::byte *q{ext(extension_type::signature_algorithms, sigalg_bytes)};
		q = wire_put_u16(q, static_cast<::std::uint_least16_t>(sizeof(supported_signature_schemes)));
		for (::std::uint_least16_t s : supported_signature_schemes)
		{
			q = wire_put_u16(q, s);
		}
		p = q;
	}
	wire_put_u16(ext_len, static_cast<::std::uint_least16_t>(p - ext_begin));
	return p;
}

struct server_hello_info
{
	::std::uint_least16_t cipher_suite{};
	::std::byte const *key_share_public_key{};
	::std::size_t key_share_public_key_size{};
	named_group key_share_group{};
	bool supported_versions_tls13{};
	bool session_id_echo_match{};
	bool is_hello_retry_request{};
	bool downgrade_sentinel_seen{};
	bool has_pre_shared_key{};
};

/*
Parse a ServerHello handshake body (after the 4-byte header).
session_id is our CH session id, 32 bytes -- the echo must match.
Returns false on malformed input. The caller rejects if
!supported_versions_tls13 (downgrade / non-1.3 server).
*/
inline constexpr bool server_hello_parse(server_hello_info &info,
										 ::std::byte const *body, ::std::size_t body_size,
										 ::std::byte const *session_id, ::std::size_t session_id_size) noexcept
{
	wire_reader r{body, body + body_size};
	::std::uint_least16_t legacy_version;
	::std::byte const *random;
	if (!r.take_u16(legacy_version) || !r.take_bytes(random, 32))
	{
		return false;
	}
	if (legacy_version != 0x0303)
	{
		return false;
	}
	bool same{true};
	for (::std::size_t i{}; i != 32; ++i)
	{
		same &= (random[i] == hello_retry_request_random[i]);
	}
	info.is_hello_retry_request = same;
	same = true;
	for (::std::size_t i{}; i != 8; ++i)
	{
		same &= (random[24 + i] == downgrade_sentinel_tls12[i]);
	}
	info.downgrade_sentinel_seen = same;
	same = true;
	for (::std::size_t i{}; i != 8; ++i)
	{
		same &= (random[24 + i] == downgrade_sentinel_tls11_or_below[i]);
	}
	info.downgrade_sentinel_seen |= same;

	::std::byte const *sid;
	::std::size_t sid_size;
	if (!r.take_vector8(sid, sid_size))
	{
		return false;
	}
	info.session_id_echo_match = sid_size == session_id_size;
	for (::std::size_t i{}; info.session_id_echo_match && i != sid_size; ++i)
	{
		info.session_id_echo_match = (sid[i] == session_id[i]);
	}
	if (!r.take_u16(info.cipher_suite))
	{
		return false;
	}
	::std::uint_least8_t compression;
	if (!r.take_u8(compression) || compression != 0)
	{
		return false;
	}
	wire_reader exts;
	if (!r.take_sub16(exts))
	{
		return false;
	}
	while (!exts.empty())
	{
		::std::uint_least16_t et;
		::std::byte const *ep;
		::std::size_t en;
		if (!exts.take_u16(et) || !exts.take_vector16(ep, en))
		{
			return false;
		}
		wire_reader e{ep, ep + en};
		switch (static_cast<extension_type>(et))
		{
		case extension_type::supported_versions:
		{
			::std::uint_least16_t v;
			if (!e.take_u16(v) || !e.empty())
			{
				return false;
			}
			info.supported_versions_tls13 = (v == protocol_version_tls13);
			break;
		}
		case extension_type::key_share:
		{
			::std::uint_least16_t group;
			if (!e.take_u16(group))
			{
				return false;
			}
			info.key_share_group = static_cast<named_group>(group);
			if (!e.take_vector16(info.key_share_public_key, info.key_share_public_key_size) || !e.empty())
			{
				return false;
			}
			break;
		}
		case extension_type::pre_shared_key:
			info.has_pre_shared_key = true;
			break;
		default:
			/* unknown extension in SH is not fatal per se, but a strict
			   client rejects anything it didn't offer */
			return false;
		}
	}
	return true;
}

struct certificate_verify_info
{
	signature_scheme scheme{};
	::std::byte const *signature{};
	::std::size_t signature_size{};
};

inline constexpr bool certificate_verify_parse(certificate_verify_info &info,
											   ::std::byte const *body, ::std::size_t body_size) noexcept
{
	wire_reader r{body, body + body_size};
	::std::uint_least16_t scheme;
	if (!r.take_u16(scheme) || !r.take_vector16(info.signature, info.signature_size))
	{
		return false;
	}
	info.scheme = static_cast<signature_scheme>(scheme);
	return true;
}

/*
the covered content the CertificateVerify signature signs:
64 x 0x20 || "TLS 1.3, server CertificateVerify" || 0x00 || transcript
*/
inline constexpr ::std::size_t certificate_verify_content_prefix_size{98};

template <typename ctx>
inline constexpr void certificate_verify_content_write(::std::byte *out, ::std::byte const *transcript_digest) noexcept
{
	for (::std::size_t i{}; i != 64; ++i)
	{
		out[i] = ::std::byte{0x20};
	}
	char8_t const *ctx_str{u8"TLS 1.3, server CertificateVerify"};
	::fast_io::details::non_overlapped_copy_n(ctx_str, 33, reinterpret_cast<char8_t *>(out + 64));
	out[97] = ::std::byte{0};
	::fast_io::details::non_overlapped_copy_n(transcript_digest, ctx::digest_size, out + 98);
}

} // namespace fast_io::tls::details
