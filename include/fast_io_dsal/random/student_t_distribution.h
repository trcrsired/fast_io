#pragma once

#include "gamma_distribution.h"
#include "math.h"
#include "normal_distribution.h"

#include <limits>

namespace fast_io::random
{

// Student's t distribution.
//
// A direct port of libc++'s student_t_distribution. The cached normal variate
// lives in the member normal_distribution, so reset() resets that too.
// operator!= and the stream operators are omitted.
template <::std::floating_point RealType = double>
	requires(details::valid_real_type<RealType>)
class student_t_distribution
{
public:
	using result_type = RealType;

	class param_type
	{
	public:
		using distribution_type = student_t_distribution;

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

	constexpr student_t_distribution() noexcept = default;
	explicit constexpr student_t_distribution(result_type n) noexcept
		: param_(n)
	{}
	explicit constexpr student_t_distribution(param_type const &parm) noexcept
		: param_(parm)
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
		return this->operator()(g, param_);
	}

	template <typename G>
		requires ::fast_io::random::uniform_random_bit_generator<G>
	inline constexpr result_type operator()(G &g, param_type const &parm)
		FAST_IO_HERBCEPTIONS_THROWS_IF(!details::engine_draw_no_throw<G>)
	{
		gamma_distribution<result_type> gd(parm.n() * result_type(.5), 2);
		return nd_(g) * details::math::sqrt(parm.n() / gd(g));
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
		return -::std::numeric_limits<result_type>::infinity();
	}
	inline constexpr result_type max() const noexcept
	{
		return ::std::numeric_limits<result_type>::infinity();
	}

	friend inline constexpr bool operator==(
		student_t_distribution const &lhs,
		student_t_distribution const &rhs) noexcept
	{
		return lhs.param_ == rhs.param_;
	}

private:
	param_type param_{};
	normal_distribution<result_type> nd_{};
};

} // namespace fast_io::random
