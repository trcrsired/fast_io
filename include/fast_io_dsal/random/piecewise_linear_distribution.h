#pragma once

#include "engine.h"
#include "math.h"
#include "uniform_real_distribution.h"
#include "upper_bound.h"

#include <cstddef>
#include <initializer_list>

#include <fast_io_dsal/vector.h>

namespace fast_io::random
{

// Piecewise linear distribution.
//
// A direct port of libc++'s piecewise_linear_distribution. The internal
// std::vector is replaced by fast_io::containers::vector and the hosted
// algorithms by equivalent loops. operator!= and the stream operators are
// omitted.
template <::std::floating_point RealType = double>
	requires(details::valid_real_type<RealType>)
class piecewise_linear_distribution
{
public:
	using result_type = RealType;
	using real_vector =
		::fast_io::containers::vector<RealType, ::fast_io::native_global_allocator>;

	class param_type
	{
	public:
		using distribution_type = piecewise_linear_distribution;

		param_type()
			FAST_IO_HERBCEPTIONS_THROWS
			: b_(2),
			  densities_(2, result_type(1.0)),
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
				densities_.assign(2, result_type(1.0));
				areas_.assign(1, result_type());
			}
			else
			{
				densities_.reserve(b_.size());
				for (::std::size_t i{}; i != b_.size(); ++i)
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
				densities_.assign(2, result_type(1.0));
				areas_.assign(1, result_type());
			}
			else
			{
				densities_.reserve(b_.size());
				for (::std::size_t i{}; i != b_.size(); ++i)
				{
					densities_.push_back(fw(b_[i]));
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
			densities_.reserve(b_.size());
			for (::std::size_t i{}; i != n; ++i)
			{
				b_[i] = xmin + static_cast<result_type>(i) * d;
				densities_.push_back(fw(b_[i]));
			}
			b_[n] = xmax;
			densities_.push_back(fw(b_[n]));
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
			areas_.assign(densities_.size() - 1, result_type());
			result_type sp{};
			for (::std::size_t i{}; i != areas_.size(); ++i)
			{
				areas_[i] = (densities_[i + 1] + densities_[i]) *
							(b_[i + 1] - b_[i]) * result_type(.5);
				sp += areas_[i];
			}
			for (::std::size_t i{areas_.size()}; i > 1;)
			{
				--i;
				areas_[i] = areas_[i - 1] / sp;
			}
			areas_[0] = 0;
			for (::std::size_t i{1}; i != areas_.size(); ++i)
			{
				areas_[i] += areas_[i - 1];
			}
			for (::std::size_t i{}; i != densities_.size(); ++i)
			{
				densities_[i] /= sp;
			}
		}

		real_vector b_{};
		real_vector densities_{};
		real_vector areas_{};

		friend class piecewise_linear_distribution;
	};

	piecewise_linear_distribution() = default;
	template <typename InputIteratorB, typename InputIteratorW>
	piecewise_linear_distribution(
		InputIteratorB first_b, InputIteratorB last_b, InputIteratorW first_w)
		FAST_IO_HERBCEPTIONS_THROWS
		: param_(first_b, last_b, first_w)
	{}
	template <typename UnaryOperation>
	piecewise_linear_distribution(
		::std::initializer_list<result_type> bl, UnaryOperation fw)
		FAST_IO_HERBCEPTIONS_THROWS
		: param_(bl, fw)
	{}
	template <typename UnaryOperation>
	piecewise_linear_distribution(
		::std::size_t count, result_type xmin, result_type xmax,
		UnaryOperation fw)
		FAST_IO_HERBCEPTIONS_THROWS
		: param_(count, xmin, xmax, fw)
	{}
	explicit piecewise_linear_distribution(param_type const &parm)
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
		result_type u{gen(g)};
		::std::size_t const k{
			details::upper_bound_index(
				parm.areas_.data(), parm.areas_.size(), u) -
			1};
		u -= parm.areas_[k];
		result_type const dk{parm.densities_[k]};
		result_type const dk1{parm.densities_[k + 1]};
		result_type const deltad{dk1 - dk};
		result_type const bk{parm.b_[k]};
		if (deltad == 0)
		{
			return u / dk + bk;
		}
		result_type const bk1{parm.b_[k + 1]};
		result_type const deltab{bk1 - bk};
		return (bk * dk1 - bk1 * dk +
				details::math::sqrt(
					deltab * (deltab * dk * dk + 2 * deltad * u))) /
			   deltad;
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
		piecewise_linear_distribution const &lhs,
		piecewise_linear_distribution const &rhs)
		FAST_IO_HERBCEPTIONS_THROWS_IF_NOT_NOEXCEPT(lhs.param_ == rhs.param_)
	{
		return lhs.param_ == rhs.param_;
	}

private:
	param_type param_{};
};

} // namespace fast_io::random
