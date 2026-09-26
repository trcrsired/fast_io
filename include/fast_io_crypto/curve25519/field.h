#pragma once

namespace fast_io::curve25519
{

using u256 = ::fast_io::containers::array<std::uint_least64_t, 4>;
using u512 = ::fast_io::containers::array<std::uint_least64_t, 8>;

inline constexpr void addition_u256_discard(field_number &z,
											field_number const &x,
											field_number const &y) noexcept
{
	bool carry{};
	z.front_unchecked() = ::fast_io::intrinsics::addc(x.front_unchecked(), y.front_unchecked(), false, carry);
	z.index_unchecked(1) = ::fast_io::intrinsics::addc(x.index_unchecked(1), y.index_unchecked(1), carry, carry);
	z.index_unchecked(2) = ::fast_io::intrinsics::addc(x.index_unchecked(2), y.index_unchecked(2), carry, carry);
	z.back_unchecked() = ::fast_io::intrinsics::addc(x.back_unchecked(), y.back_unchecked(), carry, carry);
}

inline constexpr std::uint_least64_t addition_u256(field_number &z,
												   field_number const &x,
												   field_number const &y) noexcept
{
	using unsigned_type = field_number::value_type;
	bool carry{};
	z.front_unchecked() = ::fast_io::intrinsics::addc(x.front_unchecked(), y.front_unchecked(), false, carry);
	z.index_unchecked(1) = ::fast_io::intrinsics::addc(x.index_unchecked(1), y.index_unchecked(1), carry, carry);
	z.index_unchecked(2) = ::fast_io::intrinsics::addc(x.index_unchecked(2), y.index_unchecked(2), carry, carry);
	z.back_unchecked() = ::fast_io::intrinsics::addc(x.back_unchecked(), y.back_unchecked(), carry, carry);
	constexpr unsigned_type zero{};
	unsigned_type res{::fast_io::intrinsics::addc(zero, zero, carry, carry)};
	return res;
}

inline constexpr void field_number_addition(field_number &f,
											field_number const &x,
											field_number const &y) noexcept
{
	using unsigned_type = field_number::value_type;
	constexpr unsigned_type zero{};
	std::uint_least64_t f0, f1, f2, f3;
	bool carry{};
	f0 = ::fast_io::intrinsics::addc(x.front_unchecked(), y.front_unchecked(), false, carry);
	f1 = ::fast_io::intrinsics::addc(x.index_unchecked(1), y.index_unchecked(1), carry, carry);
	f2 = ::fast_io::intrinsics::addc(x.index_unchecked(2), y.index_unchecked(2), carry, carry);
	f3 = ::fast_io::intrinsics::addc(x.back_unchecked(), y.back_unchecked(), carry, carry);
	unsigned_type v{::fast_io::intrinsics::subc(zero, zero, carry, carry)};
	v &= static_cast<unsigned_type>(38);
	f0 = ::fast_io::intrinsics::addc(f0, v, false, carry);
	f1 = ::fast_io::intrinsics::addc(f1, zero, carry, carry);
	f2 = ::fast_io::intrinsics::addc(f2, zero, carry, carry);
	f3 = ::fast_io::intrinsics::addc(f3, zero, carry, carry);
	v = ::fast_io::intrinsics::subc(v, v, carry, carry);
	v &= static_cast<unsigned_type>(38);
	f.front_unchecked() = ::fast_io::intrinsics::addc(f0, v, false, carry);
	f.index_unchecked(1) = ::fast_io::intrinsics::addc(f1, zero, carry, carry);
	f.index_unchecked(2) = ::fast_io::intrinsics::addc(f2, zero, carry, carry);
	f.back_unchecked() = ::fast_io::intrinsics::addc(f3, zero, carry, carry);
}
/*
f=x-y
*/

inline constexpr std::uint_least64_t subtraction_u256(field_number &f,
													  field_number const &x,
													  field_number const &y) noexcept
{
	using unsigned_type = field_number::value_type;
	bool borrow{};
	f.front_unchecked() = ::fast_io::intrinsics::subc(x.front_unchecked(), y.front_unchecked(), false, borrow);
	f.index_unchecked(1) = ::fast_io::intrinsics::subc(x.index_unchecked(1), y.index_unchecked(1), borrow, borrow);
	f.index_unchecked(2) = ::fast_io::intrinsics::subc(x.index_unchecked(2), y.index_unchecked(2), borrow, borrow);
	f.back_unchecked() = ::fast_io::intrinsics::subc(x.back_unchecked(), y.back_unchecked(), borrow, borrow);
	constexpr unsigned_type zero{};
	unsigned_type res{::fast_io::intrinsics::subc(zero, zero, borrow, borrow)};
	return res;
}
/*
f=(x-y) mod (2^255-19)
*/
inline constexpr void field_number_subtraction(field_number &f,
											   field_number const &x,
											   field_number const &y) noexcept
{
	using unsigned_type = field_number::value_type;
	constexpr unsigned_type zero{};
	std::uint_least64_t f0, f1, f2, f3;
	bool borrow{};
	f0 = ::fast_io::intrinsics::subc(x.front_unchecked(), y.front_unchecked(), false, borrow);
	f1 = ::fast_io::intrinsics::subc(x.index_unchecked(1), y.index_unchecked(1), borrow, borrow);
	f2 = ::fast_io::intrinsics::subc(x.index_unchecked(2), y.index_unchecked(2), borrow, borrow);
	f3 = ::fast_io::intrinsics::subc(x.back_unchecked(), y.back_unchecked(), borrow, borrow);
	unsigned_type v{::fast_io::intrinsics::subc(zero, zero, borrow, borrow)};
	v &= static_cast<unsigned_type>(38);
	f0 = ::fast_io::intrinsics::subc(f0, v, false, borrow);
	f1 = ::fast_io::intrinsics::subc(f1, zero, borrow, borrow);
	f2 = ::fast_io::intrinsics::subc(f2, zero, borrow, borrow);
	f3 = ::fast_io::intrinsics::subc(f3, zero, borrow, borrow);
	v = ::fast_io::intrinsics::subc(v, v, borrow, borrow);
	v &= static_cast<unsigned_type>(38);
	f.front_unchecked() = ::fast_io::intrinsics::subc(f0, v, false, borrow);
	f.index_unchecked(1) = ::fast_io::intrinsics::subc(f1, zero, borrow, borrow);
	f.index_unchecked(2) = ::fast_io::intrinsics::subc(f2, zero, borrow, borrow);
	f.back_unchecked() = ::fast_io::intrinsics::subc(f3, zero, borrow, borrow);
}

/*
dest[0..4] += x*b starting at offset (dest is a u512 accumulating a partial row)
*/
template <std::size_t offset, bool first = false>
inline constexpr void multiply_single(u512 &dest, field_number const &x, std::uint_least64_t b) noexcept
{
	std::uint_least64_t h0;
	std::uint_least64_t t0{::fast_io::intrinsics::umul(x.front_unchecked(), b, h0)};
	std::uint_least64_t h1;
	std::uint_least64_t t1{::fast_io::intrinsics::umul(x.index_unchecked(1), b, h1)};
	std::uint_least64_t h2;
	std::uint_least64_t t2{::fast_io::intrinsics::umul(x.index_unchecked(2), b, h2)};
	std::uint_least64_t h3;
	std::uint_least64_t t3{::fast_io::intrinsics::umul(x.back_unchecked(), b, h3)};
	constexpr std::uint_least64_t zero{};
	bool carry{};
	if constexpr (first)
	{
		dest.index_unchecked(offset) = t0;
		dest.index_unchecked(offset + 1) = ::fast_io::intrinsics::addc(t1, h0, false, carry);
		dest.index_unchecked(offset + 2) = ::fast_io::intrinsics::addc(t2, h1, carry, carry);
		dest.index_unchecked(offset + 3) = ::fast_io::intrinsics::addc(t3, h2, carry, carry);
		dest.index_unchecked(offset + 4) = ::fast_io::intrinsics::addc(zero, h3, carry, carry);
	}
	else
	{
		t1 = ::fast_io::intrinsics::addc(t1, h0, false, carry);
		t2 = ::fast_io::intrinsics::addc(t2, h1, carry, carry);
		t3 = ::fast_io::intrinsics::addc(t3, h2, carry, carry);
		h3 = ::fast_io::intrinsics::addc(zero, h3, carry, carry);

		dest.index_unchecked(offset) = ::fast_io::intrinsics::addc(t0, dest.index_unchecked(offset), false, carry);
		dest.index_unchecked(offset + 1) = ::fast_io::intrinsics::addc(t1, dest.index_unchecked(offset + 1), carry, carry);
		dest.index_unchecked(offset + 2) = ::fast_io::intrinsics::addc(t2, dest.index_unchecked(offset + 2), carry, carry);
		dest.index_unchecked(offset + 3) = ::fast_io::intrinsics::addc(t3, dest.index_unchecked(offset + 3), carry, carry);
		dest.index_unchecked(offset + 4) = ::fast_io::intrinsics::addc(zero, h3, carry, carry);
	}
}

/*
z = y + constant*x, then fold the carry with *38. Constant-time.
*/
inline constexpr void multiplication_add_reduce(field_number &z, field_number const &y, std::uint_least64_t constant, field_number const &x) noexcept
{
	constexpr std::uint_least64_t zero{};
	std::uint_least64_t h0;
	std::uint_least64_t t0{::fast_io::intrinsics::umul(x.front_unchecked(), constant, h0)};
	std::uint_least64_t h1;
	std::uint_least64_t t1{::fast_io::intrinsics::umul(x.index_unchecked(1), constant, h1)};
	std::uint_least64_t h2;
	std::uint_least64_t t2{::fast_io::intrinsics::umul(x.index_unchecked(2), constant, h2)};
	std::uint_least64_t h3;
	std::uint_least64_t t3{::fast_io::intrinsics::umul(x.back_unchecked(), constant, h3)};

	bool carry{};
	t0 = ::fast_io::intrinsics::addc(t0, y.front_unchecked(), false, carry);
	t1 = ::fast_io::intrinsics::addc(t1, y.index_unchecked(1), carry, carry);
	t2 = ::fast_io::intrinsics::addc(t2, y.index_unchecked(2), carry, carry);
	t3 = ::fast_io::intrinsics::addc(t3, y.back_unchecked(), carry, carry);
	h3 = ::fast_io::intrinsics::addc(h3, zero, carry, carry);


	t1 = ::fast_io::intrinsics::addc(t1, h0, false, carry);
	t2 = ::fast_io::intrinsics::addc(t2, h1, carry, carry);
	t3 = ::fast_io::intrinsics::addc(t3, h2, carry, carry);
	h3 = ::fast_io::intrinsics::addc(h3, zero, carry, carry);

	constexpr std::uint_least64_t constant38{38};

	std::uint_least64_t v;
	std::uint_least64_t t4{::fast_io::intrinsics::umul(h3, constant38, v)};
	t0 = ::fast_io::intrinsics::addc(t0, t4, false, carry);
	t1 = ::fast_io::intrinsics::addc(t1, v, carry, carry);
	t2 = ::fast_io::intrinsics::addc(t2, zero, carry, carry);
	t3 = ::fast_io::intrinsics::addc(t3, zero, carry, carry);


	t4 = ::fast_io::intrinsics::subc(zero, zero, carry, carry);
	t4 &= constant38;
	z.front_unchecked() = ::fast_io::intrinsics::addc(t0, t4, false, carry);
	z.index_unchecked(1) = ::fast_io::intrinsics::addc(t1, zero, carry, carry);
	z.index_unchecked(2) = ::fast_io::intrinsics::addc(t2, zero, carry, carry);
	z.back_unchecked() = ::fast_io::intrinsics::addc(t3, zero, carry, carry);
}

/*
u512 variant for the same op: z = t[0..3] + constant*t[4..7]
*/
inline constexpr void multiplication_add_reduce_hi(field_number &z, u512 const &t, std::uint_least64_t constant) noexcept
{
	constexpr std::uint_least64_t zero{};
	std::uint_least64_t h0;
	std::uint_least64_t t0{::fast_io::intrinsics::umul(t.index_unchecked(4), constant, h0)};
	std::uint_least64_t h1;
	std::uint_least64_t t1{::fast_io::intrinsics::umul(t.index_unchecked(5), constant, h1)};
	std::uint_least64_t h2;
	std::uint_least64_t t2{::fast_io::intrinsics::umul(t.index_unchecked(6), constant, h2)};
	std::uint_least64_t h3;
	std::uint_least64_t t3{::fast_io::intrinsics::umul(t.index_unchecked(7), constant, h3)};

	bool carry{};
	t0 = ::fast_io::intrinsics::addc(t0, t.index_unchecked(0), false, carry);
	t1 = ::fast_io::intrinsics::addc(t1, t.index_unchecked(1), carry, carry);
	t2 = ::fast_io::intrinsics::addc(t2, t.index_unchecked(2), carry, carry);
	t3 = ::fast_io::intrinsics::addc(t3, t.index_unchecked(3), carry, carry);
	h3 = ::fast_io::intrinsics::addc(h3, zero, carry, carry);


	t1 = ::fast_io::intrinsics::addc(t1, h0, false, carry);
	t2 = ::fast_io::intrinsics::addc(t2, h1, carry, carry);
	t3 = ::fast_io::intrinsics::addc(t3, h2, carry, carry);
	h3 = ::fast_io::intrinsics::addc(h3, zero, carry, carry);

	constexpr std::uint_least64_t constant38{38};

	std::uint_least64_t v;
	std::uint_least64_t t4{::fast_io::intrinsics::umul(h3, constant38, v)};
	t0 = ::fast_io::intrinsics::addc(t0, t4, false, carry);
	t1 = ::fast_io::intrinsics::addc(t1, v, carry, carry);
	t2 = ::fast_io::intrinsics::addc(t2, zero, carry, carry);
	t3 = ::fast_io::intrinsics::addc(t3, zero, carry, carry);


	t4 = ::fast_io::intrinsics::subc(zero, zero, carry, carry);
	t4 &= constant38;
	z.front_unchecked() = ::fast_io::intrinsics::addc(t0, t4, false, carry);
	z.index_unchecked(1) = ::fast_io::intrinsics::addc(t1, zero, carry, carry);
	z.index_unchecked(2) = ::fast_io::intrinsics::addc(t2, zero, carry, carry);
	z.back_unchecked() = ::fast_io::intrinsics::addc(t3, zero, carry, carry);
}

inline constexpr void reduce_final(field_number &f, u512 const &x) noexcept
{
	constexpr std::uint_least64_t constant{38};
	multiplication_add_reduce_hi(f, x, constant);
}


inline constexpr void field_number_multiplication(field_number &r, field_number const &x, field_number const &y) noexcept
{
	u512 t;
	multiply_single<0, true>(t, x, y.front_unchecked());
	multiply_single<1>(t, x, y.index_unchecked(1));
	multiply_single<2>(t, x, y.index_unchecked(2));
	multiply_single<3>(t, x, y.back_unchecked());
	reduce_final(r, t);
}

inline constexpr std::uint_least64_t muladd_w0(std::uint_least64_t add_value, std::uint_least64_t mul_value, std::uint_least64_t &high) noexcept
{
	constexpr std::uint_least64_t constant{38};
	constexpr std::uint_least64_t zero{};
	std::uint_least64_t low{::fast_io::intrinsics::umul(mul_value, constant, high)};
	bool carry{};
	low = ::fast_io::intrinsics::addc(add_value, low, false, carry);
	high = ::fast_io::intrinsics::addc(zero, high, carry, carry);
	return low;
}

inline constexpr std::uint_least64_t muladd_w1(std::uint_least64_t add_value, std::uint_least64_t mul_value, std::uint_least64_t &high, std::uint_least64_t last_high) noexcept
{
	constexpr std::uint_least64_t constant{38};
	constexpr std::uint_least64_t zero{};
	std::uint_least64_t low{::fast_io::intrinsics::umul(mul_value, constant, high)};
	bool carry{};
	low = ::fast_io::intrinsics::addc(last_high, low, false, carry);
	high = ::fast_io::intrinsics::addc(zero, high, carry, carry);
	low = ::fast_io::intrinsics::addc(add_value, low, false, carry);
	high = ::fast_io::intrinsics::addc(zero, high, carry, carry);
	return low;
}

inline constexpr void field_number_square(field_number &r, field_number const &x) noexcept
{
	constexpr std::uint_least64_t zero{};
	constexpr std::uint_least64_t constant{38};

	std::uint_least64_t x0{x.front_unchecked()}, x1{x.index_unchecked(1)}, x2{x.index_unchecked(2)}, x3{x.back_unchecked()};

	std::uint_least64_t a2;
	std::uint_least64_t a1{::fast_io::intrinsics::umul(x0, x1, a2)};

	std::uint_least64_t b0;
	std::uint_least64_t a3{::fast_io::intrinsics::umul(x0, x3, b0)};

	std::uint_least64_t b2;
	std::uint_least64_t b1{::fast_io::intrinsics::umul(x2, x3, b2)};

	std::uint_least64_t b3_x0x2;
	std::uint_least64_t a0{::fast_io::intrinsics::umul(x0, x2, b3_x0x2)};

	std::uint_least64_t c1;
	std::uint_least64_t c0{::fast_io::intrinsics::umul(x1, x3, c1)};

	std::uint_least64_t c0_x1x2;
	std::uint_least64_t b3{::fast_io::intrinsics::umul(x1, x2, c0_x1x2)};

	bool carry{};
	b3 = ::fast_io::intrinsics::addc(b3_x0x2, b3, false, carry);
	c0 = ::fast_io::intrinsics::addc(c0, c0_x1x2, carry, carry);
	c1 = ::fast_io::intrinsics::addc(zero, c1, carry, carry);

	a2 = ::fast_io::intrinsics::addc(a0, a2, false, carry);
	a3 = ::fast_io::intrinsics::addc(b3, a3, carry, carry);
	b0 = ::fast_io::intrinsics::addc(c0, b0, carry, carry);
	b1 = ::fast_io::intrinsics::addc(c1, b1, carry, carry);
	b2 = ::fast_io::intrinsics::addc(zero, b2, carry, carry);

	// Multiply by 2 (the rcl-through-carry idiom)
	a1 = ::fast_io::intrinsics::addc(a1, a1, false, carry);
	a2 = ::fast_io::intrinsics::addc(a2, a2, carry, carry);
	a3 = ::fast_io::intrinsics::addc(a3, a3, carry, carry);
	b0 = ::fast_io::intrinsics::addc(b0, b0, carry, carry);
	b1 = ::fast_io::intrinsics::addc(b1, b1, carry, carry);
	b2 = ::fast_io::intrinsics::addc(b2, b2, carry, carry);
	b3 = ::fast_io::intrinsics::addc(zero, zero, carry, carry);

	std::uint_least64_t y1;
	x0 = ::fast_io::intrinsics::umul(x0, x0, y1);

	std::uint_least64_t y3;
	x1 = ::fast_io::intrinsics::umul(x1, x1, y3);

	std::uint_least64_t y5;
	x2 = ::fast_io::intrinsics::umul(x2, x2, y5);

	std::uint_least64_t y7;
	x3 = ::fast_io::intrinsics::umul(x3, x3, y7);

	y1 = ::fast_io::intrinsics::addc(a1, y1, false, carry);
	x1 = ::fast_io::intrinsics::addc(a2, x1, carry, carry);
	y3 = ::fast_io::intrinsics::addc(a3, y3, carry, carry);
	x2 = ::fast_io::intrinsics::addc(b0, x2, carry, carry);
	y5 = ::fast_io::intrinsics::addc(b1, y5, carry, carry);
	x3 = ::fast_io::intrinsics::addc(b2, x3, carry, carry);
	y7 = ::fast_io::intrinsics::addc(b3, y7, carry, carry);

	std::uint_least64_t high;
	a0 = muladd_w0(x0, x2, high);
	a1 = muladd_w1(y1, y5, high, high);
	a2 = muladd_w1(x1, x3, high, high);
	a3 = muladd_w1(y3, y7, high, high);

	std::uint_least64_t low{::fast_io::intrinsics::umul(constant, high, high)};

	a0 = ::fast_io::intrinsics::addc(low, a0, false, carry);
	a1 = ::fast_io::intrinsics::addc(high, a1, carry, carry);
	a2 = ::fast_io::intrinsics::addc(zero, a2, carry, carry);
	a3 = ::fast_io::intrinsics::addc(zero, a3, carry, carry);
	low = ::fast_io::intrinsics::subc(low, low, carry, carry);

	low &= constant;
	r.front_unchecked() = ::fast_io::intrinsics::addc(low, a0, false, carry);
	r.index_unchecked(1) = ::fast_io::intrinsics::addc(zero, a1, carry, carry);
	r.index_unchecked(2) = ::fast_io::intrinsics::addc(zero, a2, carry, carry);
	r.back_unchecked() = ::fast_io::intrinsics::addc(zero, a3, carry, carry);
}

/*
Reference from mehdi
The curve used is y2 = x^3 + 486662x^2 + x, a Montgomery curve, over
the prime field defined by the prime number 2^255 - 19, and it uses the
base point x = 9.

Protocol uses compressed elliptic point (only X coordinates), so it
allows for efficient use of the Montgomery ladder for ECDH, using only
XZ coordinates.

The curve is birationally equivalent to Ed25519 (Twisted Edwards curve).

b = 256
p = 2**255 - 19
l = 2**252 + 27742317777372353535851937790883648493
*/

/*
Using Euler theorem to calculate the z^(-2) of an element over a finite field
2^255 - 21
*/

inline constexpr void field_number_pow_minus2(field_number &r, field_number const &z) noexcept
{
	field_number t0, t1, z2, z9, z11;
	field_number z2_5_0, z2_10_0, z2_20_0, z2_50_0, z2_100_0;

	/*
	Montgomery ladder
	*/

	/* 2 */ field_number_square(z2, z);
	/* 4 */ field_number_square(t1, z2);
	/* 8 */ field_number_square(t0, t1);
	/* 9 */ field_number_multiplication(z9, t0, z);
	/* 11 */ field_number_multiplication(z11, z9, z2);
	/* 22 */ field_number_square(t0, z11);
	/* 2^5 - 2^0 = 31 */ field_number_multiplication(z2_5_0, t0, z9);

	/* 2^6 - 2^1 */ field_number_square(t0, z2_5_0);
	/* 2^7 - 2^2 */ field_number_square(t1, t0);
	/* 2^8 - 2^3 */ field_number_square(t0, t1);
	/* 2^9 - 2^4 */ field_number_square(t1, t0);
	/* 2^10 - 2^5 */ field_number_square(t0, t1);
	/* 2^10 - 2^0 */ field_number_multiplication(z2_10_0, t0, z2_5_0);

	/* 2^11 - 2^1 */ field_number_square(t0, z2_10_0);
	/* 2^12 - 2^2 */ field_number_square(t1, t0);
	/* 2^20 - 2^10 */ for (std::uint_fast8_t i{}; i != 4; ++i)
	{
		field_number_square(t0, t1);
		field_number_square(t1, t0);
	}
	/* 2^20 - 2^0 */ field_number_multiplication(z2_20_0, t1, z2_10_0);

	/* 2^21 - 2^1 */ field_number_square(t0, z2_20_0);
	/* 2^22 - 2^2 */ field_number_square(t1, t0);

	/* 2^40 - 2^20 */ for (std::uint_fast8_t i{}; i != 9; ++i)
	{
		field_number_square(t0, t1);
		field_number_square(t1, t0);
	}
	/* 2^40 - 2^0 */ field_number_multiplication(t0, t1, z2_20_0);

	/* 2^41 - 2^1 */ field_number_square(t1, t0);
	/* 2^42 - 2^2 */ field_number_square(t0, t1);
	/* 2^50 - 2^10 */ for (std::uint_fast8_t i{}; i != 4; ++i)
	{
		field_number_square(t1, t0);
		field_number_square(t0, t1);
	}
	/* 2^50 - 2^0 */ field_number_multiplication(z2_50_0, t0, z2_10_0);

	/* 2^51 - 2^1 */ field_number_square(t0, z2_50_0);
	/* 2^52 - 2^2 */ field_number_square(t1, t0);
	/* 2^100 - 2^50 */ for (std::uint_fast8_t i{}; i != 24; ++i)
	{
		field_number_square(t0, t1);
		field_number_square(t1, t0);
	}
	/* 2^100 - 2^0 */ field_number_multiplication(z2_100_0, t1, z2_50_0);

	/* 2^101 - 2^1 */ field_number_square(t1, z2_100_0);
	/* 2^102 - 2^2 */ field_number_square(t0, t1);
	/* 2^200 - 2^100 */ for (std::uint_fast8_t i{}; i != 49; ++i)
	{
		field_number_square(t1, t0);
		field_number_square(t0, t1);
	}
	/* 2^200 - 2^0 */ field_number_multiplication(t1, t0, z2_100_0);

	/* 2^201 - 2^1 */ field_number_square(t0, t1);
	/* 2^202 - 2^2 */ field_number_square(t1, t0);
	/* 2^250 - 2^50 */ for (std::uint_fast8_t i{}; i != 24; ++i)
	{
		field_number_square(t0, t1);
		field_number_square(t1, t0);
	}
	/* 2^250 - 2^0 */ field_number_multiplication(t0, t1, z2_50_0);

	/* 2^251 - 2^1 */ field_number_square(t1, t0);
	/* 2^252 - 2^2 */ field_number_square(t0, t1);
	/* 2^253 - 2^3 */ field_number_square(t1, t0);
	/* 2^254 - 2^4 */ field_number_square(t0, t1);
	/* 2^255 - 2^5 */ field_number_square(t1, t0);
	/* 2^255 - 21 */ field_number_multiplication(r, t1, z11);
}

/*
Using Euler theorem to calculate the inverse of an element over a finite field.
2^255 - 20
*/


inline constexpr field_number pow_minus2(field_number const &z) noexcept
{
	field_number number;
	field_number_pow_minus2(number, z);
	return number;
}

inline constexpr void field_number_reduce_to_25519(field_number &x) noexcept
{
	constexpr std::uint_least64_t c1{UINT64_MAX};
	constexpr std::uint_least64_t c2_constant{c1 - 18};
	constexpr std::uint_least64_t c1r1_constant{c1 >> 1};
	std::uint_least64_t c2{c2_constant};
	std::uint_least64_t c1r1{c1r1_constant};
	std::uint_least64_t t{};
	std::uint_least64_t r0, r1, r2, r3;
	bool carry{};
	r0 = ::fast_io::intrinsics::subc(x.front_unchecked(), c2, false, carry);
	r1 = ::fast_io::intrinsics::subc(x.index_unchecked(1), c1, carry, carry);
	r2 = ::fast_io::intrinsics::subc(x.index_unchecked(2), c1, carry, carry);
	r3 = ::fast_io::intrinsics::subc(x.back_unchecked(), c1r1, carry, carry);
	t = ::fast_io::intrinsics::subc(t, t, carry, carry);
	// undo if carry is true
	c1r1 &= t;
	c2 &= t;

	r0 = ::fast_io::intrinsics::addc(r0, c2, false, carry);
	r1 = ::fast_io::intrinsics::addc(r1, t, carry, carry);
	r2 = ::fast_io::intrinsics::addc(r2, t, carry, carry);
	r3 = ::fast_io::intrinsics::addc(r3, c1r1, carry, carry);

	c1r1 = c1r1_constant;
	c2 = c2_constant;

	// we need to do this 2nd time.
	r0 = ::fast_io::intrinsics::subc(r0, c2, false, carry);
	r1 = ::fast_io::intrinsics::subc(r1, c1, carry, carry);
	r2 = ::fast_io::intrinsics::subc(r2, c1, carry, carry);
	r3 = ::fast_io::intrinsics::subc(r3, c1r1, carry, carry);
	// undo if carry is true
	t = ::fast_io::intrinsics::subc(t, t, carry, carry);
	c1r1 &= t;
	c2 &= t;
	x.front_unchecked() = ::fast_io::intrinsics::addc(r0, c2, false, carry);
	x.index_unchecked(1) = ::fast_io::intrinsics::addc(r1, t, carry, carry);
	x.index_unchecked(2) = ::fast_io::intrinsics::addc(r2, t, carry, carry);
	x.back_unchecked() = ::fast_io::intrinsics::addc(r3, c1r1, carry, carry);
}

inline constexpr void field_number_inverse(field_number &r, field_number const &z) noexcept
{
	field_number_pow_minus2(r, z);
	field_number_reduce_to_25519(r);
}

inline constexpr void field_multiplication_mod(field_number &z, field_number const &x, field_number const &y) noexcept
{
	field_number_multiplication(z, x, y);
	field_number_reduce_to_25519(z);
}

inline constexpr field_number inverse(field_number const &z) noexcept
{
	field_number number;
	field_number_inverse(number, z);
	return number;
}

inline constexpr field_number operator*(field_number const &x, field_number const &y) noexcept
{
	field_number f;
	field_multiplication_mod(f, x, y);
	return f;
}

struct xz_point
{
	field_number x, z;
};

inline constexpr ::fast_io::containers::array<std::byte, 32> montgomery_curve_base_point_x{std::byte{9}};

/* Y = X + X */
inline constexpr void montgomery_curve_point_double(xz_point &y, xz_point const &x) noexcept
{
	field_number a, b;
	/*  x2 = (x+z)^2 * (x-z)^2 */
	/*  z2 = ((x+z)^2 - (x-z)^2)*((x+z)^2 + ((A-2)/4)((x+z)^2 - (x-z)^2)) */
	field_number_addition(a, x.x, x.z);
	field_number_subtraction(b, x.x, x.z);
	field_number_square(a, a);
	field_number_square(b, b);
	field_number_multiplication(y.x, a, b);
	field_number_subtraction(b, a, b);

	constexpr std::uint_least64_t constant{121665};
	multiplication_add_reduce(a, a, constant, b);
	field_number_multiplication(y.z, a, b);
}

inline constexpr void montgomery_curve_mont(xz_point &p, xz_point &q, field_number const &base) noexcept
{
	field_number a, b, c, d, e;
	field_number_subtraction(a, p.x, p.z);
	field_number_addition(b, p.x, p.z);
	field_number_subtraction(c, q.x, q.z);
	field_number_addition(d, q.x, q.z);
	field_number_multiplication(a, a, d);
	field_number_multiplication(b, b, c);
	field_number_addition(e, a, b);
	field_number_subtraction(b, a, b);
	field_number_square(p.x, e);
	field_number_square(a, b);
	field_number_multiplication(p.z, a, base);

	field_number_square(a, d);
	field_number_square(b, c);

	field_number_multiplication(q.x, a, b);
	field_number_subtraction(b, a, b);

	constexpr std::uint_least64_t constant{121665};
	multiplication_add_reduce(a, a, constant, b);
	field_number_multiplication(q.z, a, b);
}

inline
#if __cpp_lib_bit_cast >= 201806L && __cpp_lib_is_constant_evaluated >= 201811L
	constexpr
#endif
	void montgomery_curve_point_multiplication(std::byte *public_key, std::byte const *base_point, std::byte const *secret_key, field_number const &zr) noexcept
{
	constexpr std::size_t total_len{32};
	::fast_io::containers::array<std::byte, 32> sk;
	::fast_io::freestanding::nonoverlapped_bytes_copy_n(secret_key, total_len, sk.data());
	field_number x;
	::fast_io::freestanding::type_punning_from_bytes(base_point, x);
	xz_point p, q;
	::fast_io::containers::array<xz_point *, 2> pp;
	::fast_io::containers::array<xz_point *, 2> qp;
	for (std::size_t j{total_len}; j--;)
	{
		char unsigned k{static_cast<char unsigned>(sk.index_unchecked(j))};
		for (std::size_t i{}; i < 8; ++i)
		{
			if (k & 0x80)
			{
				addition_u256_discard(p.z, x, zr);
				field_number_multiplication(p.x, x, p.z);
				montgomery_curve_point_double(q, p);
				pp.front_unchecked() = __builtin_addressof(q);
				pp.back_unchecked() = __builtin_addressof(p);
				qp.front_unchecked() = __builtin_addressof(p);
				qp.back_unchecked() = __builtin_addressof(q);
				auto mont{[&](char unsigned n) noexcept -> void {
					int unsigned chosen{static_cast<int unsigned>((k >> n) & 1)};
					montgomery_curve_mont(*pp.index_unchecked(chosen), *qp.index_unchecked(chosen), x);
				}};
				for (; ++i < 8; mont(7))
				{
					k <<= 1;
				}
				for (; j--;)
				{
					k = static_cast<char unsigned>(sk.index_unchecked(j));
					for (char unsigned m{8}; m--; mont(m))
						;
				}
				field_number_inverse(q.z, p.z);
				field_multiplication_mod(x, p.x, q.z);

				::fast_io::freestanding::type_punning_to_bytes(x, public_key);
				return;
			}
			k <<= 1;
		}
	}
	::fast_io::none_secure_clear(public_key, total_len);
}

inline
#if __cpp_lib_bit_cast >= 201806L && __cpp_lib_is_constant_evaluated >= 201811L
	constexpr
#endif
	void montgomery_curve_point_multiplication(std::byte *public_key, std::byte const *base_point, std::byte const *secret_key) noexcept
{
	montgomery_curve_point_multiplication(public_key, base_point, secret_key, ::fast_io::curve25519::custom_blindings::zr);
}

namespace details
{

inline constexpr void x25519_trim_secret_key(::fast_io::containers::index_span<::std::byte, 32> x) noexcept
{
	x.front_unchecked() = std::byte{static_cast<char8_t>(0xf8 & static_cast<char8_t>(x.front_unchecked()))};
	x.back_unchecked() = std::byte{static_cast<char8_t>((static_cast<char8_t>(x.back_unchecked()) | 0x40) & 0x7f)};
}

} // namespace details

} // namespace fast_io::curve25519

namespace fast_io::diffie_hellman::details
{

inline constexpr std::size_t key_size{32};

inline constexpr ::fast_io::containers::array<std::byte, 32> base_point{std::byte{9}};

inline constexpr void calculate_public_key_to_ptr(std::byte *pk, std::byte *sk) noexcept
{
	::fast_io::curve25519::details::x25519_trim_secret_key(::fast_io::containers::index_span<::std::byte, 32>{::fast_io::containers::index_unchecked, sk});
	::fast_io::curve25519::montgomery_curve_point_multiplication(pk, base_point.data(), sk);
}

inline constexpr void calculate_public_key_to_ptr_with_zr(std::byte *pk, std::byte *sk, ::fast_io::curve25519::field_number const &zr) noexcept
{
	::fast_io::curve25519::details::x25519_trim_secret_key(::fast_io::containers::index_span<::std::byte, 32>{::fast_io::containers::index_unchecked, sk});
	::fast_io::curve25519::montgomery_curve_point_multiplication(pk, base_point.data(), sk, zr);
}

inline constexpr void create_shared_key_to_ptr(std::byte *shared, std::byte const *pk, std::byte *sk) noexcept
{
	::fast_io::curve25519::details::x25519_trim_secret_key(::fast_io::containers::index_span<::std::byte, 32>{::fast_io::containers::index_unchecked, sk});
	::fast_io::curve25519::montgomery_curve_point_multiplication(shared, pk, sk);
}

inline constexpr void create_shared_key_to_ptr_with_zr(std::byte *shared, std::byte const *pk, std::byte *sk, ::fast_io::curve25519::field_number const &zr) noexcept
{
	::fast_io::curve25519::details::x25519_trim_secret_key(::fast_io::containers::index_span<::std::byte, 32>{::fast_io::containers::index_unchecked, sk});
	::fast_io::curve25519::montgomery_curve_point_multiplication(shared, pk, sk, zr);
}

inline constexpr void calculate_public_key(::fast_io::containers::index_span<std::byte, 32> public_key, ::fast_io::containers::index_span<std::byte, 32> secret_key) noexcept
{
	::fast_io::diffie_hellman::details::calculate_public_key_to_ptr(public_key.data(), secret_key.data());
}

} // namespace fast_io::diffie_hellman::details
