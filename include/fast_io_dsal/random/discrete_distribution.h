#pragma once

#include "engine.h"
#include "uniform_real_distribution.h"
#include "upper_bound.h"

#include <cstddef>
#include <initializer_list>

#include <fast_io_dsal/vector.h>

namespace fast_io::random
{

// Discrete distribution on {0, ..., n - 1}.
//
// A direct port of libc++'s discrete_distribution. The internal std::vector is
// replaced by fast_io::containers::vector, and the std::accumulate /
// partial_sum / adjacent_difference / upper_bound calls are replaced by
// equivalent loops, since the hosted algorithms are not available in
// freestanding mode. operator!= and the stream operators are omitted.
template <::std::integral IntType = int>
	requires(details::valid_int_type<IntType>)
class discrete_distribution
{
public:
	using result_type = IntType;
	using probability_vector = ::fast_io::containers::vector<
		double, ::fast_io::native_global_allocator>;

	class param_type
	{
	public:
		using distribution_type = discrete_distribution;

		param_type() = default;

		template <typename InputIterator>
		param_type(InputIterator first, InputIterator last)
			FAST_IO_HERBCEPTIONS_THROWS
		{
			for (; first != last; ++first)
			{
				cumulative_.push_back(*first);
			}
			init();
		}

		param_type(::std::initializer_list<double> wl)
			FAST_IO_HERBCEPTIONS_THROWS
			: cumulative_(wl)
		{
			init();
		}

		template <typename UnaryOperation>
		param_type(
			::std::size_t count, double xmin, double xmax, UnaryOperation fw)
			FAST_IO_HERBCEPTIONS_THROWS
		{
			if (count > 1)
			{
				cumulative_.reserve(count - 1);
				double const d{(xmax - xmin) / static_cast<double>(count)};
				double const d2{d / 2};
				for (::std::size_t k{}; k != count; ++k)
				{
					cumulative_.push_back(
						fw(xmin + static_cast<double>(k) * d + d2));
				}
				init();
			}
		}

		[[nodiscard]] probability_vector probabilities() const
			FAST_IO_HERBCEPTIONS_THROWS
		{
			::std::size_t const n{cumulative_.size()};
			probability_vector p(n + 1);
			for (::std::size_t i{}; i != n; ++i)
			{
				p[i] = i == 0 ? cumulative_[i] : cumulative_[i] - cumulative_[i - 1];
			}
			if (n > 0)
			{
				p[n] = 1 - cumulative_[n - 1];
			}
			else
			{
				p[0] = 1;
			}
			return p;
		}

		friend bool operator==(param_type const &lhs, param_type const &rhs)
			FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(lhs.cumulative_ ==
														rhs.cumulative_)
		{
			return lhs.cumulative_ == rhs.cumulative_;
		}

	private:
		void init() FAST_IO_HERBCEPTIONS_THROWS
		{
			if (cumulative_.size() != 0)
			{
				if (cumulative_.size() > 1)
				{
					double sum{};
					for (double const value : cumulative_)
					{
						sum += value;
					}
					for (double &value : cumulative_)
					{
						value /= sum;
					}
					probability_vector partial(cumulative_.size() - 1);
					double running{cumulative_[0]};
					partial[0] = running;
					for (::std::size_t i{1}; i != partial.size(); ++i)
					{
						running += cumulative_[i];
						partial[i] = running;
					}
					::fast_io::containers::swap(cumulative_, partial);
				}
				else
				{
					cumulative_.clear();
					cumulative_.shrink_to_fit();
				}
			}
		}

		probability_vector cumulative_{};

		friend class discrete_distribution;
	};

	discrete_distribution() = default;
	template <typename InputIterator>
	discrete_distribution(InputIterator first, InputIterator last)
		FAST_IO_HERBCEPTIONS_THROWS
		: param_(first, last)
	{}
	discrete_distribution(::std::initializer_list<double> wl)
		FAST_IO_HERBCEPTIONS_THROWS
		: param_(wl)
	{}
	template <typename UnaryOperation>
	discrete_distribution(
		::std::size_t count, double xmin, double xmax, UnaryOperation fw)
		FAST_IO_HERBCEPTIONS_THROWS
		: param_(count, xmin, xmax, fw)
	{}
	explicit discrete_distribution(param_type const &parm)
		FAST_IO_HERBCEPTIONS_THROWS
		: param_(parm)
	{}

	inline constexpr void reset() noexcept
	{}

	template <typename G>
		requires ::fast_io::random::uniform_random_bit_generator<G>
	inline result_type operator()(G &g)
		FAST_IO_HERBCEPTIONS_THROWS_IF(!details::engine_draw_no_throw<G>)
	{
		return this->operator()(g, param_);
	}

	template <typename G>
		requires ::fast_io::random::uniform_random_bit_generator<G>
	inline result_type operator()(G &g, param_type const &parm)
		FAST_IO_HERBCEPTIONS_THROWS_IF(!details::engine_draw_no_throw<G>)
	{
		uniform_real_distribution<double> gen;
		return static_cast<result_type>(details::upper_bound_index(
			parm.cumulative_.data(), parm.cumulative_.size(), gen(g)));
	}

	[[nodiscard]] probability_vector probabilities() const
		FAST_IO_HERBCEPTIONS_THROWS
	{
		return param_.probabilities();
	}

	inline param_type param() const FAST_IO_HERBCEPTIONS_THROWS
	{
		return param_;
	}
	inline void param(param_type const &parm) FAST_IO_HERBCEPTIONS_THROWS
	{
		param_ = parm;
	}

	inline constexpr result_type min() const noexcept
	{
		return 0;
	}
	inline constexpr result_type max() const noexcept
	{
		return static_cast<result_type>(param_.cumulative_.size());
	}

	friend bool operator==(
		discrete_distribution const &lhs,
		discrete_distribution const &rhs)
		FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(lhs.param_ == rhs.param_)
	{
		return lhs.param_ == rhs.param_;
	}

private:
	param_type param_{};
};

} // namespace fast_io::random
