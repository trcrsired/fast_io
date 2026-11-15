#pragma once

#include "math.h"
#include "uniform_real_distribution.h"

#include <limits>

namespace fast_io::random
{

// Extreme-value distribution.
//
// A direct port of libc++'s extreme_value_distribution. operator!= and the
// stream operators are omitted.
template <::std::floating_point RealType = double>
	requires(details::valid_real_type<RealType>)
class extreme_value_distribution
{
public:
	using result_type = RealType;

	class param_type
	{
	public:
		using distribution_type = extreme_value_distribution;

		explicit constexpr param_type(
			result_type a = 0, result_type b = 1) noexcept
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

	constexpr extreme_value_distribution() noexcept = default;
	explicit constexpr extreme_value_distribution(
		result_type a, result_type b = 1) noexcept
		: param_(a, b)
	{}
	explicit constexpr extreme_value_distribution(
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
		return parm.a() -
			   parm.b() *
				   details::math::log(-details::math::log(
					   result_type(1) - uniform_real_distribution<result_type>()(g)));
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
		return -::std::numeric_limits<result_type>::infinity();
	}
	inline constexpr result_type max() const noexcept
	{
		return ::std::numeric_limits<result_type>::infinity();
	}

	friend inline constexpr bool operator==(
		extreme_value_distribution const &lhs,
		extreme_value_distribution const &rhs) noexcept
	{
		return lhs.param_ == rhs.param_;
	}

private:
	param_type param_{};
};

} // namespace fast_io::random
