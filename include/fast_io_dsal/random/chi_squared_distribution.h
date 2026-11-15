#pragma once

#include "gamma_distribution.h"

#include <limits>

namespace fast_io::random
{

// Chi-squared distribution: gamma(n / 2, 2). A direct port of libc++'s
// chi_squared_distribution. operator!= and the stream operators are omitted.
template <::std::floating_point RealType = double>
	requires(details::valid_real_type<RealType>)
class chi_squared_distribution
{
public:
	using result_type = RealType;

	class param_type
	{
	public:
		using distribution_type = chi_squared_distribution;

		explicit constexpr param_type(result_type n = 1) noexcept : n_(n)
		{}

		inline constexpr result_type n() const noexcept
		{
			return n_;
		}

		friend inline constexpr bool operator==(
			param_type const &lhs, param_type const &rhs) noexcept
		{
			return lhs.n_ == rhs.n_;
		}

	private:
		result_type n_{};
	};

	constexpr chi_squared_distribution() noexcept = default;
	explicit constexpr chi_squared_distribution(result_type n) noexcept
		: param_(n)
	{}
	explicit constexpr chi_squared_distribution(param_type const &parm) noexcept
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
		return gamma_distribution<result_type>(parm.n() / 2, 2)(g);
	}

	inline constexpr result_type n() const noexcept
	{
		return param_.n();
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
		chi_squared_distribution const &lhs,
		chi_squared_distribution const &rhs) noexcept
	{
		return lhs.param_ == rhs.param_;
	}

private:
	param_type param_{};
};

} // namespace fast_io::random
