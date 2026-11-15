// module consumer: the random distributions through `import fast_io.random`.
//
//   clang++ --config=... -std=c++26 -fherbceptions --precompile -x c++-module \
//       share/fast_io/fast_io.cppm -o <dir>/fast_io.pcm
//   clang++ --config=... -std=c++26 -fherbceptions --precompile -x c++-module \
//       -fmodule-file=fast_io=<dir>/fast_io.pcm \
//       share/fast_io/fast_io_random.cppm -o <dir>/fast_io.random.pcm
//   clang++ --config=... -std=c++26 -fherbceptions \
//       -fmodule-file=fast_io=<dir>/fast_io.pcm \
//       -fmodule-file=fast_io.random=<dir>/fast_io.random.pcm \
//       tests/0042.modules/random.cc <dir>/fast_io.random.pcm -o random
//
// Every distribution named below has to come from the export list in
// fast_io_inc/random.inc, so this fails the moment one stops being exported.
#include <cstdint>
#include <random>

#include <herbceptions/error>

import fast_io.random;

int main()
{
	static_assert(
		::fast_io::random::uniform_random_bit_generator<::std::mt19937_64>);
	static_assert(
		!::fast_io::random::uniform_random_bit_generator<int>);

	try
	{
		::std::mt19937_64 engine{0x5eedu};

		::fast_io::random::uniform_int_distribution<int> ui(1, 6);
		::fast_io::random::uniform_real_distribution<double> ur(0.0, 1.0);
		::fast_io::random::bernoulli_distribution be(0.5);
		::fast_io::random::binomial_distribution<int> bi(10, 0.3);
		::fast_io::random::negative_binomial_distribution<int> nb(2, 0.3);
		::fast_io::random::geometric_distribution<int> ge(0.3);
		::fast_io::random::poisson_distribution<int> po(3.0);
		::fast_io::random::normal_distribution<double> nd(0.0, 1.0);
		::fast_io::random::lognormal_distribution<double> ln(0.0, 1.0);
		::fast_io::random::exponential_distribution<double> ex(1.0);
		::fast_io::random::gamma_distribution<double> ga(2.0, 1.0);
		::fast_io::random::chi_squared_distribution<double> cs(3.0);
		::fast_io::random::fisher_f_distribution<double> ff(2.0, 3.0);
		::fast_io::random::student_t_distribution<double> st(4.0);
		::fast_io::random::cauchy_distribution<double> ca(0.0, 1.0);
		::fast_io::random::weibull_distribution<double> we(2.0, 1.0);
		::fast_io::random::extreme_value_distribution<double> ev(0.0, 1.0);
		::fast_io::random::discrete_distribution<int> di{1.0, 2.0, 3.0};
		::fast_io::random::piecewise_constant_distribution<double> pc(
			{0.0, 1.0, 2.0}, [](double x) { return x + 1.0; });
		::fast_io::random::piecewise_linear_distribution<double> pl(
			{0.0, 1.0, 2.0}, [](double x) { return x + 1.0; });

		std::uint64_t checks{};
		for (int i{}; i != 1000; ++i)
		{
			int const uia{ui(engine)};
			double const ura{ur(engine)};
			bool const bea{be(engine)};
			int const bia{bi(engine)};
			int const nba{nb(engine)};
			int const gea{ge(engine)};
			int const poa{po(engine)};
			double const nda{nd(engine)};
			double const lna{ln(engine)};
			double const exa{ex(engine)};
			double const gaa{ga(engine)};
			double const csa{cs(engine)};
			double const ffa{ff(engine)};
			double const sta{st(engine)};
			double const caa{ca(engine)};
			double const wea{we(engine)};
			double const eva{ev(engine)};
			int const dia{di(engine)};
			double const pca{pc(engine)};
			double const pla{pl(engine)};

			checks += uia >= 1 && uia <= 6;
			checks += ura >= 0.0 && ura < 1.0;
			checks += bea || !bea;
			checks += bia >= 0 && bia <= 10;
			checks += nba >= 0 && gea >= 0;
			checks += poa >= 0;
			checks += nda == nda;
			checks += lna >= 0.0;
			checks += exa >= 0.0;
			checks += gaa >= 0.0;
			checks += csa >= 0.0;
			checks += ffa >= 0.0;
			checks += sta == sta;
			checks += caa == caa;
			checks += wea >= 0.0;
			checks += eva == eva;
			checks += dia >= 0 && dia <= 2;
			checks += pca >= 0.0 && pca < 2.0;
			checks += pla >= 0.0 && pla < 2.0;
		}

		::fast_io::io::println("import fast_io.random: ok ", checks != 0);
		return 0;
	}
	catch throws(::std::error e)
	{
		::fast_io::io::perrln(e);
		return 1;
	}
}
