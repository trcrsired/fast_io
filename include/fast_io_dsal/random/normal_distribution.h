#pragma once

#include "math.h"
#include "uniform_real_distribution.h"

#include <limits>

namespace fast_io::random
{

// Normal (Gaussian) distribution.
//
// A direct port of libc++'s normal_distribution, including the cached second
// variate. operator!= and the stream operators are omitted.
template <::std::floating_point RealType = double>
	requires(details::valid_real_type<RealType>)
class normal_distribution
{
public:
	using result_type = RealType;

	class param_type
	{
	public:
		using distribution_type = normal_distribution;

		explicit constexpr param_type(
			result_type mean = 0, result_type stddev = 1) noexcept
			: mean_(mean), stddev_(stddev)
		{}

		inline constexpr result_type mean() const noexcept
		{
			return mean_;
		}
		inline constexpr result_type stddev() const noexcept
		{
			return stddev_;
		}

		friend inline constexpr bool operator==(
			param_type const &lhs, param_type const &rhs) noexcept
		{
			return lhs.mean_ == rhs.mean_ && lhs.stddev_ == rhs.stddev_;
		}

	private:
		result_type mean_{};
		result_type stddev_{};
	};

	constexpr normal_distribution() noexcept = default;
	explicit constexpr normal_distribution(
		result_type mean, result_type stddev = 1) noexcept
		: param_(mean, stddev)
	{}
	explicit constexpr normal_distribution(param_type const &parm) noexcept
		: param_(parm)
	{}

	inline constexpr void reset() noexcept
	{
		hot_ = false;
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
		result_type up;
		if (hot_)
		{
			hot_ = false;
			up = cached_;
		}
		else
		{
			uniform_real_distribution<result_type> uni(-1, 1);
			result_type u;
			result_type v;
			result_type s;
			do
			{
				u = uni(g);
				v = uni(g);
				s = u * u + v * v;
			} while (s > 1 || s == 0);
			result_type const fp{details::math::sqrt(-2 * details::math::log(s) / s)};
			cached_ = v * fp;
			hot_ = true;
			up = u * fp;
		}
		return up * parm.stddev() + parm.mean();
	}

	inline constexpr result_type mean() const noexcept
	{
		return param_.mean();
	}
	inline constexpr result_type stddev() const noexcept
	{
		return param_.stddev();
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
		return -::std::numeric_limits<result_type>::infinity();
	}
	inline constexpr result_type max() const noexcept
	{
		return ::std::numeric_limits<result_type>::infinity();
	}

	friend inline constexpr bool operator==(
		normal_distribution const &lhs, normal_distribution const &rhs) noexcept
	{
		return lhs.param_ == rhs.param_ && lhs.hot_ == rhs.hot_ &&
			   (!lhs.hot_ || lhs.cached_ == rhs.cached_);
	}

private:
	param_type param_{};
	result_type cached_{};
	bool hot_{};
};

} // namespace fast_io::random
