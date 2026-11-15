// Design test for fast_io::random::uniform_int_distribution.
//
// Everything here is a design constraint, not a conformance suite:
//   * every least-width integer type from 8 to 64 bits instantiates;
//   * results stay inside the closed interval, including negative intervals;
//   * the full range of a narrow type is reached and roughly uniform;
//   * a wide target is filled from a narrow engine;
//   * generate() is preferred over operator() when both exist;
//   * an engine with only generate() works;
//   * a throwing generate() propagates through a `throws` operator();
//   * stream operators are absent.
//
// Build:
//   clang++ --config=$HOME/herbcfgs/x86_64-linux-gnu-libcxx.cfg \
//       -fherbceptions -O2 -march=native \
//       tests/0049.random/uniform_int_distribution.cc -o /tmp/uniform_int_distribution

#include <fast_io.h>

#include <fast_io_dsal/random.h>

#include <cerrno>
#include <cstdint>
#include <ostream>
#include <random>
#include <type_traits>

namespace
{

namespace random = ::fast_io::random;

using i8 = ::std::int_least8_t;
using u8 = ::std::uint_least8_t;
using i16 = ::std::int_least16_t;
using u16 = ::std::uint_least16_t;
using i32 = ::std::int_least32_t;
using u32 = ::std::uint_least32_t;
using i64 = ::std::int_least64_t;
using u64 = ::std::uint_least64_t;

// Dependent variable templates: on this compiler an unsatisfied
// expression inside a non-dependent requires-expression is a hard error.
template <typename D>
inline constexpr bool stream_insertable =
	requires(::std::ostream &os, D &dis) { os << dis; };

template <typename D>
inline constexpr bool stream_extractable =
	requires(::std::istream &is, D &dis) { is >> dis; };

// Dependent so a rejected engine/type is a substitution failure here.
template <typename G>
inline constexpr bool distribution_accepts =
	requires(random::uniform_int_distribution<int> &dis, G &g) { dis(g); };

template <typename T>
inline constexpr bool distribution_exists =
	requires { typename random::uniform_int_distribution<T>; };

struct no_min_engine
{
	using result_type = u64;
	static constexpr result_type max() noexcept
	{
		return ~result_type{};
	}
	result_type generate() noexcept
	{
		return 0;
	}
};

struct degenerate_engine
{
	using result_type = u64;
	static constexpr result_type min() noexcept
	{
		return 0;
	}
	static constexpr result_type max() noexcept
	{
		return 0;
	}
	result_type generate() noexcept
	{
		return 0;
	}
};

struct signed_result_engine
{
	using result_type = ::std::int_least64_t;
	static constexpr result_type min() noexcept
	{
		return 0;
	}
	static constexpr result_type max() noexcept
	{
		return 100;
	}
	result_type generate() noexcept
	{
		return 0;
	}
};

struct no_draw_engine
{
	using result_type = u64;
	static constexpr result_type min() noexcept
	{
		return 0;
	}
	static constexpr result_type max() noexcept
	{
		return ~result_type{};
	}
};

struct void_generate_engine
{
	using result_type = u64;
	static constexpr result_type min() noexcept
	{
		return 0;
	}
	static constexpr result_type max() noexcept
	{
		return ~result_type{};
	}
	void generate() noexcept
	{}
};

template <typename T, typename G>
inline bool check_bounds(G &g, T first, T last, ::std::size_t n) throws
{
	random::uniform_int_distribution<T> dis(first, last);
	if (dis.a() != first || dis.b() != last || dis.min() != first || dis.max() != last)
	{
		return false;
	}
	if (!(dis == random::uniform_int_distribution<T>(first, last)))
	{
		return false;
	}
	for (::std::size_t i{}; i != n; ++i)
	{
		T const value{dis(g)};
		if (value < first || last < value)
		{
			return false;
		}
	}
	dis.reset();
	return true;
}

// An engine exposing both entry points. If the distribution reached for
// operator() the counters below would show it.
struct both_ways_engine
{
	using result_type = u64;
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

	// Deliberately poisoned: reaching it means generate() was not preferred.
	result_type operator()() noexcept
	{
		++call_calls;
		return 0;
	}
};

// generate() only; operator() is not usable at all.
struct generate_only_engine
{
	using result_type = u64;
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

// generate() reports a herbception; operator() pretends everything is fine.
struct throwing_engine
{
	using result_type = u64;
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

// Non-power-of-two range: the rejection bucket path is exercised.
struct lcg_engine
{
	using result_type = u32;
	static constexpr result_type min() noexcept
	{
		return 1;
	}
	static constexpr result_type max() noexcept
	{
		return 1000;
	}

	result_type state{12345};

	result_type operator()() noexcept
	{
		state = state * 1664525u + 1013904223u;
		return static_cast<result_type>(state % 1000u + 1u);
	}
};

// generate() is noexcept while operator() throws. The contract must follow
// generate(), because that is the entry point the distribution actually uses.
struct generate_wins_engine
{
	using result_type = u64;
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
		return 0x0102030405060708u;
	}
	result_type operator()() FAST_IO_HERBCEPTIONS_THROWS
	{
		::fast_io::throw_posix_error(EIO);
	}
};

// Counts draws, and is narrow enough that a 64-bit result needs two.
struct counting_32_engine
{
	using result_type = u32;
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

// Always returns the same word, so the extracted bits are observable.
struct constant_engine
{
	using result_type = u64;
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
		return 0xABCD000000000000u;
	}
};

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

	static_assert(::std::is_same_v<random::uniform_int_distribution<>::result_type, int>);
	static_assert(::std::is_same_v<random::uniform_int_distribution<i8>::result_type, i8>);
	static_assert(::std::is_same_v<random::uniform_int_distribution<u64>::result_type, u64>);

	// Type and engine constraints.
	static_assert(distribution_exists<int>);
	static_assert(distribution_exists<i8>);
	static_assert(distribution_exists<u64>);
	static_assert(!distribution_exists<bool>);
	static_assert(!distribution_exists<__int128>);
	static_assert(distribution_accepts<::std::mt19937_64>);
	static_assert(distribution_accepts<both_ways_engine>);
	static_assert(distribution_accepts<generate_only_engine>);
	static_assert(distribution_accepts<generate_wins_engine>);
	static_assert(!distribution_accepts<no_min_engine>);
	static_assert(!distribution_accepts<degenerate_engine>);
	static_assert(!distribution_accepts<signed_result_engine>);
	static_assert(!distribution_accepts<no_draw_engine>);
	static_assert(!distribution_accepts<void_generate_engine>);

	// Default intervals follow numeric_limits<T>; a defaults to 0, as in the
	// standard, so the signed default is [0, max], not the whole type domain.
	static_assert(random::uniform_int_distribution<i8>{}.min() == 0);
	static_assert(random::uniform_int_distribution<i8>{}.max() == 127);
	static_assert(random::uniform_int_distribution<u8>{}.max() == 255);
	static_assert(random::uniform_int_distribution<i16>{}.max() == 32767);
	static_assert(random::uniform_int_distribution<u16>{}.max() == 65535);
	static_assert(random::uniform_int_distribution<i32>{}.max() == 2147483647);
	static_assert(random::uniform_int_distribution<u32>{}.max() == 4294967295u);
	static_assert(random::uniform_int_distribution<i64>{}.max() == 9223372036854775807);
	static_assert(random::uniform_int_distribution<u64>{}.max() == 18446744073709551615u);

	// No stream operators: this is the compile-time win over <random>.
	// Dependent on D so an unsatisfied expression is a substitution failure on
	// this compiler rather than an eager diagnostic.
	static_assert(!stream_insertable<random::uniform_int_distribution<>>);
	static_assert(!stream_extractable<random::uniform_int_distribution<>>);

	// param_type shape.
	using param = random::uniform_int_distribution<i16>::param_type;
	static_assert(param{}.a() == 0);
	static_assert(param{}.b() == 32767);
	static_assert(param{-3, 7} == param{-3, 7});
	static_assert(!(param{-3, 7} == param{-3, 8}));

	::std::mt19937_64 eng64{0x5eed1234u};
	::std::mt19937 eng32{0x5eed1234u};

	try
	{
		report(check_bounds<i8>(eng64, ::std::numeric_limits<i8>::min(), ::std::numeric_limits<i8>::max(), 4096), "i8 full range");
		report(check_bounds<u8>(eng64, 0, 255, 4096), "u8 full range");
		report(check_bounds<i16>(eng64, ::std::numeric_limits<i16>::min(), ::std::numeric_limits<i16>::max(), 4096), "i16 full range");
		report(check_bounds<u16>(eng64, 0, 65535, 4096), "u16 full range");
		report(check_bounds<i32>(eng64, ::std::numeric_limits<i32>::min(), ::std::numeric_limits<i32>::max(), 4096), "i32 full range");
		report(check_bounds<u32>(eng64, 0, 4294967295u, 4096), "u32 full range");
		report(check_bounds<i64>(eng64, ::std::numeric_limits<i64>::min(), ::std::numeric_limits<i64>::max(), 4096), "i64 full range");
		report(check_bounds<u64>(eng64, 0, ::std::numeric_limits<u64>::max(), 4096), "u64 full range");

		// Negative-only and negative-start intervals.
		report(check_bounds<i8>(eng64, -128, -1, 4096), "i8 negative only");
		report(check_bounds<i8>(eng64, -10, 10, 4096), "i8 straddling");
		report(check_bounds<i16>(eng64, -1000, -999, 4096), "i16 two values");
		report(check_bounds<i64>(eng64, -1000000000000ll, -999999999999ll, 4096), "i64 negative narrow");

		// A degenerate interval collapses to its single value.
		{
			random::uniform_int_distribution<u32> dis(42, 42);
			bool ok{true};
			for (::std::size_t i{}; i != 64; ++i)
			{
				ok = ok && dis(eng64) == 42;
			}
			report(ok, "u32 degenerate interval");
			report(dis(eng64, random::uniform_int_distribution<u32>::param_type{7, 7}) == 7, "u32 param degenerate interval");
		}

		// param() round-trips and param(set) replaces the stored interval.
		{
			using dis32 = random::uniform_int_distribution<i32>;
			dis32 dis(1, 10);
			dis32::param_type const stored{dis.param()};
			report(stored.a() == 1 && stored.b() == 10, "param() round-trips");
			dis.param(dis32::param_type{-5, 5});
			report(dis.a() == -5 && dis.b() == 5, "param(set) replaces the interval");
			bool ok{true};
			for (::std::size_t i{}; i != 64; ++i)
			{
				i32 const value{dis(eng64, dis32::param_type{100, 100})};
				ok = ok && value == 100;
			}
			report(ok, "operator()(g, p) uses p, not the stored interval");
		}

		// Whole u8 domain reached, counts within a wide band around 64 samples.
		{
			random::uniform_int_distribution<u8> dis;
			constexpr ::std::size_t samples{256 * 64};
			::std::size_t counts[256]{};
			for (::std::size_t i{}; i != samples; ++i)
			{
				++counts[dis(eng64)];
			}
			bool every_value_seen{true};
			bool counts_plausible{true};
			for (auto count : counts)
			{
				every_value_seen = every_value_seen && count != 0;
				counts_plausible = counts_plausible && 16 <= count && count <= 200;
			}
			report(every_value_seen, "u8 every value reached");
			report(counts_plausible, "u8 counts plausible");
		}

		// Dice: the small-range path with a wide engine.
		{
			random::uniform_int_distribution<u32> dis(1, 6);
			::std::size_t counts[7]{};
			constexpr ::std::size_t samples{60000};
			for (::std::size_t i{}; i != samples; ++i)
			{
				++counts[dis(eng64)];
			}
			bool ok{true};
			for (unsigned face{1}; face != 7; ++face)
			{
				ok = ok && 9000 <= counts[face] && counts[face] <= 11000;
			}
			report(ok, "dice faces roughly uniform");
		}

		// Narrow engine feeding a 64-bit result: high bits must move.
		{
			random::uniform_int_distribution<u64> dis;
			u64 seen{};
			for (::std::size_t i{}; i != 1024; ++i)
			{
				seen |= dis(eng32);
			}
			report(seen == ::std::numeric_limits<u64>::max(), "u64 from 32-bit engine fills every bit");
		}

		// Non-power-of-two engine range [1, 1000]: rejection bucket, all values.
		{
			lcg_engine eng;
			random::uniform_int_distribution<u32> dis(0, 999);
			bool covered[1000]{};
			::std::size_t samples{100000};
			for (::std::size_t i{}; i != samples; ++i)
			{
				covered[dis(eng)] = true;
			}
			bool all{true};
			for (bool c : covered)
			{
				all = all && c;
			}
			report(all, "engine with gap [1,1000] reaches every result");
		}

		// generate() is preferred over operator().
		{
			both_ways_engine eng;
			random::uniform_int_distribution<u32> dis(0, 99);
			for (::std::size_t i{}; i != 64; ++i)
			{
				(void)dis(eng);
			}
			report(eng.generate_calls != 0, "generate() was used");
			report(eng.call_calls == 0, "operator() was not used");
		}

		// generate()-only engine.
		{
			generate_only_engine eng;
			random::uniform_int_distribution<u32> dis(0, 9);
			bool ok{true};
			for (::std::size_t i{}; i != 64; ++i)
			{
				u32 const value{dis(eng)};
				ok = ok && value <= 9;
			}
			report(ok, "generate()-only engine");
		}

		// A nothrow engine gives a nothrow distribution call. Standard-library
		// engines do not mark operator() noexcept, so they are conservatively
		// treated as throwing; a fast_io engine that marks it noexcept is not.
		static_assert(noexcept(::std::declval<random::uniform_int_distribution<u32> &>()(::std::declval<both_ways_engine &>())));
		static_assert(!noexcept(::std::declval<random::uniform_int_distribution<u32> &>()(::std::declval<::std::mt19937 &>())));

		// The condition is generate()'s, not operator()'s: a throwing operator()
		// must not make the call throwing when generate() is the one used.
		static_assert(noexcept(::std::declval<random::uniform_int_distribution<u32> &>()(::std::declval<generate_wins_engine &>())));
		{
			generate_wins_engine eng;
			random::uniform_int_distribution<u32> dis;
			report(dis(eng) == 0x01020304u, "noexcept generate() wins over throwing operator()");
		}

		// A throwing generate() makes the call throwing, and the error arrives.
		static_assert(!noexcept(::std::declval<random::uniform_int_distribution<u32> &>()(::std::declval<throwing_engine &>())));
		{
			bool caught{};
			try
			{
				random::uniform_int_distribution<u32> dis(0, 9);
				throwing_engine eng;
				(void)dis(eng);
			}
			catch throws(::std::error e)
			{
				caught = true;
				::fast_io::io::println("caught through distribution: ", e);
			}
			report(caught, "throwing generate() propagates");
		}

		// Integration with the real fast_io white hole engine.
		{
			::fast_io::ibuf_white_hole_engine eng;
			random::uniform_int_distribution<u64> dis(0, 999999);
			bool ok{true};
			for (::std::size_t i{}; i != 1000; ++i)
			{
				ok = ok && dis(eng) <= 999999;
			}
			report(ok, "white hole engine integration");
		}

		// A standard engine with a non-power-of-two range and min() != 0.
		{
			::std::minstd_rand eng{123};
			random::uniform_int_distribution<u32> dis(0, 999);
			bool covered[1000]{};
			for (::std::size_t i{}; i != 60000; ++i)
			{
				covered[dis(eng)] = true;
			}
			bool all{true};
			for (bool c : covered)
			{
				all = all && c;
			}
			report(all, "std::minstd_rand reaches every result");
		}

		// Draw count: a 64-bit engine feeds every width in one draw, a 32-bit
		// engine needs exactly two for a 64-bit result.
		{
			both_ways_engine eng;
			random::uniform_int_distribution<u64> dis;
			constexpr ::std::size_t samples{128};
			for (::std::size_t i{}; i != samples; ++i)
			{
				(void)dis(eng);
			}
			report(eng.generate_calls == samples, "u64 from 64-bit engine costs one draw per result");
		}
		{
			counting_32_engine eng;
			random::uniform_int_distribution<u64> dis;
			constexpr ::std::size_t samples{128};
			for (::std::size_t i{}; i != samples; ++i)
			{
				(void)dis(eng);
			}
			report(eng.calls == 2 * samples, "u64 from 32-bit engine costs two draws per result");
		}
		{
			counting_32_engine eng;
			random::uniform_int_distribution<u8> dis;
			constexpr ::std::size_t samples{128};
			for (::std::size_t i{}; i != samples; ++i)
			{
				(void)dis(eng);
			}
			report(eng.calls == samples, "u8 from 32-bit engine costs one draw per result");
		}

		// The chunk is taken from the top of the engine word.
		{
			constant_engine eng;
			random::uniform_int_distribution<u8> d8;
			random::uniform_int_distribution<u16> d16;
			random::uniform_int_distribution<u32> d32;
			random::uniform_int_distribution<u64> d64;
			report(d8(eng) == 0xAB, "u8 uses the top 8 bits");
			report(d16(eng) == 0xABCD, "u16 uses the top 16 bits");
			report(d32(eng) == 0xABCD0000u, "u32 uses the top 32 bits");
			report(d64(eng) == 0xABCD000000000000u, "u64 uses the whole word");

			// Signed intervals: the offset wraps through the unsigned type and back.
			// 0xAB = 171, so [-128, 127] maps it to 171 - 128 = 43.
			random::uniform_int_distribution<i8> dsigned(-128, 127);
			report(dsigned(eng) == 43, "i8 full range maps the offset");
			// [-128, -1] has span 127, so 7 bits are taken: 0xAB top 7 = 85,
			// and unsigned 128 + 85 = 213 wraps to signed -43.
			random::uniform_int_distribution<i8> dlow(-128, -1);
			report(dlow(eng) == -43, "i8 negative range maps the offset");
		}
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
	::fast_io::io::println("uniform_int_distribution: ok");
	return 0;
}
