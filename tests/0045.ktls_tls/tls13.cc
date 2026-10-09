#include <fast_io.h>
#include <fast_io_crypto.h>

/*
offline TLS 1.3 unit checks: pem/base64, DER/x509 parse + chain verify
+ SAN hostname matching, RFC5869 HKDF vectors, wire encode/decode.
The live kTLS handshake needs a socket + the kernel tls module, so it
lives in a separate example; run it with openssl s_server.
*/

namespace fi = ::fast_io;

/* throwaway openssl-generated test chain (localhost leaf <- fast_io Test CA).
   PEM text exercises pem_next_block + pem_decode_block. */
inline constexpr char8_t srv_pem[]{u8R"(-----BEGIN CERTIFICATE-----
MIIDFDCCAfygAwIBAgIUF1QG8GyxDIhuH3AoD93TNo4HM7MwDQYJKoZIhvcNAQEL
BQAwGjEYMBYGA1UEAwwPZmFzdF9pbyBUZXN0IENBMB4XDTI2MTAwOTE2NTM1NFoX
DTI2MTAxMTE2NTM1NFowFDESMBAGA1UEAwwJbG9jYWxob3N0MIIBIjANBgkqhkiG
9w0BAQEFAAOCAQ8AMIIBCgKCAQEA0tPopwCrVYwmNGO7VfKUui48tvY5YKsZIkmt
Si+U9UsPFsT+dRYQf/2/QadmH2X3fiOK5HLS9C95qlIfbQs3HsUHFCGvX+IV1a5/
2rfj+2WdqMdFi2KsfAcDWYBZ5wy6lh1uhaG+CeLmTOLKnAXo9ArH2YprUpAZGWKi
Dc4EZOuC/awshhVuwVUFwB5gWAhzi4OQaQj8JQWLRAJ0UBZvDnNTUs2vH0yMtms/
hs3Ku0SwhV4yo9Zp3QDZFcQhH8oGHYDF4nnrY2FmuLXUJRNNNgUJjQYGxmcS0SZU
pFggwoMHkVOV1owllYD0zgBCgzMe7u0gNlMSxoChnQpgkeF2ywIDAQABo1gwVjAU
BgNVHREEDTALgglsb2NhbGhvc3QwHQYDVR0OBBYEFJTH62y53WJncrRIz3L390zB
+lgjMB8GA1UdIwQYMBaAFA21XCoBn/QFhmq4atAD3FzdwLtoMA0GCSqGSIb3DQEB
CwUAA4IBAQBjpaYoTHd3a9SFcE8/oCL6JY8Etw75sQ3W4V6bqdwtnyNi1Y1t4Obp
oTLPot01Z8WaZwRI3cFSNaFO+0UFL4mQzbD+5LMXujUfnNu6Up+N9zqs8qk5mQwa
uSCxymhJq+7wgjMpxo9XSCZBbcQSGTOhhfGDxP/UbHufi8uh5GaRC/WURmxoRit1
J4LUf88hlq2+TdNfKAoeXtUrGpi0Xc5KWRbd1UNz5qWXyFFNoPfAr1YIG7qypw+/
/UABNhFy3vVC5g3UdXb+Kdk1gDaShUWnTCjX3Ql7oSGxFBhHMcYQDlCqag/s8u4z
nV5XAoYeacoiaGcmWBxA7PKpOTvRcO4X
-----END CERTIFICATE-----
)"};

inline constexpr char8_t ca_pem[]{u8R"(-----BEGIN CERTIFICATE-----
MIIDFTCCAf2gAwIBAgIUfW3CKvgyxoFolT4eiBATiwH6LWcwDQYJKoZIhvcNAQEL
BQAwGjEYMBYGA1UEAwwPZmFzdF9pbyBUZXN0IENBMB4XDTI2MTAwOTE2NTM1NFoX
DTI2MTAxMTE2NTM1NFowGjEYMBYGA1UEAwwPZmFzdF9pbyBUZXN0IENBMIIBIjAN
BgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAv3WPACAzhHNrOTybXIx4SGYDJaA6
JboaiRMoFpjKvmFUL1/WEPSTuIs7uqkykSVsREr/FZCXw9hWVRdzsMSd1nQk0P1E
2i8LfQE1OwNI/yA00v8su8wD+BSX4X2+UwHDJO2EeT6HZWBz8Gc8uSLDblLo/YRI
Z9Dwags/RjmOridQGXus2Wtnyb4nNsFpwdbsWotdqAFp/Jvj2wwoHpHXuxPGUzDz
NaVGahxV0R9cVHc22pTJbfld8IdfMffM4Zp9x+p0md5jz6jXmLyvQX7+rPXkK5CR
xDLKRGkkMrDhP+zY6poWhX9ytwz5cXIrIe6RDXPKPEdCFr15SxKzSh3rwwIDAQAB
o1MwUTAdBgNVHQ4EFgQUDbVcKgGf9AWGarhq0APcXN3Au2gwHwYDVR0jBBgwFoAU
DbVcKgGf9AWGarhq0APcXN3Au2gwDwYDVR0TAQH/BAUwAwEB/zANBgkqhkiG9w0B
AQsFAAOCAQEAerKn4dmXeoq/t5HGr/CoXiX1mbmd2rkwiLB8vqevl8RcGbnwX44g
ehbbxlOhNfGluawLuyIxITtiTgRVOkZ0+QvvBih48cuTdz3UQQT6fDflY/SYSxcH
krCXbb+q6Dx0CQ13sbbn+nPhTw4KIeB8PiiLEUYVE68zVkpQZi11y/DKwWDMjkBL
scaYfB8lpoMijK4lx+5sUGJE5AtRUJNzxfRjQNBlmfZyf03soCOb9AmTKJzNTDlX
HekccGCwiJ2vHyKo8v9LKXTpC/q0PByNi1rBD2z6eZ/A/bMhxrBEzFfsMc9xyMzb
1fqDFfhoyEGlcpxOu/sPOHfyK4OsPknOXQ==
-----END CERTIFICATE-----
)"};

inline constexpr bool check(bool cond) noexcept
{
	return cond;
}

int main()
{
	using namespace ::fast_io::tls;
	using namespace ::fast_io::tls::details;

	::std::byte srv_der[2048], ca_der[2048];
	::std::size_t srv_der_size{}, ca_der_size{};
	{
		char8_t const *cur{srv_pem}, *end{srv_pem + sizeof(srv_pem) - 1};
		char8_t const *label, *b64;
		::std::size_t lsz, bsz;
		if (!pem_next_block(cur, end, label, lsz, b64, bsz))
		{
			return 1;
		}
		if (lsz != 11 || ::fast_io::freestanding::my_memcmp(label, u8"CERTIFICATE", 11) != 0)
		{
			return 2;
		}
		srv_der_size = pem_decode_block(srv_der, sizeof(srv_der), b64, bsz);
		if (srv_der_size == 0)
		{
			return 3;
		}
	}
	{
		char8_t const *cur{ca_pem}, *end{ca_pem + sizeof(ca_pem) - 1};
		char8_t const *label, *b64;
		::std::size_t lsz, bsz;
		if (!pem_next_block(cur, end, label, lsz, b64, bsz))
		{
			return 4;
		}
		ca_der_size = pem_decode_block(ca_der, sizeof(ca_der), b64, bsz);
		if (ca_der_size == 0)
		{
			return 5;
		}
	}

	x509_certificate srv{}, ca{};
	if (!x509_certificate_parse(srv, srv_der, srv_der_size))
	{
		return 6;
	}
	if (!x509_certificate_parse(ca, ca_der, ca_der_size))
	{
		return 7;
	}
	/* SAN must be present and contain localhost */
	if (srv.san == nullptr || !x509_hostname_match(srv, u8"localhost", 9))
	{
		return 8;
	}
	if (x509_hostname_match(srv, u8"other.local", 11) ||
		x509_hostname_match(srv, u8"localhost2", 10) ||
		x509_hostname_match(srv, u8"xlocalhost", 10))
	{
		return 9;
	}

	/* leaf signed by the CA root */
	x509_certificate presented[]{srv};
	x509_certificate roots[]{ca};
	::std::int_least64_t const inside{srv.not_before + 60}; /* inside both validity windows */
	switch (x509_chain_verify(presented, 1, roots, 1, inside))
	{
	case x509_chain_result::ok:
		break;
	default:
		return 10;
	}
	/* wrong time -> expired */
	if (x509_chain_verify(presented, 1, roots, 1, srv.not_after + 60) != x509_chain_result::expired)
	{
		return 11;
	}
	/* empty trust -> untrusted */
	if (x509_chain_verify(presented, 1, roots, 0, inside) != x509_chain_result::untrusted)
	{
		return 12;
	}
	/* corrupted signature -> bad_signature */
	{
		::std::byte bad_der[2048];
		::fast_io::freestanding::non_overlapped_copy_n(srv_der, srv_der_size, bad_der);
		bad_der[srv_der_size - 1] ^= ::std::byte{0xff}; /* flip a signature byte */
		x509_certificate bad{};
		if (!x509_certificate_parse(bad, bad_der, srv_der_size))
		{
			return 13;
		}
		x509_certificate presented2[]{bad};
		if (x509_chain_verify(presented2, 1, roots, 1, inside) != x509_chain_result::bad_signature)
		{
			return 14;
		}
	}
	/* srv cert is not a CA -> can't anchor anything */
	{
		x509_certificate presented2[]{ca};
		if (x509_chain_verify(presented2, 1, presented, 1, inside) != x509_chain_result::untrusted)
		{
			return 15;
		}
	}

	/* ---- HKDF rfc5869 test case 1 (sha256) ---- */
	{
		::std::byte ikm[22], salt[13], info[10], prk[32], okm[42];
		for (::std::size_t i{}; i != sizeof(ikm); ++i)
		{
			ikm[i] = ::std::byte{0x0b};
		}
		for (::std::size_t i{}; i != sizeof(salt); ++i)
		{
			salt[i] = static_cast<::std::byte>(i);
		}
		for (::std::size_t i{}; i != sizeof(info); ++i)
		{
			info[i] = static_cast<::std::byte>(0xf0 + i);
		}
		hkdf_extract_to_ptr<::fast_io::sha256_context>(prk, salt, sizeof(salt), ikm, sizeof(ikm));
		constexpr ::std::byte expect_prk[]{
			::std::byte{0x07}, ::std::byte{0x77}, ::std::byte{0x09}, ::std::byte{0x36}, ::std::byte{0x2c}, ::std::byte{0x2e},
			::std::byte{0x32}, ::std::byte{0xdf}, ::std::byte{0x0d}, ::std::byte{0xdc}, ::std::byte{0x3f}, ::std::byte{0x0d},
			::std::byte{0xc4}, ::std::byte{0x7b}, ::std::byte{0xba}, ::std::byte{0x63}, ::std::byte{0x90}, ::std::byte{0xb6},
			::std::byte{0xc7}, ::std::byte{0x3b}, ::std::byte{0xb5}, ::std::byte{0x0f}, ::std::byte{0x9c}, ::std::byte{0x31},
			::std::byte{0x22}, ::std::byte{0xec}, ::std::byte{0x84}, ::std::byte{0x4a}, ::std::byte{0xd7}, ::std::byte{0xc2},
			::std::byte{0xb3}, ::std::byte{0xe5}};
		if (::fast_io::freestanding::my_memcmp(prk, expect_prk, 32) != 0)
		{
			return 20;
		}
		hkdf_expand_to_ptr<::fast_io::sha256_context>(okm, sizeof(okm), prk, info, sizeof(info));
		constexpr ::std::byte expect_okm[]{
			::std::byte{0x3c}, ::std::byte{0xb2}, ::std::byte{0x5f}, ::std::byte{0x25}, ::std::byte{0xfa}, ::std::byte{0xac},
			::std::byte{0xd5}, ::std::byte{0x7a}, ::std::byte{0x90}, ::std::byte{0x43}, ::std::byte{0x4f}, ::std::byte{0x64},
			::std::byte{0xd0}, ::std::byte{0x36}, ::std::byte{0x2f}, ::std::byte{0x2a}, ::std::byte{0x2d}, ::std::byte{0x2d},
			::std::byte{0x0a}, ::std::byte{0x90}, ::std::byte{0xcf}, ::std::byte{0x1a}, ::std::byte{0x5a}, ::std::byte{0x4c},
			::std::byte{0x5d}, ::std::byte{0xb0}, ::std::byte{0x2d}, ::std::byte{0x56}, ::std::byte{0xec}, ::std::byte{0xc4},
			::std::byte{0xc5}, ::std::byte{0xbf}, ::std::byte{0x34}, ::std::byte{0x00}, ::std::byte{0x72}, ::std::byte{0x08},
			::std::byte{0xd5}, ::std::byte{0xb8}, ::std::byte{0x87}, ::std::byte{0x18}, ::std::byte{0x58}, ::std::byte{0x65}};
		if (::fast_io::freestanding::my_memcmp(okm, expect_okm, 42) != 0)
		{
			return 21;
		}
	}

	/* ---- TLS 1.3 hkdf_expand_label shape ---- */
	{
		::std::byte secret[32]{};
		::std::byte out[16];
		hkdf_expand_label_to_ptr<::fast_io::sha256_context>(out, sizeof(out), secret, u8"key", 3, nullptr, 0);
		/* just check it produces something and is deterministic */
		::std::byte out2[16];
		hkdf_expand_label_to_ptr<::fast_io::sha256_context>(out2, sizeof(out2), secret, u8"key", 3, nullptr, 0);
		if (::fast_io::freestanding::my_memcmp(out, out2, 16) != 0)
		{
			return 30;
		}
		::std::byte out3[16];
		hkdf_expand_label_to_ptr<::fast_io::sha256_context>(out3, sizeof(out3), secret, u8"iv", 2, nullptr, 0);
		if (::fast_io::freestanding::my_memcmp(out, out3, 16) == 0)
		{
			return 31; /* different labels must differ */
		}
	}

	/* ---- wire encode/decode roundtrip ---- */
	{
		::std::byte buf[64];
		::std::byte *p{buf};
		p = wire_put_u16(p, 0x0304);
		p = wire_put_u24(p, 0x00a1b2);
		p = wire_put_u64(p, 0x0102030405060708ull);
		p = wire_put_bytes(p, srv_der, 4);
		wire_reader r{buf, p};
		::std::uint_least16_t v16;
		::std::uint_least32_t v24;
		::std::byte const *bytes;
		if (!r.take_u16(v16) || v16 != 0x0304)
		{
			return 40;
		}
		::std::uint_least32_t v24b;
		if (!r.take_u24(v24b) || v24b != 0x00a1b2)
		{
			return 41;
		}
		::std::uint_least64_t v64{};
		for (unsigned i{}; i != 8; ++i)
		{
			::std::uint_least8_t b;
			if (!r.take_u8(b))
			{
				return 42;
			}
			v64 = (v64 << 8u) | b;
		}
		if (v64 != 0x0102030405060708ull)
		{
			return 43;
		}
		if (!r.take_bytes(bytes, 4) || ::fast_io::freestanding::my_memcmp(bytes, srv_der, 4) != 0)
		{
			return 44;
		}
		if (!r.empty() || r.take_u8(*reinterpret_cast<::std::uint_least8_t *>(buf)))
		{
			return 45;
		}
		(void)v24;
	}

	/* ---- client_hello encoder sanity ---- */
	{
		::std::byte session_id[32]{}, random[32]{}, pubkey[32]{};
		client_hello_params params{};
		params.hostname = u8"example.com";
		params.hostname_size = 11;
		params.session_id = session_id;
		params.random = random;
		params.x25519_public_key = pubkey;
		::std::byte buf[1024];
		::std::size_t const body_size{client_hello_size(params)};
		::std::byte *const endp{client_hello_write_body(buf, params)};
		if (static_cast<::std::size_t>(endp - buf) != body_size)
		{
			return 50; /* size accounting must match what we write */
		}
		wire_reader r{buf, endp};
		::std::uint_least16_t ver;
		::std::byte const *rnd;
		if (!r.take_u16(ver) || ver != 0x0303 || !r.take_bytes(rnd, 32))
		{
			return 51;
		}
		::std::byte const *sid;
		::std::size_t sidn;
		if (!r.take_vector8(sid, sidn) || sidn != 32)
		{
			return 52;
		}
		::std::byte const *suites;
		::std::size_t suitesn;
		if (!r.take_vector16(suites, suitesn) || suitesn != 6)
		{
			return 53;
		}
		::std::byte const *comp;
		::std::size_t compn;
		if (!r.take_vector8(comp, compn) || compn != 1 || comp[0] != ::std::byte{})
		{
			return 54;
		}
		/* verify supported_versions ext only offers 0x0304: scan exts */
		wire_reader exts;
		if (!r.take_sub16(exts))
		{
			return 58;
		}
		bool saw_sv{};
		while (!exts.empty())
		{
			::std::uint_least16_t et;
			::std::byte const *ep;
			::std::size_t en;
			if (!exts.take_u16(et) || !exts.take_vector16(ep, en))
			{
				return 55;
			}
			if (et == static_cast<::std::uint_least16_t>(extension_type::supported_versions))
			{
				saw_sv = true;
				if (en != 3 || ep[0] != ::std::byte{2} || ep[1] != ::std::byte{3} || ep[2] != ::std::byte{4})
				{
					return 56; /* must offer exactly TLS 1.3 */
				}
			}
		}
		if (!saw_sv)
		{
			return 57;
		}
	}

	/* ---- AES-128-GCM: NIST case 3 (key=iv=0, pt=16 zeros) ---- */
	{
		::std::byte key[16]{}, nonce[12]{}, pt[16]{}, ct[16], tag[16];
		::fast_io::aes_gcm_seal_to_ptr<16>(ct, tag, key, nonce, nullptr, 0, pt, 16);
		constexpr ::std::byte want_ct[]{
			::std::byte{0x03}, ::std::byte{0x88}, ::std::byte{0xda}, ::std::byte{0xce}, ::std::byte{0x60}, ::std::byte{0xb6},
			::std::byte{0xa3}, ::std::byte{0x92}, ::std::byte{0xf3}, ::std::byte{0x28}, ::std::byte{0xc2}, ::std::byte{0xb9},
			::std::byte{0x71}, ::std::byte{0xb2}, ::std::byte{0xfe}, ::std::byte{0x78}};
		constexpr ::std::byte want_tag[]{
			::std::byte{0xab}, ::std::byte{0x6e}, ::std::byte{0x47}, ::std::byte{0xd4}, ::std::byte{0x2c}, ::std::byte{0xec},
			::std::byte{0x13}, ::std::byte{0xbd}, ::std::byte{0xf5}, ::std::byte{0x3a}, ::std::byte{0x67}, ::std::byte{0xb2},
			::std::byte{0x12}, ::std::byte{0x57}, ::std::byte{0xbd}, ::std::byte{0xdf}};
		if (::fast_io::freestanding::my_memcmp(ct, want_ct, 16) != 0 ||
			::fast_io::freestanding::my_memcmp(tag, want_tag, 16) != 0)
		{
			return 60;
		}
		::std::byte dec[16];
		if (!::fast_io::aes_gcm_open_to_ptr<16>(dec, key, nonce, nullptr, 0, ct, 16, tag) ||
			::fast_io::freestanding::my_memcmp(dec, pt, 16) != 0)
		{
			return 61;
		}
		tag[0] ^= ::std::byte{1};
		if (::fast_io::aes_gcm_open_to_ptr<16>(dec, key, nonce, nullptr, 0, ct, 16, tag))
		{
			return 62; /* tampered tag must fail */
		}
	}

	/* ---- ChaCha20-Poly1305: RFC 8439 A.1 seal ---- */
	{
		constexpr char8_t key_hex[]{u8"808182838485868788898a8b8c8d8e8f909192939495969798999a9b9c9d9e9f"};
		constexpr char8_t nonce_hex[]{u8"070000004041424344454647"};
		constexpr char8_t aad_hex[]{u8"50515253c0c1c2c3c4c5c6c7"};
		auto unhex{[](::std::byte *out, char8_t const *hx, ::std::size_t n) noexcept {
			auto v{[](char8_t c) noexcept {
				return c <= u8'9' ? c - u8'0' : c <= u8'f' ? c - u8'a' + 10 : c - u8'A' + 10;
			}};
			for (::std::size_t i{}; i != n; ++i)
			{
				out[i] = static_cast<::std::byte>(v(hx[2 * i]) * 16 + v(hx[2 * i + 1]));
			}
		}};
		::std::byte key[32], nonce[12], aad[12];
		unhex(key, key_hex, 32);
		unhex(nonce, nonce_hex, 12);
		unhex(aad, aad_hex, 12);
		char8_t const pt[]{u8"Ladies and Gentlemen of the class of '99: If I could offer you only one tip for the future, sunscreen would be it."};
		::std::size_t const n{sizeof(pt) - 1};
		::std::byte ct[128], tag[16];
		::fast_io::chacha20_poly1305_seal_to_ptr(ct, tag, key, nonce, aad, 12,
											   reinterpret_cast<::std::byte const *>(pt), n);
		constexpr ::std::byte want_tag[]{
			::std::byte{0x1a}, ::std::byte{0xe1}, ::std::byte{0x0b}, ::std::byte{0x59}, ::std::byte{0x4f}, ::std::byte{0x09},
			::std::byte{0xe2}, ::std::byte{0x6a}, ::std::byte{0x7e}, ::std::byte{0x90}, ::std::byte{0x2e}, ::std::byte{0xcb},
			::std::byte{0xd0}, ::std::byte{0x60}, ::std::byte{0x06}, ::std::byte{0x91}};
		constexpr ::std::byte ct_head[]{::std::byte{0xd3}, ::std::byte{0x1a}, ::std::byte{0x8d}, ::std::byte{0x34},
										::std::byte{0x64}, ::std::byte{0x8e}, ::std::byte{0x60}, ::std::byte{0xdb}};
		if (::fast_io::freestanding::my_memcmp(ct, ct_head, 8) != 0 ||
			::fast_io::freestanding::my_memcmp(tag, want_tag, 16) != 0)
		{
			return 63;
		}
		::std::byte dec[128];
		if (!::fast_io::chacha20_poly1305_open_to_ptr(dec, key, nonce, aad, 12, ct, n, tag) ||
			::fast_io::freestanding::my_memcmp(dec, pt, n) != 0)
		{
			return 64;
		}
	}

	::fast_io::println(::fast_io::u8out(), u8"tls13 offline tests: all passed");
	return 0;
}
