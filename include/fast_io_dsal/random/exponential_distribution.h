#pragma once

#include "generate_canonical.h"
#include "math.h"

#include <limits>

namespace fast_io::random
{

// Exponential distribution.
//
// A direct port of libc++'s exponential_distribution. operator!= and the
// stream operators are omitted.
template <::std::floating_point RealType = double>
	requires(details::valid_real_type<RealType>)
class exponential_distribution
{
public:
	using result_type = RealType;

	class param_type
	{
	public:
		using distribution_type = exponential_distribution;

		explicit constexpr param_type(result_type lambda = 1) noexcept
			: lambda_(lambda)
		{}

		inline constexpr result_type lambda() const noexcept
		{
			return lambda_;
		}

		friend inline constexpr bool operator==(
			param_type const &lhs, param_type const &rhs) noexcept
		{
			return lhs.lambda_ == rhs.lambda_;
		}

	private:
		result_type lambda_{};
	};

	constexpr exponential_distribution() noexcept = default;
	explicit constexpr exponential_distribution(result_type lambda) noexcept
		: param_(lambda)
	{}
	explicit constexpr exponential_distribution(param_type const &parm) noexcept
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
		return -details::math::log(
				   result_type(1) -
				   details::generate_canonical<
					   result_type, ::std::numeric_limits<result_type>::digits>(g)) /
			   parm.lambda();
	}

	inline constexpr result_type lambda() const noexcept
	{
		return param_.lambda();
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
		exponential_distribution const &lhs,
		exponential_distribution const &rhs) noexcept
	{
		return lhs.param_ == rhs.param_;
	}

private:
	param_type param_{};
};

} // namespace fast_io::random
