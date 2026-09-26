#pragma once

namespace fast_io::curve25519
{

/*
Mod BPO (Base Point Order) operations

  BPO = 2**252 + 27742317777372353535851937790883648493
  BPO = 0x1000000000000000000000000000000014DEF9DEA2F79CD65812631A5CF5D3ED
*/

inline constexpr ::fast_io::containers::array<field_number, 16> NxBPOraw{{
	{0x0, 0x0, 0x0, 0x0},
	{0x5812631a5cf5d3ed, 0x14def9dea2f79cd6, 0x0, 0x1000000000000000},
	{0xb024c634b9eba7da, 0x29bdf3bd45ef39ac, 0x0, 0x2000000000000000},
	{0x837294f16e17bc7, 0x3e9ced9be8e6d683, 0x0, 0x3000000000000000},
	{0x60498c6973d74fb4, 0x537be77a8bde7359, 0x0, 0x4000000000000000},
	{0xb85bef83d0cd23a1, 0x685ae1592ed6102f, 0x0, 0x5000000000000000},
	{0x106e529e2dc2f78e, 0x7d39db37d1cdad06, 0x0, 0x6000000000000000},
	{0x6880b5b88ab8cb7b, 0x9218d51674c549dc, 0x0, 0x7000000000000000},
	{0xc09318d2e7ae9f68, 0xa6f7cef517bce6b2, 0x0, 0x8000000000000000},
	{0x18a57bed44a47355, 0xbbd6c8d3bab48389, 0x0, 0x9000000000000000},
	{0x70b7df07a19a4742, 0xd0b5c2b25dac205f, 0x0, 0xa000000000000000},
	{0xc8ca4221fe901b2f, 0xe594bc9100a3bd35, 0x0, 0xb000000000000000},
	{0x20dca53c5b85ef1c, 0xfa73b66fa39b5a0c, 0x0, 0xc000000000000000},
	{0x78ef0856b87bc309, 0xf52b04e4692f6e2, 0x1, 0xd000000000000000},
	{0xd1016b71157196f6, 0x2431aa2ce98a93b8, 0x1, 0xe000000000000000},
	{0x2913ce8b72676ae3, 0x3910a40b8c82308f, 0x1, 0xf000000000000000},
}};

/* X mod BPO */
inline constexpr void eco_mod(field_number &x) noexcept
{
	::std::uint_least64_t const c{subtraction_u256(x, x, NxBPOraw.index_unchecked(x.back_unchecked() >> 60))};
	constexpr std::uint_least64_t zero{};
	addition_u256_discard(x, x, NxBPOraw.index_unchecked(zero - c));
}

/* T = X*Y (512-bit, no reduction) */
inline constexpr void multiplication_u256(u512 &t, field_number const &x, field_number const &y) noexcept
{
	multiply_single<0, true>(t, x, y.front_unchecked());
	multiply_single<1>(t, x, y.index_unchecked(1));
	multiply_single<2>(t, x, y.index_unchecked(2));
	multiply_single<3>(t, x, y.back_unchecked());
}

/*
Calculate: Y = [b:X] mod BPO
For R = 2^256, we calculate Y = b*R + X mod BPO.
Since -R mod BPO is only 129-bits, it reduces number of multiplications if
we calculate: Y = X - b*(-R) mod BPO instead.
-R mod BPO = {0x812631A5CF5D3ED0, 0x4DEF9DEA2F79CD65, 1, 0}

X is any indexable 4-word view (field_number or the low half of a u512).
*/
template <typename X>
inline constexpr void eco_reduce_hi_word(field_number &y, std::uint_least64_t b, X const &x) noexcept
{
	constexpr std::uint_least64_t minus_r0{0x812631A5CF5D3ED0ULL};
	constexpr std::uint_least64_t minus_r1{0x4DEF9DEA2F79CD65ULL};
	constexpr std::uint_least64_t zero{};

	std::uint_least64_t t1;
	std::uint_least64_t t0{::fast_io::intrinsics::umul(b, minus_r0, t1)};
	std::uint_least64_t t2;
	std::uint_least64_t lo1{::fast_io::intrinsics::umul(b, minus_r1, t2)};
	bool carry{};
	t1 = ::fast_io::intrinsics::addc(t1, lo1, false, carry);
	t2 = ::fast_io::intrinsics::addc(t2, b, carry, carry);
	std::uint_least64_t t3{::fast_io::intrinsics::addc(zero, zero, carry, carry)};

	bool borrow{};
	y.front_unchecked() = ::fast_io::intrinsics::subc(x.index_unchecked(0), t0, false, borrow);
	y.index_unchecked(1) = ::fast_io::intrinsics::subc(x.index_unchecked(1), t1, borrow, borrow);
	y.index_unchecked(2) = ::fast_io::intrinsics::subc(x.index_unchecked(2), t2, borrow, borrow);
	y.back_unchecked() = ::fast_io::intrinsics::subc(x.index_unchecked(3), t3, borrow, borrow);

	/* Add BPO if there is a borrow */
	std::uint_least64_t mask{::fast_io::intrinsics::subc(zero, zero, borrow, borrow)};
	y.front_unchecked() = ::fast_io::intrinsics::addc(y.front_unchecked(), mask & static_cast<std::uint_least64_t>(0x5812631A5CF5D3EDULL), false, carry);
	y.index_unchecked(1) = ::fast_io::intrinsics::addc(y.index_unchecked(1), mask & static_cast<std::uint_least64_t>(0x14DEF9DEA2F79CD6ULL), carry, carry);
	y.index_unchecked(2) = ::fast_io::intrinsics::addc(y.index_unchecked(2), zero, carry, carry);
	y.back_unchecked() = ::fast_io::intrinsics::addc(y.back_unchecked(), mask & static_cast<std::uint_least64_t>(0x1000000000000000ULL), carry, carry);
}

/*
In-place variant: reduce words t[off..off+3] in place using b = t[off+4].
Equivalent to eco_reduce_hi_word(t+off, t[off+4], t+off).
*/
template <std::size_t off>
inline constexpr void eco_reduce_hi_word_inplace(u512 &t) noexcept
{
	constexpr std::uint_least64_t minus_r0{0x812631A5CF5D3ED0ULL};
	constexpr std::uint_least64_t minus_r1{0x4DEF9DEA2F79CD65ULL};
	constexpr std::uint_least64_t zero{};

	std::uint_least64_t const b{t.index_unchecked(off + 4)};
	std::uint_least64_t t1;
	std::uint_least64_t t0{::fast_io::intrinsics::umul(b, minus_r0, t1)};
	std::uint_least64_t t2;
	std::uint_least64_t lo1{::fast_io::intrinsics::umul(b, minus_r1, t2)};
	bool carry{};
	t1 = ::fast_io::intrinsics::addc(t1, lo1, false, carry);
	t2 = ::fast_io::intrinsics::addc(t2, b, carry, carry);
	std::uint_least64_t t3{::fast_io::intrinsics::addc(zero, zero, carry, carry)};

	bool borrow{};
	t.index_unchecked(off) = ::fast_io::intrinsics::subc(t.index_unchecked(off), t0, false, borrow);
	t.index_unchecked(off + 1) = ::fast_io::intrinsics::subc(t.index_unchecked(off + 1), t1, borrow, borrow);
	t.index_unchecked(off + 2) = ::fast_io::intrinsics::subc(t.index_unchecked(off + 2), t2, borrow, borrow);
	t.index_unchecked(off + 3) = ::fast_io::intrinsics::subc(t.index_unchecked(off + 3), t3, borrow, borrow);

	/* Add BPO if there is a borrow */
	std::uint_least64_t mask{::fast_io::intrinsics::subc(zero, zero, borrow, borrow)};
	t.index_unchecked(off) = ::fast_io::intrinsics::addc(t.index_unchecked(off), mask & static_cast<std::uint_least64_t>(0x5812631A5CF5D3EDULL), false, carry);
	t.index_unchecked(off + 1) = ::fast_io::intrinsics::addc(t.index_unchecked(off + 1), mask & static_cast<std::uint_least64_t>(0x14DEF9DEA2F79CD6ULL), carry, carry);
	t.index_unchecked(off + 2) = ::fast_io::intrinsics::addc(t.index_unchecked(off + 2), zero, carry, carry);
	t.index_unchecked(off + 3) = ::fast_io::intrinsics::addc(t.index_unchecked(off + 3), mask & static_cast<std::uint_least64_t>(0x1000000000000000ULL), carry, carry);
}

/* Z = X*Y mod BPO */
inline constexpr void eco_mul_reduce(field_number &z, field_number const &x, field_number const &y) noexcept
{
	u512 t;
	multiplication_u256(t, x, y);
	eco_reduce_hi_word_inplace<3>(t);
	eco_reduce_hi_word_inplace<2>(t);
	eco_reduce_hi_word_inplace<1>(t);
	eco_reduce_hi_word(z, t.index_unchecked(4), t);
}

/* Z = X + Y mod BPO */
inline constexpr void eco_add_reduce(field_number &z, field_number const &x, field_number const &y) noexcept
{
	std::uint_least64_t const c{addition_u256(z, x, y)};
	eco_reduce_hi_word(z, c, z);
}

inline constexpr std::uint_least64_t bytes_to_u64_little_endian(std::byte const *p) noexcept
{
	std::uint_least64_t v;
	::fast_io::freestanding::type_punning_from_bytes(p, v);
	if constexpr (::std::endian::native == ::std::endian::big)
	{
		v = ::fast_io::byte_swap(v);
	}
	return v;
}

inline constexpr void u64_to_bytes_little_endian(std::byte *p, std::uint_least64_t v) noexcept
{
	if constexpr (::std::endian::native == ::std::endian::big)
	{
		v = ::fast_io::byte_swap(v);
	}
	::fast_io::freestanding::type_punning_to_bytes(v, p);
}

/* Y = D mod BPO where D is a 512-bit little-endian byte array (i.e SHA512 digest) */
inline constexpr void eco_digest_to_words(field_number &y, std::byte const *md) noexcept
{
	u512 t;
	for (std::uint_fast8_t i{}; i != 8; ++i)
	{
		t.index_unchecked(i) = bytes_to_u64_little_endian(md + i * 8);
	}
	eco_reduce_hi_word_inplace<3>(t);
	eco_reduce_hi_word_inplace<2>(t);
	eco_reduce_hi_word_inplace<1>(t);
	eco_reduce_hi_word(y, t.index_unchecked(4), t);
}

} // namespace fast_io::curve25519
