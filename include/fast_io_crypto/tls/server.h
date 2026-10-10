#pragma once

/*
TLS 1.3 server handshake driver over a byte-stream transport.

Sequence:

  [plaintext phase]
	read ClientHello record(s) (+ optional plaintext CCS)
	require supported_versions offers 0x0304 -- no downgrade, ever
	pick a cipher suite, grab the client's x25519 share
	x25519 -> handshake secrets
  [our flight]
	write ServerHello + compat CCS in plaintext, then seal under the
	server handshake traffic key:
	  EncryptedExtensions, Certificate, CertificateVerify, Finished
  [client second flight]
	plaintext CCS; sealed client Finished under c_hs. A client that
	offered early_data may prepend early-data records we cannot
	decrypt -- those are skipped (0-RTT is not negotiated).
  [application phase]
	master secret -> c_ap (our RX) / s_ap (our TX); Linux SOL_TLS
	offload or the userspace record layer.

The session state is basic_tls_client -- its fields are
direction-neutral (tx_/rx_ keys, pending plaintext, offload flag), so
the post-handshake record pump, KeyUpdate and close_notify machinery
all serve the server unchanged; only the handshake driver differs.
*/

namespace fast_io::tls
{

struct tls_server_config
{
	/* certificate chain, leaf first, DER-encoded */
	::std::byte const *const *certs_der{};
	::std::size_t const *cert_sizes{};
	::std::size_t cert_count{};
	/* private key: PKCS#8, PKCS#1 RSAPrivateKey, or a raw ed25519
	   (32-byte seed / 64-byte expanded) -- see pkey.h */
	::std::byte const *private_key_der{};
	::std::size_t private_key_size{};
	/* kernel record offload (kTLS). false -> always the userspace
	   record AEAD; true -> still falls back when the ulp won't attach */
	bool offload{true};
};

namespace details
{

/* ---------------- ClientHello parse ---------------- */

struct server_client_hello_info
{
	::std::byte const *session_id{};
	::std::size_t session_id_size{};
	::std::byte const *x25519_public_key{}; /* 32 */
	::std::uint_least16_t cipher_suite{};
	::std::uint_least16_t sigalgs[64]{};
	::std::size_t sigalg_count{};
	bool tls13{};
	bool x25519_group{};
	bool x25519_share{};
	bool null_compression{};
	bool has_sigalgs{};
	bool early_data{};
	bool has_sni{};
};

/*
parse a ClientHello handshake body (after the 4-byte header).
legacy_version is ignored per rfc8446 4.1.2; the supported_versions
extension is the version gate. The cipher suite is negotiated here in
client preference order -- the first offered suite we implement wins.
Unknown extensions are ignored (they are legal in CH); the ones that
matter are decoded.
*/
inline constexpr bool server_client_hello_parse(server_client_hello_info &info,
												::std::byte const *body, ::std::size_t body_size) noexcept
{
	wire_reader r{body, body + body_size};
	::std::uint_least16_t legacy_version;
	::std::byte const *random;
	if (!r.take_u16(legacy_version) || !r.take_bytes(random, 32))
	{
		return false;
	}
	(void)legacy_version; /* rfc8446: ignored for all purposes */
	if (!r.take_vector8(info.session_id, info.session_id_size))
	{
		return false;
	}
	::std::byte const *suites;
	::std::size_t suites_size;
	if (!r.take_vector16(suites, suites_size))
	{
		return false;
	}
	{
		wire_reader sl{suites, suites + suites_size};
		while (!sl.empty())
		{
			::std::uint_least16_t v;
			if (!sl.take_u16(v))
			{
				return false;
			}
			if (v == static_cast<::std::uint_least16_t>(cipher_suite::aes_128_gcm_sha256) ||
				v == static_cast<::std::uint_least16_t>(cipher_suite::aes_256_gcm_sha384) ||
				v == static_cast<::std::uint_least16_t>(cipher_suite::chacha20_poly1305_sha256))
			{
				if (info.cipher_suite == 0)
				{
					info.cipher_suite = v;
				}
			}
		}
	}
	::std::byte const *compressions;
	::std::size_t compressions_size;
	if (!r.take_vector8(compressions, compressions_size))
	{
		return false;
	}
	for (::std::size_t i{}; i != compressions_size; ++i)
	{
		info.null_compression |= (compressions[i] == ::std::byte{});
	}
	::std::byte const *exts_p;
	::std::size_t exts_size;
	if (!r.take_vector16(exts_p, exts_size) || !r.empty())
	{
		return false;
	}
	wire_reader exts{exts_p, exts_p + exts_size};
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
			::std::byte const *vs;
			::std::size_t vn;
			if (!e.take_vector8(vs, vn) || !e.empty())
			{
				return false;
			}
			for (::std::size_t i{}; i + 2 <= vn; i += 2)
			{
				if ((static_cast<::std::uint_least16_t>(vs[i]) << 8u |
					 static_cast<::std::uint_least16_t>(vs[i + 1])) == protocol_version_tls13)
				{
					info.tls13 = true;
				}
			}
			break;
		}
		case extension_type::supported_groups:
		{
			::std::byte const *gs;
			::std::size_t gn;
			if (!e.take_vector16(gs, gn) || !e.empty())
			{
				return false;
			}
			for (::std::size_t i{}; i + 2 <= gn; i += 2)
			{
				if ((static_cast<::std::uint_least16_t>(gs[i]) << 8u |
					 static_cast<::std::uint_least16_t>(gs[i + 1])) ==
					static_cast<::std::uint_least16_t>(named_group::x25519))
				{
					info.x25519_group = true;
				}
			}
			break;
		}
		case extension_type::key_share:
		{
			::std::byte const *ks;
			::std::size_t kn;
			if (!e.take_vector16(ks, kn) || !e.empty())
			{
				return false;
			}
			wire_reader shares{ks, ks + kn};
			while (!shares.empty())
			{
				::std::uint_least16_t group;
				::std::byte const *key;
				::std::size_t key_size;
				if (!shares.take_u16(group) || !shares.take_vector16(key, key_size))
				{
					return false;
				}
				if (group == static_cast<::std::uint_least16_t>(named_group::x25519) &&
					key_size == 32 && !info.x25519_share)
				{
					info.x25519_share = true;
					info.x25519_public_key = key;
				}
			}
			break;
		}
		case extension_type::signature_algorithms:
		{
			::std::byte const *sa;
			::std::size_t sn;
			if (!e.take_vector16(sa, sn) || !e.empty())
			{
				return false;
			}
			info.has_sigalgs = true;
			wire_reader l{sa, sa + sn};
			while (!l.empty())
			{
				::std::uint_least16_t v;
				if (!l.take_u16(v))
				{
					return false;
				}
				if (info.sigalg_count != sizeof(info.sigalgs) / sizeof(info.sigalgs[0]))
				{
					info.sigalgs[info.sigalg_count++] = v;
				}
			}
			break;
		}
		case extension_type::server_name:
		{
			info.has_sni = true;
			break;
		}
		case extension_type::early_data:
		{
			info.early_data = true;
			break;
		}
		default:
			break; /* unrecognized CH extensions are ignored */
		}
	}
	return true;
}

/* the server's verdict on a parsed ClientHello -- the caller maps it to
   an alert (it owns the socket) */
enum class client_hello_check : ::std::uint_least8_t
{
	ok,
	downgrade,
	bad_compression,
	no_cipher_suite,
	no_key_share,
	missing_signature_algorithms,
	session_id_overflow
};

inline constexpr client_hello_check
server_client_hello_validate(server_client_hello_info const &info) noexcept
{
	if (info.session_id_size > 32)
	{
		return client_hello_check::session_id_overflow;
	}
	if (!info.tls13)
	{
		/* no supported_versions=0x0304: not a 1.3 client -- never
		   downgrade */
		return client_hello_check::downgrade;
	}
	if (!info.null_compression)
	{
		return client_hello_check::bad_compression;
	}
	if (info.cipher_suite == 0)
	{
		return client_hello_check::no_cipher_suite;
	}
	if (!info.x25519_share)
	{
		/* group advertised but no share would warrant a
		   HelloRetryRequest; for now any missing share fails */
		return client_hello_check::no_key_share;
	}
	if (!info.has_sigalgs)
	{
		/* rfc8446 4.2.3: a cert-authenticating server MUST abort when
		   signature_algorithms is absent */
		return client_hello_check::missing_signature_algorithms;
	}
	return client_hello_check::ok;
}

/* ---------------- ServerHello build ---------------- */

struct server_hello_params
{
	::std::byte const *random{};            /* 32 */
	::std::byte const *session_id{};        /* echo of the client's */
	::std::size_t session_id_size{};
	cipher_suite suite{};
	::std::byte const *x25519_public_key{}; /* our public, 32 */
};

inline constexpr ::std::size_t server_hello_size(server_hello_params const &params) noexcept
{
	return 2 /* legacy_version */ + 32 /* random */ +
		   1 + params.session_id_size /* session id echo */ +
		   2 /* cipher suite */ + 1 /* compression */ +
		   2 /* ext list len */ +
		   (4 + 2) /* supported_versions: u16 */ +
		   (4 + 2 + 2 + 32) /* key_share: group + vec + key */;
}

/* writes the ServerHello BODY (after the 4-byte handshake header) */
inline constexpr ::std::byte *server_hello_write_body(::std::byte *p, server_hello_params const &params) noexcept
{
	p = wire_put_u16(p, 0x0303); /* legacy_version */
	p = wire_put_bytes(p, params.random, 32);
	*p++ = static_cast<::std::byte>(params.session_id_size);
	p = wire_put_bytes(p, params.session_id, params.session_id_size);
	p = wire_put_u16(p, static_cast<::std::uint_least16_t>(params.suite));
	*p++ = ::std::byte{0}; /* null compression */

	::std::byte *const ext_len{p};
	p += 2;
	::std::byte *const ext_begin{p};
	{
		/* supported_versions: u16 form in ServerHello */
		p = wire_put_u16(p, static_cast<::std::uint_least16_t>(extension_type::supported_versions));
		p = wire_put_u16(p, 2);
		p = wire_put_u16(p, protocol_version_tls13);
	}
	{
		/* key_share: KeyShareEntry { group, u16 vec key } */
		p = wire_put_u16(p, static_cast<::std::uint_least16_t>(extension_type::key_share));
		p = wire_put_u16(p, 2 + 2 + 32);
		p = wire_put_u16(p, static_cast<::std::uint_least16_t>(named_group::x25519));
		p = wire_put_u16(p, 32);
		p = wire_put_bytes(p, params.x25519_public_key, 32);
	}
	wire_put_u16(ext_len, static_cast<::std::uint_least16_t>(p - ext_begin));
	return p;
}

/* assemble the whole ServerHello record; msg_out/msg_size_out report
   the handshake-message span for the transcript */
inline ::std::size_t server_hello_record_write(
	::std::byte *rec, ::std::size_t cap, server_hello_params const &params,
	::std::byte const **msg_out, ::std::size_t *msg_size_out) noexcept
{
	::std::size_t const body_size{server_hello_size(params)};
	if (body_size + 9 > cap)
	{
		return 0;
	}
	::std::byte *const msg{rec + record_header_size};
	::std::byte *const body{handshake_header_write(
		msg, handshake_type::server_hello, static_cast<::std::uint_least32_t>(body_size))};
	::std::byte *const endp{server_hello_write_body(body, params)};
	::std::size_t const msg_size{static_cast<::std::size_t>(endp - msg)};
	::std::byte *const hdr_end{record_header_write(
		rec, content_type::handshake, static_cast<::std::uint_least16_t>(msg_size))};
	*msg_out = msg;
	*msg_size_out = msg_size;
	return static_cast<::std::size_t>(hdr_end - rec) + msg_size;
}

/*
CertificateVerify scheme selection: the leaf cert's SPKI algorithm fixes
the family (ed25519 -> ed25519; rsaEncryption -> rsa_pss_rsae_*;
rsassaPss SPKI -> rsa_pss_pss_*); the client's signature_algorithms list
picks within it, in client preference order. Returns 0 when nothing
overlaps.
*/
inline constexpr ::std::uint_least16_t
tls_server_scheme_pick(der_tlv const &leaf_spki_oid,
					   ::std::uint_least16_t const *sigalgs, ::std::size_t sigalg_count) noexcept
{
	if (der_oid_eq(leaf_spki_oid, oid::ed25519))
	{
		for (::std::size_t i{}; i != sigalg_count; ++i)
		{
			if (sigalgs[i] == static_cast<::std::uint_least16_t>(signature_scheme::ed25519))
			{
				return sigalgs[i];
			}
		}
		return 0;
	}
	bool const pss_spki{der_oid_eq(leaf_spki_oid, oid::rsassa_pss)};
	if (!pss_spki && !der_oid_eq(leaf_spki_oid, oid::rsa_encryption))
	{
		return 0; /* ecdsa and friends cannot sign yet */
	}
	::std::uint_least16_t const cands[3]{
		pss_spki ? static_cast<::std::uint_least16_t>(signature_scheme::rsa_pss_pss_sha256)
				 : static_cast<::std::uint_least16_t>(signature_scheme::rsa_pss_rsae_sha256),
		pss_spki ? static_cast<::std::uint_least16_t>(signature_scheme::rsa_pss_pss_sha384)
				 : static_cast<::std::uint_least16_t>(signature_scheme::rsa_pss_rsae_sha384),
		pss_spki ? static_cast<::std::uint_least16_t>(signature_scheme::rsa_pss_pss_sha512)
				 : static_cast<::std::uint_least16_t>(signature_scheme::rsa_pss_rsae_sha512)};
	for (::std::size_t i{}; i != sigalg_count; ++i)
	{
		for (::std::size_t j{}; j != 3; ++j)
		{
			if (sigalgs[i] == cands[j])
			{
				return sigalgs[i];
			}
		}
	}
	return 0;
}

/* ---------------- the driver ---------------- */

template <typename allocator_type, typename socket_observer_type, typename crypto>
inline void tls_server_handshake(basic_tls_client<allocator_type, socket_observer_type, crypto> *client,
								 tls_server_config const *cfg) FAST_IO_HERBCEPTIONS_THROWS
{
	bool const offload{cfg->offload && details::tls_try_offload(client->sock_)};

	/* ---- read the ClientHello (plaintext records; CCS skipped) ---- */
	::std::byte const *ch_raw{};
	::std::size_t ch_raw_size{};
	::std::byte const *ch_body{};
	::std::size_t ch_body_size{};
	{
		handshake_queue q{};
		for (;;)
		{
			handshake_type mt;
			::std::byte const *body, *raw;
			::std::size_t body_size, raw_size;
			while (handshake_queue_next(__builtin_addressof(q), __builtin_addressof(mt),
										__builtin_addressof(body), __builtin_addressof(body_size),
										__builtin_addressof(raw), __builtin_addressof(raw_size)))
			{
				if (mt != handshake_type::client_hello)
				{
					/* the first flight holds exactly one message */
					details::tls_fail(client->sock_, alert_description::unexpected_message, false);
				}
				ch_raw = raw;
				ch_raw_size = raw_size;
				ch_body = body;
				ch_body_size = body_size;
				goto have_ch;
			}
			{
				::std::byte rec[17408];
				content_type ctype;
				::std::size_t const n{details::tls_read_plaintext_record(
					client->sock_, rec, sizeof(rec), __builtin_addressof(ctype))};
				switch (ctype)
				{
				case content_type::handshake:
					handshake_queue_feed(__builtin_addressof(q), rec, n);
					break;
				case content_type::change_cipher_spec:
					break; /* compat CCS before/around CH */
				case content_type::alert:
					details::tls_throw_peer_alert(rec, n);
				default:
					details::tls_fail(client->sock_, alert_description::unexpected_message, false);
				}
			}
		}
	have_ch:;
		/* keep q alive until the transcript has digested ch_raw */
		server_client_hello_info chi{};
		if (!server_client_hello_parse(chi, ch_body, ch_body_size))
		{
			details::tls_fail(client->sock_, alert_description::decode_error, false);
		}
		switch (server_client_hello_validate(chi))
		{
		case client_hello_check::ok:
			break;
		case client_hello_check::downgrade:
			details::tls_fail(client->sock_, alert_description::protocol_version, false);
		case client_hello_check::bad_compression:
			details::tls_fail(client->sock_, alert_description::illegal_parameter, false);
		case client_hello_check::missing_signature_algorithms:
			details::tls_fail(client->sock_, alert_description::missing_extension, false);
		case client_hello_check::session_id_overflow:
			details::tls_fail(client->sock_, alert_description::decode_error, false);
		default:
			details::tls_fail(client->sock_, alert_description::handshake_failure, false);
		}
		cipher_suite const suite{static_cast<cipher_suite>(chi.cipher_suite)};

		/* our own cert + key: a misconfiguration is our fault, not the
		   client's -- surface it as internal_error */
		if (cfg->cert_count == 0 || cfg->private_key_der == nullptr)
		{
			details::tls_fail(client->sock_, alert_description::internal_error, false);
		}
		::fast_io::tls::details::x509_certificate leaf{};
		if (!::fast_io::tls::details::x509_certificate_parse(leaf, cfg->certs_der[0], cfg->cert_sizes[0]))
		{
			details::tls_fail(client->sock_, alert_description::internal_error, false);
		}
		::std::uint_least16_t const scheme{
			tls_server_scheme_pick(leaf.spki_algorithm.oid, chi.sigalgs, chi.sigalg_count)};
		if (scheme == 0)
		{
			details::tls_fail(client->sock_, alert_description::handshake_failure, false);
		}

		::std::byte random[32], sk[32], pk[32];
		details::tls_fill_random(random, 32);
		details::tls_fill_random(sk, 32);
		crypto::x25519_keypair(pk, sk);

		::std::byte shared[32];
		crypto::x25519_shared_secret(shared, chi.x25519_public_key, sk);
		::fast_io::secure_clear(sk, sizeof(sk));
		if (details::tls_all_zero(shared, 32))
		{
			/* rfc8446 4.2.8.1: all-zero X25519 result aborts */
			details::tls_fail(client->sock_, alert_description::illegal_parameter, false);
		}

		typename crypto::md const md{crypto::md_for(suite)};
		::std::size_t const digest_size{crypto::md_digest_size(md)};
		::std::size_t const key_size{::fast_io::tls::details::cipher_suite_key_size(suite)};

		typename crypto::hash_ctx transcript{md};
		transcript.update(ch_raw, ch_raw + ch_raw_size);

		::fast_io::tls::details::key_schedule<crypto> ks{md};
		ks.init_early();
		ks.derive_empty();
		ks.extract_into(shared, 32);
		::fast_io::secure_clear(shared, sizeof(shared));

		::std::byte c_hs[64], s_hs[64];
		/* transcript is CH || SH once the SH below is folded */
		{
			::std::byte sh[512];
			details::server_hello_params shp{};
			shp.random = random;
			shp.session_id = chi.session_id;
			shp.session_id_size = chi.session_id_size;
			shp.suite = suite;
			shp.x25519_public_key = pk;
			::std::byte const *sh_msg{};
			::std::size_t sh_msg_size{};
			::std::size_t const rec_size{details::server_hello_record_write(
				sh, sizeof(sh), shp, __builtin_addressof(sh_msg), __builtin_addressof(sh_msg_size))};
			if (rec_size == 0)
			{
				details::tls_fail(client->sock_, alert_description::internal_error, false);
			}
			details::tls_write_full(client->sock_, sh, rec_size);
			/* compat CCS: plaintext, right after ServerHello */
			::std::byte ccs[6];
			::std::byte *p{details::record_header_write(ccs, content_type::change_cipher_spec, 1)};
			*p++ = ::std::byte{1};
			details::tls_write_full(client->sock_, ccs, static_cast<::std::size_t>(p - ccs));
			transcript.update(sh_msg, sh_msg + sh_msg_size);
		}
		::fast_io::secure_clear(random, sizeof(random));
		::fast_io::secure_clear(pk, sizeof(pk));

		ks.derive_to_ptr(c_hs, u8"c hs traffic", 12, transcript);
		ks.derive_to_ptr(s_hs, u8"s hs traffic", 12, transcript);

		::std::byte key[32], iv[12];
		::std::byte hs_tx_key[32], hs_tx_iv[12], hs_rx_key[32], hs_rx_iv[12];
		::fast_io::tls::details::traffic_key_iv_to_ptr<crypto>(md, key, key_size, iv, s_hs);
		::fast_io::freestanding::non_overlapped_copy_n(key, key_size, hs_tx_key);
		::fast_io::freestanding::non_overlapped_copy_n(iv, 12, hs_tx_iv);
		::fast_io::tls::details::traffic_key_iv_to_ptr<crypto>(md, key, key_size, iv, c_hs);
		::fast_io::freestanding::non_overlapped_copy_n(key, key_size, hs_rx_key);
		::fast_io::freestanding::non_overlapped_copy_n(iv, 12, hs_rx_iv);
		::fast_io::secure_clear(key, sizeof(key));

		/* ---- our encrypted flight: EE || Certificate || CV || Fin ----
		   built as one handshake octet stream, emitted in <=16K
		   records; the transcript folds each message as produced */
		{
			::std::size_t cert_list_size{3};
			for (::std::size_t i{}; i != cfg->cert_count; ++i)
			{
				cert_list_size += 3 + cfg->cert_sizes[i] + 2;
			}
			auto flight{
				tls_alloc_construct<::fast_io::vector<::std::byte, allocator_type>, allocator_type>(
					client->allocator_handle)};
			flight.reserve(4 + 2 + 4 + 1 + cert_list_size + 4 + 4 + 1024 + 4 + 64);
			auto emit{[&](::std::size_t n) -> ::std::byte * {
				::std::size_t const off{flight.size()};
				flight.resize(off + n);
				return flight.data() + off;
			}};
			::std::size_t transcripted{};
			auto fold{[&]() noexcept {
				transcript.update(flight.data() + transcripted, flight.data() + flight.size());
				transcripted = flight.size();
			}};

			/* EncryptedExtensions: empty extension list */
			{
				::std::byte *p{emit(handshake_header_size + 2)};
				p = handshake_header_write(p, handshake_type::encrypted_extensions, 2);
				wire_put_u16(p, 0);
			}
			fold();

			/* Certificate: empty request_context, then the DER list */
			{
				::std::byte *p{emit(handshake_header_size + 1 + 3 + cert_list_size - 3)};
				p = handshake_header_write(
					p, handshake_type::certificate,
					static_cast<::std::uint_least32_t>(1 + cert_list_size));
				*p++ = ::std::byte{0}; /* no certificate_request_context */
				p = wire_put_u24(p, static_cast<::std::uint_least32_t>(cert_list_size - 3));
				for (::std::size_t i{}; i != cfg->cert_count; ++i)
				{
					p = wire_put_u24(p, static_cast<::std::uint_least32_t>(cfg->cert_sizes[i]));
					p = wire_put_bytes(p, cfg->certs_der[i], cfg->cert_sizes[i]);
					p = wire_put_u16(p, 0); /* no per-entry extensions */
				}
			}
			fold();

			/* CertificateVerify over Hash(CH..Certificate) */
			{
				::std::byte cv_transcript[64];
				::fast_io::tls::details::transcript_digest_to_ptr<crypto>(transcript, cv_transcript);
				::std::byte covered[::fast_io::tls::details::certificate_verify_content_prefix_size + 64];
				::fast_io::tls::details::certificate_verify_content_write(covered, cv_transcript, digest_size);
				::std::byte salt[64];
				details::tls_fill_random(salt, sizeof(salt));
				::std::byte sig[1024];
				::std::size_t sig_size{};
				if (!crypto::cert_cv_sign(static_cast<signature_scheme>(scheme), covered,
										  ::fast_io::tls::details::certificate_verify_content_prefix_size + digest_size,
										  cfg->private_key_der, cfg->private_key_size,
										  salt, sig, __builtin_addressof(sig_size)))
				{
					details::tls_fail(client->sock_, alert_description::internal_error, false);
				}
				::fast_io::secure_clear(salt, sizeof(salt));
				::std::byte *p{emit(handshake_header_size + 2 + 2 + sig_size)};
				p = handshake_header_write(
					p, handshake_type::certificate_verify,
					static_cast<::std::uint_least32_t>(2 + 2 + sig_size));
				p = wire_put_u16(p, scheme);
				p = wire_put_u16(p, static_cast<::std::uint_least16_t>(sig_size));
				p = wire_put_bytes(p, sig, sig_size);
				::fast_io::secure_clear(sig, sizeof(sig));
			}
			fold();

			/* server Finished: verify_data MACs CH..CV */
			{
				::std::byte fin[64];
				::fast_io::tls::details::finished_verify_data_to_ptr<crypto>(md, fin, s_hs, transcript);
				::std::byte *p{emit(handshake_header_size + digest_size)};
				p = handshake_header_write(
					p, handshake_type::finished, static_cast<::std::uint_least32_t>(digest_size));
				p = wire_put_bytes(p, fin, digest_size);
			}
			fold();

			/* ship the flight: <=16K inner plaintext per record */
			::std::byte rec[details::record_header_size + 16641 + 16];
			::std::uint_least64_t hs_tx_seq{};
			for (::std::size_t off{}; off != flight.size();)
			{
				::std::size_t const chunk{
					flight.size() - off < 16384 ? flight.size() - off : 16384};
				::std::size_t const rec_size{crypto::record_seal(
					rec, content_type::handshake, flight.data() + off, chunk,
					suite, hs_tx_key, hs_tx_iv, hs_tx_seq++)};
				details::tls_write_full(client->sock_, rec, rec_size);
				off += chunk;
			}
		}

		/* ---- master + application secrets: transcript is CH..serverFin ---- */
		ks.derive_empty();
		{
			::std::byte zero[64]{};
			ks.extract_into(zero, digest_size);
		}
		::std::byte c_ap[64], s_ap[64];
		ks.derive_to_ptr(c_ap, u8"c ap traffic", 12, transcript);
		ks.derive_to_ptr(s_ap, u8"s ap traffic", 12, transcript);

		/* ---- the client's second flight: CCS, then {Finished} ---- */
		{
			handshake_queue fq{};
			::std::byte recbuf[17408];
			::std::uint_least64_t hs_rx_seq{};
			::std::size_t early_skipped{};
			for (;;)
			{
				handshake_type mt;
				::std::byte const *body, *raw;
				::std::size_t body_size, raw_size;
				while (handshake_queue_next(__builtin_addressof(fq), __builtin_addressof(mt),
											__builtin_addressof(body), __builtin_addressof(body_size),
											__builtin_addressof(raw), __builtin_addressof(raw_size)))
				{
					switch (mt)
					{
					case handshake_type::end_of_early_data:
						if (!chi.early_data)
						{
							details::tls_fail(client->sock_, alert_description::unexpected_message, false);
						}
						break; /* dropped -- 0-RTT was not negotiated */
					case handshake_type::finished:
					{
						if (body_size != digest_size)
						{
							details::tls_fail(client->sock_, alert_description::decode_error, false);
						}
						/* the client MACs CH..server Finished -- exactly
						   the running transcript now */
						::std::byte expect[64];
						::fast_io::tls::details::finished_verify_data_to_ptr<crypto>(
							md, expect, c_hs, transcript);
						bool same{true};
						for (::std::size_t i{}; i != digest_size; ++i)
						{
							same &= (body[i] == expect[i]);
						}
						if (!same)
						{
							details::tls_fail(client->sock_, alert_description::decrypt_error, false);
						}
						goto established;
					}
					default:
						details::tls_fail(client->sock_, alert_description::unexpected_message, false);
					}
				}
				content_type inner{};
				::std::size_t inner_size{};
				if (!details::tls_recv_flight_record<crypto>(
						client->sock_, recbuf, sizeof(recbuf),
						__builtin_addressof(inner), __builtin_addressof(inner_size),
						suite, hs_rx_key, hs_rx_iv, hs_rx_seq))
				{
					if (chi.early_data && early_skipped != 8)
					{
						/* undecryptable records before the client's
						   c_hs epoch are early data -- skip, do not
						   advance the sequence */
						++early_skipped;
						continue;
					}
					details::tls_fail(client->sock_, alert_description::bad_record_mac, false);
				}
				if (inner != content_type::change_cipher_spec)
				{
					if (inner_size == 0)
					{
						continue;
					}
					++hs_rx_seq;
				}
				switch (inner)
				{
				case content_type::handshake:
					handshake_queue_feed(__builtin_addressof(fq), recbuf, inner_size);
					break;
				case content_type::change_cipher_spec:
					break;
				case content_type::alert:
					details::tls_throw_peer_alert(recbuf, inner_size);
				default:
					details::tls_fail(client->sock_, alert_description::unexpected_message, false);
				}
			}
		established:;
		}

		/* ---- epoch switch: TX <- s_ap, RX <- c_ap ---- */
		details::app_traffic_key_iv tx_ki{}, rx_ki{};
		::fast_io::tls::details::traffic_key_iv_to_ptr<crypto>(md, key, key_size, iv, s_ap);
		::fast_io::freestanding::non_overlapped_copy_n(key, key_size, tx_ki.key);
		::fast_io::freestanding::non_overlapped_copy_n(iv, 12, tx_ki.iv);
		if (offload)
		{
			details::ktls_offload_key(client->sock_, details::tls_tx, suite, key, iv);
		}
		::fast_io::tls::details::traffic_key_iv_to_ptr<crypto>(md, key, key_size, iv, c_ap);
		::fast_io::freestanding::non_overlapped_copy_n(key, key_size, rx_ki.key);
		::fast_io::freestanding::non_overlapped_copy_n(iv, 12, rx_ki.iv);
		if (offload)
		{
			details::ktls_offload_key(client->sock_, details::tls_rx, suite, key, iv);
		}
		::fast_io::secure_clear(key, sizeof(key));
		::fast_io::secure_clear(iv, sizeof(iv));
		::std::byte tx_secret[48], rx_secret[48];
		::fast_io::freestanding::non_overlapped_copy_n(s_ap, digest_size, tx_secret);
		::fast_io::freestanding::non_overlapped_copy_n(c_ap, digest_size, rx_secret);
		tls_client_set_established(client, suite, tx_secret, rx_secret, digest_size, offload, tx_ki, rx_ki);
		::fast_io::secure_clear(tx_secret, sizeof(tx_secret));
		::fast_io::secure_clear(rx_secret, sizeof(rx_secret));
		::fast_io::secure_clear(&tx_ki, sizeof(tx_ki));
		::fast_io::secure_clear(&rx_ki, sizeof(rx_ki));
		::fast_io::secure_clear(c_ap, sizeof(c_ap));
		::fast_io::secure_clear(s_ap, sizeof(s_ap));
		::fast_io::secure_clear(c_hs, sizeof(c_hs));
		::fast_io::secure_clear(s_hs, sizeof(s_hs));
		::fast_io::secure_clear(hs_tx_key, sizeof(hs_tx_key));
		::fast_io::secure_clear(hs_rx_key, sizeof(hs_rx_key));
		::fast_io::secure_clear(hs_tx_iv, sizeof(hs_tx_iv));
		::fast_io::secure_clear(hs_rx_iv, sizeof(hs_rx_iv));
		::fast_io::secure_clear(&ks, sizeof(ks));
		::fast_io::secure_clear(&transcript, sizeof(transcript));
	}
}
} // namespace details



/* fully-configured server handshake: caller supplies the cert chain and
   private key -- the same basic_tls_io_observer entry point the client
   config uses, overloaded on the config type */
template <::std::integral ch_type, typename allocator_type, typename socket_observer_type, typename crypto>
inline void handshake_define(basic_tls_io_observer<ch_type, allocator_type, socket_observer_type, crypto> tob,
							 tls_server_config const &cfg) FAST_IO_HERBCEPTIONS_THROWS
{
	details::tls_server_handshake(tob.handle, __builtin_addressof(cfg));
}

} // namespace fast_io::tls
