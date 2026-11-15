#pragma once

#include "engine.h"

#include <limits>

namespace fast_io::random::details
{

// Port of libc++'s std::generate_canonical, which won the bernoulli benchmark
// against libstdc++ and the MSVC STL. Produces a value in [0, 1) with `bits`
// bits of precision, using the engine's full range and no rejection loop:
//
//   k     = ceil(bits / floor(log2(range_count)))
//   value = sum over i<k of (draw_i - min) * range_count^i
//   value / range_count^k
//
// `engine_bit_width<G>` is floor(log2(G::max() - G::min() + 1)), including the
// full-64-bit-range case. Drawing goes through the engine seam, so generate()
// is preferred and the throws clause follows exactly as elsewhere.
template <typename RealType, unsigned bits, typename G>
inline constexpr RealType generate_canonical(G &g)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!details::engine_draw_no_throw<G>)
{
	constexpr unsigned digits{
		static_cast<unsigned>(::std::numeric_limits<RealType>::digits)};
	constexpr unsigned wanted{digits < bits ? digits : bits};
	constexpr unsigned log_range{details::engine_bit_width<G>};
	constexpr unsigned count{
		wanted / log_range + (wanted % log_range != 0) + (wanted == 0)};

	using E = details::engine_unsigned<G>;
	RealType const range_count{
		static_cast<RealType>(G::max() - G::min()) + RealType(1)};
	RealType base{range_count};
	RealType sum{
		static_cast<RealType>(details::draw_engine(g) - static_cast<E>(G::min()))};
	for (unsigned i{1}; i != count; ++i, base *= range_count)
	{
		sum += static_cast<RealType>(
				   details::draw_engine(g) - static_cast<E>(G::min())) *
			   base;
	}
	return sum / base;
}

} // namespace fast_io::random::details
