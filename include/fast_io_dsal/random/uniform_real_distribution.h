#pragma once

#include "engine.h"
#include "generate_canonical.h"

#include <limits>

namespace fast_io::random
{

// Uniform real on [a, b).
//
// A direct port of libc++'s uniform_real_distribution: the API is identical
// except that operator!= (synthesized from operator== in C++20) and the stream
// insertion/extraction operators are omitted.
template <::std::floating_point RealType = double>
	requires(details::valid_real_type<RealType>)
class uniform_real_distribution
{
public:
	using result_type = RealType;

	class param_type
	{
	public:
		using distribution_type = uniform_real_distribution;

		explicit constexpr param_type(
			result_type first = 0, result_type last = 1) noexcept
			: first_(first), last_(last)
		{}

		inline constexpr result_type a() const noexcept
		{
			return first_;
		}
		inline constexpr result_type b() const noexcept
		{
			return last_;
		}

		friend inline constexpr bool operator==(
			param_type const &lhs, param_type const &rhs) noexcept
		{
			return lhs.first_ == rhs.first_ && lhs.last_ == rhs.last_;
		}

	private:
		result_type first_{};
		result_type last_{};
	};

	constexpr uniform_real_distribution() noexcept = default;
	explicit constexpr uniform_real_distribution(
		result_type first, result_type last = 1) noexcept
		: param_(first, last)
	{}
	explicit constexpr uniform_real_distribution(param_type const &parm) noexcept
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
		return (parm.b() - parm.a()) *
				   details::generate_canonical<
					   result_type, ::std::numeric_limits<result_type>::digits>(g) +
			   parm.a();
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
		return a();
	}
	inline constexpr result_type max() const noexcept
	{
		return b();
	}

	friend inline constexpr bool operator==(
		uniform_real_distribution const &lhs,
		uniform_real_distribution const &rhs) noexcept
	{
		return lhs.param_ == rhs.param_;
	}

private:
	param_type param_{};
};

} // namespace fast_io::random
