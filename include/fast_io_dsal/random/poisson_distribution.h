#pragma once

#include "clamp_to_integral.h"
#include "exponential_distribution.h"
#include "math.h"
#include "normal_distribution.h"
#include "uniform_real_distribution.h"

#include <limits>

namespace fast_io::random
{

// Poisson distribution.
//
// A direct port of libc++'s poisson_distribution: the multiplicative method for
// mean < 10 and the PTRS transformed-rejection method above that, including
// libc++'s treatment of an infinite mean. operator!= and the stream operators
// are omitted.
template <::std::integral IntType = int>
	requires(details::valid_int_type<IntType>)
class poisson_distribution
{
public:
	using result_type = IntType;

	class param_type
	{
	public:
		using distribution_type = poisson_distribution;

		explicit constexpr param_type(double mean = 1.0) noexcept
			: mean_(details::math::isinf(mean)
						? ::std::numeric_limits<double>::max()
						: mean)
		{
			if (mean_ < 10)
			{
				s_ = 0;
				d_ = 0;
				l_ = details::math::exp(-mean_);
				omega_ = 0;
				c3_ = 0;
				c2_ = 0;
				c1_ = 0;
				c0_ = 0;
				c_ = 0;
			}
			else
			{
				s_ = details::math::sqrt(mean_);
				d_ = 6 * mean_ * mean_;
				l_ = details::math::trunc(mean_ - 1.1484);
				omega_ = .3989423 / s_;
				double const b1{.4166667E-1 / mean_};
				double const b2{.3 * b1 * b1};
				c3_ = .1428571 * b1 * b2;
				c2_ = b2 - 15. * c3_;
				c1_ = b1 - 6. * b2 + 45. * c3_;
				c0_ = 1. - b1 + 3. * b2 - 15. * c3_;
				c_ = .1069 / mean_;
			}
		}

		inline constexpr double mean() const noexcept
		{
			return mean_;
		}

		friend inline constexpr bool operator==(
			param_type const &lhs, param_type const &rhs) noexcept
		{
			return lhs.mean_ == rhs.mean_;
		}

	private:
		double mean_{};
		double s_{};
		double d_{};
		double l_{};
		double omega_{};
		double c0_{};
		double c1_{};
		double c2_{};
		double c3_{};
		double c_{};

		friend class poisson_distribution;
	};

	constexpr poisson_distribution() noexcept = default;
	explicit constexpr poisson_distribution(double mean) noexcept
		: param_(mean)
	{}
	explicit constexpr poisson_distribution(param_type const &parm) noexcept
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
		double tx;
		uniform_real_distribution<double> urd;
		if (parm.mean_ < 10)
		{
			tx = 0;
			for (double p = urd(g); p > parm.l_; ++tx)
			{
				p *= urd(g);
			}
		}
		else
		{
			double difmuk;
			double const gauss{parm.mean_ + parm.s_ * normal_distribution<double>()(g)};
			double u;
			if (gauss > 0)
			{
				tx = details::math::trunc(gauss);
				if (tx >= parm.l_)
				{
					return details::clamp_to_integral<result_type>(tx);
				}
				difmuk = parm.mean_ - tx;
				u = urd(g);
				if (parm.d_ * u >= difmuk * difmuk * difmuk)
				{
					return details::clamp_to_integral<result_type>(tx);
				}
			}
			exponential_distribution<double> edist;
			for (bool using_exp_dist = false; true; using_exp_dist = true)
			{
				double e{};
				if (using_exp_dist || gauss <= 0)
				{
					double t;
					do
					{
						e = edist(g);
						u = urd(g);
						u += u - 1;
						t = 1.8 + (u < 0 ? -e : e);
					} while (t <= -.6744);
					tx = details::math::trunc(parm.mean_ + parm.s_ * t);
					difmuk = parm.mean_ - tx;
					using_exp_dist = true;
				}
				double px;
				double py;
				if (tx < 10 && tx >= 0)
				{
					constexpr double fac[]{1, 1, 2, 6, 24, 120, 720, 5040, 40320, 362880};
					px = -parm.mean_;
					py = details::math::pow(parm.mean_, tx) /
						 fac[static_cast<int>(tx)];
				}
				else
				{
					double del{.8333333E-1 / tx};
					del -= 4.8 * del * del * del;
					double const v{difmuk / tx};
					if (details::math::fabs(v) > 0.25)
					{
						px = tx * details::math::log(1 + v) - difmuk - del;
					}
					else
					{
						px = tx * v * v *
								 (((((((.1250060 * v + -.1384794) * v + .1421878) * v +
									  -.1661269) *
										 v +
									 .2000118) *
										v +
									-.2500068) *
									   v +
								   .3333333) *
									  v +
								  -.5) -
							 del;
					}
					py = .3989423 / details::math::sqrt(tx);
				}
				double const r{(0.5 - difmuk) / parm.s_};
				double const r2{r * r};
				double const fx{-0.5 * r2};
				double const fy{
					parm.omega_ *
					(((parm.c3_ * r2 + parm.c2_) * r2 + parm.c1_) * r2 + parm.c0_)};
				if (using_exp_dist)
				{
					if (parm.c_ * details::math::fabs(u) <=
						py * details::math::exp(px + e) -
							fy * details::math::exp(fx + e))
					{
						break;
					}
				}
				else
				{
					if (fy - u * fy <= py * details::math::exp(px - fx))
					{
						break;
					}
				}
			}
		}
		return details::clamp_to_integral<result_type>(tx);
	}

	inline constexpr double mean() const noexcept
	{
		return param_.mean();
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
		poisson_distribution const &lhs, poisson_distribution const &rhs) noexcept
	{
		return lhs.param_ == rhs.param_;
	}

private:
	param_type param_{};
};

} // namespace fast_io::random
