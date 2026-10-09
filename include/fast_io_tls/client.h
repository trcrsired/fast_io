#pragma once

/*
TLS 1.3 client handshake driver on top of Linux kTLS.

Sequence:

  [plaintext phase]
	write ClientHello record
	read ServerHello (+ optional plaintext CCS) record(s)
	require supported_versions == 0x0304   -- no downgrade, ever
	x25519 -> handshake secrets
	setsockopt RX = s_hs_traffic key      -- kernel decrypts from here
  [encrypted handshake phase]
	recvmsg records (kernel decrypts; cmsg reports real content type):
	  EncryptedExtensions, [CertificateRequest], Certificate,
	  CertificateVerify, Finished         -- transcript-hashed raw
	verify cert chain + SAN hostname + CV signature + Finished MAC
  [switchover]
	plaintext CCS out
	setsockopt TX = c_hs_traffic key
	sendmsg [empty Certificate if CR] + Finished (record-type cmsg)
	setsockopt TX = c_ap_traffic, RX = s_ap_traffic (new epoch, seq 0)
  [application phase]
	read/write plaintext; alerts/NST/KeyUpdate arrive via cmsg
*/

#if defined(__linux__)

#include "../fast_io_dsal/vector.h"

namespace fast_io::tls
{

/* thrown as a value when the handshake aborts; alert is what we sent
   (or would have sent before the record keys existed) */
struct handshake_error
{
	alert_description alert;
	::std::uint_least32_t where; /* diagnostic stage below */
};

namespace details
{

/* the herbceptions error channel needs a domain singleton; our code
   packs (alert_description << 16) | stage */
inline bool tls_error_domain_equivalent(::std::size_t code,
										::std::error_domain_singleton const *other,
										::std::size_t other_code) noexcept;

inline ::std::errc tls_error_domain_to_errc(::std::size_t) noexcept;

inline void tls_error_domain_query(::std::size_t, ::std::error_query_information kind,
								   ::std::error_reporter_encoding, void *cookie,
								   ::std::error_reporter_io_cookie_function emit) noexcept;

inline constexpr ::std::error_domain_singleton tls_error_domain_singleton{
	nullptr, tls_error_domain_equivalent, tls_error_domain_query, tls_error_domain_to_errc, nullptr};

inline bool tls_error_domain_equivalent(::std::size_t code,
										::std::error_domain_singleton const *other,
										::std::size_t other_code) noexcept
{
	return other == &tls_error_domain_singleton && code == other_code;
}

inline ::std::errc tls_error_domain_to_errc(::std::size_t) noexcept
{
	return ::std::errc::protocol_error;
}

inline void tls_error_domain_query(::std::size_t, ::std::error_query_information kind,
								   ::std::error_reporter_encoding, void *cookie,
								   ::std::error_reporter_io_cookie_function emit) noexcept
{
	char8_t const *name{u8"fast_io::tls"};
	char8_t const *msg{u8"tls handshake failure"};
	::std::io_scatter_t const sc{kind == ::std::error_query_information::name ? name : msg,
								 kind == ::std::error_query_information::name ? ::std::size_t{12} : ::std::size_t{22}};
	emit(cookie, &sc, 1);
}

} // namespace details

} // namespace fast_io::tls

namespace std
{

/* registers handshake_error as throwable via `throw throws` */
template <>
class error_domain<::fast_io::tls::handshake_error>
{
public:
	using errc_type = ::fast_io::tls::handshake_error;
	static inline constexpr ::std::error_domain_singleton const *domain() noexcept
	{
		return &::fast_io::tls::details::tls_error_domain_singleton;
	}
	static inline constexpr ::std::size_t code(errc_type e) noexcept
	{
		return (static_cast<::std::size_t>(e.alert) << 16u) | static_cast<::std::size_t>(e.where);
	}
};

} // namespace std

namespace fast_io::tls
{

inline constexpr ::std::uint_least32_t hs_stage_send_ch{1};
inline constexpr ::std::uint_least32_t hs_stage_recv_sh{2};
inline constexpr ::std::uint_least32_t hs_stage_parse_sh{3};
inline constexpr ::std::uint_least32_t hs_stage_sh_version{4};
inline constexpr ::std::uint_least32_t hs_stage_sh_echo{5};
inline constexpr ::std::uint_least32_t hs_stage_sh_suite{6};
inline constexpr ::std::uint_least32_t hs_stage_sh_keyshare{7};
inline constexpr ::std::uint_least32_t hs_stage_x25519{8};
inline constexpr ::std::uint_least32_t hs_stage_flight2{9};
inline constexpr ::std::uint_least32_t hs_stage_cert{10};
inline constexpr ::std::uint_least32_t hs_stage_chain{11};
inline constexpr ::std::uint_least32_t hs_stage_hostname{12};
inline constexpr ::std::uint_least32_t hs_stage_cv{13};
inline constexpr ::std::uint_least32_t hs_stage_fin{14};
inline constexpr ::std::uint_least32_t hs_stage_send_fin{15};
inline constexpr ::std::uint_least32_t hs_stage_app{16};

struct tls13_client_config
{
	char8_t const *hostname{};
	::std::size_t hostname_size{};
	/* DER-encoded trust anchors */
	::std::byte const *const *roots{};
	::std::size_t const *root_sizes{};
	::std::size_t root_count{};
	::std::int_least64_t now{};
	bool check_hostname{true};
	bool check_chain{true};
};

/* parsed+stored peer chain */
struct peer_certificates
{
	::fast_io::vector<::std::byte> storage{};
	::std::size_t offsets[16]{};
	::std::size_t sizes[16]{};
	::std::size_t count{};
};

class ktls_client
{
	int fd_{-1};
	cipher_suite suite_{};
	/* current application traffic secrets, needed for KeyUpdate rekey */
	::std::byte tx_secret_[48]{};
	::std::byte rx_secret_[48]{};
	::std::size_t secret_size_{};
	bool established_{};

	inline void key_update_received(::std::uint_least8_t request) FAST_IO_HERBCEPTIONS_THROWS;

public:
	inline constexpr ktls_client() noexcept = default;
	inline explicit constexpr ktls_client(int fd) noexcept : fd_{fd}
	{}

	inline constexpr int fd() const noexcept
	{
		return fd_;
	}
	inline constexpr cipher_suite suite() const noexcept
	{
		return suite_;
	}
	inline constexpr bool established() const noexcept
	{
		return established_;
	}

	inline void handshake(tls13_client_config const &cfg) FAST_IO_HERBCEPTIONS_THROWS;

	/* application phase */
	inline ::std::size_t read_some(::std::byte *buf, ::std::size_t buf_size) FAST_IO_HERBCEPTIONS_THROWS;
	inline void write_all(::std::byte const *buf, ::std::size_t buf_size) FAST_IO_HERBCEPTIONS_THROWS;
	inline void send_close_notify() noexcept;

	/* filled by handshake internals */
	inline void set_established(cipher_suite suite,
								::std::byte const *tx_secret, ::std::byte const *rx_secret,
								::std::size_t secret_size) noexcept
	{
		suite_ = suite;
		secret_size_ = secret_size;
		::fast_io::freestanding::non_overlapped_copy_n(tx_secret, secret_size, tx_secret_);
		::fast_io::freestanding::non_overlapped_copy_n(rx_secret, secret_size, rx_secret_);
		established_ = true;
	}
};

namespace details
{

inline void tls13_send_alert(int fd, alert_description desc, bool tx_offloaded) noexcept
{
	/* best-effort; nothing to do if this fails too */
	::std::byte const level{desc == alert_description::close_notify ? ::std::byte{1} : ::std::byte{2}};
	::std::byte body[2]{level, static_cast<::std::byte>(desc)};
	if (tx_offloaded)
	{
		try
		{
			details::ktls_send_record(fd, content_type::alert, body, 2);
		}
		catch throws(::std::error)
		{
		}
		return;
	}
	::std::byte rec[7];
	::std::byte *p{details::record_header_write(rec, content_type::alert, 2)};
	p = wire_put_bytes(p, body, 2);
	try
	{
		details::tls_write_full(fd, rec, static_cast<::std::size_t>(p - rec));
	}
	catch throws(::std::error)
	{
	}
}

[[noreturn]] inline void tls13_fail(int fd, alert_description desc, ::std::uint_least32_t where,
									bool tx_offloaded) FAST_IO_HERBCEPTIONS_THROWS
{
	tls13_send_alert(fd, desc, tx_offloaded);
	throw throws handshake_error{desc, where};
}

/*
read one plaintext record (pre-offload phase). Returns payload size;
ctype gets the record type.
*/
inline ::std::size_t tls13_read_plaintext_record(int fd, ::std::byte *buf, ::std::size_t buf_cap,
												 content_type &ctype) FAST_IO_HERBCEPTIONS_THROWS
{
	::std::byte hdr[record_header_size];
	details::tls_read_full(fd, hdr, record_header_size);
	wire_reader h{hdr, hdr + record_header_size};
	::std::uint_least8_t t;
	::std::uint_least16_t ver, len;
	if (!h.take_u8(t) || !h.take_u16(ver) || !h.take_u16(len))
	{
		details::tls13_fail(fd, alert_description::decode_error, hs_stage_recv_sh, false);
	}
	(void)ver;
	if (len > buf_cap)
	{
		details::tls13_fail(fd, alert_description::record_overflow, hs_stage_recv_sh, false);
	}
	details::tls_read_full(fd, buf, len);
	ctype = static_cast<content_type>(t);
	return len;
}

/*
handshake message accumulator: handshake octets may straddle records.
feed() appends record plaintext; next() pulls complete [hdr|body]
messages for transcript hashing.
*/
struct handshake_queue
{
	::fast_io::vector<::std::byte> pending{};
	::std::size_t consumed{};

	inline void feed(::std::byte const *data, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	{
		if (consumed != 0)
		{
			::fast_io::freestanding::overlapped_copy_n(pending.data() + consumed,
													   pending.size() - consumed, pending.data());
			pending.resize(pending.size() - consumed);
			consumed = 0;
		}
		::std::size_t const old{pending.size()};
		pending.resize(old + n);
		::fast_io::freestanding::non_overlapped_copy_n(data, n, pending.data() + old);
		if (pending.size() > (1u << 20u)) /* 1MB handshake cap */
		{
			::fast_io::throw_posix_error(EPROTO);
		}
	}

	inline bool next(handshake_type &type, ::std::byte const *&body, ::std::size_t &body_size,
					 ::std::byte const *&raw, ::std::size_t &raw_size) noexcept
	{
		::std::byte const *const p{pending.data() + consumed};
		::std::size_t const avail{pending.size() - consumed};
		if (avail < handshake_header_size)
		{
			return false;
		}
		wire_reader r{p, p + avail};
		::std::uint_least8_t t;
		::std::uint_least32_t len;
		if (!r.take_u8(t) || !r.take_u24(len))
		{
			return false;
		}
		if (len > (1u << 20u) || avail < handshake_header_size + len)
		{
			return false;
		}
		type = static_cast<handshake_type>(t);
		raw = p;
		raw_size = handshake_header_size + len;
		body = p + handshake_header_size;
		body_size = len;
		consumed += raw_size;
		return true;
	}
};

/* Certificate body: u8 request_context || u24 cert_list of
   {u24 der || u16 extensions}. DERs are copied into peer.storage. */
inline bool certificate_body_parse(peer_certificates &peer,
								   ::std::byte const *body, ::std::size_t body_size) noexcept
{
	wire_reader r{body, body + body_size};
	::std::byte const *ctx;
	::std::size_t ctx_size;
	::std::byte const *list;
	::std::size_t list_size;
	if (!r.take_vector8(ctx, ctx_size) || !r.take_vector24(list, list_size) || !r.empty())
	{
		return false;
	}
	wire_reader l{list, list + list_size};
	while (!l.empty())
	{
		::std::byte const *der;
		::std::size_t der_size;
		::std::byte const *ext;
		::std::size_t ext_size;
		if (!l.take_vector24(der, der_size) || !l.take_vector16(ext, ext_size))
		{
			return false;
		}
		if (peer.count == 16 || der_size == 0)
		{
			return false;
		}
		::std::size_t const off{peer.storage.size()};
		peer.storage.resize(off + der_size);
		::fast_io::freestanding::non_overlapped_copy_n(der, der_size, peer.storage.data() + off);
		peer.offsets[peer.count] = off;
		peer.sizes[peer.count] = der_size;
		++peer.count;
	}
	return true;
}

inline ::std::uint_least8_t alert_level_of(::std::byte const *alert_body, ::std::size_t n,
										   alert_description &desc) noexcept
{
	if (n != 2)
	{
		desc = alert_description::decode_error;
		return 2;
	}
	desc = static_cast<alert_description>(alert_body[1]);
	return static_cast<::std::uint_least8_t>(alert_body[0]);
}

} // namespace details

/*
the encrypted server flight + client second flight. ch_msg/sh_msg are
the raw handshake octets (with 4-byte headers) already exchanged.
tx_secret_out/rx_secret_out receive the application traffic secrets for
later KeyUpdate rekey.

Transcript rule (rfc8446 4.4.1): CertificateVerify's covered content
hashes CH..Certificate; server Finished's verify_data MACs CH..CV. Both
exclude the message carrying them, so the running hash is snapshotted
before each message is folded in.
*/
template <typename hash_ctx>
inline void ktls_handshake_flight2(int fd, cipher_suite suite,
								   ::std::byte const *shared_secret,
								   ::std::byte const *ch_msg, ::std::size_t ch_msg_size,
								   ::std::byte const *sh_msg, ::std::size_t sh_msg_size,
								   tls13_client_config const &cfg,
								   peer_certificates &peer,
								   ::std::byte *tx_secret_out, ::std::byte *rx_secret_out,
								   ::std::size_t &secret_size_out) FAST_IO_HERBCEPTIONS_THROWS
{
	constexpr ::std::size_t digest_size{hash_ctx::digest_size};
	secret_size_out = digest_size;

	hash_ctx transcript{};
	transcript.update(ch_msg, ch_msg + ch_msg_size);
	transcript.update(sh_msg, sh_msg + sh_msg_size);

	::fast_io::tls::details::key_schedule<hash_ctx> ks{};
	ks.init_early();
	ks.derive_empty();
	ks.extract_into(shared_secret, 32); /* handshake secret */

	::std::byte c_hs[digest_size], s_hs[digest_size];
	ks.derive_to_ptr(c_hs, u8"c hs traffic", 12, transcript);
	ks.derive_to_ptr(s_hs, u8"s hs traffic", 12, transcript);

	::std::byte key[32], iv[12];
	::std::size_t const key_size{::fast_io::tls::details::cipher_suite_key_size(suite)};
	::fast_io::tls::details::traffic_key_iv_to_ptr<hash_ctx>(key, key_size, iv, s_hs);
	details::ktls_set_key(fd, details::tls_rx, suite, key, iv, 0); /* next RX record uses s_hs */
	::fast_io::secure_clear(key, sizeof(key));

	enum : ::std::uint_least8_t
	{
		want_ee,
		want_cert_or_cr,
		want_cv,
		want_fin,
		flight_done
	} state{want_ee};
	bool cert_request_seen{};
	::std::byte cr_context[255];
	::std::size_t cr_context_size{};
	signature_scheme cv_scheme{};
	::fast_io::vector<::std::byte> cv_sig{};
	::std::byte cv_transcript[digest_size]; /* Hash(CH..Certificate) */

	details::handshake_queue q{};
	::std::byte recbuf[17408];
	for (;;)
	{
		handshake_type mt;
		::std::byte const *body, *raw;
		::std::size_t body_size, raw_size;
		while (q.next(mt, body, body_size, raw, raw_size))
		{
			/* snapshot BEFORE this msg: CV covered and Fin MAC hash only
			   the preceding handshake messages */
			hash_ctx pre{transcript};
			switch (mt)
			{
			case handshake_type::encrypted_extensions:
				if (state != want_ee)
				{
					details::tls13_fail(fd, alert_description::unexpected_message, hs_stage_flight2, true);
				}
				{
					wire_reader ee{body, body + body_size};
					wire_reader exts;
					if (!ee.take_sub16(exts) || !ee.empty())
					{
						details::tls13_fail(fd, alert_description::decode_error, hs_stage_flight2, true);
					}
					while (!exts.empty())
					{
						::std::uint_least16_t et;
						::std::byte const *ep;
						::std::size_t en;
						if (!exts.take_u16(et) || !exts.take_vector16(ep, en))
						{
							details::tls13_fail(fd, alert_description::decode_error, hs_stage_flight2, true);
						}
						/* these are SH-only extensions (rfc8446 4.2) */
						if (et == static_cast<::std::uint_least16_t>(extension_type::key_share) ||
							et == static_cast<::std::uint_least16_t>(extension_type::supported_versions) ||
							et == static_cast<::std::uint_least16_t>(extension_type::pre_shared_key))
						{
							details::tls13_fail(fd, alert_description::illegal_parameter, hs_stage_flight2, true);
						}
					}
					state = want_cert_or_cr;
				}
				break;
			case handshake_type::certificate_request:
				if (state != want_cert_or_cr)
				{
					details::tls13_fail(fd, alert_description::unexpected_message, hs_stage_flight2, true);
				}
				{
					wire_reader cr{body, body + body_size};
					::std::byte const *ctx;
					::std::size_t ctx_size;
					if (!cr.take_vector8(ctx, ctx_size))
					{
						details::tls13_fail(fd, alert_description::decode_error, hs_stage_flight2, true);
					}
					::fast_io::freestanding::non_overlapped_copy_n(ctx, ctx_size, cr_context);
					cr_context_size = ctx_size;
					cert_request_seen = true;
				}
				break;
			case handshake_type::certificate:
				if (state != want_cert_or_cr)
				{
					details::tls13_fail(fd, alert_description::unexpected_message, hs_stage_cert, true);
				}
				if (!details::certificate_body_parse(peer, body, body_size))
				{
					details::tls13_fail(fd, alert_description::decode_error, hs_stage_cert, true);
				}
				if (peer.count == 0)
				{
					/* empty client-visible chain is an abort in 1.3 */
					details::tls13_fail(fd, alert_description::bad_certificate, hs_stage_cert, true);
				}
				state = want_cv;
				break;
			case handshake_type::certificate_verify:
			{
				if (state != want_cv)
				{
					details::tls13_fail(fd, alert_description::unexpected_message, hs_stage_cv, true);
				}
				::fast_io::tls::details::certificate_verify_info cvi;
				if (!::fast_io::tls::details::certificate_verify_parse(cvi, body, body_size))
				{
					details::tls13_fail(fd, alert_description::decode_error, hs_stage_cv, true);
				}
				cv_scheme = cvi.scheme;
				cv_sig.assign(cvi.signature_size, {});
				::fast_io::freestanding::non_overlapped_copy_n(cvi.signature, cvi.signature_size, cv_sig.data());
				/* pre holds Hash(CH..Certificate) -- what the CV signs */
				::fast_io::tls::details::transcript_digest_to_ptr(pre, cv_transcript);
				state = want_fin;
				break;
			}
			case handshake_type::finished:
			{
				if (state != want_fin || body_size != digest_size)
				{
					details::tls13_fail(fd, alert_description::decode_error, hs_stage_fin, true);
				}
				/* pre holds Hash(CH..CV) -- what server Finished MACs */
				::std::byte expect[digest_size];
				::fast_io::tls::details::finished_verify_data_to_ptr<hash_ctx>(expect, s_hs, pre);
				bool same{true};
				for (::std::size_t i{}; i != digest_size; ++i)
				{
					same &= (body[i] == expect[i]);
				}
				if (!same)
				{
					details::tls13_fail(fd, alert_description::decrypt_error, hs_stage_fin, true);
				}
				state = flight_done;
				break;
			}
			default:
				details::tls13_fail(fd, alert_description::unexpected_message, hs_stage_flight2, true);
			}
			transcript.update(raw, raw + raw_size); /* fold in after processing */
		}
		if (state == flight_done)
		{
			break;
		}
		details::ktls_recv_result rr{details::ktls_recv_record(fd, recbuf, sizeof(recbuf))};
		switch (rr.ctype)
		{
		case content_type::handshake:
			if (rr.size != 0)
			{
				q.feed(recbuf, rr.size);
			}
			break;
		case content_type::change_cipher_spec:
			break; /* compat CCS between epochs; never transcripted */
		case content_type::alert:
		{
			alert_description desc;
			(void)details::alert_level_of(recbuf, rr.size, desc);
			throw throws handshake_error{desc, hs_stage_flight2};
		}
		default:
			details::tls13_fail(fd, alert_description::unexpected_message, hs_stage_flight2, true);
		}
	}

	/* ---- verify the certificate chain + leaf hostname ---- */
	::fast_io::tls::details::x509_certificate presented[16];
	for (::std::size_t i{}; i != peer.count; ++i)
	{
		if (!::fast_io::tls::details::x509_certificate_parse(
				presented[i], peer.storage.data() + peer.offsets[i], peer.sizes[i]))
		{
			details::tls13_fail(fd, alert_description::bad_certificate, hs_stage_cert, true);
		}
	}
	if (cfg.check_chain)
	{
		::fast_io::tls::details::x509_certificate roots[16];
		::std::size_t const nroots{cfg.root_count < 16 ? cfg.root_count : 16};
		for (::std::size_t i{}; i != nroots; ++i)
		{
			if (!::fast_io::tls::details::x509_certificate_parse(roots[i], cfg.roots[i], cfg.root_sizes[i]))
			{
				details::tls13_fail(fd, alert_description::bad_certificate, hs_stage_chain, true);
			}
		}
		switch (::fast_io::tls::details::x509_chain_verify(presented, peer.count, roots, nroots, cfg.now))
		{
		case ::fast_io::tls::details::x509_chain_result::ok:
			break;
		case ::fast_io::tls::details::x509_chain_result::expired:
		case ::fast_io::tls::details::x509_chain_result::not_yet_valid:
			details::tls13_fail(fd, alert_description::certificate_expired, hs_stage_chain, true);
		case ::fast_io::tls::details::x509_chain_result::untrusted:
			details::tls13_fail(fd, alert_description::unknown_ca, hs_stage_chain, true);
		default:
			details::tls13_fail(fd, alert_description::bad_certificate, hs_stage_chain, true);
		}
	}
	if (cfg.check_hostname && cfg.hostname_size &&
		!::fast_io::tls::details::x509_hostname_match(presented[0], cfg.hostname, cfg.hostname_size))
	{
		details::tls13_fail(fd, alert_description::bad_certificate, hs_stage_hostname, true);
	}

	/* ---- CertificateVerify over covered content ---- */
	{
		constexpr ::std::size_t covered_size{
			::fast_io::tls::details::certificate_verify_content_prefix_size + digest_size};
		::std::byte covered[covered_size];
		::fast_io::tls::details::certificate_verify_content_write<hash_ctx>(covered, cv_transcript);
		switch (::fast_io::tls::details::tls_certificate_verify(
			cv_scheme, covered, covered_size, cv_sig.data(), cv_sig.size(),
			presented[0].spki_algorithm, presented[0].public_key, presented[0].public_key_size))
		{
		case ::fast_io::tls::details::x509_verify_result::ok:
			break;
		case ::fast_io::tls::details::x509_verify_result::unsupported_algorithm:
			details::tls13_fail(fd, alert_description::illegal_parameter, hs_stage_cv, true);
		default:
			details::tls13_fail(fd, alert_description::decrypt_error, hs_stage_cv, true);
		}
	}

	/* ---- master secret + application traffic secrets ---- */
	ks.derive_empty();
	::std::byte zero[digest_size]{};
	ks.extract_into(zero, digest_size); /* master secret */

	::std::byte c_ap[digest_size], s_ap[digest_size];
	ks.derive_to_ptr(c_ap, u8"c ap traffic", 13, transcript);
	ks.derive_to_ptr(s_ap, u8"s ap traffic", 13, transcript);

	/* ---- client second flight ---- */

	/* compat CCS: still plaintext -- must go out BEFORE the TX key is
	   installed or the kernel would encrypt it as a handshake record */
	{
		::std::byte ccs[6];
		::std::byte *p{details::record_header_write(ccs, content_type::change_cipher_spec, 1)};
		*p++ = ::std::byte{1};
		details::tls_write_full(fd, ccs, static_cast<::std::size_t>(p - ccs));
	}
	::fast_io::tls::details::traffic_key_iv_to_ptr<hash_ctx>(key, key_size, iv, c_hs);
	details::ktls_set_key(fd, details::tls_tx, suite, key, iv, 0);
	::fast_io::secure_clear(key, sizeof(key));

	::std::byte flight[4 + 255 + 3 + 4 + 64];
	::std::byte *fp{flight};
	if (cert_request_seen)
	{
		/* empty Certificate echoing the request_context */
		fp = details::handshake_header_write(fp, handshake_type::certificate,
											 static_cast<::std::uint_least32_t>(1 + cr_context_size + 3));
		*fp++ = static_cast<::std::byte>(cr_context_size);
		fp = wire_put_bytes(fp, cr_context, cr_context_size);
		fp = wire_put_u24(fp, 0); /* empty certificate_list */
		transcript.update(flight, fp);
	}
	::std::byte fin[digest_size];
	::fast_io::tls::details::finished_verify_data_to_ptr<hash_ctx>(fin, c_hs, transcript);
	::std::byte *const fin_start{fp};
	fp = details::handshake_header_write(fp, handshake_type::finished, digest_size);
	fp = wire_put_bytes(fp, fin, digest_size);
	details::ktls_send_record(fd, content_type::handshake, flight, static_cast<::std::size_t>(fp - flight));
	transcript.update(fin_start, fp);

	/* epoch switch: TX -> c_ap (after Finished is on the wire), RX -> s_ap */
	::fast_io::tls::details::traffic_key_iv_to_ptr<hash_ctx>(key, key_size, iv, c_ap);
	details::ktls_set_key(fd, details::tls_tx, suite, key, iv, 0);
	::fast_io::tls::details::traffic_key_iv_to_ptr<hash_ctx>(key, key_size, iv, s_ap);
	details::ktls_set_key(fd, details::tls_rx, suite, key, iv, 0);
	::fast_io::secure_clear(key, sizeof(key));
	::fast_io::secure_clear(iv, sizeof(iv));

	::fast_io::freestanding::non_overlapped_copy_n(c_ap, digest_size, tx_secret_out);
	::fast_io::freestanding::non_overlapped_copy_n(s_ap, digest_size, rx_secret_out);
}

} // namespace fast_io::tls

namespace fast_io::tls::details
{

/*
dispatch the post-SH handshake on the hash implied by the cipher suite.
Returns the negotiated suite's hash size via secret_size_out.
*/
inline void ktls_handshake_dispatch(int fd, cipher_suite suite,
									::std::byte const *shared_secret,
									::std::byte const *ch_msg, ::std::size_t ch_msg_size,
									::std::byte const *sh_msg, ::std::size_t sh_msg_size,
									tls13_client_config const &cfg,
									peer_certificates &peer,
									::std::byte *tx_secret_out, ::std::byte *rx_secret_out,
									::std::size_t &secret_size_out) FAST_IO_HERBCEPTIONS_THROWS
{
	if (suite == cipher_suite::aes_256_gcm_sha384)
	{
		ktls_handshake_flight2<::fast_io::sha384_context>(fd, suite, shared_secret,
														  ch_msg, ch_msg_size, sh_msg, sh_msg_size,
														  cfg, peer, tx_secret_out, rx_secret_out, secret_size_out);
	}
	else
	{
		ktls_handshake_flight2<::fast_io::sha256_context>(fd, suite, shared_secret,
														  ch_msg, ch_msg_size, sh_msg, sh_msg_size,
														  cfg, peer, tx_secret_out, rx_secret_out, secret_size_out);
	}
}

} // namespace fast_io::tls::details

namespace fast_io::tls
{

inline void ktls_client::handshake(tls13_client_config const &cfg) FAST_IO_HERBCEPTIONS_THROWS
{
	::std::byte session_id[32], random[32], sk[32], pk[32];
	details::tls_fill_random(session_id, 32);
	details::tls_fill_random(random, 32);
	details::tls_fill_random(sk, 32);
	::fast_io::diffie_hellman::x25519::trim_secret_key(
		::fast_io::containers::index_span<::std::byte, 32>{::fast_io::freestanding::from_range, sk});
	::fast_io::diffie_hellman::x25519::calculate_public_key_to_ptr(pk, sk);

	::fast_io::tls::details::client_hello_params params{};
	params.hostname = cfg.hostname;
	params.hostname_size = cfg.hostname_size;
	params.session_id = session_id;
	params.random = random;
	params.x25519_public_key = pk;

	::std::byte ch[2048];
	::std::size_t const body_size{::fast_io::tls::details::client_hello_size(params)};
	if (body_size + 9 > sizeof(ch))
	{
		throw throws handshake_error{alert_description::internal_error, hs_stage_send_ch};
	}
	::std::byte *const msg{ch + 5};
	::std::byte *const body{::fast_io::tls::details::handshake_header_write(
		msg, handshake_type::client_hello, static_cast<::std::uint_least32_t>(body_size))};
	::std::byte *const endp{::fast_io::tls::details::client_hello_write_body(body, params)};
	::std::size_t const ch_msg_size{static_cast<::std::size_t>(endp - msg)};
	::std::byte *hdr{ch};
	hdr = ::fast_io::tls::details::record_header_write(hdr, content_type::handshake,
													   static_cast<::std::uint_least16_t>(ch_msg_size));
	::std::size_t const rec_size{static_cast<::std::size_t>(hdr - ch) + ch_msg_size};
	try
	{
		details::tls_write_full(fd_, ch, rec_size);
	}
	catch throws(::std::error)
	{
		throw throws handshake_error{alert_description::internal_error, hs_stage_send_ch};
	}

	/* ---- read ServerHello (+ maybe plaintext CCS) ---- */
	::std::byte sh_msg[4096];
	::std::size_t sh_msg_size{};
	{
		::std::byte rec[8192];
		for (;;)
		{
			content_type ctype;
			::std::size_t const n{details::tls13_read_plaintext_record(fd_, rec, sizeof(rec), ctype)};
			if (ctype == content_type::change_cipher_spec)
			{
				continue;
			}
			if (ctype == content_type::alert)
			{
				alert_description desc;
				(void)details::alert_level_of(rec, n, desc);
				throw throws handshake_error{desc, hs_stage_recv_sh};
			}
			if (ctype != content_type::handshake || n < 4 || n > sizeof(sh_msg))
			{
				details::tls13_fail(fd_, alert_description::unexpected_message, hs_stage_recv_sh, false);
			}
			::fast_io::freestanding::non_overlapped_copy_n(rec, n, sh_msg);
			sh_msg_size = n;
			break;
		}
	}
	::std::byte const *sh_body{sh_msg + 4};
	::std::size_t sh_body_size{sh_msg_size - 4};
	if (sh_msg[0] != static_cast<::std::byte>(handshake_type::server_hello) ||
		sh_msg_size < 4 ||
		((static_cast<::std::size_t>(sh_msg[1]) << 16u) |
		 (static_cast<::std::size_t>(sh_msg[2]) << 8u) |
		 static_cast<::std::size_t>(sh_msg[3])) != sh_body_size)
	{
		details::tls13_fail(fd_, alert_description::decode_error, hs_stage_parse_sh, false);
	}
	::fast_io::tls::details::server_hello_info shi{};
	if (!::fast_io::tls::details::server_hello_parse(shi, sh_body, sh_body_size, session_id, 32))
	{
		details::tls13_fail(fd_, alert_description::decode_error, hs_stage_parse_sh, false);
	}
	if (shi.is_hello_retry_request || shi.has_pre_shared_key)
	{
		/* we only offer x25519 -- nothing to retry with */
		details::tls13_fail(fd_, alert_description::handshake_failure, hs_stage_parse_sh, false);
	}
	if (!shi.supported_versions_tls13 || shi.downgrade_sentinel_seen)
	{
		/* absolutely no downgrade: not 1.2, not anything else */
		details::tls13_fail(fd_, alert_description::protocol_version, hs_stage_sh_version, false);
	}
	if (!shi.session_id_echo_match)
	{
		details::tls13_fail(fd_, alert_description::illegal_parameter, hs_stage_sh_echo, false);
	}
	cipher_suite const suite{static_cast<cipher_suite>(shi.cipher_suite)};
	if (suite != cipher_suite::aes_128_gcm_sha256 &&
		suite != cipher_suite::aes_256_gcm_sha384 &&
		suite != cipher_suite::chacha20_poly1305_sha256)
	{
		details::tls13_fail(fd_, alert_description::handshake_failure, hs_stage_sh_suite, false);
	}
	if (shi.key_share_group != named_group::x25519 ||
		shi.key_share_public_key_size != 32)
	{
		details::tls13_fail(fd_, alert_description::illegal_parameter, hs_stage_sh_keyshare, false);
	}

	::std::byte shared[32];
	::fast_io::diffie_hellman::x25519::create_shared_key_to_ptr(
		shared, shi.key_share_public_key, sk);
	::fast_io::secure_clear(sk, sizeof(sk));
	{
		/* rfc8446 4.2.8.1: all-zero X25519 result aborts */
		::std::byte acc{};
		for (::std::size_t i{}; i != 32; ++i)
		{
			acc |= shared[i];
		}
		if (acc == ::std::byte{})
		{
			details::tls13_fail(fd_, alert_description::illegal_parameter, hs_stage_x25519, false);
		}
	}

	peer_certificates peer{};
	::std::byte tx_secret[48], rx_secret[48];
	::std::size_t secret_size{};
	details::ktls_handshake_dispatch(fd_, suite, shared,
									 msg, ch_msg_size, sh_msg, sh_msg_size,
									 cfg, peer, tx_secret, rx_secret, secret_size);
	::fast_io::secure_clear(shared, sizeof(shared));
	set_established(suite, tx_secret, rx_secret, secret_size);
	::fast_io::secure_clear(tx_secret, sizeof(tx_secret));
	::fast_io::secure_clear(rx_secret, sizeof(rx_secret));
}

/*
post-handshake record pump. KeyUpdate rotates the direction's traffic
secret and reinstalls the kernel key (new epoch, seq 0); NST is dropped;
alerts map: close_notify -> 0 return (eof), else throw the alert.
*/
inline ::std::size_t ktls_client::read_some(::std::byte *buf, ::std::size_t buf_size) FAST_IO_HERBCEPTIONS_THROWS
{
	for (;;)
	{
		auto rr{details::ktls_recv_record(fd_, buf, buf_size)};
		switch (rr.ctype)
		{
		case content_type::application_data:
			return rr.size;
		case content_type::change_cipher_spec:
			break;
		case content_type::alert:
		{
			if (rr.size == 2 && buf[0] == ::std::byte{1} && buf[1] == ::std::byte{0})
			{
				return 0;
			}
			alert_description desc;
			(void)details::alert_level_of(buf, rr.size, desc);
			throw throws handshake_error{desc, hs_stage_app};
		}
		case content_type::handshake:
			/* post-handshake: walk each msg; KeyUpdate -> rekey, NST -> drop */
			{
				wire_reader r{buf, buf + rr.size};
				while (!r.empty())
				{
					::std::uint_least8_t mt;
					::std::uint_least32_t mlen;
					if (!r.take_u8(mt) || !r.take_u24(mlen) || r.remaining() < mlen)
					{
						break; /* split msg; kernel keeps records atomic -- malformed */
					}
					::std::byte const *body{r.cur};
					r.cur += mlen;
					if (static_cast<handshake_type>(mt) == handshake_type::key_update && mlen == 1)
					{
						key_update_received(static_cast<::std::uint_least8_t>(body[0]));
					}
					/* new_session_ticket and friends: not a resumption client -- drop */
				}
			}
			break;
		default:
			break;
		}
	}
}

inline void ktls_client::key_update_received(::std::uint_least8_t request) FAST_IO_HERBCEPTIONS_THROWS
{
	/* next = HKDF-Expand-Label(secret, "traffic upd", "", hash_size) */
	::std::byte next[48];
	::std::byte key[32], iv[12];
	::std::size_t const key_size{::fast_io::tls::details::cipher_suite_key_size(suite_)};
	if (secret_size_ == ::fast_io::sha384_context::digest_size)
	{
		::fast_io::tls::details::hkdf_expand_label_to_ptr<::fast_io::sha384_context>(
			next, secret_size_, rx_secret_, u8"traffic upd", 11, nullptr, 0);
		::fast_io::freestanding::non_overlapped_copy_n(next, secret_size_, rx_secret_);
		::fast_io::tls::details::traffic_key_iv_to_ptr<::fast_io::sha384_context>(key, key_size, iv, rx_secret_);
	}
	else
	{
		::fast_io::tls::details::hkdf_expand_label_to_ptr<::fast_io::sha256_context>(
			next, secret_size_, rx_secret_, u8"traffic upd", 11, nullptr, 0);
		::fast_io::freestanding::non_overlapped_copy_n(next, secret_size_, rx_secret_);
		::fast_io::tls::details::traffic_key_iv_to_ptr<::fast_io::sha256_context>(key, key_size, iv, rx_secret_);
	}
	::fast_io::tls::details::ktls_set_key(fd_, details::tls_rx, suite_, key, iv, 0);
	if (request != 0)
	{
		/* peer asked us to rotate TX too: send our KeyUpdate then rekey */
		::std::byte ku[5];
		::std::byte *p{details::handshake_header_write(ku, handshake_type::key_update, 1)};
		*p++ = ::std::byte{0}; /* update_not_requested */
		::fast_io::tls::details::ktls_send_record(fd_, content_type::handshake, ku,
												  static_cast<::std::size_t>(p - ku));
		if (secret_size_ == ::fast_io::sha384_context::digest_size)
		{
			::fast_io::tls::details::hkdf_expand_label_to_ptr<::fast_io::sha384_context>(
				next, secret_size_, tx_secret_, u8"traffic upd", 11, nullptr, 0);
			::fast_io::freestanding::non_overlapped_copy_n(next, secret_size_, tx_secret_);
			::fast_io::tls::details::traffic_key_iv_to_ptr<::fast_io::sha384_context>(key, key_size, iv, tx_secret_);
		}
		else
		{
			::fast_io::tls::details::hkdf_expand_label_to_ptr<::fast_io::sha256_context>(
				next, secret_size_, tx_secret_, u8"traffic upd", 11, nullptr, 0);
			::fast_io::freestanding::non_overlapped_copy_n(next, secret_size_, tx_secret_);
			::fast_io::tls::details::traffic_key_iv_to_ptr<::fast_io::sha256_context>(key, key_size, iv, tx_secret_);
		}
		::fast_io::tls::details::ktls_set_key(fd_, details::tls_tx, suite_, key, iv, 0);
	}
	::fast_io::secure_clear(key, sizeof(key));
	::fast_io::secure_clear(iv, sizeof(iv));
}

inline void ktls_client::write_all(::std::byte const *buf, ::std::size_t buf_size) FAST_IO_HERBCEPTIONS_THROWS
{
	/* kernel wraps plaintext into application_data records itself */
	details::tls_write_full(fd_, buf, buf_size);
}

inline void ktls_client::send_close_notify() noexcept
{
	details::tls13_send_alert(fd_, alert_description::close_notify, true);
}

} // namespace fast_io::tls

#endif
