#pragma once

#include "engine.h"
#include "generate_canonical.h"

#include <limits>

namespace fast_io::random
{

// Bernoulli trial: true with probability p, false with probability 1 - p.
//
// The API mirrors std::bernoulli_distribution: same members, same param_type,
// same equality. Stream insertion/extraction are deliberately omitted; see
// README.md.
//
// The generating step is libc++'s: canonical double in [0, 1) compared
// against p. As in libc++, p <= 0 (and NaN) therefore yield false and p >= 1
// yields true without extra branches; the standard's 0 <= p <= 1 precondition
// is never relied on for memory safety.
class bernoulli_distribution
{
public:
	using result_type = bool;

	class param_type
	{
	public:
		using distribution_type = bernoulli_distribution;

		explicit constexpr param_type(double probability = 0.5) noexcept
			: probability_(probability)
		{}

		inline constexpr double p() const noexcept
		{
			return probability_;
		}

		friend inline constexpr bool operator==(
			param_type const &lhs, param_type const &rhs) noexcept
		{
			return lhs.probability_ == rhs.probability_;
		}

	private:
		double probability_{};
	};

	constexpr bernoulli_distribution() noexcept = default;
	explicit constexpr bernoulli_distribution(double probability) noexcept
		: param_(probability)
	{}
	explicit constexpr bernoulli_distribution(param_type const &parm) noexcept
		: param_(parm)
	{}

	inline constexpr double p() const noexcept
	{
		return param_.p();
	}

	inline constexpr result_type min() const noexcept
	{
		return false;
	}
	inline constexpr result_type max() const noexcept
	{
		return true;
	}

	inline constexpr void reset() noexcept
	{}

	inline constexpr param_type param() const noexcept
	{
		return param_;
	}
	inline constexpr void param(param_type const &parm) noexcept
	{
		param_ = parm;
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
		return details::generate_canonical<double, 53>(g) < parm.p();
	}

	friend inline constexpr bool operator==(
		bernoulli_distribution const &lhs,
		bernoulli_distribution const &rhs) noexcept
	{
		return lhs.param_ == rhs.param_;
	}

private:
	param_type param_{};
};

} // namespace fast_io::random
