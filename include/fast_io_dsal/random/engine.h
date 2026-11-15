#pragma once

#include <fast_io_dsal/impl/misc/push_macros.h>

#include <bit>
#include <concepts>
#include <cstdint>
#include <limits>
#include <type_traits>

namespace fast_io::random
{

// Minimal counterpart of std::uniform_random_bit_generator, kept local to
// this header so a distribution does not need <random>.
//
// Unlike the standard concept, a draw may come from either operator() or
// generate(); fast_io engines are allowed to expose only generate() so that
// they can report herbceptions.
template <typename G>
concept uniform_random_bit_generator =
	requires {
		typename G::result_type;
		requires ::std::unsigned_integral<typename G::result_type>;
		{ G::min() } -> ::std::same_as<typename G::result_type>;
		{ G::max() } -> ::std::same_as<typename G::result_type>;
		requires ::std::bool_constant<(G::min() < G::max())>::value;
	} &&
	(requires(G &g) {
     { g() } -> ::std::convertible_to<typename G::result_type>; } || requires(G &g) {
     { g.generate() } -> ::std::convertible_to<typename G::result_type>; });

namespace details
{

// [rand.req.genl]/1.4: a RealType parameter is float, double or long double,
// cv-unqualified.
template <typename T>
inline constexpr bool valid_real_type =
	::std::same_as<T, float> || ::std::same_as<T, double> ||
	::std::same_as<T, long double>;

// [rand.req.genl]/1.5 names short/int/long/long long and their unsigned
// forms. The least-width aliases are accepted as an extension, exactly like
// libc++'s int8_t/uint8_t extension. bool and anything wider than 64 bits are
// rejected.
template <typename T>
inline constexpr bool valid_int_type =
	::std::integral<T> && !::std::same_as<T, bool> &&
	::std::same_as<T, ::std::remove_cv_t<T>> &&
	sizeof(T) <= sizeof(::std::uint_least64_t);

// The unsigned type a single engine draw lives in.
template <typename G>
using engine_unsigned = ::std::make_unsigned_t<typename G::result_type>;

// An engine may expose generate() instead of (or in addition to) operator().
// generate() is allowed to report herbceptions, so the distribution prefers
// it: that is the error channel the caller actually wants.
template <typename G>
concept engine_has_generate = requires(G &g) {
	{ g.generate() } -> ::std::convertible_to<engine_unsigned<G>>;
};

template <typename G>
concept engine_generate_no_throw = requires(G &g) {
	{ g.generate() } noexcept -> ::std::convertible_to<engine_unsigned<G>>;
};

template <typename G>
concept engine_call_no_throw = requires(G &g) {
	{ g() } noexcept -> ::std::convertible_to<engine_unsigned<G>>;
};

// When generate() exists its specification alone decides the distribution's
// throws clause; operator() is irrelevant, even if it is marked throws.
template <typename G>
inline constexpr bool engine_draw_no_throw =
	engine_has_generate<G> ? engine_generate_no_throw<G> : engine_call_no_throw<G>;

// Engines yield values in [min, max], so there are `span + 1` outcomes. Only a
// power-of-two bucket of those outcomes can be split into independent bits, so
// `2^engine_bit_width` is taken as the bucket and draws outside it are
// rejected. For a typical engine (min == 0, max == 2^w - 1) the bucket is the
// whole range and nothing is ever rejected.
template <typename G>
inline constexpr unsigned engine_bit_width = []() constexpr {
	using U = details::engine_unsigned<G>;
	U const span{static_cast<U>(G::max()) - static_cast<U>(G::min())};
	constexpr unsigned width{::std::numeric_limits<U>::digits};
	if (span == static_cast<U>(~U{}))
	{
		return width;
	}
	// floor(log2(span + 1)): the largest power of two that fits in span + 1.
	return static_cast<unsigned>(::std::bit_width(static_cast<U>(span + 1u))) - 1u;
}();

template <typename G>
inline constexpr details::engine_unsigned<G> engine_bucket_mask = []() constexpr {
	using U = details::engine_unsigned<G>;
	constexpr unsigned bits{details::engine_bit_width<G>};
	constexpr unsigned width{::std::numeric_limits<U>::digits};
	if constexpr (bits == width)
	{
		return static_cast<U>(~U{});
	}
	else
	{
		return static_cast<U>((U{1} << bits) - 1u);
	}
}();

// The engine's [min, max] is exactly the bucket, so no draw can be rejected.
template <typename G>
inline constexpr bool engine_accepts_every_draw = []() constexpr {
	using U = details::engine_unsigned<G>;
	U const span{static_cast<U>(G::max()) - static_cast<U>(G::min())};
	return span == details::engine_bucket_mask<G>;
}();

// One engine draw, preferring generate().
template <typename G>
inline constexpr details::engine_unsigned<G> draw_engine(G &g)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!details::engine_draw_no_throw<G>)
{
	if constexpr (details::engine_has_generate<G>)
	{
		return static_cast<details::engine_unsigned<G>>(g.generate());
	}
	else
	{
		return static_cast<details::engine_unsigned<G>>(g());
	}
}

// `count` independent uniform bits, count in [0, 64], assembled into a 64-bit
// word from most significant chunk to least significant chunk. Narrow engines
// are called as many times as needed.
//
// Each chunk is taken from the top of the engine bucket rather than the
// bottom. Both are uniform for a good engine, but the high bits of a weak
// engine (an LCG is the classic case) are far better than its low bits.
template <typename G>
inline constexpr ::std::uint_least64_t draw_bits(G &g, unsigned count)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!details::engine_draw_no_throw<G>)
{
	using E = details::engine_unsigned<G>;
	constexpr E bucket{details::engine_bucket_mask<G>};
	constexpr unsigned available{details::engine_bit_width<G>};
	::std::uint_least64_t value{};
	for (unsigned produced{}; produced < count;)
	{
		E bits{};
		if constexpr (details::engine_accepts_every_draw<G>)
		{
			bits = static_cast<E>(details::draw_engine(g) - static_cast<E>(G::min()));
		}
		else
		{
			do
			{
				bits = static_cast<E>(details::draw_engine(g) - static_cast<E>(G::min()));
			} while ((bits & bucket) != bits);
		}
		unsigned const take{(count - produced) < available ? count - produced : available};
		::std::uint_least64_t const chunk{
			static_cast<::std::uint_least64_t>(bits) >> (available - take)};
		// The first chunk is the whole word; later chunks are appended below it.
		// Guarding the shift also keeps take == 64 out of a shift-by-64.
		if (produced != 0)
		{
			value <<= take;
		}
		value |= chunk;
		produced += take;
	}
	return value;
}

} // namespace details

} // namespace fast_io::random
