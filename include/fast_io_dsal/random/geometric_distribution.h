#pragma once

#include "negative_binomial_distribution.h"

#include <limits>

namespace fast_io::random
{

// Geometric distribution: negative_binomial(1, p).
//
// A direct port of libc++'s geometric_distribution. operator!= and the stream
// operators are omitted.
template <::std::integral IntType = int>
	requires(details::valid_int_type<IntType>)
class geometric_distribution
{
public:
	using result_type = IntType;

	class param_type
	{
	public:
		using distribution_type = geometric_distribution;

		explicit constexpr param_type(double p = 0.5) noexcept : p_(p)
		{}

		inline constexpr double p() const noexcept
		{
			return p_;
		}

		friend inline constexpr bool operator==(
			param_type const &lhs, param_type const &rhs) noexcept
		{
			return lhs.p_ == rhs.p_;
		}

	private:
		double p_{};
	};

	constexpr geometric_distribution() noexcept = default;
	explicit constexpr geometric_distribution(double p) noexcept : param_(p)
	{}
	explicit constexpr geometric_distribution(param_type const &parm) noexcept
		: param_(parm)
	{}

	inline constexpr void reset() noexcept
	{}

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
		return negative_binomial_distribution<result_type>(1, parm.p())(g);
	}

	inline constexpr double p() const noexcept
	{
		return param_.p();
	}

	inline constexpr param_type param() const noexcept
	{
		return param_;
	}
	inline constexpr void param(param_type const &parm) noexcept
	{
		param_ = parm;
	}

	inline constexpr result_type min() const noexcept
	{
		return 0;
	}
	inline constexpr result_type max() const noexcept
	{
		return ::std::numeric_limits<result_type>::max();
	}

	friend inline constexpr bool operator==(
		geometric_distribution const &lhs, geometric_distribution const &rhs) noexcept
	{
		return lhs.param_ == rhs.param_;
	}

private:
	param_type param_{};
};

} // namespace fast_io::random
