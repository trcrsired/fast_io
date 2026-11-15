// Design test for fast_io::random::bernoulli_distribution.
//
//   clang++ --config=$HOME/herbcfgs/x86_64-linux-gnu-libcxx.cfg \
//       -fherbceptions -lherbceptions -O2 -march=native \
//       tests/0049.random/bernoulli_distribution.cc -o /tmp/bernoulli_distribution

#include <fast_io.h>

#include <fast_io_dsal/random.h>

#include <cerrno>
#include <cstdint>
#include <limits>
#include <ostream>
#include <random>
#include <type_traits>

namespace
{

namespace random = ::fast_io::random;

template <typename D>
inline constexpr bool stream_insertable =
	requires(::std::ostream &os, D &dis) { os << dis; };

template <typename D>
inline constexpr bool stream_extractable =
	requires(::std::istream &is, D &dis) { is >> dis; };

template <typename G>
inline constexpr bool distribution_accepts =
	requires(random::bernoulli_distribution &dis, G &g) { dis(g); };

struct both_ways_engine
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

	result_type state{};
	::std::size_t generate_calls{};
	::std::size_t call_calls{};

	result_type generate() noexcept
	{
		++generate_calls;
		state = state * 6364136223846793005u + 1442695040888963407u;
		return state;
	}

	result_type operator()() noexcept
	{
		++call_calls;
		return 0;
	}
};

struct generate_only_engine
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

	result_type state{};

	result_type generate() noexcept
	{
		state = state * 6364136223846793005u + 1442695040888963407u;
		return state;
	}

	result_type operator()() = delete;
};

// generate() is noexcept while operator() throws: the throws contract must
// follow generate(), the entry point actually used.
struct generate_wins_engine
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

	result_type generate() const noexcept
	{
		return 0x8000000000000000u;
	}
	result_type operator()() FAST_IO_HERBCEPTIONS_THROWS
	{
		::fast_io::throw_posix_error(EIO);
	}
};

// generate() reports a herbception; operator() pretends everything is fine.
struct throwing_engine
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

	result_type generate() FAST_IO_HERBCEPTIONS_THROWS
	{
		::fast_io::throw_posix_error(EIO);
	}

	result_type operator()() noexcept
	{
		return 0;
	}
};

struct counting_32_engine
{
	using result_type = ::std::uint_least32_t;
	static constexpr result_type min() noexcept
	{
		return 0;
	}
	static constexpr result_type max() noexcept
	{
		return 0xFFFFFFFFu;
	}

	result_type state{};
	::std::size_t calls{};

	result_type generate() noexcept
	{
		++calls;
		state = state * 1664525u + 1013904223u;
		return state;
	}
};

template <typename G>
inline bool rate_is(G &g, double p, double expected, double tolerance, ::std::size_t n) throws
{
	random::bernoulli_distribution dis(p);
	::std::size_t hits{};
	for (::std::size_t i{}; i != n; ++i)
	{
		hits += dis(g) ? 1u : 0u;
	}
	double const rate{static_cast<double>(hits) / static_cast<double>(n)};
	return expected - tolerance <= rate && rate <= expected + tolerance;
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

	static_assert(::std::is_same_v<random::bernoulli_distribution::result_type, bool>);
	static_assert(random::bernoulli_distribution{}.p() == 0.5);
	static_assert(random::bernoulli_distribution{}.min() == false);
	static_assert(random::bernoulli_distribution{}.max() == true);
	static_assert(random::bernoulli_distribution{0.25}.p() == 0.25);
	static_assert(random::bernoulli_distribution::param_type{}.p() == 0.5);
	static_assert(random::bernoulli_distribution::param_type{0.75}.p() == 0.75);
	static_assert(random::bernoulli_distribution::param_type{0.25} ==
				  random::bernoulli_distribution::param_type{0.25});
	static_assert(!(random::bernoulli_distribution::param_type{0.25} ==
					random::bernoulli_distribution::param_type{0.5}));
	static_assert(random::bernoulli_distribution{0.25} == random::bernoulli_distribution{0.25});
	static_assert(!(random::bernoulli_distribution{0.25} == random::bernoulli_distribution{0.5}));

	// No stream operators, by design.
	static_assert(!stream_insertable<random::bernoulli_distribution>);
	static_assert(!stream_extractable<random::bernoulli_distribution>);

	// Engine constraints.
	static_assert(distribution_accepts<::std::mt19937_64>);
	static_assert(distribution_accepts<both_ways_engine>);
	static_assert(distribution_accepts<generate_only_engine>);

	// Throws contract follows generate() when present.
	static_assert(noexcept(::std::declval<random::bernoulli_distribution &>()(::std::declval<both_ways_engine &>())));
	static_assert(noexcept(::std::declval<random::bernoulli_distribution &>()(::std::declval<generate_wins_engine &>())));
	static_assert(!noexcept(::std::declval<random::bernoulli_distribution &>()(::std::declval<throwing_engine &>())));
	static_assert(!noexcept(::std::declval<random::bernoulli_distribution &>()(::std::declval<::std::mt19937 &>())));

	try
	{
		// Boundary probabilities.
		{
			::std::mt19937_64 eng{1};
			random::bernoulli_distribution never(0.0);
			random::bernoulli_distribution always(1.0);
			bool ok{true};
			for (::std::size_t i{}; i != 4096; ++i)
			{
				ok = ok && !never(eng);
				ok = ok && always(eng);
			}
			report(ok, "p=0 is always false, p=1 is always true");
		}

		// Documented out-of-range behavior: defined, never UB.
		{
			::std::mt19937_64 eng{2};
			random::bernoulli_distribution negative(-0.25);
			random::bernoulli_distribution over(1.25);
			random::bernoulli_distribution nan(::std::numeric_limits<double>::quiet_NaN());
			bool ok{true};
			for (::std::size_t i{}; i != 1024; ++i)
			{
				ok = ok && !negative(eng);
				ok = ok && over(eng);
				ok = ok && !nan(eng);
			}
			report(ok, "p<0 / p>1 / NaN are defined (false / true / false)");
		}

		// Probabilities, 200k trials, tolerance is ~5 sigma or looser.
		{
			::std::mt19937_64 eng{0x5eed1234u};
			report(rate_is(eng, 0.5, 0.5, 0.01, 200000), "p=0.5 rate");
			report(rate_is(eng, 0.25, 0.25, 0.01, 200000), "p=0.25 rate");
			report(rate_is(eng, 0.9, 0.9, 0.01, 200000), "p=0.9 rate");
			report(rate_is(eng, 0.01, 0.01, 0.002, 200000), "p=0.01 rate");
			report(rate_is(eng, 1.0 / 3.0, 1.0 / 3.0, 0.01, 200000), "p=1/3 rate");
		}

		// Narrow engine: same probabilities, one draw per trial.
		{
			::std::mt19937 eng{0x5eed1234u};
			report(rate_is(eng, 0.5, 0.5, 0.01, 200000), "p=0.5 rate on 32-bit engine");
		}
		{
			counting_32_engine eng;
			random::bernoulli_distribution dis(0.5);
			constexpr ::std::size_t samples{256};
			for (::std::size_t i{}; i != samples; ++i)
			{
				(void)dis(eng);
			}
			report(eng.calls == 2 * samples, "53-bit canonical double costs two draws from a 32-bit engine");
		}

		// generate() preferred over operator(), even when operator() also exists.
		{
			both_ways_engine eng;
			random::bernoulli_distribution dis(0.5);
			for (::std::size_t i{}; i != 64; ++i)
			{
				(void)dis(eng);
			}
			report(eng.generate_calls == 64, "one 64-bit draw per trial");
			report(eng.call_calls == 0, "operator() was not used");
		}

		// generate()-only engine.
		{
			generate_only_engine eng;
			random::bernoulli_distribution dis(0.5);
			bool ok{true};
			for (::std::size_t i{}; i != 64; ++i)
			{
				(void)dis(eng);
				ok = true;
			}
			report(ok, "generate()-only engine");
		}

		// The param overload ignores the stored probability.
		{
			::std::mt19937_64 eng{3};
			random::bernoulli_distribution dis(0.0);
			bool ok{true};
			for (::std::size_t i{}; i != 256; ++i)
			{
				ok = ok && dis(eng, random::bernoulli_distribution::param_type{1.0});
			}
			report(ok, "operator()(g, p) uses the argument");
		}

		// A throwing generate() propagates.
		{
			bool caught{};
			try
			{
				random::bernoulli_distribution dis(0.5);
				throwing_engine eng;
				(void)dis(eng);
			}
			catch throws(::std::error e)
			{
				caught = true;
				::fast_io::io::println("caught through bernoulli: ", e);
			}
			report(caught, "throwing generate() propagates");
		}

		// White hole integration.
		{
			::fast_io::ibuf_white_hole_engine eng;
			random::bernoulli_distribution dis(0.5);
			::std::size_t hits{};
			constexpr ::std::size_t samples{20000};
			for (::std::size_t i{}; i != samples; ++i)
			{
				hits += dis(eng) ? 1u : 0u;
			}
			report(8000 <= hits && hits <= 12000, "white hole integration");
		}

		// Same seed, same sequence.
		{
			::std::mt19937_64 a{42}, b{42};
			random::bernoulli_distribution dis(0.3);
			bool same{true};
			for (::std::size_t i{}; i != 1024; ++i)
			{
				same = same && (dis(a) == dis(b));
			}
			report(same, "deterministic for the same seed");
		}

#if defined(_LIBCPP_VERSION)
		// The algorithm is libc++'s, so the sequences must agree exactly.
		{
			::std::mt19937_64 a{0x1234u}, b{0x1234u};
			::std::mt19937 c{0x5678u}, d{0x5678u};
			bool same{true};
			random::bernoulli_distribution o64(0.3), o32(0.7);
			::std::bernoulli_distribution s64(0.3), s32(0.7);
			for (::std::size_t i{}; i != 4096; ++i)
			{
				same = same && (o64(a) == s64(b));
				same = same && (o32(c) == s32(d));
			}
			report(same, "matches libc++ bernoulli bit-for-bit");
		}
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
	::fast_io::io::println("bernoulli_distribution: ok");
	return 0;
}
