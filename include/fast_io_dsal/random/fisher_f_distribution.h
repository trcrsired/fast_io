#pragma once

#include "gamma_distribution.h"

#include <limits>

namespace fast_io::random
{

// Fisher-Snedecor F distribution.
//
// A direct port of libc++'s fisher_f_distribution. operator!= and the stream
// operators are omitted.
template <::std::floating_point RealType = double>
	requires(details::valid_real_type<RealType>)
class fisher_f_distribution
{
public:
	using result_type = RealType;

	class param_type
	{
	public:
		using distribution_type = fisher_f_distribution;

		explicit constexpr param_type(
			result_type m = 1, result_type n = 1) noexcept
			: m_(m), n_(n)
		{}

		inline constexpr result_type m() const noexcept
		{
			return m_;
		}
		inline constexpr result_type n() const noexcept
		{
			return n_;
		}

		friend inline constexpr bool operator==(
			param_type const &lhs, param_type const &rhs) noexcept
		{
			return lhs.m_ == rhs.m_ && lhs.n_ == rhs.n_;
		}

	private:
		result_type m_{};
		result_type n_{};
	};

	constexpr fisher_f_distribution() noexcept = default;
	explicit constexpr fisher_f_distribution(
		result_type m, result_type n = 1) noexcept
		: param_(m, n)
	{}
	explicit constexpr fisher_f_distribution(param_type const &parm) noexcept
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
		gamma_distribution<result_type> gdm(parm.m() * result_type(.5));
		gamma_distribution<result_type> gdn(parm.n() * result_type(.5));
		return parm.n() * gdm(g) / (parm.m() * gdn(g));
	}

	inline constexpr result_type m() const noexcept
	{
		return param_.m();
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
		fisher_f_distribution const &lhs, fisher_f_distribution const &rhs) noexcept
	{
		return lhs.param_ == rhs.param_;
	}

private:
	param_type param_{};
};

} // namespace fast_io::random
