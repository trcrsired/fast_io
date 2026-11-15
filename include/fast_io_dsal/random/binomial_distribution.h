#pragma once

#include "math.h"
#include "uniform_real_distribution.h"

#include <limits>

namespace fast_io::random
{

// Binomial distribution.
//
// A direct port of libc++'s binomial_distribution, which uses the Kemp (1986)
// modal method. operator!= and the stream operators are omitted.
template <::std::integral IntType = int>
	requires(details::valid_int_type<IntType>)
class binomial_distribution
{
public:
	using result_type = IntType;

	class param_type
	{
	public:
		using distribution_type = binomial_distribution;

		explicit constexpr param_type(result_type t = 1, double p = 0.5) noexcept
			: t_(t), p_(p)
		{
			if (0 < p_ && p_ < 1)
			{
				r0_ = static_cast<result_type>(
					static_cast<double>(t_ + 1) * p_);
				pr_ = details::math::exp(
					details::math::lgamma_r(static_cast<double>(t_) + 1.) -
					details::math::lgamma_r(static_cast<double>(r0_) + 1.) -
					details::math::lgamma_r(
						static_cast<double>(t_ - r0_) + 1.) +
					static_cast<double>(r0_) * details::math::log(p_) +
					static_cast<double>(t_ - r0_) *
						details::math::log(1 - p_));
				odds_ratio_ = p_ / (1 - p_);
			}
		}

		inline constexpr result_type t() const noexcept
		{
			return t_;
		}
		inline constexpr double p() const noexcept
		{
			return p_;
		}

		friend inline constexpr bool operator==(
			param_type const &lhs, param_type const &rhs) noexcept
		{
			return lhs.t_ == rhs.t_ && lhs.p_ == rhs.p_;
		}

	private:
		result_type t_{};
		double p_{};
		double pr_{};
		double odds_ratio_{};
		result_type r0_{};

		friend class binomial_distribution;
	};

	constexpr binomial_distribution() noexcept = default;
	explicit constexpr binomial_distribution(
		result_type t, double p = 0.5) noexcept
		: param_(t, p)
	{}
	explicit constexpr binomial_distribution(param_type const &parm) noexcept
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
		if (parm.t_ == 0 || parm.p_ == 0)
		{
			return 0;
		}
		if (parm.p_ == 1)
		{
			return parm.t_;
		}
		uniform_real_distribution<double> gen;
		double u{gen(g) - parm.pr_};
		if (u < 0)
		{
			return parm.r0_;
		}
		double pu{parm.pr_};
		double pd{pu};
		result_type ru{parm.r0_};
		result_type rd{ru};
		while (true)
		{
			bool done{true};
			if (rd >= 1)
			{
				pd *= static_cast<double>(rd) /
					  (parm.odds_ratio_ * static_cast<double>(parm.t_ - rd + 1));
				u -= pd;
				done = false;
				if (u < 0)
				{
					return rd - 1;
				}
			}
			if (rd != 0)
			{
				--rd;
			}
			++ru;
			if (ru <= parm.t_)
			{
				pu *= static_cast<double>(parm.t_ - ru + 1) *
					  parm.odds_ratio_ / static_cast<double>(ru);
				u -= pu;
				done = false;
				if (u < 0)
				{
					return ru;
				}
			}
			if (done)
			{
				return 0;
			}
		}
	}

	inline constexpr result_type t() const noexcept
	{
		return param_.t();
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
		return t();
	}

	friend inline constexpr bool operator==(
		binomial_distribution const &lhs, binomial_distribution const &rhs) noexcept
	{
		return lhs.param_ == rhs.param_;
	}

private:
	param_type param_{};
};

} // namespace fast_io::random
