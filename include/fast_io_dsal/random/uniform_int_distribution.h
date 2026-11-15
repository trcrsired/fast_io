#pragma once

#include "engine.h"

#include <limits>
#include <type_traits>

namespace fast_io::random
{

namespace details
{

// Uniform value in [0, span]. Rejection sampling: at most one extra draw on
// average, never a modulo bias.
template <typename G, ::std::unsigned_integral U>
inline constexpr U uniform_span(G &g, U span)
	FAST_IO_HERBCEPTIONS_THROWS_IF(!details::engine_draw_no_throw<G>)
{
	if (span == 0)
	{
		return 0;
	}
	unsigned const count{static_cast<unsigned>(::std::bit_width(span))};
	U value{};
	do
	{
		value = static_cast<U>(details::draw_bits<G>(g, count));
	} while (value > span);
	return value;
}

} // namespace details

// Uniform integer on the closed interval [a, b].
//
// The API mirrors std::uniform_int_distribution: same members, same
// param_type, same equality. Stream insertion/extraction are deliberately
// omitted; see README.md.
//
// IntType may be any integral type up to 64 bits except bool, including
// int_least8_t/uint_least8_t ... int_least64_t/uint_least64_t. The standard
// only names the short/int/long/long long spellings; the least-width aliases
// are what fast_io wants to offer.
template <::std::integral IntType = int>
	requires(::std::same_as<IntType, ::std::remove_cv_t<IntType>> &&
			 !::std::same_as<IntType, bool> &&
			 sizeof(IntType) <= sizeof(::std::uint_least64_t))
class uniform_int_distribution
{
public:
	using result_type = IntType;

	class param_type
	{
	public:
		using distribution_type = uniform_int_distribution;

		explicit constexpr param_type(
			result_type first = 0,
			result_type last = ::std::numeric_limits<result_type>::max()) noexcept
			: first_(first), last_(last)
		{}

		inline constexpr result_type a() const noexcept
		{
			return first_;
		}
		inline constexpr result_type b() const noexcept
		{
			return last_;
		}

		friend inline constexpr bool operator==(
			param_type const &lhs, param_type const &rhs) noexcept
		{
			return lhs.first_ == rhs.first_ && lhs.last_ == rhs.last_;
		}

	private:
		result_type first_{};
		result_type last_{};
	};

	explicit constexpr uniform_int_distribution(
		result_type first = 0,
		result_type last = ::std::numeric_limits<result_type>::max()) noexcept
		: param_(first, last)
	{}

	explicit constexpr uniform_int_distribution(param_type const &parm) noexcept
		: param_(parm)
	{}

	inline constexpr result_type a() const noexcept
	{
		return param_.a();
	}
	inline constexpr result_type b() const noexcept
	{
		return param_.b();
	}

	inline constexpr result_type min() const noexcept
	{
		return param_.a();
	}
	inline constexpr result_type max() const noexcept
	{
		return param_.b();
	}

	inline constexpr void reset() noexcept
	{}

	inline constexpr param_type param() const noexcept
	{
		return param_;
	}
	inline constexpr void param(param_type const &parm) noexcept
	{
		param_ = parm;
	}

	template <typename G>
		requires ::fast_io::random::uniform_random_bit_generator<G>
	inline constexpr result_type operator()(G &g)
		FAST_IO_HERBCEPTIONS_THROWS_IF(!details::engine_draw_no_throw<G>)
	{
		return this->operator()(g, param_);
	}

	template <typename G>
		requires ::fast_io::random::uniform_random_bit_generator<G>
	inline constexpr result_type operator()(G &g, param_type const &parm)
		FAST_IO_HERBCEPTIONS_THROWS_IF(!details::engine_draw_no_throw<G>)
	{
		using U = ::std::make_unsigned_t<result_type>;
		U const span{
			static_cast<U>(static_cast<U>(parm.b()) - static_cast<U>(parm.a()))};
		U const offset{details::uniform_span(g, span)};
		return static_cast<result_type>(
			static_cast<U>(static_cast<U>(parm.a()) + offset));
	}

	friend inline constexpr bool operator==(
		uniform_int_distribution const &lhs,
		uniform_int_distribution const &rhs) noexcept
	{
		return lhs.param_ == rhs.param_;
	}

private:
	param_type param_{};
};

} // namespace fast_io::random
