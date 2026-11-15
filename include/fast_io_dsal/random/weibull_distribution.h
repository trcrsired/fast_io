#pragma once

#include "exponential_distribution.h"
#include "math.h"

#include <limits>

namespace fast_io::random
{

// Weibull distribution.
//
// A direct port of libc++'s weibull_distribution. operator!= and the stream
// operators are omitted.
template <::std::floating_point RealType = double>
	requires(details::valid_real_type<RealType>)
class weibull_distribution
{
public:
	using result_type = RealType;

	class param_type
	{
	public:
		using distribution_type = weibull_distribution;

		explicit constexpr param_type(
			result_type a = 1, result_type b = 1) noexcept
			: a_(a), b_(b)
		{}

		inline constexpr result_type a() const noexcept
		{
			return a_;
		}
		inline constexpr result_type b() const noexcept
		{
			return b_;
		}

		friend inline constexpr bool operator==(
			param_type const &lhs, param_type const &rhs) noexcept
		{
			return lhs.a_ == rhs.a_ && lhs.b_ == rhs.b_;
		}

	private:
		result_type a_{};
		result_type b_{};
	};

	constexpr weibull_distribution() noexcept = default;
	explicit constexpr weibull_distribution(
		result_type a, result_type b = 1) noexcept
		: param_(a, b)
	{}
	explicit constexpr weibull_distribution(param_type const &parm) noexcept
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
		return parm.b() *
			   details::math::pow(
				   exponential_distribution<result_type>()(g),
				   result_type(1) / parm.a());
	}

	inline constexpr result_type a() const noexcept
	{
		return param_.a();
	}
	inline constexpr result_type b() const noexcept
	{
		return param_.b();
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
		weibull_distribution const &lhs, weibull_distribution const &rhs) noexcept
	{
		return lhs.param_ == rhs.param_;
	}

private:
	param_type param_{};
};

} // namespace fast_io::random
