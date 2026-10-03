module;

#include <fast_io_freestanding.h>

/*
Shortest round-trip floating-point conversion, separately compiled.

Implements the nearest-even shortest-decimal conversion for binary16,
bfloat16, binary32, binary64, binary80 and binary128.  Narrow formats
(mantissa <= 24 bits) use B = 2^34 fixed point, binary64 uses B = 2^64, and
the wide formats compute the cached power of ten by square-and-multiply in
320-bit fixed point so that no large table is needed for them.
Round-to-nearest-ties-to-even is the only supported policy.

The 12 KiB conversion cache (2048 exponent-shift bytes + 618 normalized
128-bit power-of-ten significands) is embedded from da_cache.bin, which is
generated and verified by gen_da_cache.py.  Compilers without #embed support
cannot build this module; users who do not link it simply get no floating
point output.

The conversion functions are defined in the global module fragment so they
have ordinary external linkage: the declarations in
fast_io_unit/floating/roundtrip.h resolve against this object.
*/

#if !defined(__has_embed) || (__has_embed("da_cache.bin") != __STDC_EMBED_FOUND__)
#error "fast_io floating-point module requires a compiler with #embed support"
#endif

#if defined(__clang__) && __has_warning("-Wc23-extensions")
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc23-extensions"
#endif

namespace fast_io::details
{

// ---------------------------------------------------------------------------
// embedded cache layout (little-endian, produced by gen_da_cache.py):
//   [0, 2048)         one shift byte per raw binary64 exponent field,
//                     s(raw) = compute_exponent_shift(e, d+1) + extra_shift
//                     where e = raw - 1075 (raw 0 maps to e = -1074),
//                     d = floor(e * log10(2)), extra_shift = 6
//   [2048, 2048+9888) 618 x {hi, lo} u64 pairs: the normalized 128-bit
//                     significand of 10^q for q = index - 293, top bit set
// ---------------------------------------------------------------------------

inline constexpr unsigned char floating_decimal_cache[]{
#embed "da_cache.bin"
};

#if defined(__clang__) && __has_warning("-Wc23-extensions")
#pragma clang diagnostic pop
#endif

inline constexpr ::std::uint_least64_t floating_cache_u64(::std::size_t offset) noexcept
{
	auto const *p{floating_decimal_cache + offset};
	return ::std::uint_least64_t{p[0]} | (::std::uint_least64_t{p[1]} << 8u) |
		   (::std::uint_least64_t{p[2]} << 16u) | (::std::uint_least64_t{p[3]} << 24u) |
		   (::std::uint_least64_t{p[4]} << 32u) | (::std::uint_least64_t{p[5]} << 40u) |
		   (::std::uint_least64_t{p[6]} << 48u) | (::std::uint_least64_t{p[7]} << 56u);
}

struct floating_pow10
{
	::std::uint_least64_t hi;
	::std::uint_least64_t lo;
};

// normalized 128-bit lower-endpoint significand of 10^k, k in [-293, 324]
inline constexpr floating_pow10 floating_pow10_get(::std::int_least32_t k) noexcept
{
	auto const offset{2048u + (static_cast<::std::uint_least32_t>(k + 293) << 4u)};
	return {floating_cache_u64(offset), floating_cache_u64(offset + 8u)};
}

// high 64 bits of x*y + c, i.e. floor((x*y + c) / 2^64)
inline constexpr ::std::uint_least64_t floating_umul_add_hi64(::std::uint_least64_t x, ::std::uint_least64_t y,
															::std::uint_least64_t c) noexcept
{
	::std::uint_least64_t high;
	auto const low{::fast_io::intrinsics::umul(x, y, high)};
	auto const sum{low + c};
	return high + (sum < low);
}

struct floating_product
{
	::std::uint_least64_t hi;
	::std::uint_least64_t lo;
};

// bits [64, 191] of the 192-bit product x * (hi:lo), returned as {hi64, mid64}
inline constexpr floating_product floating_umul64x128_hi(::std::uint_least64_t x, ::std::uint_least64_t lo,
													   ::std::uint_least64_t hi) noexcept
{
	::std::uint_least64_t top;
	auto const mid0{::fast_io::intrinsics::umul(x, hi, top)};
	auto const mid{mid0 + ::fast_io::intrinsics::umulh(x, lo)};
	return {top + (mid < mid0), mid};
}

// ---------------------------------------------------------------------------
// conversion core.  A = I + F/B is the source scaled by 10^{-(d+1)} into
// cell units of the coarser of the two candidate grids.  With B = 2^64 the
// endpoint membership predicates are
//
//     F < H      <=>  I   in R(x)
//     B - F <= H <=>  I+1 in R(x)
//
// where H is the scaled cell half-width with +even closing both midpoint
// endpoints for an even significand.  When neither holds, the nearest
// fine-grid digit is round(10F/B), with the B/4 -> 2.5 -> even-2 exception.
// ---------------------------------------------------------------------------

// Shared tail: significand (with implicit bit for normals), binary exponent
// e = raw - offset, and the effective raw table index (irregular ignores it).
inline m10_result<::std::uint_least64_t>
floating_to_decimal_binary64_common(::std::uint_least64_t significand, ::std::int_least32_t binary_exponent,
									::std::uint_least32_t effective_raw_exponent, bool regular) noexcept
{
	::std::uint_least64_t integral;
	::std::uint_least32_t digit;
	::std::int_least32_t decimal_exponent;
	bool has_last_digit;
	if (regular) [[likely]]
	{
		// floor(e * log10(2)) == (e * 78913) >> 18 exhaustively on [-1074, 971]
		decimal_exponent = (binary_exponent * 78913) >> 18u;
		auto const shift{static_cast<::std::uint_least32_t>(floating_decimal_cache[effective_raw_exponent])};
		auto const power{floating_pow10_get(-decimal_exponent - 1)};
		auto const product{floating_umul64x128_hi(significand << shift, power.lo, power.hi)};
		integral = product.hi >> 6u;
		auto const fractional{(product.hi << 58u) | (product.lo >> 6u)};
		auto const half_ulp{(power.hi >> (7u - shift)) + (1u - (significand & 1u))};
		auto const round_up{static_cast<bool>(fractional + half_ulp < fractional)};
		auto const round_down{static_cast<bool>(half_ulp > fractional)};
		integral += round_up;
		digit = static_cast<::std::uint_least32_t>(
			floating_umul_add_hi64(fractional, 10u, (::std::uint_least64_t{1} << 63u) + 6u));
		if (fractional == (::std::uint_least64_t{1} << 62u))
		{
			// 10F/B = 2.5 is the only exact half where add-half selects the odd
			// upper digit; nearest-even requires 2.
			digit = 2u;
		}
		has_last_digit = !(round_up || round_down);
	}
	else
	{
		// exact power of two: asymmetric cell [x - 2^(e-2), x + 2^(e-1)], so
		// the decimal exponent uses floor(log10(3/4 * 2^e)) and the lower
		// radius is half the upper radius.
		decimal_exponent = (binary_exponent * 315653 - 131072) >> 20u;
		auto const shift{static_cast<::std::uint_least32_t>(
			binary_exponent + (((-decimal_exponent - 1) * 217707) >> 16) + 7)};
		auto const power{floating_pow10_get(-decimal_exponent - 1)};
		auto const product{floating_umul64x128_hi(significand << shift, power.lo, power.hi)};
		integral = product.hi >> 6u;
		auto const fractional{(product.hi << 58u) | (product.lo >> 6u)};
		auto const half_ulp{power.hi >> (7u - shift)};
		auto const round_up{static_cast<bool>(half_ulp > ~(::std::uint_least64_t{0}) - fractional)};
		// the power-of-two significand is even, so the closer lower boundary
		// is closed: membership is inclusive here (F <= H/2, not F < H/2)
		auto const round_down{static_cast<bool>((half_ulp >> 1u) >= fractional)};
		integral += round_up;
		digit = static_cast<::std::uint_least32_t>(
			floating_umul_add_hi64(fractional, 10u, (::std::uint_least64_t{1} << 63u) - 1u));
		auto const lower{static_cast<::std::uint_least32_t>(
			floating_umul_add_hi64(fractional - (half_ulp >> 1u), 10u, ~(::std::uint_least64_t{0})))};
		if (digit < lower)
		{
			// the nearest tenth lay below the asymmetric cell's closer lower
			// boundary; select its first member
			digit = lower;
		}
		has_last_digit = !(round_up || round_down);
	}
	::std::uint_least64_t m10;
	::std::int_least32_t e10;
	if (has_last_digit)
	{
		m10 = integral * 10u + digit;
		e10 = decimal_exponent;
	}
	else
	{
		m10 = integral;
		e10 = decimal_exponent + 1;
	}
	if (m10 % 10u == 0u) [[unlikely]]
	{
		auto const [v, n]{::fast_io::bitops::rtz_iec559(m10)};
		m10 = v;
		e10 += static_cast<::std::int_least32_t>(n);
	}
	return {m10, e10};
}

// (raw mantissa, raw exponent field) -> {m10, e10}, shortest round-trip
m10_result<::std::uint_least64_t>
to_decimal_binary64(::std::uint_least64_t m2, ::std::int_least32_t e2) noexcept
{
	if (e2 == 0)
	{
		// subnormal: m2 * 2^-1074 == m2 * 2^(1-1075); the regular cell holds
		// because the subnormal half-width never fills a cell
		return floating_to_decimal_binary64_common(m2, -1074, 1u, true);
	}
	if (m2 == 0)
	{
		return floating_to_decimal_binary64_common(::std::uint_least64_t{1} << 52u, e2 - 1075,
												   static_cast<::std::uint_least32_t>(e2), false);
	}
	return floating_to_decimal_binary64_common(m2 | (::std::uint_least64_t{1} << 52u), e2 - 1075,
											   static_cast<::std::uint_least32_t>(e2), true);
}

// ---------------------------------------------------------------------------
// narrow formats (mantissa <= 24 bits): B = 2^34, one 64x64->128 product.
// Template parameters are the explicit significand bits, the exponent field
// bits, and the offset into the shared binary64 shift table
// (1075 - bias - mantissa_bits).
// ---------------------------------------------------------------------------

template <::std::uint_least32_t mantissa_bits, ::std::uint_least32_t exponent_bits,
		  ::std::uint_least32_t shift_table_offset>
inline m10_result<::std::uint_least32_t>
floating_to_decimal_narrow_common(::std::uint_least32_t significand, ::std::int_least32_t binary_exponent,
								  ::std::uint_least32_t effective_raw_exponent, bool regular) noexcept
{
	::std::uint_least64_t integral;
	::std::uint_least32_t digit;
	::std::int_least32_t decimal_exponent;
	bool has_last_digit;
	if (regular) [[likely]]
	{
		decimal_exponent = (binary_exponent * 78913) >> 18u;
		// the shared binary64 shift table is indexed at raw + offset; +28 moves
		// the guard width from 6 to 34
		auto const shift{static_cast<::std::uint_least32_t>(
			floating_decimal_cache[effective_raw_exponent + shift_table_offset] + 28u)};
		auto const power_high{floating_pow10_get(-decimal_exponent - 1).hi};
		auto const product{::fast_io::intrinsics::umulh(
			power_high + 1u, static_cast<::std::uint_least64_t>(significand) << shift)};
		auto const fractional{product & ((::std::uint_least64_t{1} << 34u) - 1u)};
		auto const half_ulp{(power_high >> (65u - shift)) + (1u - (significand & 1u))};
		auto const round_up{static_cast<bool>((fractional + half_ulp) >> 34u)};
		auto const round_down{static_cast<bool>(half_ulp > fractional)};
		integral = (product >> 34u) + round_up;
		digit = static_cast<::std::uint_least32_t>(
			(fractional * 10u + (::std::uint_least64_t{1} << 33u)) >> 34u);
		if (fractional == (::std::uint_least64_t{1} << 32u))
		{
			digit = 2u;
		}
		has_last_digit = !(round_up || round_down);
	}
	else
	{
		// For narrow powers of two the closed lower cell boundary can itself
		// be a decimal member of R at a scale one coarser than the working
		// grid; the fixed-point predicates assume an open lower edge and miss
		// it.  This only occurs when 2^(mbits+2)-1 supplies a factor of five,
		// which holds for binary16 (4095) but not bfloat16 (511) or binary32.
		// Find the coarsest scale with a member inside [v - 2^(e-2),
		// v + 2^(e-1)] exactly and take the member nearest to v.
		if constexpr (((1u << (mantissa_bits + 2u)) - 1u) % 5u == 0u)
		{
			if (binary_exponent >= 3)
			{
				auto const lo{static_cast<::std::uint_least64_t>((1u << (mantissa_bits + 2u)) - 1u)
							  << (binary_exponent - 2)};
				auto const hi{static_cast<::std::uint_least64_t>((1u << (mantissa_bits + 1u)) + 1u)
							  << (binary_exponent - 1)};
				auto const value{::std::uint_least64_t{1}
								 << (mantissa_bits + static_cast<::std::uint_least32_t>(binary_exponent))};
				::std::uint_least64_t scale{1u};
				::std::int_least32_t scale_exp{};
				for (;;)
				{
					auto const next{scale * 10u};
					auto const candidate_lo{(lo + next - 1u) / next};
					auto const candidate_hi{hi / next};
					if (candidate_lo > candidate_hi)
					{
						break;
					}
					scale = next;
					++scale_exp;
				}
				if (scale != 1u)
				{
					auto const candidate_lo{(lo + scale - 1u) / scale};
					auto const candidate_hi{hi / scale};
					auto candidate{(value + (scale >> 1u)) / scale};
					candidate = candidate < candidate_lo ? candidate_lo
						: candidate > candidate_hi		   ? candidate_hi
														   : candidate;
					return {static_cast<::std::uint_least32_t>(candidate), scale_exp};
				}
			}
		}
		// narrow powers of two share the binary64 asymmetric path
		auto const r{floating_to_decimal_binary64_common(
			significand, binary_exponent, effective_raw_exponent, false)};
		return {static_cast<::std::uint_least32_t>(r.m10), r.e10};
	}
	::std::uint_least32_t m10;
	::std::int_least32_t e10;
	if (has_last_digit)
	{
		m10 = static_cast<::std::uint_least32_t>(integral * 10u + digit);
		e10 = decimal_exponent;
	}
	else
	{
		m10 = static_cast<::std::uint_least32_t>(integral);
		e10 = decimal_exponent + 1;
	}
	if (m10 % 10u == 0u) [[unlikely]]
	{
		auto const [v, n]{::fast_io::bitops::rtz_iec559(m10)};
		m10 = v;
		e10 += static_cast<::std::int_least32_t>(n);
	}
	return {m10, e10};
}

template <::std::uint_least32_t mantissa_bits, ::std::uint_least32_t exponent_bits,
		  ::std::uint_least32_t shift_table_offset>
inline m10_result<::std::uint_least32_t>
floating_to_decimal_narrow(::std::uint_least32_t m2, ::std::int_least32_t e2) noexcept
{
	constexpr ::std::int_least32_t exponent_offset{
		static_cast<::std::int_least32_t>((1u << (exponent_bits - 1u)) + mantissa_bits - 1u)};
	constexpr ::std::uint_least32_t implicit_bit{1u << mantissa_bits};
	if (e2 == 0)
	{
		return floating_to_decimal_narrow_common<mantissa_bits, exponent_bits, shift_table_offset>(
			m2, 1 - exponent_offset, 1u, true);
	}
	if (m2 == 0)
	{
		return floating_to_decimal_narrow_common<mantissa_bits, exponent_bits, shift_table_offset>(
			implicit_bit, static_cast<::std::int_least32_t>(e2) - exponent_offset,
			static_cast<::std::uint_least32_t>(e2), false);
	}
	return floating_to_decimal_narrow_common<mantissa_bits, exponent_bits, shift_table_offset>(
		m2 | implicit_bit, static_cast<::std::int_least32_t>(e2) - exponent_offset,
		static_cast<::std::uint_least32_t>(e2), true);
}

m10_result<::std::uint_least32_t>
to_decimal_binary32(::std::uint_least32_t m2, ::std::int_least32_t e2) noexcept
{
	return floating_to_decimal_narrow<23u, 8u, 925u>(m2, e2);
}

m10_result<::std::uint_least32_t>
to_decimal_binary16(::std::uint_least32_t m2, ::std::int_least32_t e2) noexcept
{
	return floating_to_decimal_narrow<10u, 5u, 1050u>(m2, e2);
}

m10_result<::std::uint_least32_t>
to_decimal_bfloat16(::std::uint_least32_t m2, ::std::int_least32_t e2) noexcept
{
	return floating_to_decimal_narrow<7u, 8u, 941u>(m2, e2);
}

#if defined(__SIZEOF_INT128__)

// ---------------------------------------------------------------------------
// wide formats (binary80, binary128): the cached power of ten is computed by
// square-and-multiply in 320-bit fixed point, so no table is needed.
// Multi-limb arithmetic uses the same add-with-carry style as curve25519.
// All limb arrays are little-endian uint_least64_t.
// ---------------------------------------------------------------------------

inline constexpr ::std::size_t floating_big_work_limbs{5u};  // 320 bits
inline constexpr ::std::size_t floating_big_pow10_limbs{4u}; // 256 bits
inline constexpr ::std::int_least32_t floating_big_work_bits{320};

// out[0..na+nb) = a[0..na) * b[0..nb), out must be zeroed by the caller
inline void floating_limb_mul(::std::uint_least64_t const *a, ::std::size_t na,
							  ::std::uint_least64_t const *b, ::std::size_t nb,
							  ::std::uint_least64_t *out) noexcept
{
	for (::std::size_t i{}; i != na; ++i)
	{
		::std::uint_least64_t carry{};
		for (::std::size_t j{}; j != nb; ++j)
		{
			::std::uint_least64_t high;
			auto const low{::fast_io::intrinsics::umul(a[i], b[j], high)};
			bool c1, c2;
			auto const sum{::fast_io::intrinsics::addc(out[i + j], low, false, c1)};
			out[i + j] = ::fast_io::intrinsics::addc(sum, carry, false, c2);
			// high <= 2^64 - 2 and c1 + c2 <= 2 can never wrap carry to zero:
			// high is maximal only when low == 1, and then c1 + c2 <= 1.
			carry = high + static_cast<::std::uint_least64_t>(c1) + static_cast<::std::uint_least64_t>(c2);
		}
		out[i + nb] = carry;
	}
}

// out[0..outn) = bits [shift, shift + 64*outn) of a[0..n)
inline void floating_limb_extract(::std::uint_least64_t const *a, ::std::size_t n,
								  ::std::uint_least32_t shift, ::std::uint_least64_t *out,
								  ::std::size_t outn) noexcept
{
	auto const word_shift{shift >> 6u};
	auto const bit_shift{shift & 63u};
	for (::std::size_t i{}; i != outn; ++i)
	{
		auto const low{word_shift + i < n ? a[word_shift + i] : ::std::uint_least64_t{}};
		auto const high{word_shift + i + 1u < n ? a[word_shift + i + 1u] : ::std::uint_least64_t{}};
		out[i] = bit_shift ? (low >> bit_shift | high << (64u - bit_shift)) : low;
	}
}

// r = a * b normalized so that the top bit of r[4] is set; returns the binary
// exponent shift applied to the product
inline ::std::int_least32_t floating_limb_fmul(::std::uint_least64_t const *a,
											   ::std::uint_least64_t const *b,
											   ::std::uint_least64_t *r) noexcept
{
	::std::uint_least64_t product[2u * floating_big_work_limbs]{};
	floating_limb_mul(a, floating_big_work_limbs, b, floating_big_work_limbs, product);
	// both operands are normalized, so the product MSB is bit 638 or 639
	auto const shift{floating_big_work_bits - 1 +
					 static_cast<::std::int_least32_t>(product[2u * floating_big_work_limbs - 1u] >> 63u)};
	floating_limb_extract(product, 2u * floating_big_work_limbs,
						  static_cast<::std::uint_least32_t>(shift), r, floating_big_work_limbs);
	return shift;
}

// r = base**n at 320-bit precision (a lower approximation of base**n);
// returns rexp with r * 2**rexp <= base**n.  base is clobbered.
inline ::std::int_least32_t floating_fpow(::std::uint_least64_t *base, ::std::int_least32_t base_exp,
										  ::std::uint_least32_t n, ::std::uint_least64_t *r) noexcept
{
	r[0] = r[1] = r[2] = r[3] = 0;
	r[4] = ::std::uint_least64_t{1} << 63u; // 1.0
	auto rexp{1 - floating_big_work_bits};
	while (n != 0u)
	{
		if ((n & 1u) != 0u)
		{
			rexp += base_exp + floating_limb_fmul(r, base, r);
		}
		n >>= 1u;
		if (n != 0u)
		{
			base_exp = 2 * base_exp + floating_limb_fmul(base, base, base);
		}
	}
	return rexp;
}

// m[0..4) = floor(10**x / 2**e) normalized to 256 bits (top bit set), for
// |x| <= 6000; returns e.
inline ::std::int_least32_t floating_pow10_big(::std::int_least32_t x, ::std::uint_least64_t *m) noexcept
{
	::std::uint_least64_t p[floating_big_work_limbs];
	::std::int_least32_t power_exponent;
	if (x >= 0)
	{
		::std::uint_least64_t five[floating_big_work_limbs]{};
		five[floating_big_work_limbs - 1u] = ::std::uint_least64_t{5} << 61u;
		power_exponent = floating_fpow(five, 3 - floating_big_work_bits,
									   static_cast<::std::uint_least32_t>(x), p);
	}
	else
	{
		::std::uint_least64_t inv5[floating_big_work_limbs];
		for (auto &limb : inv5)
		{
			limb = ::std::uint_least64_t{0xccccccccccccccccu}; // floor(2**322 / 5)
		}
		power_exponent = floating_fpow(inv5, -floating_big_work_bits - 2,
									   static_cast<::std::uint_least32_t>(-x), p);
	}
	// truncate p to 256 bits; the guard limbs absorb fpow rounding to an
	// exact floor
	for (::std::size_t k{}; k != floating_big_pow10_limbs; ++k)
	{
		m[k] = p[k + (floating_big_work_limbs - floating_big_pow10_limbs)];
	}
	return power_exponent + static_cast<::std::int_least32_t>(
								(floating_big_work_limbs - floating_big_pow10_limbs) * 64u) + x;
}

struct floating_big_result
{
	__uint128_t significand;
	::std::int_least32_t exponent;
};

// (full significand as two limbs, binary exponent, regular) -> {m10, e10}.
// significand is the complete significand: for normals it already includes
// the implicit/integer bit; the caller also tells whether the cell is the
// symmetric regular one or the asymmetric power-of-two one.
inline floating_big_result
floating_to_decimal_big(::std::uint_least64_t significand_low, ::std::uint_least64_t significand_high,
						::std::int_least32_t binary_exponent, bool regular) noexcept
{
	// dec_exp = floor(bin_exp * log10(2) [+ log10(3/4) at a power of two]);
	// scaling by 10^-dec_exp puts the ulp in [1, 10) so the shortest form is
	// the integral part or a neighbor.
	constexpr ::std::int_least64_t log10_2_sig{20201781};  // round(log10(2) * 2^26)
	constexpr ::std::int_least64_t log10_3_4_sig{8384497}; // round(-log10(3/4) * 2^26)
	auto const decimal_exponent{static_cast<::std::int_least32_t>(
		(static_cast<::std::int_least64_t>(binary_exponent) * log10_2_sig -
		 (regular ? ::std::int_least64_t{} : log10_3_4_sig)) >>
		26u)};

	::std::uint_least64_t power_sig[floating_big_pow10_limbs];
	auto const power_exponent{floating_pow10_big(-decimal_exponent, power_sig)};
	// value * 10^-dec_exp = significand * power_sig / 2^shift
	auto const shift{static_cast<::std::uint_least32_t>(-(binary_exponent + power_exponent))};

	::std::uint_least64_t const significand_limbs[2]{significand_low, significand_high};
	::std::uint_least64_t product[2u + floating_big_pow10_limbs]{};
	floating_limb_mul(significand_limbs, 2u, power_sig, floating_big_pow10_limbs, product);
	::std::uint_least64_t scaled[4];
	floating_limb_extract(product, 2u + floating_big_pow10_limbs, shift - 128u, scaled, 4u);
	auto const integral{(static_cast<__uint128_t>(scaled[3]) << 64u) | scaled[2]};
	auto const fractional{(static_cast<__uint128_t>(scaled[1]) << 64u) | scaled[0]};
	auto const last_digit{static_cast<::std::uint_least64_t>(integral % 10u)};

	// half_ulp = floor(power_sig / 2^(shift-123)): half a binary ulp in cell
	// units
	::std::uint_least64_t half_ulp_limbs[2];
	floating_limb_extract(power_sig, floating_big_pow10_limbs, shift - 123u, half_ulp_limbs, 2u);
	auto const half_ulp{(static_cast<__uint128_t>(half_ulp_limbs[1]) << 64u) | half_ulp_limbs[0]};

	auto const candidate{(static_cast<__uint128_t>(last_digit) << 124u) | (fractional >> 4u)};
	constexpr __uint128_t half{static_cast<__uint128_t>(1) << 127u};
	auto const even{static_cast<bool>((significand_low & 1u) == 0u)};

	// round_up keeps all digits and rounds the significand to nearest;
	// trim_down drops the last digit; trim_up drops it and carries to the
	// next ten.  Exact boundaries are detected by equality and broken toward
	// an even significand.
	bool round_up, trim_down;
	if (regular)
	{
		round_up = fractional >= half;
		if (fractional == half)
		{
			round_up = (static_cast<::std::uint_least64_t>(integral) & 1u) != 0u;
		}
		trim_down = candidate <= half_ulp;
		if (candidate == half_ulp)
		{
			// 124-bit tie: the low 64 bits break it, to even on exact match
			::std::uint_least64_t ulp_low;
			floating_limb_extract(power_sig, floating_big_pow10_limbs, shift - 127u, &ulp_low, 1u);
			auto const fraction_low{static_cast<::std::uint_least64_t>(fractional)};
			trim_down = fraction_low == ulp_low ? even : fraction_low < ulp_low;
		}
	}
	else
	{
		round_up = fractional > half;
		auto const quarter_ulp{half_ulp >> 1u};
		if ((fractional >> 4u) > quarter_ulp)
		{
			round_up = true;
		}
		trim_down = candidate <= quarter_ulp;
	}

	// trim_up iff value + half_ulp reaches the next ten, i.e. candidate +
	// half_ulp reaches ten; compared as candidate >= ten - half_ulp so the
	// sum cannot overflow 128 bits.  A boundary (gap in {0,1}) breaks to
	// even, guarded by decimal_exponent == 0 for the exact gap == 0 case.
	constexpr __uint128_t ten{static_cast<__uint128_t>(10) << 124u};
	auto trim_up{static_cast<bool>(candidate >= ten - half_ulp)};
	auto const gap{ten - half_ulp - candidate};
	if (gap <= 1u && (decimal_exponent == 0 || gap == 1u))
	{
		trim_up = even;
	}

	auto significand{trim_down || trim_up ? integral - last_digit +
											  static_cast<::std::uint_least64_t>(trim_up) * 10u
										: integral + round_up};
	auto exponent{decimal_exponent};
	if (significand % 10u == 0u) [[unlikely]]
	{
		do
		{
			significand /= 10u;
			++exponent;
		} while (significand % 10u == 0u);
	}
	return {significand, exponent};
}

// (raw mantissa, raw exponent field) -> {m10, e10} for IEEE binary128
m10_result<__uint128_t>
to_decimal_binary128(__uint128_t m2, ::std::int_least32_t e2) noexcept
{
	constexpr ::std::int_least32_t exponent_offset{16495};
	constexpr __uint128_t implicit_bit{static_cast<__uint128_t>(1) << 112u};
	floating_big_result result;
	if (e2 == 0)
	{
		result = floating_to_decimal_big(static_cast<::std::uint_least64_t>(m2),
										 static_cast<::std::uint_least64_t>(m2 >> 64u),
										 1 - exponent_offset, true);
	}
	else if (m2 == 0)
	{
		// implicit bit 2^112 is bit 48 of the high significand limb
		result = floating_to_decimal_big(0, ::std::uint_least64_t{1} << 48u, e2 - exponent_offset, false);
	}
	else
	{
		auto const significand{m2 | implicit_bit};
		result = floating_to_decimal_big(static_cast<::std::uint_least64_t>(significand),
										 static_cast<::std::uint_least64_t>(significand >> 64u),
										 e2 - exponent_offset, true);
	}
	return {result.significand, result.exponent};
}

// (raw mantissa without the explicit integer bit, raw exponent field) ->
// {m10, e10} for x87 binary80.  The caller masks the stored significand's
// integer bit off, matching the ieee559 decomposition convention.
m10_result<__uint128_t>
to_decimal_binary80(::std::uint_least64_t m2, ::std::int_least32_t e2) noexcept
{
	constexpr ::std::int_least32_t exponent_offset{16446};
	constexpr ::std::uint_least64_t explicit_bit{::std::uint_least64_t{1} << 63u};
	floating_big_result result;
	if (e2 == 0)
	{
		result = floating_to_decimal_big(m2, 0, 1 - exponent_offset, true);
	}
	else if (m2 == 0)
	{
		result = floating_to_decimal_big(explicit_bit, 0, e2 - exponent_offset, false);
	}
	else
	{
		result = floating_to_decimal_big(m2 | explicit_bit, 0, e2 - exponent_offset, true);
	}
	return {result.significand, result.exponent};
}

#endif

} // namespace fast_io::details

export module fast_io.floating;
