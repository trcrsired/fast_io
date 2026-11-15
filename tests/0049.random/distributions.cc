// Design test for the ported fast_io::random distributions.
//
// Every distribution is a direct port of libc++'s implementation. Under libc++
// the test therefore checks the strongest possible property: the generated
// sequence is bit-for-bit identical to std:: for the same engine and seed.
//
//   clang++ --config=$HOME/herbcfgs/x86_64-linux-gnu-libcxx.cfg \
//       -fherbceptions -lherbceptions -O2 -march=native \
//       tests/0049.random/distributions.cc -o /tmp/distributions

#include <fast_io.h>

#include <fast_io_dsal/random.h>

#include <cstdint>
#include <limits>
#include <ostream>
#include <random>
#include <type_traits>
#include <utility>

namespace
{

namespace random = ::fast_io::random;

template <typename D>
inline constexpr bool stream_insertable =
	requires(::std::ostream &os, D &dis) { os << dis; };

template <typename D>
inline constexpr bool stream_extractable =
	requires(::std::istream &is, D &dis) { is >> dis; };

// A full-range generator whose draw is noexcept: every distribution's
// operator() must be noexcept for it.
struct noexcept_engine
{
	using result_type = ::std::uint_least64_t;
	static constexpr result_type min() noexcept
	{
		return 0;
	}
	static constexpr result_type max() noexcept
	{
		return ~result_type{};
	}

	result_type state{1};

	result_type generate() noexcept
	{
		state = state * 6364136223846793005u + 1442695040888963407u;
		return state;
	}
};

template <typename OD, typename SD>
inline bool same_sequence(OD ours, SD stdlib) throws
{
	::std::mt19937_64 a{0x5eedu}, b{0x5eedu};
	for (::std::size_t i{}; i != 2000; ++i)
	{
		if (!(ours(a) == stdlib(b)))
		{
			return false;
		}
	}
	return true;
}

template <typename OD, typename SD>
inline bool same_sequence_32(OD ours, SD stdlib) throws
{
	::std::mt19937 a{0x5eedu}, b{0x5eedu};
	for (::std::size_t i{}; i != 2000; ++i)
	{
		if (!(ours(a) == stdlib(b)))
		{
			return false;
		}
	}
	return true;
}

template <typename OD, typename SD>
inline bool same_sequence_minstd(OD ours, SD stdlib) throws
{
	// minstd_rand has min() == 1 and a non-power-of-two range.
	::std::minstd_rand a{0x5eedu}, b{0x5eedu};
	for (::std::size_t i{}; i != 2000; ++i)
	{
		if (!(ours(a) == stdlib(b)))
		{
			return false;
		}
	}
	return true;
}

} // namespace

int main()
{
	int failures{};
	auto report = [&](bool ok, char const *what) throws {
		if (!ok)
		{
			++failures;
			::fast_io::io::println("FAIL ", ::fast_io::mnp::os_c_str(what));
		}
	};

	// ---------------------------------------------------------------------
	// API shape: result types, param_type equality and accessors.
	// ---------------------------------------------------------------------
	static_assert(
		::std::is_same_v<random::uniform_real_distribution<>::result_type, double>);
	static_assert(
		::std::is_same_v<random::normal_distribution<float>::result_type, float>);
	static_assert(
		::std::is_same_v<random::poisson_distribution<long>::result_type, long>);
	static_assert(::std::is_same_v<
				  random::binomial_distribution<unsigned>::result_type,
				  unsigned>);

	{
		random::normal_distribution<double> dis(1.0, 2.0);
		report(dis.mean() == 1.0 && dis.stddev() == 2.0, "normal accessors");
		random::normal_distribution<double>::param_type const parm(3.0, 4.0);
		dis.param(parm);
		report(dis.param() == parm, "normal param round-trip");
		report(dis == random::normal_distribution<double>(3.0, 4.0),
			   "normal equality");
		report(!(dis == random::normal_distribution<double>(3.0, 5.0)),
			   "normal inequality");
	}

	{
		// Default-constructed distributions must use the standard's defaults,
		// not the zero-initialized members behind the user-provided param_type
		// constructor.
		random::normal_distribution<double> const nd;
		report(nd.mean() == 0.0 && nd.stddev() == 1.0, "normal defaults");
		random::lognormal_distribution<double> const ln;
		report(ln.m() == 0.0 && ln.s() == 1.0, "lognormal defaults");
		random::exponential_distribution<double> const ex;
		report(ex.lambda() == 1.0, "exponential default");
		random::gamma_distribution<double> const ga;
		report(ga.alpha() == 1.0 && ga.beta() == 1.0, "gamma defaults");
		random::cauchy_distribution<double> const ca;
		report(ca.a() == 0.0 && ca.b() == 1.0, "cauchy defaults");
		random::weibull_distribution<double> const we;
		report(we.a() == 1.0 && we.b() == 1.0, "weibull defaults");
		random::extreme_value_distribution<double> const ev;
		report(ev.a() == 0.0 && ev.b() == 1.0, "extreme_value defaults");
		random::uniform_real_distribution<double> const ur;
		report(ur.a() == 0.0 && ur.b() == 1.0, "uniform_real defaults");
		random::poisson_distribution<int> const po;
		report(po.mean() == 1.0, "poisson default");
		random::binomial_distribution<int> const bi;
		report(bi.t() == 1 && bi.p() == 0.5, "binomial defaults");
		random::negative_binomial_distribution<int> const nb;
		report(nb.k() == 1 && nb.p() == 0.5, "negative_binomial defaults");
		random::geometric_distribution<int> const ge;
		report(ge.p() == 0.5, "geometric default");
		random::chi_squared_distribution<double> const cs;
		report(cs.n() == 1.0, "chi_squared default");
		random::fisher_f_distribution<double> const ff;
		report(ff.m() == 1.0 && ff.n() == 1.0, "fisher_f defaults");
		random::student_t_distribution<double> const st;
		report(st.n() == 1.0, "student_t default");
	}

	{
		random::gamma_distribution<double> dis(2.0, 3.0);
		report(dis.alpha() == 2.0 && dis.beta() == 3.0, "gamma accessors");
		random::binomial_distribution<int> bin(10, 0.25);
		report(bin.t() == 10 && bin.p() == 0.25 && bin.max() == 10,
			   "binomial accessors");
		random::negative_binomial_distribution<int> nb(4, 0.75);
		report(nb.k() == 4 && nb.p() == 0.75, "negative_binomial accessors");
		random::poisson_distribution<int> po(7.5);
		report(po.mean() == 7.5, "poisson accessor");
		random::geometric_distribution<int> ge(0.125);
		report(ge.p() == 0.125, "geometric accessor");
		random::weibull_distribution<double> we(5.0, 6.0);
		report(we.a() == 5.0 && we.b() == 6.0, "weibull accessors");
		random::extreme_value_distribution<double> ev(5.0, 6.0);
		report(ev.a() == 5.0 && ev.b() == 6.0, "extreme_value accessors");
		random::cauchy_distribution<double> ca(5.0, 6.0);
		report(ca.a() == 5.0 && ca.b() == 6.0, "cauchy accessors");
		random::fisher_f_distribution<double> ff(5.0, 6.0);
		report(ff.m() == 5.0 && ff.n() == 6.0, "fisher_f accessors");
		random::student_t_distribution<double> st(7.0);
		report(st.n() == 7.0, "student_t accessor");
		random::chi_squared_distribution<double> cs(8.0);
		report(cs.n() == 8.0, "chi_squared accessor");
		random::lognormal_distribution<double> ln(1.0, 2.0);
		report(ln.m() == 1.0 && ln.s() == 2.0, "lognormal accessors");
		random::exponential_distribution<double> ex(3.0);
		report(ex.lambda() == 3.0, "exponential accessor");
		random::uniform_real_distribution<double> ur(1.0, 4.0);
		report(ur.a() == 1.0 && ur.b() == 4.0 && ur.min() == 1.0 &&
				   ur.max() == 4.0,
			   "uniform_real accessors");
	}

	// ---------------------------------------------------------------------
	// Stream operators are gone from every distribution.
	// ---------------------------------------------------------------------
	static_assert(!stream_insertable<random::uniform_real_distribution<double>>);
	static_assert(!stream_extractable<random::uniform_real_distribution<double>>);
	static_assert(!stream_insertable<random::normal_distribution<double>>);
	static_assert(!stream_extractable<random::normal_distribution<double>>);
	static_assert(!stream_insertable<random::lognormal_distribution<double>>);
	static_assert(!stream_insertable<random::gamma_distribution<double>>);
	static_assert(!stream_insertable<random::chi_squared_distribution<double>>);
	static_assert(!stream_insertable<random::poisson_distribution<int>>);
	static_assert(!stream_insertable<random::binomial_distribution<int>>);
	static_assert(!stream_insertable<random::discrete_distribution<int>>);
	static_assert(
		!stream_insertable<random::piecewise_constant_distribution<double>>);
	static_assert(
		!stream_insertable<random::piecewise_linear_distribution<double>>);

	// ---------------------------------------------------------------------
	// Throws contract follows the engine's generate().
	// ---------------------------------------------------------------------
	{
		noexcept_engine eng;
		random::uniform_real_distribution<double> ur;
		random::normal_distribution<double> nd;
		random::gamma_distribution<double> ga(2.0);
		random::poisson_distribution<int> po(3.0);
		random::binomial_distribution<int> bi(10, 0.3);
		random::geometric_distribution<int> ge(0.3);
		static_assert(noexcept(ur(eng)));
		static_assert(noexcept(nd(eng)));
		static_assert(noexcept(ga(eng)));
		static_assert(noexcept(po(eng)));
		static_assert(noexcept(bi(eng)));
		static_assert(noexcept(ge(eng)));
	}

	try
	{
		// -------------------------------------------------------------------
		// discrete / piecewise: the replaced std::vector and algorithms still
		// produce the documented probability-related accessors.
		// -------------------------------------------------------------------
		{
			random::discrete_distribution<int> dis{1.0, 2.0, 1.0};
			auto const p{dis.probabilities()};
			report(p.size() == 3 && p[0] == 0.25 && p[1] == 0.5 && p[2] == 0.25,
				   "discrete probabilities");
			report(dis.min() == 0 && dis.max() == 2, "discrete bounds");
		}

		{
			random::piecewise_constant_distribution<double> dis(
				{0.0, 1.0, 2.0}, [](double x) { return x + 1.0; });
			report(dis.min() == 0.0 && dis.max() == 2.0, "piecewise_constant bounds");
			report(dis.intervals().size() == 3 && dis.densities().size() == 2,
				   "piecewise_constant shapes");
		}

		{
			random::piecewise_linear_distribution<double> dis(
				{0.0, 1.0, 2.0}, [](double x) { return x + 1.0; });
			report(dis.min() == 0.0 && dis.max() == 2.0, "piecewise_linear bounds");
			report(dis.intervals().size() == 3 && dis.densities().size() == 3,
				   "piecewise_linear shapes");
		}

		// -------------------------------------------------------------------
		// reset() drops cached state (normal's second variate).
		// -------------------------------------------------------------------
		{
			::std::mt19937_64 c{7}, d{7};
			random::normal_distribution<double> dis;
			double const x{dis(c)};
			dis.reset();
			report(x == dis(d), "normal reset drops the cached variate");
		}

#if defined(_LIBCPP_VERSION)
		// -------------------------------------------------------------------
		// Bit-for-bit agreement with libc++, which is the implementation that
		// was ported.
		// -------------------------------------------------------------------
		report(same_sequence(random::uniform_real_distribution<double>(0.0, 1.0),
							 ::std::uniform_real_distribution<double>(0.0, 1.0)),
			   "uniform_real matches libc++");
		report(same_sequence(random::uniform_real_distribution<float>(-2.0f, 3.0f),
							 ::std::uniform_real_distribution<float>(-2.0f, 3.0f)),
			   "uniform_real<float> matches libc++");
		report(same_sequence(random::normal_distribution<double>(1.0, 2.0),
							 ::std::normal_distribution<double>(1.0, 2.0)),
			   "normal matches libc++");
		report(same_sequence(random::normal_distribution<float>(1.0f, 2.0f),
							 ::std::normal_distribution<float>(1.0f, 2.0f)),
			   "normal<float> matches libc++");
		report(same_sequence(random::lognormal_distribution<double>(1.0, 2.0),
							 ::std::lognormal_distribution<double>(1.0, 2.0)),
			   "lognormal matches libc++");
		report(same_sequence(random::exponential_distribution<double>(2.0),
							 ::std::exponential_distribution<double>(2.0)),
			   "exponential matches libc++");
		report(same_sequence(random::cauchy_distribution<double>(1.0, 2.0),
							 ::std::cauchy_distribution<double>(1.0, 2.0)),
			   "cauchy matches libc++");
		report(same_sequence(random::cauchy_distribution<float>(1.0f, 2.0f),
							 ::std::cauchy_distribution<float>(1.0f, 2.0f)),
			   "cauchy<float> matches libc++");
		report(same_sequence(random::weibull_distribution<double>(2.0, 3.0),
							 ::std::weibull_distribution<double>(2.0, 3.0)),
			   "weibull matches libc++");
		report(same_sequence(random::extreme_value_distribution<double>(1.0, 2.0),
							 ::std::extreme_value_distribution<double>(1.0, 2.0)),
			   "extreme_value matches libc++");
		report(same_sequence(random::gamma_distribution<double>(2.5, 3.0),
							 ::std::gamma_distribution<double>(2.5, 3.0)),
			   "gamma(>1) matches libc++");
		report(same_sequence(random::gamma_distribution<double>(0.5, 3.0),
							 ::std::gamma_distribution<double>(0.5, 3.0)),
			   "gamma(<1) matches libc++");
		report(same_sequence(random::chi_squared_distribution<double>(3.0),
							 ::std::chi_squared_distribution<double>(3.0)),
			   "chi_squared matches libc++");
		report(same_sequence(random::fisher_f_distribution<double>(2.0, 3.0),
							 ::std::fisher_f_distribution<double>(2.0, 3.0)),
			   "fisher_f matches libc++");
		report(same_sequence(random::student_t_distribution<double>(4.0),
							 ::std::student_t_distribution<double>(4.0)),
			   "student_t matches libc++");
		report(same_sequence(random::poisson_distribution<int>(3.0),
							 ::std::poisson_distribution<int>(3.0)),
			   "poisson(<10) matches libc++");
		report(same_sequence(random::poisson_distribution<int>(30.0),
							 ::std::poisson_distribution<int>(30.0)),
			   "poisson(>10) matches libc++");
		report(same_sequence(random::binomial_distribution<int>(10, 0.3),
							 ::std::binomial_distribution<int>(10, 0.3)),
			   "binomial matches libc++");
		report(
			same_sequence(random::negative_binomial_distribution<int>(2, 0.3),
						  ::std::negative_binomial_distribution<int>(2, 0.3)),
			"negative_binomial matches libc++");
		report(same_sequence(random::geometric_distribution<int>(0.3),
							 ::std::geometric_distribution<int>(0.3)),
			   "geometric matches libc++");
		report(same_sequence(random::discrete_distribution<int>{1.0, 2.0, 3.0},
							 ::std::discrete_distribution<int>{1.0, 2.0, 3.0}),
			   "discrete matches libc++");
		report(same_sequence(
				   random::piecewise_constant_distribution<double>(
					   {0.0, 1.0, 2.0}, [](double x) { return x + 1.0; }),
				   ::std::piecewise_constant_distribution<double>(
					   {0.0, 1.0, 2.0}, [](double x) { return x + 1.0; })),
			   "piecewise_constant matches libc++");
		report(same_sequence(
				   random::piecewise_linear_distribution<double>(
					   {0.0, 1.0, 2.0}, [](double x) { return x + 1.0; }),
				   ::std::piecewise_linear_distribution<double>(
					   {0.0, 1.0, 2.0}, [](double x) { return x + 1.0; })),
			   "piecewise_linear matches libc++");

		// 32-bit engine: generate_canonical needs two draws, so this also pins
		// down the draw order and chunk assembly of the shared helper.
		report(same_sequence_32(
				   random::uniform_real_distribution<double>(0.0, 1.0),
				   ::std::uniform_real_distribution<double>(0.0, 1.0)),
			   "uniform_real matches libc++ (mt19937)");
		report(same_sequence_32(random::normal_distribution<double>(1.0, 2.0),
								::std::normal_distribution<double>(1.0, 2.0)),
			   "normal matches libc++ (mt19937)");
		report(same_sequence_32(random::gamma_distribution<double>(2.5, 3.0),
								::std::gamma_distribution<double>(2.5, 3.0)),
			   "gamma matches libc++ (mt19937)");
		report(same_sequence_32(random::poisson_distribution<int>(30.0),
								::std::poisson_distribution<int>(30.0)),
			   "poisson matches libc++ (mt19937)");
		report(same_sequence_32(random::binomial_distribution<int>(10, 0.3),
								::std::binomial_distribution<int>(10, 0.3)),
			   "binomial matches libc++ (mt19937)");
		report(same_sequence_32(
				   random::discrete_distribution<int>{1.0, 2.0, 3.0},
				   ::std::discrete_distribution<int>{1.0, 2.0, 3.0}),
			   "discrete matches libc++ (mt19937)");
		report(same_sequence_minstd(
				   random::uniform_real_distribution<double>(0.0, 1.0),
				   ::std::uniform_real_distribution<double>(0.0, 1.0)),
			   "uniform_real matches libc++ (minstd_rand)");
		report(same_sequence_minstd(
				   random::poisson_distribution<int>(30.0),
				   ::std::poisson_distribution<int>(30.0)),
			   "poisson matches libc++ (minstd_rand)");
#endif
	}
	catch throws(::std::error e)
	{
		::fast_io::io::println("unexpected error: ", e);
		return 2;
	}

	if (failures != 0)
	{
		::fast_io::io::println(failures, " check(s) failed");
		return 1;
	}
	::fast_io::io::println("distributions: ok");
	return 0;
}
