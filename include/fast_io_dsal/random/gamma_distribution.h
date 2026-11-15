#pragma once

#include "exponential_distribution.h"
#include "math.h"
#include "uniform_real_distribution.h"

#include <limits>

namespace fast_io::random
{

// Gamma distribution.
//
// A direct port of libc++'s gamma_distribution: Marsaglia-Tsang for alpha > 1,
// the Johnk/Boost-style rejection method for alpha < 1, and the exponential
// special case for alpha == 1. operator!= and the stream operators are
// omitted.
template <::std::floating_point RealType = double>
	requires(details::valid_real_type<RealType>)
class gamma_distribution
{
public:
	using result_type = RealType;

	class param_type
	{
	public:
		using distribution_type = gamma_distribution;

		explicit constexpr param_type(
			result_type alpha = 1, result_type beta = 1) noexcept
			: alpha_(alpha), beta_(beta)
		{}

		inline constexpr result_type alpha() const noexcept
		{
			return alpha_;
		}
		inline constexpr result_type beta() const noexcept
		{
			return beta_;
		}

		friend inline constexpr bool operator==(
			param_type const &lhs, param_type const &rhs) noexcept
		{
			return lhs.alpha_ == rhs.alpha_ && lhs.beta_ == rhs.beta_;
		}

	private:
		result_type alpha_{};
		result_type beta_{};
	};

	constexpr gamma_distribution() noexcept = default;
	explicit constexpr gamma_distribution(
		result_type alpha, result_type beta = 1) noexcept
		: param_(alpha, beta)
	{}
	explicit constexpr gamma_distribution(param_type const &parm) noexcept
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
		result_type const a{parm.alpha()};
		uniform_real_distribution<result_type> gen(0, 1);
		exponential_distribution<result_type> egen;
		result_type x;
		if (a == 1)
		{
			x = egen(g);
		}
		else if (a > 1)
		{
			result_type const b{a - 1};
			result_type const c{3 * a - result_type(0.75)};
			while (true)
			{
				result_type const u{gen(g)};
				result_type const v{gen(g)};
				result_type const w{u * (result_type(1) - u)};
				if (w != 0)
				{
					result_type const y{
						details::math::sqrt(c / w) * (u - result_type(0.5))};
					x = b + y;
					if (x >= 0)
					{
						result_type const z{
							result_type(64) * w * w * w * v * v};
						if (z <= result_type(1) - 2 * y * y / x)
						{
							break;
						}
						if (details::math::log(z) <=
							2 * (b * details::math::log(x / b) - y))
						{
							break;
						}
					}
				}
			}
		}
		else // a < 1
		{
			while (true)
			{
				result_type const u{gen(g)};
				result_type const es{egen(g)};
				if (u <= result_type(1) - a)
				{
					x = details::math::pow(u, result_type(1) / a);
					if (x <= es)
					{
						break;
					}
				}
				else
				{
					result_type const e{
						-details::math::log((result_type(1) - u) / a)};
					x = details::math::pow(
						result_type(1) - a + a * e, result_type(1) / a);
					if (x <= e + es)
					{
						break;
					}
				}
			}
		}
		return x * parm.beta();
	}

	inline constexpr result_type alpha() const noexcept
	{
		return param_.alpha();
	}
	inline constexpr result_type beta() const noexcept
	{
		return param_.beta();
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
		return ::std::numeric_limits<result_type>::infinity();
	}

	friend inline constexpr bool operator==(
		gamma_distribution const &lhs, gamma_distribution const &rhs) noexcept
	{
		return lhs.param_ == rhs.param_;
	}

private:
	param_type param_{};
};

} // namespace fast_io::random
