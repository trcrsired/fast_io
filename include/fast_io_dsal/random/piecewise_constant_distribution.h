#pragma once

#include "engine.h"
#include "uniform_real_distribution.h"
#include "upper_bound.h"

#include <cstddef>
#include <initializer_list>

#include <fast_io_dsal/vector.h>

namespace fast_io::random
{

// Piecewise constant distribution.
//
// A direct port of libc++'s piecewise_constant_distribution. The internal
// std::vector is replaced by fast_io::containers::vector and the hosted
// algorithms by equivalent loops. operator!= and the stream operators are
// omitted.
template <::std::floating_point RealType = double>
	requires(details::valid_real_type<RealType>)
class piecewise_constant_distribution
{
public:
	using result_type = RealType;
	using real_vector =
		::fast_io::containers::vector<RealType, ::fast_io::native_global_allocator>;

	class param_type
	{
	public:
		using distribution_type = piecewise_constant_distribution;

		param_type()
			FAST_IO_HERBCEPTIONS_THROWS
			: b_(2),
			  densities_(1, result_type(1.0)),
			  areas_(1, result_type())
		{
			b_[1] = 1;
		}

		template <typename InputIteratorB, typename InputIteratorW>
		param_type(
			InputIteratorB first_b, InputIteratorB last_b, InputIteratorW first_w)
			FAST_IO_HERBCEPTIONS_THROWS
		{
			for (; first_b != last_b; ++first_b)
			{
				b_.push_back(*first_b);
			}
			if (b_.size() < 2)
			{
				b_.resize(2);
				b_[0] = 0;
				b_[1] = 1;
				densities_.assign(1, result_type(1.0));
				areas_.assign(1, result_type());
			}
			else
			{
				densities_.reserve(b_.size() - 1);
				for (::std::size_t i{}; i != b_.size() - 1; ++i)
				{
					densities_.push_back(*first_w);
					++first_w;
				}
				init();
			}
		}

		template <typename UnaryOperation>
		param_type(::std::initializer_list<result_type> bl, UnaryOperation fw)
			FAST_IO_HERBCEPTIONS_THROWS
			: b_(bl)
		{
			if (b_.size() < 2)
			{
				b_.resize(2);
				b_[0] = 0;
				b_[1] = 1;
				densities_.assign(1, result_type(1.0));
				areas_.assign(1, result_type());
			}
			else
			{
				densities_.reserve(b_.size() - 1);
				for (::std::size_t i{}; i != b_.size() - 1; ++i)
				{
					densities_.push_back(
						fw((b_[i + 1] + b_[i]) * result_type(.5)));
				}
				init();
			}
		}

		template <typename UnaryOperation>
		param_type(
			::std::size_t count, result_type xmin, result_type xmax,
			UnaryOperation fw)
			FAST_IO_HERBCEPTIONS_THROWS
			: b_(count == 0 ? 2 : count + 1)
		{
			::std::size_t const n{b_.size() - 1};
			result_type const d{(xmax - xmin) / static_cast<result_type>(n)};
			densities_.reserve(n);
			for (::std::size_t i{}; i != n; ++i)
			{
				b_[i] = xmin + static_cast<result_type>(i) * d;
				densities_.push_back(fw(b_[i] + d * result_type(.5)));
			}
			b_[n] = xmax;
			init();
		}

		param_type(param_type const &) = default;
		param_type &operator=(param_type const &rhs)
			FAST_IO_HERBCEPTIONS_THROWS
		{
			// These can throw
			b_.reserve(rhs.b_.size());
			densities_.reserve(rhs.densities_.size());
			areas_.reserve(rhs.areas_.size());

			// These cannot throw
			b_ = rhs.b_;
			densities_ = rhs.densities_;
			areas_ = rhs.areas_;
			return *this;
		}

		[[nodiscard]] real_vector intervals() const
			FAST_IO_HERBCEPTIONS_THROWS
		{
			return b_;
		}
		[[nodiscard]] real_vector densities() const
			FAST_IO_HERBCEPTIONS_THROWS
		{
			return densities_;
		}

		friend bool operator==(param_type const &lhs, param_type const &rhs)
			FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(lhs.densities_ ==
															rhs.densities_ &&
														lhs.b_ == rhs.b_)
		{
			return lhs.densities_ == rhs.densities_ && lhs.b_ == rhs.b_;
		}

	private:
		void init() FAST_IO_HERBCEPTIONS_THROWS
		{
			// densities_ contains non-normalized areas
			result_type total_area{};
			for (result_type const value : densities_)
			{
				total_area += value;
			}
			for (result_type &value : densities_)
			{
				value /= total_area;
			}
			// densities_ contains normalized areas
			areas_.assign(densities_.size(), result_type());
			for (::std::size_t i{1}; i != densities_.size(); ++i)
			{
				areas_[i] = areas_[i - 1] + densities_[i - 1];
			}
			// areas_ contains partial sums of normalized areas: [0, densities - 1]
			densities_.back() = 1 - areas_.back(); // correct round-off error
			for (::std::size_t i{}; i != densities_.size(); ++i)
			{
				densities_[i] /= (b_[i + 1] - b_[i]);
			}
		}

		real_vector b_{};
		real_vector densities_{};
		real_vector areas_{};

		friend class piecewise_constant_distribution;
	};

	piecewise_constant_distribution() = default;
	template <typename InputIteratorB, typename InputIteratorW>
	piecewise_constant_distribution(
		InputIteratorB first_b, InputIteratorB last_b, InputIteratorW first_w)
		FAST_IO_HERBCEPTIONS_THROWS
		: param_(first_b, last_b, first_w)
	{}
	template <typename UnaryOperation>
	piecewise_constant_distribution(
		::std::initializer_list<result_type> bl, UnaryOperation fw)
		FAST_IO_HERBCEPTIONS_THROWS
		: param_(bl, fw)
	{}
	template <typename UnaryOperation>
	piecewise_constant_distribution(
		::std::size_t count, result_type xmin, result_type xmax,
		UnaryOperation fw)
		FAST_IO_HERBCEPTIONS_THROWS
		: param_(count, xmin, xmax, fw)
	{}
	explicit piecewise_constant_distribution(param_type const &parm)
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
		uniform_real_distribution<result_type> gen;
		result_type const u{gen(g)};
		::std::size_t const k{
			details::upper_bound_index(
				parm.areas_.data(), parm.areas_.size(), u) -
			1};
		return (u - parm.areas_[k]) / parm.densities_[k] + parm.b_[k];
	}

	[[nodiscard]] real_vector intervals() const
		FAST_IO_HERBCEPTIONS_THROWS
	{
		return param_.intervals();
	}
	[[nodiscard]] real_vector densities() const
		FAST_IO_HERBCEPTIONS_THROWS
	{
		return param_.densities();
	}

	inline param_type param() const FAST_IO_HERBCEPTIONS_THROWS
	{
		return param_;
	}
	inline void param(param_type const &parm) FAST_IO_HERBCEPTIONS_THROWS
	{
		param_ = parm;
	}

	inline result_type min() const FAST_IO_HERBCEPTIONS_THROWS
	{
		return param_.b_.front();
	}
	inline result_type max() const FAST_IO_HERBCEPTIONS_THROWS
	{
		return param_.b_.back();
	}

	friend bool operator==(
		piecewise_constant_distribution const &lhs,
		piecewise_constant_distribution const &rhs)
		FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(lhs.param_ == rhs.param_)
	{
		return lhs.param_ == rhs.param_;
	}

private:
	param_type param_{};
};

} // namespace fast_io::random
