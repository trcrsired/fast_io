#pragma once

#include "math.h"
#include "normal_distribution.h"

#include <limits>

namespace fast_io::random
{

// Log-normal distribution: exp of a normal variate.
//
// A direct port of libc++'s lognormal_distribution. operator!= and the stream
// operators are omitted.
template <::std::floating_point RealType = double>
	requires(details::valid_real_type<RealType>)
class lognormal_distribution
{
public:
	using result_type = RealType;

	class param_type
	{
	public:
		using distribution_type = lognormal_distribution;

		explicit constexpr param_type(
			result_type m = 0, result_type s = 1) noexcept
			: m_(m), s_(s)
		{}

		inline constexpr result_type m() const noexcept
		{
			return m_;
		}
		inline constexpr result_type s() const noexcept
		{
			return s_;
		}

		friend inline constexpr bool operator==(
			param_type const &lhs, param_type const &rhs) noexcept
		{
			return lhs.m_ == rhs.m_ && lhs.s_ == rhs.s_;
		}

	private:
		result_type m_{};
		result_type s_{};
	};

	lognormal_distribution() = default;
	explicit constexpr lognormal_distribution(
		result_type m, result_type s = 1) noexcept
		: nd_(m, s)
	{}
	explicit constexpr lognormal_distribution(param_type const &parm) noexcept
		: nd_(parm.m(), parm.s())
	{}

	inline constexpr void reset() noexcept
	{
		nd_.reset();
	}

	template <typename G>
		requires ::fast_io::random::uniform_random_bit_generator<G>
	inline constexpr result_type operator()(G &g)
		FAST_IO_HERBCEPTIONS_THROWS_IF(!details::engine_draw_no_throw<G>)
	{
		return details::math::exp(nd_(g));
	}

	template <typename G>
		requires ::fast_io::random::uniform_random_bit_generator<G>
	inline constexpr result_type operator()(G &g, param_type const &parm)
		FAST_IO_HERBCEPTIONS_THROWS_IF(!details::engine_draw_no_throw<G>)
	{
		typename normal_distribution<result_type>::param_type const pn{
			parm.m(), parm.s()};
		return details::math::exp(nd_(g, pn));
	}

	inline constexpr result_type m() const noexcept
	{
		return nd_.mean();
	}
	inline constexpr result_type s() const noexcept
	{
		return nd_.stddev();
	}

	inline constexpr param_type param() const noexcept
	{
		return param_type(nd_.mean(), nd_.stddev());
	}
	inline constexpr void param(param_type const &parm) noexcept
	{
		typename normal_distribution<result_type>::param_type const pn{
			parm.m(), parm.s()};
		nd_.param(pn);
	}

	inline constexpr result_type min() const noexcept
	{
		return 0;
	}
	inline constexpr result_type max() const noexcept
	{
		return ::std::numeric_limits<result_type>::infinity();
	}

	friend inline constexpr bool operator==(
		lognormal_distribution const &lhs, lognormal_distribution const &rhs) noexcept
	{
		return lhs.nd_ == rhs.nd_;
	}

private:
	normal_distribution<result_type> nd_{};
};

} // namespace fast_io::random
