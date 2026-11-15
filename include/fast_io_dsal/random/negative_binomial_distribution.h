#pragma once

#include "bernoulli_distribution.h"
#include "gamma_distribution.h"
#include "poisson_distribution.h"

#include <limits>

namespace fast_io::random
{

// Negative binomial distribution.
//
// A direct port of libc++'s negative_binomial_distribution: a Bernoulli loop
// when k is small relative to p, otherwise Poisson(gamma(k, (1-p)/p)).
// operator!= and the stream operators are omitted.
template <::std::integral IntType = int>
	requires(details::valid_int_type<IntType>)
class negative_binomial_distribution
{
public:
	using result_type = IntType;

	class param_type
	{
	public:
		using distribution_type = negative_binomial_distribution;

		explicit constexpr param_type(result_type k = 1, double p = 0.5) noexcept
			: k_(k), p_(p)
		{}

		inline constexpr result_type k() const noexcept
		{
			return k_;
		}
		inline constexpr double p() const noexcept
		{
			return p_;
		}

		friend inline constexpr bool operator==(
			param_type const &lhs, param_type const &rhs) noexcept
		{
			return lhs.k_ == rhs.k_ && lhs.p_ == rhs.p_;
		}

	private:
		result_type k_{};
		double p_{};
	};

	constexpr negative_binomial_distribution() noexcept = default;
	explicit constexpr negative_binomial_distribution(
		result_type k, double p = 0.5) noexcept
		: param_(k, p)
	{}
	explicit constexpr negative_binomial_distribution(
		param_type const &parm) noexcept
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
		result_type const k{parm.k()};
		double const p{parm.p()};
		// When the integer type is small, the failure count below is too likely to
		// overflow, so the Poisson/Gamma route is used instead.
		if (static_cast<double>(k) <= 21 * p && sizeof(IntType) > 1)
		{
			bernoulli_distribution gen(p);
			result_type f{};
			result_type s{};
			while (s < k)
			{
				if (gen(g))
				{
					++s;
				}
				else
				{
					++f;
				}
			}
			return f;
		}
		return poisson_distribution<result_type>(
			gamma_distribution<double>(
				static_cast<double>(k), (1 - p) / p)(g))(g);
	}

	inline constexpr result_type k() const noexcept
	{
		return param_.k();
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
		negative_binomial_distribution const &lhs,
		negative_binomial_distribution const &rhs) noexcept
	{
		return lhs.param_ == rhs.param_;
	}

private:
	param_type param_{};
};

} // namespace fast_io::random
