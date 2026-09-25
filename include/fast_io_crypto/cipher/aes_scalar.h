#pragma once
/*
Software AES implementation (FIPS-197).

Portable, byte-oriented, constexpr-capable. Fallback backend for
platforms without AES hardware (wasm32, generic arm, ...).

C-style free functions only: no templates, no lambdas, no macros.
Inner per-byte loops are manually unrolled so the state stays in
registers; the block loop and the round loop are the only runtime loops.
A future SIMD backend should use ::fast_io::intrinsics::simd_vector
(__builtin_shufflevector based), never ISA-specific intrinsics.
*/

namespace fast_io::details::aes
{

inline constexpr ::std::uint_least8_t sbox[256]{
	0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
	0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
	0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
	0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
	0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
	0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
	0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
	0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
	0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
	0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
	0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
	0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
	0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
	0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
	0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
	0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16};

inline constexpr ::std::uint_least8_t inv_sbox[256]{
	0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
	0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
	0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
	0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
	0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
	0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
	0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
	0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
	0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
	0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
	0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
	0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
	0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
	0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
	0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
	0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d};

/* rcon[0] is unused; index is the key-expansion round number */
inline constexpr ::std::uint_least8_t rcon[11]{
	0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36};

inline constexpr ::std::uint_least8_t xtime(::std::uint_least8_t x) noexcept
{
	return static_cast<::std::uint_least8_t>((x << 1) ^ (((x >> 7) & 1u) * 0x1bu));
}

inline constexpr ::std::uint_least8_t gmul(::std::uint_least8_t x, ::std::uint_least8_t y) noexcept
{
	::std::uint_least8_t r{};
	for (; y; y >>= 1)
	{
		if (y & 1u)
		{
			r ^= x;
		}
		x = xtime(x);
	}
	return r;
}

/* load/store a 32-bit AES word: byte 0 of the group is the MSB */
inline constexpr ::std::uint_least32_t load_be32(::std::byte const *p) noexcept
{
	if consteval
	{
		return (static_cast<::std::uint_least32_t>(::std::to_integer<::std::uint_least8_t>(p[0])) << 24) |
			   (static_cast<::std::uint_least32_t>(::std::to_integer<::std::uint_least8_t>(p[1])) << 16) |
			   (static_cast<::std::uint_least32_t>(::std::to_integer<::std::uint_least8_t>(p[2])) << 8) |
			   static_cast<::std::uint_least32_t>(::std::to_integer<::std::uint_least8_t>(p[3]));
	}
	else
	{
		::std::uint_least32_t v;
		::fast_io::details::my_memcpy(__builtin_addressof(v), p, 4);
		return ::fast_io::big_endian(v);
	}
}

inline constexpr void store_be32(::std::uint_least8_t *p, ::std::uint_least32_t v) noexcept
{
	if consteval
	{
		p[0] = static_cast<::std::uint_least8_t>(v >> 24);
		p[1] = static_cast<::std::uint_least8_t>(v >> 16);
		p[2] = static_cast<::std::uint_least8_t>(v >> 8);
		p[3] = static_cast<::std::uint_least8_t>(v);
	}
	else
	{
		v = ::fast_io::big_endian(v);
		::fast_io::details::my_memcpy(p, __builtin_addressof(v), 4);
	}
}

/* state <-> byte block transfer */
inline constexpr void load_block(::std::uint_least8_t *state, ::std::byte const *in) noexcept
{
	if consteval
	{
		state[0] = ::std::to_integer<::std::uint_least8_t>(in[0]);
		state[1] = ::std::to_integer<::std::uint_least8_t>(in[1]);
		state[2] = ::std::to_integer<::std::uint_least8_t>(in[2]);
		state[3] = ::std::to_integer<::std::uint_least8_t>(in[3]);
		state[4] = ::std::to_integer<::std::uint_least8_t>(in[4]);
		state[5] = ::std::to_integer<::std::uint_least8_t>(in[5]);
		state[6] = ::std::to_integer<::std::uint_least8_t>(in[6]);
		state[7] = ::std::to_integer<::std::uint_least8_t>(in[7]);
		state[8] = ::std::to_integer<::std::uint_least8_t>(in[8]);
		state[9] = ::std::to_integer<::std::uint_least8_t>(in[9]);
		state[10] = ::std::to_integer<::std::uint_least8_t>(in[10]);
		state[11] = ::std::to_integer<::std::uint_least8_t>(in[11]);
		state[12] = ::std::to_integer<::std::uint_least8_t>(in[12]);
		state[13] = ::std::to_integer<::std::uint_least8_t>(in[13]);
		state[14] = ::std::to_integer<::std::uint_least8_t>(in[14]);
		state[15] = ::std::to_integer<::std::uint_least8_t>(in[15]);
	}
	else
	{
		::fast_io::details::my_memcpy(state, in, 16);
	}
}

inline constexpr void store_block(::std::byte *out, ::std::uint_least8_t const *state) noexcept
{
	if consteval
	{
		out[0] = ::std::byte{state[0]};
		out[1] = ::std::byte{state[1]};
		out[2] = ::std::byte{state[2]};
		out[3] = ::std::byte{state[3]};
		out[4] = ::std::byte{state[4]};
		out[5] = ::std::byte{state[5]};
		out[6] = ::std::byte{state[6]};
		out[7] = ::std::byte{state[7]};
		out[8] = ::std::byte{state[8]};
		out[9] = ::std::byte{state[9]};
		out[10] = ::std::byte{state[10]};
		out[11] = ::std::byte{state[11]};
		out[12] = ::std::byte{state[12]};
		out[13] = ::std::byte{state[13]};
		out[14] = ::std::byte{state[14]};
		out[15] = ::std::byte{state[15]};
	}
	else
	{
		::fast_io::details::my_memcpy(out, state, 16);
	}
}

inline constexpr ::std::uint_least32_t sub_word(::std::uint_least32_t x) noexcept
{
	return (static_cast<::std::uint_least32_t>(sbox[x >> 24]) << 24) |
		   (static_cast<::std::uint_least32_t>(sbox[(x >> 16) & 0xff]) << 16) |
		   (static_cast<::std::uint_least32_t>(sbox[(x >> 8) & 0xff]) << 8) |
		   static_cast<::std::uint_least32_t>(sbox[x & 0xff]);
}

/*
Expand key into round_keys[(rounds + 1) * 16].
nk = key_size/4 (4, 6 or 8), rounds = nk + 6.
Round keys are stored as plain bytes in state order:
round_keys[16*r + c*4 + row] hits state column c, row r.
group/rcon_i counters track i%nk and i/nk without division.
*/
inline constexpr void key_expansion(::std::byte const *key, ::std::uint_least8_t *round_keys,
									::std::size_t nk, ::std::size_t rounds) noexcept
{
	::std::size_t const total_words{4 * (rounds + 1)};
	::std::uint_least32_t w[60]{}; /* max: AES-256 -> 4 * (14 + 1) */
	for (::std::size_t i{}; i != nk; ++i)
	{
		w[i] = load_be32(key + i * 4);
	}
	::std::size_t group{};
	::std::size_t rcon_i{1};
	for (::std::size_t i{nk}; i != total_words; ++i)
	{
		::std::uint_least32_t t{w[i - 1]};
		if (group == 0)
		{
			/* canonical word: byte0 in bits 31..24 -> RotWord is rotl by 8 */
			t = sub_word(::std::rotl(t, 8)) ^
				(static_cast<::std::uint_least32_t>(rcon[rcon_i]) << 24);
			++rcon_i;
		}
		else if (nk == 8 && group == 4)
		{
			t = sub_word(t);
		}
		w[i] = w[i - nk] ^ t;
		if (++group == nk)
		{
			group = 0;
		}
	}
	for (::std::size_t i{}; i != total_words; ++i)
	{
		store_be32(round_keys + i * 4, w[i]);
		w[i] = 0;
	}
}

inline constexpr void add_round_key(::std::uint_least8_t *state, ::std::uint_least8_t const *rk) noexcept
{
	state[0] ^= rk[0];
	state[1] ^= rk[1];
	state[2] ^= rk[2];
	state[3] ^= rk[3];
	state[4] ^= rk[4];
	state[5] ^= rk[5];
	state[6] ^= rk[6];
	state[7] ^= rk[7];
	state[8] ^= rk[8];
	state[9] ^= rk[9];
	state[10] ^= rk[10];
	state[11] ^= rk[11];
	state[12] ^= rk[12];
	state[13] ^= rk[13];
	state[14] ^= rk[14];
	state[15] ^= rk[15];
}

inline constexpr void sub_bytes(::std::uint_least8_t *state) noexcept
{
	state[0] = sbox[state[0]];
	state[1] = sbox[state[1]];
	state[2] = sbox[state[2]];
	state[3] = sbox[state[3]];
	state[4] = sbox[state[4]];
	state[5] = sbox[state[5]];
	state[6] = sbox[state[6]];
	state[7] = sbox[state[7]];
	state[8] = sbox[state[8]];
	state[9] = sbox[state[9]];
	state[10] = sbox[state[10]];
	state[11] = sbox[state[11]];
	state[12] = sbox[state[12]];
	state[13] = sbox[state[13]];
	state[14] = sbox[state[14]];
	state[15] = sbox[state[15]];
}

inline constexpr void inv_sub_bytes(::std::uint_least8_t *state) noexcept
{
	state[0] = inv_sbox[state[0]];
	state[1] = inv_sbox[state[1]];
	state[2] = inv_sbox[state[2]];
	state[3] = inv_sbox[state[3]];
	state[4] = inv_sbox[state[4]];
	state[5] = inv_sbox[state[5]];
	state[6] = inv_sbox[state[6]];
	state[7] = inv_sbox[state[7]];
	state[8] = inv_sbox[state[8]];
	state[9] = inv_sbox[state[9]];
	state[10] = inv_sbox[state[10]];
	state[11] = inv_sbox[state[11]];
	state[12] = inv_sbox[state[12]];
	state[13] = inv_sbox[state[13]];
	state[14] = inv_sbox[state[14]];
	state[15] = inv_sbox[state[15]];
}

/* state[c*4 + r]: row r rotates left by r positions */
inline constexpr void shift_rows(::std::uint_least8_t *s) noexcept
{
	::std::uint_least8_t t;
	t = s[1]; s[1] = s[5]; s[5] = s[9]; s[9] = s[13]; s[13] = t;
	t = s[2]; s[2] = s[10]; s[10] = t;
	t = s[6]; s[6] = s[14]; s[14] = t;
	t = s[15]; s[15] = s[11]; s[11] = s[7]; s[7] = s[3]; s[3] = t;
}

inline constexpr void inv_shift_rows(::std::uint_least8_t *s) noexcept
{
	::std::uint_least8_t t;
	t = s[13]; s[13] = s[9]; s[9] = s[5]; s[5] = s[1]; s[1] = t;
	t = s[2]; s[2] = s[10]; s[10] = t;
	t = s[6]; s[6] = s[14]; s[14] = t;
	t = s[3]; s[3] = s[7]; s[7] = s[11]; s[11] = s[15]; s[15] = t;
}

inline constexpr void mix_columns(::std::uint_least8_t *s) noexcept
{
	::std::uint_least8_t a0, a1, a2, a3;
	a0 = s[0]; a1 = s[1]; a2 = s[2]; a3 = s[3];
	s[0] = xtime(a0) ^ static_cast<::std::uint_least8_t>(xtime(a1) ^ a1) ^ a2 ^ a3;
	s[1] = a0 ^ xtime(a1) ^ static_cast<::std::uint_least8_t>(xtime(a2) ^ a2) ^ a3;
	s[2] = a0 ^ a1 ^ xtime(a2) ^ static_cast<::std::uint_least8_t>(xtime(a3) ^ a3);
	s[3] = static_cast<::std::uint_least8_t>(xtime(a0) ^ a0) ^ a1 ^ a2 ^ xtime(a3);
	a0 = s[4]; a1 = s[5]; a2 = s[6]; a3 = s[7];
	s[4] = xtime(a0) ^ static_cast<::std::uint_least8_t>(xtime(a1) ^ a1) ^ a2 ^ a3;
	s[5] = a0 ^ xtime(a1) ^ static_cast<::std::uint_least8_t>(xtime(a2) ^ a2) ^ a3;
	s[6] = a0 ^ a1 ^ xtime(a2) ^ static_cast<::std::uint_least8_t>(xtime(a3) ^ a3);
	s[7] = static_cast<::std::uint_least8_t>(xtime(a0) ^ a0) ^ a1 ^ a2 ^ xtime(a3);
	a0 = s[8]; a1 = s[9]; a2 = s[10]; a3 = s[11];
	s[8] = xtime(a0) ^ static_cast<::std::uint_least8_t>(xtime(a1) ^ a1) ^ a2 ^ a3;
	s[9] = a0 ^ xtime(a1) ^ static_cast<::std::uint_least8_t>(xtime(a2) ^ a2) ^ a3;
	s[10] = a0 ^ a1 ^ xtime(a2) ^ static_cast<::std::uint_least8_t>(xtime(a3) ^ a3);
	s[11] = static_cast<::std::uint_least8_t>(xtime(a0) ^ a0) ^ a1 ^ a2 ^ xtime(a3);
	a0 = s[12]; a1 = s[13]; a2 = s[14]; a3 = s[15];
	s[12] = xtime(a0) ^ static_cast<::std::uint_least8_t>(xtime(a1) ^ a1) ^ a2 ^ a3;
	s[13] = a0 ^ xtime(a1) ^ static_cast<::std::uint_least8_t>(xtime(a2) ^ a2) ^ a3;
	s[14] = a0 ^ a1 ^ xtime(a2) ^ static_cast<::std::uint_least8_t>(xtime(a3) ^ a3);
	s[15] = static_cast<::std::uint_least8_t>(xtime(a0) ^ a0) ^ a1 ^ a2 ^ xtime(a3);
}

inline constexpr void inv_mix_columns(::std::uint_least8_t *s) noexcept
{
	::std::uint_least8_t a0, a1, a2, a3;
	a0 = s[0]; a1 = s[1]; a2 = s[2]; a3 = s[3];
	s[0] = gmul(a0, 14) ^ gmul(a1, 11) ^ gmul(a2, 13) ^ gmul(a3, 9);
	s[1] = gmul(a0, 9) ^ gmul(a1, 14) ^ gmul(a2, 11) ^ gmul(a3, 13);
	s[2] = gmul(a0, 13) ^ gmul(a1, 9) ^ gmul(a2, 14) ^ gmul(a3, 11);
	s[3] = gmul(a0, 11) ^ gmul(a1, 13) ^ gmul(a2, 9) ^ gmul(a3, 14);
	a0 = s[4]; a1 = s[5]; a2 = s[6]; a3 = s[7];
	s[4] = gmul(a0, 14) ^ gmul(a1, 11) ^ gmul(a2, 13) ^ gmul(a3, 9);
	s[5] = gmul(a0, 9) ^ gmul(a1, 14) ^ gmul(a2, 11) ^ gmul(a3, 13);
	s[6] = gmul(a0, 13) ^ gmul(a1, 9) ^ gmul(a2, 14) ^ gmul(a3, 11);
	s[7] = gmul(a0, 11) ^ gmul(a1, 13) ^ gmul(a2, 9) ^ gmul(a3, 14);
	a0 = s[8]; a1 = s[9]; a2 = s[10]; a3 = s[11];
	s[8] = gmul(a0, 14) ^ gmul(a1, 11) ^ gmul(a2, 13) ^ gmul(a3, 9);
	s[9] = gmul(a0, 9) ^ gmul(a1, 14) ^ gmul(a2, 11) ^ gmul(a3, 13);
	s[10] = gmul(a0, 13) ^ gmul(a1, 9) ^ gmul(a2, 14) ^ gmul(a3, 11);
	s[11] = gmul(a0, 11) ^ gmul(a1, 13) ^ gmul(a2, 9) ^ gmul(a3, 14);
	a0 = s[12]; a1 = s[13]; a2 = s[14]; a3 = s[15];
	s[12] = gmul(a0, 14) ^ gmul(a1, 11) ^ gmul(a2, 13) ^ gmul(a3, 9);
	s[13] = gmul(a0, 9) ^ gmul(a1, 14) ^ gmul(a2, 11) ^ gmul(a3, 13);
	s[14] = gmul(a0, 13) ^ gmul(a1, 9) ^ gmul(a2, 14) ^ gmul(a3, 11);
	s[15] = gmul(a0, 11) ^ gmul(a1, 13) ^ gmul(a2, 9) ^ gmul(a3, 14);
}

/* ECB encrypt: nblocks consecutive 16-byte blocks. from/to may alias. */
inline constexpr void encrypt(::std::uint_least8_t const *round_keys, ::std::size_t rounds,
							  ::std::byte const *from, ::std::size_t nblocks, ::std::byte *to) noexcept
{
	::std::uint_least8_t state[16];
	for (; nblocks; --nblocks, from += 16, to += 16)
	{
		load_block(state, from);
		add_round_key(state, round_keys);
		for (::std::size_t r{1}; r != rounds; ++r)
		{
			sub_bytes(state);
			shift_rows(state);
			mix_columns(state);
			add_round_key(state, round_keys + r * 16);
		}
		sub_bytes(state);
		shift_rows(state);
		add_round_key(state, round_keys + rounds * 16);
		store_block(to, state);
	}
	state[0] = 0; state[1] = 0; state[2] = 0; state[3] = 0;
	state[4] = 0; state[5] = 0; state[6] = 0; state[7] = 0;
	state[8] = 0; state[9] = 0; state[10] = 0; state[11] = 0;
	state[12] = 0; state[13] = 0; state[14] = 0; state[15] = 0;
}

/* ECB decrypt: same round keys, used in reverse order */
inline constexpr void decrypt(::std::uint_least8_t const *round_keys, ::std::size_t rounds,
							  ::std::byte const *from, ::std::size_t nblocks, ::std::byte *to) noexcept
{
	::std::uint_least8_t state[16];
	for (; nblocks; --nblocks, from += 16, to += 16)
	{
		load_block(state, from);
		add_round_key(state, round_keys + rounds * 16);
		for (::std::size_t r{rounds - 1}; r != 0; --r)
		{
			inv_shift_rows(state);
			inv_sub_bytes(state);
			add_round_key(state, round_keys + r * 16);
			inv_mix_columns(state);
		}
		inv_shift_rows(state);
		inv_sub_bytes(state);
		add_round_key(state, round_keys);
		store_block(to, state);
	}
	state[0] = 0; state[1] = 0; state[2] = 0; state[3] = 0;
	state[4] = 0; state[5] = 0; state[6] = 0; state[7] = 0;
	state[8] = 0; state[9] = 0; state[10] = 0; state[11] = 0;
	state[12] = 0; state[13] = 0; state[14] = 0; state[15] = 0;
}

} // namespace fast_io::details::aes
