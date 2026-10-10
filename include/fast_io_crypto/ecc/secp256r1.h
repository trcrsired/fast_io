#pragma once

/*
secp256r1 (NIST P-256, prime256v1) and secp384r1 (NIST P-384) --
ECDSA for TLS 1.3 signature_scheme::ecdsa_secp256r1_sha256 (0x0403)
and ecdsa_secp384r1_sha384 (0x0503).

  P-256: p = 2^256 - 2^224 + 2^96 + 2^64 - 1
  P-384: p = 2^384 - 2^128 - 2^96 + 2^32 - 1
  y^2 = x^3 - 3x + b over both

Field (mod p) and scalar (mod n) arithmetic run on the RSA big-number
Montgomery machinery (fast_io::details::rsa::limbs_* / montgomery_*)
with a runtime limb count; neither order is a Solinas prime so there
is no fast-reduction shortcut for them anyway, and going Montgomery
everywhere keeps one reduction path. Points are Jacobian projective
(x = X/Z^2, y = Y/Z^3) so the group law needs no inversion until the
final affine conversion; inverses are Fermat little-theorem
exponentiations a^(m-2) mod m over the fixed exponents p-2 / n-2
(constant bit pattern -- the variable-time pow loops on the exponent,
not the base, so no secret leaks through it).

This file is freestanding: limb arrays and free functions only.
*/

namespace fast_io::ecc
{

namespace details
{

using ec_limb = ::fast_io::details::rsa::value_type;
inline constexpr ::std::size_t ec_max_limbs{8}; /* room through P-521 */

/* a short-Weierstrass curve: p, order n, generator, the two Fermat
   inverse exponents as big-endian byte strings */
struct ec_curve_params
{
	ec_limb const *p{};
	ec_limb const *n{};
	ec_limb const *b{};
	ec_limb const *gx{};
	ec_limb const *gy{};
	::std::byte const *pm2{}; /* p - 2, byte_count bytes big-endian */
	::std::byte const *nm2{}; /* n - 2 */
	::std::size_t limb_count{};
	::std::size_t byte_count{};
};

/* Montgomery context for one modulus, runtime limb count */
struct ec_mont_ctx
{
	ec_limb modulus[ec_max_limbs]{};
	::std::size_t nl{};
	ec_limb n0inv{};
	ec_limb r2[ec_max_limbs]{};
	ec_limb one[ec_max_limbs]{}; /* R mod m -- the montgomery 1 */

	inline constexpr ec_mont_ctx() noexcept = default;
	inline constexpr ec_mont_ctx(ec_limb const *m, ::std::size_t nl) noexcept
		: nl{nl}
	{
		for (::std::size_t i{}; i != nl; ++i)
		{
			modulus[i] = m[i];
		}
		n0inv = ::fast_io::details::rsa::montgomery_n0_inverse(m[0]);
		/*
		r2 = R^2 mod m = 2^(2*nl*64) mod m. montgomery_r2_setup /
		limbs_pow2_mod's limb-at-a-time quotient estimate drifts for
		this modulus shape (p384), so compute it the plain way: 2nl*64
		doublings of 1, each conditional-subtracted. That is ~768
		nl-limb steps -- trivial next to a handshake.
		*/
		ec_limb d[ec_max_limbs];
		r2[0] = 1;
		for (::std::size_t i{1}; i != nl; ++i)
		{
			r2[i] = 0;
		}
		for (::std::size_t i{2 * nl * ::fast_io::details::rsa::limb_digits}; i--;)
		{
			::fast_io::details::rsa::limbs_mod_double(r2, modulus, d, nl);
		}
		ec_limb t[2 * ec_max_limbs + 1]{};
		ec_limb u[ec_max_limbs]{};
		u[0] = 1;
		::fast_io::details::rsa::montgomery_multiplication(one, u, r2, modulus, n0inv, nl, t);
	}
};

/* a curve carries both its field and order contexts */
struct ec_curve
{
	ec_mont_ctx field{};
	ec_mont_ctx order{};
	ec_limb gx[ec_max_limbs]{};
	ec_limb gy[ec_max_limbs]{};
	::std::byte const *pm2{};
	::std::byte const *nm2{};
	::std::size_t nl{};
	::std::size_t nbytes{};

	inline constexpr ec_curve() noexcept = default;
	inline constexpr explicit ec_curve(ec_curve_params const &c) noexcept
		: field{c.p, c.limb_count}, order{c.n, c.limb_count},
		  pm2{c.pm2}, nm2{c.nm2}, nl{c.limb_count}, nbytes{c.byte_count}
	{
		for (::std::size_t i{}; i != nl; ++i)
		{
			gx[i] = c.gx[i];
			gy[i] = c.gy[i];
		}
	}
};

/* z = x + y mod m (domain-blind: works on Montgomery values too) */
inline constexpr void ec_add(ec_limb *z, ec_limb const *x, ec_limb const *y,
							 ec_limb const *m, ::std::size_t nl) noexcept
{
	ec_limb const carry{::fast_io::details::rsa::limbs_addition(z, x, y, nl)};
	if (carry || !::fast_io::details::rsa::limbs_less(z, m, nl))
	{
		::fast_io::details::rsa::limbs_subtraction(z, z, m, nl);
	}
}

/* z = x - y mod m */
inline constexpr void ec_sub(ec_limb *z, ec_limb const *x, ec_limb const *y,
							 ec_limb const *m, ::std::size_t nl) noexcept
{
	if (::fast_io::details::rsa::limbs_subtraction(z, x, y, nl))
	{
		::fast_io::details::rsa::limbs_addition(z, z, m, nl);
	}
}

/* z = 2x mod m */
inline constexpr void ec_add2(ec_limb *z, ec_limb const *x,
							  ec_limb const *m, ::std::size_t nl) noexcept
{
	ec_add(z, x, x, m, nl);
}

/* z = x*y mod m in the Montgomery domain (x, y already Montgomery) */
inline constexpr void ec_mul(ec_mont_ctx const &ctx,
							 ec_limb *z, ec_limb const *x, ec_limb const *y) noexcept
{
	ec_limb t[2 * ec_max_limbs + 1];
	::fast_io::details::rsa::montgomery_multiplication(z, x, y, ctx.modulus, ctx.n0inv,
													   ctx.nl, t);
}

inline constexpr void ec_sqr(ec_mont_ctx const &ctx,
							 ec_limb *z, ec_limb const *x) noexcept
{
	ec_mul(ctx, z, x, x);
}

/* normal-domain -> Montgomery domain */
inline constexpr void ec_to_mont(ec_mont_ctx const &ctx,
								 ec_limb *z, ec_limb const *x) noexcept
{
	ec_mul(ctx, z, x, ctx.r2);
}

/* Montgomery domain -> normal domain: mont_mul(x, 1) = x*R^-1 */
inline constexpr void ec_from_mont(ec_mont_ctx const &ctx,
								   ec_limb *z, ec_limb const *x) noexcept
{
	ec_limb const one[ec_max_limbs]{1};
	ec_mul(ctx, z, x, one);
}

/* z = x^-1 mod m -- Fermat a^(m-2); e is the m-2 byte string.
   x is normal domain; montgomery_pow does the domain round-trips. */
inline constexpr void ec_invert(ec_mont_ctx const &ctx,
								ec_limb *z, ec_limb const *x,
								::std::byte const *e, ::std::size_t e_size) noexcept
{
	ec_limb scratch[5 * ec_max_limbs + 2];
	::fast_io::details::rsa::montgomery_pow(z, x, e, e_size, ctx.modulus, ctx.r2,
											ctx.n0inv, ctx.nl, scratch);
}

/* z = x*y mod m, x and y normal domain, z normal domain:
   mont_mul(xR, y) = x*y*R^-1*R = x*y */
inline constexpr void ec_mul_plain(ec_mont_ctx const &ctx,
								   ec_limb *z, ec_limb const *x, ec_limb const *y) noexcept
{
	ec_limb xm[ec_max_limbs];
	ec_to_mont(ctx, xm, x);
	ec_mul(ctx, xm, xm, y);
	for (::std::size_t i{}; i != ctx.nl; ++i)
	{
		z[i] = xm[i];
	}
}

/* ---------------- group law (Jacobian, a = -3) ---------------- */

struct ec_point
{
	ec_limb x[ec_max_limbs]{};
	ec_limb y[ec_max_limbs]{};
	ec_limb z[ec_max_limbs]{}; /* z == 0: the point at infinity */
};

inline constexpr bool ec_is_infinity(ec_mont_ctx const &f, ec_point const &pt) noexcept
{
	return ::fast_io::details::rsa::limbs_is_zero(pt.z, f.nl);
}

/*
double: delta = Z^2, gamma = Y^2, beta = X*gamma, alpha = 3(X-d)(X+d),
X' = alpha^2 - 8beta, Z' = (Y+Z)^2 - gamma - delta,
Y' = alpha(4beta - X') - 8gamma^2. All Montgomery-domain field ops.
*/
inline constexpr void ec_double(ec_mont_ctx const &f, ec_point &pt) noexcept
{
	if (ec_is_infinity(f, pt))
	{
		return;
	}
	ec_limb delta[ec_max_limbs], gamma[ec_max_limbs], beta[ec_max_limbs], alpha[ec_max_limbs];
	ec_limb t0[ec_max_limbs], t1[ec_max_limbs];
	ec_sqr(f, delta, pt.z);
	ec_sqr(f, gamma, pt.y);
	ec_mul(f, beta, pt.x, gamma);
	ec_sub(t0, pt.x, delta, f.modulus, f.nl);
	ec_add(t1, pt.x, delta, f.modulus, f.nl);
	ec_mul(f, t0, t0, t1);
	ec_add2(alpha, t0, f.modulus, f.nl);
	ec_add(alpha, alpha, t0, f.modulus, f.nl); /* alpha = 3(X^2 - Z^4) */
	/* x' = alpha^2 - 8*beta */
	ec_limb x3[ec_max_limbs];
	ec_sqr(f, x3, alpha);
	ec_add2(t0, beta, f.modulus, f.nl);
	ec_add2(t0, t0, f.modulus, f.nl);
	ec_add2(t1, t0, f.modulus, f.nl); /* 8beta */
	ec_sub(x3, x3, t1, f.modulus, f.nl);
	/* z' = (Y+Z)^2 - gamma - delta = 2YZ */
	ec_add(t1, pt.y, pt.z, f.modulus, f.nl);
	ec_sqr(f, t1, t1);
	ec_sub(t1, t1, gamma, f.modulus, f.nl);
	ec_sub(t1, t1, delta, f.modulus, f.nl);
	for (::std::size_t i{}; i != f.nl; ++i)
	{
		pt.z[i] = t1[i];
	}
	/* y' = alpha(4beta - x') - 8gamma^2 */
	ec_sub(t0, t0, x3, f.modulus, f.nl); /* 4beta - x' */
	ec_mul(f, t0, alpha, t0);
	ec_sqr(f, t1, gamma);
	ec_add2(t1, t1, f.modulus, f.nl);
	ec_add2(t1, t1, f.modulus, f.nl);
	ec_add2(t1, t1, f.modulus, f.nl); /* 8gamma^2 */
	ec_sub(t0, t0, t1, f.modulus, f.nl);
	for (::std::size_t i{}; i != f.nl; ++i)
	{
		pt.x[i] = x3[i];
		pt.y[i] = t0[i];
	}
}

/*
add (Jacobian + Jacobian): U1 = X1 Z2^2, U2 = X2 Z1^2, S1 = Y1 Z2^3,
S2 = Y2 Z1^3, H = U2-U1, R = S2-S1;
X3 = R^2 - H^3 - 2 U1 H^2, Y3 = R(U1 H^2 - X3) - S1 H^3, Z3 = Z1 Z2 H.
Infinity inputs pass through; equal inputs double; negatives give
infinity.
*/
inline constexpr void ec_add_point(ec_mont_ctx const &f,
								   ec_point &a, ec_point const &b) noexcept
{
	if (ec_is_infinity(f, a))
	{
		a = b;
		return;
	}
	if (ec_is_infinity(f, b))
	{
		return;
	}
	ec_limb z1z1[ec_max_limbs], z2z2[ec_max_limbs], u1[ec_max_limbs], u2[ec_max_limbs];
	ec_limb s1[ec_max_limbs], s2[ec_max_limbs], h[ec_max_limbs], r[ec_max_limbs];
	ec_limb t0[ec_max_limbs];
	ec_sqr(f, z1z1, a.z);
	ec_sqr(f, z2z2, b.z);
	ec_mul(f, u1, a.x, z2z2);
	ec_mul(f, u2, b.x, z1z1);
	ec_mul(f, s1, a.y, b.z);
	ec_mul(f, s1, s1, z2z2);
	ec_mul(f, s2, b.y, a.z);
	ec_mul(f, s2, s2, z1z1);
	ec_sub(h, u2, u1, f.modulus, f.nl);
	ec_sub(r, s2, s1, f.modulus, f.nl);
	if (::fast_io::details::rsa::limbs_is_zero(h, f.nl))
	{
		if (::fast_io::details::rsa::limbs_is_zero(r, f.nl))
		{
			ec_double(f, a);
		}
		else
		{
			/* p + (-p) = infinity */
			for (::std::size_t i{}; i != f.nl; ++i)
			{
				a.x[i] = 0;
				a.y[i] = 0;
				a.z[i] = 0;
			}
		}
		return;
	}
	ec_limb hh[ec_max_limbs], hhh[ec_max_limbs], v[ec_max_limbs];
	ec_sqr(f, hh, h);
	ec_mul(f, hhh, h, hh);
	ec_mul(f, v, u1, hh);
	ec_limb x3[ec_max_limbs], y3[ec_max_limbs], z3[ec_max_limbs];
	ec_sqr(f, x3, r);
	ec_sub(x3, x3, hhh, f.modulus, f.nl);
	ec_add2(t0, v, f.modulus, f.nl);
	ec_sub(x3, x3, t0, f.modulus, f.nl);
	ec_sub(t0, v, x3, f.modulus, f.nl);
	ec_mul(f, y3, r, t0);
	ec_mul(f, t0, s1, hhh);
	ec_sub(y3, y3, t0, f.modulus, f.nl);
	ec_mul(f, z3, a.z, b.z);
	ec_mul(f, z3, z3, h);
	for (::std::size_t i{}; i != f.nl; ++i)
	{
		a.x[i] = x3[i];
		a.y[i] = y3[i];
		a.z[i] = z3[i];
	}
}

/* scalar*P -- double-and-add, most significant bit first.
   scalar is a big-endian byte string; the result stays Jacobian. */
inline constexpr void ec_scalar_mul(ec_mont_ctx const &f,
									ec_point &out,
									::std::byte const *scalar, ::std::size_t scalar_size,
									ec_point const &pt) noexcept
{
	for (::std::size_t i{}; i != ec_max_limbs; ++i)
	{
		out.x[i] = 0;
		out.y[i] = 0;
		out.z[i] = 0;
	}
	for (::std::size_t byte_i{}; byte_i != scalar_size; ++byte_i)
	{
		unsigned const b{static_cast<unsigned>(scalar[byte_i])};
		for (unsigned bit{8}; bit--;)
		{
			ec_double(f, out);
			if (((b >> bit) & 1u) != 0)
			{
				ec_add_point(f, out, pt);
			}
		}
	}
}

/* the affine x of a Jacobian point, back in the normal domain.
   Returns false at infinity. */
inline constexpr bool ec_affine_x(ec_curve const &c,
								  ec_limb *x, ec_point const &pt) noexcept
{
	if (ec_is_infinity(c.field, pt))
	{
		return false;
	}
	ec_limb zn[ec_max_limbs], zinv[ec_max_limbs], zinv2[ec_max_limbs];
	ec_from_mont(c.field, zn, pt.z);
	ec_invert(c.field, zinv, zn, c.pm2, c.nbytes);
	ec_to_mont(c.field, zinv, zinv);
	ec_sqr(c.field, zinv2, zinv);
	ec_mul(c.field, x, pt.x, zinv2);
	ec_from_mont(c.field, x, x);
	return true;
}

/* load a point's affine coordinates into Montgomery domain */
inline constexpr bool ec_point_load(ec_curve const &c, ec_point &pt,
									ec_limb const *x, ec_limb const *y) noexcept
{
	namespace nr = ::fast_io::details::rsa;
	if (!nr::limbs_less(x, c.field.modulus, c.nl) ||
		!nr::limbs_less(y, c.field.modulus, c.nl))
	{
		return false;
	}
	ec_to_mont(c.field, pt.x, x);
	ec_to_mont(c.field, pt.y, y);
	for (::std::size_t i{}; i != c.nl; ++i)
	{
		pt.z[i] = c.field.one[i];
	}
	return true;
}

} // namespace details

/* ---------------- curves ---------------- */

inline constexpr details::ec_limb secp256r1_p[]{
	0xffffffffffffffffull, 0x00000000ffffffffull, 0x0000000000000000ull, 0xffffffff00000001ull};
inline constexpr details::ec_limb secp256r1_n[]{
	0xf3b9cac2fc632551ull, 0xbce6faada7179e84ull, 0xffffffffffffffffull, 0xffffffff00000000ull};
inline constexpr details::ec_limb secp256r1_b[]{
	0x3bce3c3e27d2604bull, 0x651d06b0cc53b0f6ull, 0xb3ebbd55769886bcull, 0x5ac635d8aa3a93e7ull};
inline constexpr details::ec_limb secp256r1_gx[]{
	0xf4a13945d898c296ull, 0x77037d812deb33a0ull, 0xf8bce6e563a440f2ull, 0x6b17d1f2e12c4247ull};
inline constexpr details::ec_limb secp256r1_gy[]{
	0xcbb6406837bf51f5ull, 0x2bce33576b315eceull, 0x8ee7eb4a7c0f9e16ull, 0x4fe342e2fe1a7f9bull};
inline constexpr ::std::byte secp256r1_pm2[]{
	::std::byte{0xff}, ::std::byte{0xff}, ::std::byte{0xff}, ::std::byte{0xff},
	::std::byte{0x00}, ::std::byte{0x00}, ::std::byte{0x00}, ::std::byte{0x01},
	::std::byte{0x00}, ::std::byte{0x00}, ::std::byte{0x00}, ::std::byte{0x00},
	::std::byte{0x00}, ::std::byte{0x00}, ::std::byte{0x00}, ::std::byte{0x00},
	::std::byte{0x00}, ::std::byte{0x00}, ::std::byte{0x00}, ::std::byte{0x00},
	::std::byte{0xff}, ::std::byte{0xff}, ::std::byte{0xff}, ::std::byte{0xff},
	::std::byte{0xff}, ::std::byte{0xff}, ::std::byte{0xff}, ::std::byte{0xff},
	::std::byte{0xff}, ::std::byte{0xff}, ::std::byte{0xff}, ::std::byte{0xfd}};
inline constexpr ::std::byte secp256r1_nm2[]{
	::std::byte{0xff}, ::std::byte{0xff}, ::std::byte{0xff}, ::std::byte{0xff},
	::std::byte{0x00}, ::std::byte{0x00}, ::std::byte{0x00}, ::std::byte{0x00},
	::std::byte{0xff}, ::std::byte{0xff}, ::std::byte{0xff}, ::std::byte{0xff},
	::std::byte{0xff}, ::std::byte{0xff}, ::std::byte{0xff}, ::std::byte{0xff},
	::std::byte{0xbc}, ::std::byte{0xe6}, ::std::byte{0xfa}, ::std::byte{0xad},
	::std::byte{0xa7}, ::std::byte{0x17}, ::std::byte{0x9e}, ::std::byte{0x84},
	::std::byte{0xf3}, ::std::byte{0xb9}, ::std::byte{0xca}, ::std::byte{0xc2},
	::std::byte{0xfc}, ::std::byte{0x63}, ::std::byte{0x25}, ::std::byte{0x4f}};

inline constexpr details::ec_curve secp256r1{details::ec_curve_params{
	secp256r1_p, secp256r1_n, secp256r1_b, secp256r1_gx, secp256r1_gy,
	secp256r1_pm2, secp256r1_nm2, 4, 32}};

inline constexpr details::ec_limb secp384r1_p[]{
	0x00000000ffffffffull, 0xffffffff00000000ull, 0xfffffffffffffffeull,
	0xffffffffffffffffull, 0xffffffffffffffffull, 0xffffffffffffffffull};
inline constexpr details::ec_limb secp384r1_n[]{
	0xecec196accc52973ull, 0x581a0db248b0a77aull, 0xc7634d81f4372ddfull,
	0xffffffffffffffffull, 0xffffffffffffffffull, 0xffffffffffffffffull};
inline constexpr details::ec_limb secp384r1_b[]{
	0x2a85c8edd3ec2aefull, 0xc656398d8a2ed19dull, 0x0314088f5013875aull,
	0x181d9c6efe814112ull, 0x988e056be3f82d19ull, 0xb3312fa7e23ee7e4ull};
inline constexpr details::ec_limb secp384r1_gx[]{
	0x3a545e3872760ab7ull, 0x5502f25dbf55296cull, 0x59f741e082542a38ull,
	0x6e1d3b628ba79b98ull, 0x8eb1c71ef320ad74ull, 0xaa87ca22be8b0537ull};
inline constexpr details::ec_limb secp384r1_gy[]{
	0x7a431d7c90ea0e5full, 0x0a60b1ce1d7e819dull, 0xe9da3113b5f0b8c0ull,
	0xf8f41dbd289a147cull, 0x5d9e98bf9292dc29ull, 0x3617de4a96262c6full};
inline constexpr ::std::byte secp384r1_pm2[]{
	::std::byte{0xff}, ::std::byte{0xff}, ::std::byte{0xff}, ::std::byte{0xff},
	::std::byte{0xff}, ::std::byte{0xff}, ::std::byte{0xff}, ::std::byte{0xff},
	::std::byte{0xff}, ::std::byte{0xff}, ::std::byte{0xff}, ::std::byte{0xff},
	::std::byte{0xff}, ::std::byte{0xff}, ::std::byte{0xff}, ::std::byte{0xff},
	::std::byte{0xff}, ::std::byte{0xff}, ::std::byte{0xff}, ::std::byte{0xff},
	::std::byte{0xff}, ::std::byte{0xff}, ::std::byte{0xff}, ::std::byte{0xff},
	::std::byte{0xff}, ::std::byte{0xff}, ::std::byte{0xff}, ::std::byte{0xff},
	::std::byte{0xff}, ::std::byte{0xff}, ::std::byte{0xff}, ::std::byte{0xfe},
	::std::byte{0xff}, ::std::byte{0xff}, ::std::byte{0xff}, ::std::byte{0xff},
	::std::byte{0x00}, ::std::byte{0x00}, ::std::byte{0x00}, ::std::byte{0x00},
	::std::byte{0x00}, ::std::byte{0x00}, ::std::byte{0x00}, ::std::byte{0x00},
	::std::byte{0xff}, ::std::byte{0xff}, ::std::byte{0xff}, ::std::byte{0xfd}};
inline constexpr ::std::byte secp384r1_nm2[]{
	::std::byte{0xff}, ::std::byte{0xff}, ::std::byte{0xff}, ::std::byte{0xff},
	::std::byte{0xff}, ::std::byte{0xff}, ::std::byte{0xff}, ::std::byte{0xff},
	::std::byte{0xff}, ::std::byte{0xff}, ::std::byte{0xff}, ::std::byte{0xff},
	::std::byte{0xff}, ::std::byte{0xff}, ::std::byte{0xff}, ::std::byte{0xff},
	::std::byte{0xff}, ::std::byte{0xff}, ::std::byte{0xff}, ::std::byte{0xff},
	::std::byte{0xff}, ::std::byte{0xff}, ::std::byte{0xff}, ::std::byte{0xff},
	::std::byte{0xc7}, ::std::byte{0x63}, ::std::byte{0x4d}, ::std::byte{0x81},
	::std::byte{0xf4}, ::std::byte{0x37}, ::std::byte{0x2d}, ::std::byte{0xdf},
	::std::byte{0x58}, ::std::byte{0x1a}, ::std::byte{0x0d}, ::std::byte{0xb2},
	::std::byte{0x48}, ::std::byte{0xb0}, ::std::byte{0xa7}, ::std::byte{0x7a},
	::std::byte{0xec}, ::std::byte{0xec}, ::std::byte{0x19}, ::std::byte{0x6a},
	::std::byte{0xcc}, ::std::byte{0xc5}, ::std::byte{0x29}, ::std::byte{0x71}};

inline constexpr details::ec_curve secp384r1{details::ec_curve_params{
	secp384r1_p, secp384r1_n, secp384r1_b, secp384r1_gx, secp384r1_gy,
	secp384r1_pm2, secp384r1_nm2, 6, 48}};

/* ---------------- public ECDSA surface ---------------- */

/*
ECDSA verify (FIPS 186-5 6.4.4): the caller decoded the signature
SEQUENCE { INTEGER r, INTEGER s } -- r and s arrive as big-endian byte
strings (a DER INTEGER may carry a leading zero). pubkey is the SEC1
uncompressed point 04 || X || Y (1 + 2*nbytes). digest is the
already-hashed message; the leftmost curve bits count.
*/
inline bool ecdsa_verify_to_ptr(details::ec_curve const &c,
								::std::byte const *r, ::std::size_t r_size,
								::std::byte const *s, ::std::size_t s_size,
								::std::byte const *pubkey, ::std::size_t pubkey_size,
								::std::byte const *digest, ::std::size_t digest_size) noexcept
{
	using namespace ::fast_io::ecc::details;
	namespace nr = ::fast_io::details::rsa;
	if (pubkey_size != 2 * c.nbytes + 1 || pubkey[0] != ::std::byte{0x04})
	{
		return false;
	}
	ec_limb rv[ec_max_limbs], sv[ec_max_limbs], zv[ec_max_limbs];
	if (!nr::limbs_from_bytes_be(rv, r, r_size, c.nl) ||
		!nr::limbs_from_bytes_be(sv, s, s_size, c.nl) ||
		nr::limbs_is_zero(rv, c.nl) || nr::limbs_is_zero(sv, c.nl) ||
		!nr::limbs_less(rv, c.order.modulus, c.nl) ||
		!nr::limbs_less(sv, c.order.modulus, c.nl))
	{
		return false;
	}
	::std::size_t const take{digest_size < c.nbytes ? digest_size : c.nbytes};
	if (!nr::limbs_from_bytes_be(zv, digest, take, c.nl))
	{
		return false;
	}
	if (!nr::limbs_less(zv, c.order.modulus, c.nl))
	{
		nr::limbs_subtraction(zv, zv, c.order.modulus, c.nl);
	}
	ec_mont_ctx const &o{c.order};
	ec_limb w[ec_max_limbs];
	ec_invert(o, w, sv, c.nm2, c.nbytes);
	ec_limb u1[ec_max_limbs], u2[ec_max_limbs];
	ec_mul_plain(o, u1, zv, w); /* z*w mod n */
	ec_mul_plain(o, u2, rv, w); /* r*w mod n */
	::std::byte u1b[64], u2b[64];
	nr::limbs_to_bytes_be(u1b, u1, c.nbytes, c.nl);
	nr::limbs_to_bytes_be(u2b, u2, c.nbytes, c.nl);
	ec_point g, q;
	ec_limb qx[ec_max_limbs], qy[ec_max_limbs];
	if (!nr::limbs_from_bytes_be(qx, pubkey + 1, c.nbytes, c.nl) ||
		!nr::limbs_from_bytes_be(qy, pubkey + 1 + c.nbytes, c.nbytes, c.nl))
	{
		return false;
	}
	ec_point_load(c, g, c.gx, c.gy);
	if (!ec_point_load(c, q, qx, qy))
	{
		return false;
	}
	/* P = u1*G + u2*Q */
	ec_point p1, p2;
	ec_scalar_mul(c.field, p1, u1b, c.nbytes, g);
	ec_scalar_mul(c.field, p2, u2b, c.nbytes, q);
	ec_add_point(c.field, p1, p2);
	ec_limb ax[ec_max_limbs];
	if (!ec_affine_x(c, ax, p1))
	{
		return false;
	}
	/* v = x mod n; p < 2n so one conditional subtract suffices */
	if (!nr::limbs_less(ax, c.order.modulus, c.nl))
	{
		nr::limbs_subtraction(ax, ax, c.order.modulus, c.nl);
	}
	for (::std::size_t i{}; i != c.nl; ++i)
	{
		if (ax[i] != rv[i])
		{
			return false;
		}
	}
	return true;
}

/*
ECDSA sign (FIPS 186-5 6.4.1): privkey is the d bytes, k the
per-message secret (each curve byte_count), digest the hashed message.
The signature lands DER-encoded in sig_out (2 + 2*byte_count + 6
bytes max); *sig_size receives its length. Returns false when the key
is out of range or k produced a degenerate (r or s == 0, k >= n)
signature -- the caller resamples k and retries.
*/
inline bool ecdsa_sign_to_ptr(details::ec_curve const &c,
							  ::std::byte *sig_out, ::std::size_t *sig_size,
							  ::std::byte const *privkey, ::std::byte const *k,
							  ::std::byte const *digest, ::std::size_t digest_size) noexcept
{
	using namespace ::fast_io::ecc::details;
	namespace nr = ::fast_io::details::rsa;
	ec_limb dv[ec_max_limbs], kv[ec_max_limbs], zv[ec_max_limbs];
	if (!nr::limbs_from_bytes_be(dv, privkey, c.nbytes, c.nl) ||
		!nr::limbs_from_bytes_be(kv, k, c.nbytes, c.nl) ||
		nr::limbs_is_zero(dv, c.nl) || nr::limbs_is_zero(kv, c.nl) ||
		!nr::limbs_less(dv, c.order.modulus, c.nl) ||
		!nr::limbs_less(kv, c.order.modulus, c.nl))
	{
		return false;
	}
	::std::size_t const take{digest_size < c.nbytes ? digest_size : c.nbytes};
	if (!nr::limbs_from_bytes_be(zv, digest, take, c.nl))
	{
		return false;
	}
	if (!nr::limbs_less(zv, c.order.modulus, c.nl))
	{
		nr::limbs_subtraction(zv, zv, c.order.modulus, c.nl);
	}
	ec_mont_ctx const &o{c.order};
	/* R = k*G, r = R.x mod n */
	ec_point g;
	ec_point_load(c, g, c.gx, c.gy);
	ec_point rp;
	ec_scalar_mul(c.field, rp, k, c.nbytes, g);
	ec_limb rv[ec_max_limbs];
	if (!ec_affine_x(c, rv, rp))
	{
		return false;
	}
	if (!nr::limbs_less(rv, c.order.modulus, c.nl))
	{
		nr::limbs_subtraction(rv, rv, c.order.modulus, c.nl);
	}
	if (nr::limbs_is_zero(rv, c.nl))
	{
		return false;
	}
	/* s = k^-1 (z + r*d) mod n */
	ec_limb kinv[ec_max_limbs], rd[ec_max_limbs], sv[ec_max_limbs], sum[ec_max_limbs];
	ec_invert(o, kinv, kv, c.nm2, c.nbytes);
	ec_mul_plain(o, rd, rv, dv);
	ec_to_mont(o, sum, zv);
	ec_to_mont(o, rd, rd);
	ec_add(sum, sum, rd, o.modulus, c.nl);
	ec_from_mont(o, sum, sum);
	ec_mul_plain(o, sv, kinv, sum);
	if (nr::limbs_is_zero(sv, c.nl))
	{
		return false;
	}
	/* DER: SEQUENCE { INTEGER r, INTEGER s } */
	::std::byte rb[64], sb[64];
	nr::limbs_to_bytes_be(rb, rv, c.nbytes, c.nl);
	nr::limbs_to_bytes_be(sb, sv, c.nbytes, c.nl);
	::std::size_t const nb{c.nbytes};
	auto put_int{[nb](::std::byte *p, ::std::byte const *v) noexcept {
		::std::size_t off{};
		while (off + 1 < nb && v[off] == ::std::byte{})
		{
			++off;
		}
		::std::size_t const n{nb - off};
		bool const neg{(static_cast<unsigned>(v[off]) & 0x80u) != 0u};
		*p++ = ::std::byte{0x02};
		*p++ = static_cast<::std::byte>(n + static_cast<::std::size_t>(neg));
		if (neg)
		{
			*p++ = ::std::byte{};
		}
		for (::std::size_t i{}; i != n; ++i)
		{
			*p++ = v[off + i];
		}
		return p;
	}};
	::std::byte *p{sig_out};
	::std::byte *const body{p + 2};
	p = put_int(body, rb);
	p = put_int(p, sb);
	sig_out[0] = ::std::byte{0x30};
	sig_out[1] = static_cast<::std::byte>(p - body);
	*sig_size = static_cast<::std::size_t>(p - sig_out);
	return true;
}

} // namespace fast_io::ecc
