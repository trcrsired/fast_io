# fast_io random distributions

The full set of `fast_io::random` distributions. Every distribution is a direct
port of libc++'s implementation (the fastest of libc++, libstdc++ and the MSVC
STL in the benchmarks below), with `operator!=` dropped (C++20 synthesizes it
from `operator==`) and the stream operators omitted. Herbceptions are supported
throughout. Engine implementations are out of scope; only the shared
engine-reading seam lives here.

Include the public header:

```cpp
#include <fast_io_dsal/random.h>
```

or import the module (built from `share/fast_io/fast_io_random.cppm`):

```cpp
import fast_io.random;
```

## Files

- `engine.h` -- the engine concept, `generate()` preference, throw contract,
  bucket/bit helpers, `valid_real_type` / `valid_int_type`.
- `generate_canonical.h` -- libc++'s `generate_canonical` port.
- `math.h` -- the handful of libm entry points, routed through compiler
  builtins exactly like libc++'s `__math::*` wrappers.
- `clamp_to_integral.h` -- libc++'s float-to-integer clamp helper.
- `upper_bound.h` -- a local `upper_bound` index helper (freestanding: no
  `<algorithm>`).
- `uniform_int_distribution.h` -- integer distribution.
- `uniform_real_distribution.h` -- real distribution.
- `bernoulli_distribution.h`, `binomial_distribution.h`,
  `negative_binomial_distribution.h`, `geometric_distribution.h`,
  `poisson_distribution.h` -- discrete distributions.
- `normal_distribution.h`, `lognormal_distribution.h`,
  `exponential_distribution.h`, `gamma_distribution.h`,
  `chi_squared_distribution.h`, `fisher_f_distribution.h`,
  `student_t_distribution.h`, `cauchy_distribution.h`,
  `weibull_distribution.h`, `extreme_value_distribution.h` -- continuous
  distributions.
- `discrete_distribution.h`, `piecewise_constant_distribution.h`,
  `piecewise_linear_distribution.h` -- table-driven distributions (fast_io
  containers instead of `std::vector`).
- `random.h` -- umbrella, included by `<fast_io_dsal/random.h>`.
- `tests/0049.random/{uniform_int_distribution,bernoulli_distribution,distributions}.cc`
  -- design tests (compile-time + runtime).
- `tests/0042.modules/random.cc` -- module consumer; verifies every exported
  name instantiates through `import fast_io.random`.

## Build and run

```sh
clang++ --config=$HOME/herbcfgs/x86_64-linux-gnu-libcxx.cfg \
    -std=c++26 -fherbceptions -lherbceptions -O2 -march=native -I include \
    tests/0049.random/uniform_int_distribution.cc -o /tmp/uniform_int_distribution
clang++ --config=$HOME/herbcfgs/x86_64-linux-gnu-libcxx.cfg \
    -std=c++26 -fherbceptions -lherbceptions -O2 -march=native -I include \
    tests/0049.random/bernoulli_distribution.cc -o /tmp/bernoulli_distribution
clang++ --config=$HOME/herbcfgs/x86_64-linux-gnu-libcxx.cfg \
    -std=c++26 -fherbceptions -lherbceptions -O2 -march=native -I include \
    tests/0049.random/distributions.cc -o /tmp/distributions
/tmp/uniform_int_distribution
/tmp/bernoulli_distribution
/tmp/distributions
```

Module build and consumer:

```sh
clang++ --config=$HOME/herbcfgs/x86_64-linux-gnu-libcxx.cfg \
    -std=c++26 -fherbceptions --precompile -x c++-module -I include \
    share/fast_io/fast_io.cppm -o /tmp/fast_io.pcm
clang++ --config=$HOME/herbcfgs/x86_64-linux-gnu-libcxx.cfg \
    -std=c++26 -fherbceptions --precompile -x c++-module -I include \
    -fmodule-file=fast_io=/tmp/fast_io.pcm \
    share/fast_io/fast_io_random.cppm -o /tmp/fast_io.random.pcm
clang++ --config=$HOME/herbcfgs/x86_64-linux-gnu-libcxx.cfg \
    -std=c++26 -fherbceptions -g -I include \
    -fmodule-file=fast_io=/tmp/fast_io.pcm \
    -fmodule-file=fast_io.random=/tmp/fast_io.random.pcm \
    tests/0042.modules/random.cc /tmp/fast_io.random.pcm \
    -lherbceptions -o /tmp/random_module
/tmp/random_module
```


The tests are deterministic: every engine is seeded with a fixed constant, so a
failure is reproducible.

## API: uniform_int_distribution

Same surface as `std::uniform_int_distribution`, minus the stream operators:

```cpp
template<::std::integral IntType = int>
  requires (!same_as<remove_cv_t<IntType>, bool> &&
            sizeof(IntType) <= sizeof(::std::uint_least64_t))
class uniform_int_distribution
{
public:
	using result_type = IntType;
	class param_type;

	explicit constexpr uniform_int_distribution(IntType a = 0, IntType b = max);
	explicit constexpr uniform_int_distribution(param_type const &);

	IntType a() const;
	IntType b() const;
	IntType min() const;
	IntType max() const;
	void reset();
	param_type param() const;
	void param(param_type const &);

	template<uniform_random_bit_generator G>
	IntType operator()(G &g) throws(...);

	template<uniform_random_bit_generator G>
	IntType operator()(G &g, param_type const &p) throws(...);

	friend bool operator==(uniform_int_distribution const &, uniform_int_distribution const &);
};
```

Differences from the standard:

- `IntType` may be any integral type up to 64 bits except `bool`. In
  particular `int_least8_t`/`uint_least8_t` ... `int_least64_t`/`uint_least64_t`
  are supported. The standard only names the `short`/`int`/`long`/`long long`
  spellings.
- No `operator<<` / `operator>>`. They drag locale and stream machinery into
  every translation unit that names a distribution; dropping them is the main
  compile-time win.
- `throws(...)` is present where the standard simply has no exception
  specification. It is derived from the selected engine entry point.
- A `requires` clause replaces the standard's "IntType shall be ..."
  `static_assert`.

### Why the stream operators are omitted

They are normative ([rand.req.dist]/11-20: every distribution has `os << x`
and `is >> d`), but they were never a serialization format:

- The format is unspecified: "a textual representation for the parameters and
  the additional internal data". libc++ and libstdc++ both happened to choose
  `a<space>b` in decimal, but nothing requires that, and no other implementation
  has to match.
- `is >> d` has a precondition that the text was written by an `os` with the
  *same locale* and the same `charT`/`traits` ([rand.req.dist]/16). Write with a
  grouping locale (`1,000 2,000`) and read with `classic` and extraction fails;
  the standard calls that a precondition violation, not bad input.
- The extractor does not validate `a <= b`. `"5 1"` parses successfully, sets
  `param(5, 1)` and does not set `failbit`; the resulting distribution violates
  the constructor precondition, so every later `operator()` is UB. libc++ then
  returns integers outside `[5, 1]`; libstdc++ (GCC 17) recurses without bound
  in its signed upscaling path and dies from stack overflow.
- Only `fmtflags` and `fill` are protected, not `width`: `os << setw(24) << d`
  pads *between* `a` and `b`, because the operator does `os << a` internally.
- Merely using `std::cout << d` instantiates the iostream/num_put/locale stack:
  measured +0.7 s per TU on this machine, which is the reason this prototype
  has none.

`normal_distribution` shows the intent: it streams the cached second sample so
the round-trip reproduces the exact sequence. That is a debug/state dump, not an
interchange format.

The default interval is `[0, numeric_limits<IntType>::max()]`, including for
signed types, as in the standard. `a <= b` is a precondition and is not checked
(the standard leaves violating it undefined).

## Engine contract

`fast_io::random::uniform_random_bit_generator` is a local counterpart of the
standard concept:

- `G::result_type` is unsigned;
- `G::min()` / `G::max()` return exactly `result_type` and `min() < max()`;
- the engine provides `operator()`, `generate()`, or both.

Two deliberate relaxations/extensions over `std::uniform_random_bit_generator`:

1. **`generate()` is preferred over `operator()`.** If `G` has a callable
   `generate()`, it is used even when `operator()` also exists. This is the
   herbception channel: `white_hole_engine::generate()` is `throws` and reports
   the real I/O error, while `operator()` exists for legacy callers and turns a
   `throws` into termination.
2. **A generate-only engine is accepted.** Such an engine satisfies the
   concept even if `operator()` does not exist or is deleted.

## Throw contract

The distribution call is `throws` exactly when the entry point it actually uses
is not `noexcept`:

```cpp
template<typename G>
inline constexpr bool engine_draw_no_throw =
  engine_has_generate<G> ? engine_generate_no_throw<G> : engine_call_no_throw<G>;
```

When `generate()` exists, its specification alone decides. `operator()` is
irrelevant even if it is marked `throws`, and a throwing `operator()` never
leaks into a `generate()`-using call. An engine with only `operator()` falls
back to `operator()`'s specification.

Consequences:

- `white_hole_engine` -> `generate()` is `throws(...)` -> the distribution call
  is `throws`, and a `catch throws(::std::error)` around the call receives the
  original I/O error. This is tested.
- An engine with a `noexcept generate()` -> the distribution call is `noexcept`,
  even if its `operator()` is `throws`. This is tested.
- The distribution never catches or converts anything itself; it forwards.

## Algorithm: uniform_int_distribution

1. `span = unsigned(b) - unsigned(a)`, in the distribution's unsigned type, so
   negative intervals wrap correctly.
2. `uniform_span(g, span)`:
   - `span == 0` returns 0.
   - Draw `bit_width(span)` uniform bits and reject values above `span`
     (rejection probability < 1/2, no modulo bias).
3. `draw_bits(g, k)` assembles `k` independent bits (0 < k <= 64):
   - The engine's `[min, max]` is first reduced to the largest power-of-two
     bucket `2^b <= max - min + 1`; draws above it are rejected. For a full-range
     engine (`max - min + 1 == 2^w`) nothing is rejected, and the rejection
     test is compiled out entirely (`engine_accepts_every_draw`). Without that
     short-circuit `draw_bits` is ~4x slower on `mt19937`, which the first
     benchmark run exposed.
   - A draw supplies `b` bits. Narrow engines are called repeatedly; a
     `mt19937` feeding a 64-bit result costs exactly two draws.
   - Bits are taken from the **top** of the bucket, not the bottom. Both are
     uniform for a good engine, but the high bits of an LCG are far better than
     its low bits. The test with a constant engine pins this down:
     `0xABCD000000000000` gives `0xAB`, `0xABCD`, `0xABCD0000`, and the whole
     word for 8/16/32/64-bit results.
4. `a + span` in the unsigned type, converted back to `IntType`. Conversion of
   negative `a` and back is modulo 2^N and well-defined in C++20.

Cost: one engine draw per result for the common case (64-bit engine, any
target width). A 32-bit engine costs two draws for a 64-bit result. The
rejection loops add at most one extra draw on average.

## API: bernoulli_distribution

Same surface as `std::bernoulli_distribution`, minus the stream operators:

```cpp
class bernoulli_distribution
{
public:
	using result_type = bool;
	class param_type;                  // p(), distribution_type, ==

	bernoulli_distribution() noexcept;                    // p = 0.5
	explicit bernoulli_distribution(double p) noexcept;
	explicit bernoulli_distribution(param_type const &) noexcept;

	double p() const;
	bool min() const;                  // false
	bool max() const;                  // true
	void reset();
	param_type param() const;
	void param(param_type const &);

	template<uniform_random_bit_generator G>
	bool operator()(G &g) throws(...);

	template<uniform_random_bit_generator G>
	bool operator()(G &g, param_type const &p) throws(...);
};
```

`0 <= p <= 1` is the standard's precondition. This implementation defines the
out-of-range cases instead of leaving them UB: `p <= 0` and NaN yield `false`,
`p >= 1` yields `true`. A valid distribution is unaffected, and a bad `p`
never reaches the arithmetic.

Algorithm: a direct port of libc++'s `std::generate_canonical`, which was the
fastest of libc++, libstdc++ and the MSVC STL in the bernoulli benchmark:

```cpp
return generate_canonical<double, 53>(g) < p;
```

`generate_canonical` is reproduced in `generate_canonical.h` (libc++'s
version, not the standard's rejection form):

- `k = ceil(53 / floor(log2(engine_range_size)))`;
- `sum = sum over i<k of (draw_i - min) * range_size^i`;
- return `sum / range_size^k`, a value in `[0, 1)` with 53 bits of precision.

Drawing goes through the engine seam, so `generate()` is preferred and the
throws clause follows `generate()` exactly. `p <= 0` and NaN yield false,
`p >= 1` yields true; those cases follow from the comparison itself, as in
libc++, and never touch the engine differently.

Draw counts: one draw per trial from a 64-bit engine, two from a 32-bit engine
(53 bits need two 32-bit chunks). The port produces the same hit counts as
libc++ for the same engine and seed, i.e. it is the same algorithm, not merely
the same distribution.

Benchmark that chose libc++ (50M trials, p = 0.5, `mt19937_64` / `mt19937`):

| stdlib | `mt19937_64` | `mt19937` |
| --- | --- | --- |
| libc++ | 107 ms | 200 ms |
| MSVC STL (under Wine) | 235 ms | 392 ms |
| libstdc++ | 362 ms | 490 ms |

The Wine numbers are indicative, not a native MSVC measurement, but the gap is
large enough that libc++ is the unambiguous choice.

The port reproduces libc++'s hit counts exactly, and in a plain (non-herbception)
build its timing is at parity: 104 ms vs 100 ms on `mt19937_64`, 196 ms vs
206 ms on `mt19937`. In a `-fherbceptions` build the standard-engine path is
conservatively `throws(...)` (std engines do not mark `operator()` noexcept),
which costs some codegen; the algorithm and draw counts are unchanged.

## Ported distributions

Everything except `uniform_int_distribution` is a line-by-line port of the
corresponding libc++ `__random/*.h` header. The algorithms, rejection loops,
coefficients and parameter caches are unchanged:

- `uniform_real` -- `(b - a) * generate_canonical + a`;
- `normal` -- Marsaglia polar method with the cached second variate;
- `lognormal` -- `exp` of a normal;
- `exponential` -- `-log(1 - canonical) / lambda`;
- `cauchy` -- `a + b * tan(pi * canonical)`;
- `weibull` -- `b * pow(exponential, 1 / a)`;
- `extreme_value` -- `a - b * log(-log(1 - canonical))`;
- `gamma` -- Marsaglia-Tsang for `alpha > 1`, Johnk-style rejection for
  `alpha < 1`, exponential for `alpha == 1`;
- `chi_squared` -- `gamma(n / 2, 2)`;
- `fisher_f` -- ratio of two gamma variates;
- `student_t` -- normal over `sqrt(gamma(n / 2, 2) / n)`, with the normal's
  cache carried in the member distribution;
- `poisson` -- multiplicative method for `mean < 10`, PTRS transformed
  rejection above, `inf` mean replaced by `DBL_MAX` as libc++ does;
- `binomial` -- Kemp (1986) modal method with the `lgamma`-based mode cache;
- `negative_binomial` -- Bernoulli loop for small `k`, otherwise
  `poisson(gamma(k, (1 - p) / p))`;
- `geometric` -- `negative_binomial(1, p)`;
- `discrete` -- normalized partial sums plus `upper_bound`;
- `piecewise_constant` / `piecewise_linear` -- libc++'s normalization and
  inverse-CDF formulas.

Adaptations, all forced by fast_io's freestanding rules:

- `std::vector<double>` becomes `fast_io::containers::vector` with
  `native_global_allocator` in the three table-driven distributions.
- `std::accumulate`, `partial_sum`, `adjacent_difference`, `copy_n`,
  `back_inserter` and `upper_bound` become plain loops / a local binary search.
- `__math::*` becomes `fast_io::random::details::math::*`, a type-dispatching
  wrapper over `__builtin_*` with a `<cmath>` fallback for compilers without
  those builtins.
- `operator!=` is not written (C++20 synthesizes it), and `operator<<` /
  `operator>>` are dropped.
- `operator()` carries
  `requires uniform_random_bit_generator<G>` and
  `throws(!engine_draw_no_throw<G>)`, the same contract as the other two
  distributions.
- The allocator-using constructors/accessors are marked `throws` because
  `fast_io::containers::vector` allocation can report a herbception.

`uniform_int_distribution` is deliberately not replaced: its rejection sampler
was benchmarked faster than libc++'s `__independent_bits_engine` (73 ms vs
150 ms for a full 32-bit range), while matching the standard's semantics. It is
the only distribution here that is not a libc++ copy.

The acceptance criterion for the port is exact: for every distribution the test
drives our version and `std::` with identically seeded standard engines
(`mt19937_64`, plus `mt19937` and `minstd_rand` for representative cases) and
requires bit-for-bit equal results. `uniform_int_distribution` and
`bernoulli_distribution` have the same check in their own test files. All
twenty distributions pass, which pins down the algorithms, draw order,
floating-point operations and parameter caches, not merely the marginal
distributions.

## Observed on this machine

Header cost, `clang++ -fsyntax-only`, empty `main`:

| translation unit | wall |
| --- | --- |
| `#include <random>` | 0.88 s |
| `#include "random/uniform_int_distribution.h"` | 0.19 s |

That is the concrete payoff of dropping the stream operators: nothing pulls in
locale, iostreams, or the rest of `<random>`.

`std::mt19937_64`, 50M draws, `-O2 -march=native`, sum accumulated:

| distribution | fast_io | libc++ |
| --- | --- | --- |
| `uint_least32_t` full range | 73 ms | 150 ms |
| `uint_least8_t` full range | 70 ms | 179 ms |
| `[1, 6]` dice | 230 ms | 369 ms |

The fast_io figures are one engine call per draw; libc++'s adaptor does extra
work in both directions.

## Test coverage

### uniform_int_distribution

Compile-time:

- `result_type` for 8/16/32/64-bit signed and unsigned types;
- default intervals equal `[0, numeric_limits<T>::max()]`;
- `param_type` accessors and equality;
- absence of `operator<<` / `operator>>`;
- `noexcept` follows `generate()` when present (a throwing `operator()` does
  not leak in), otherwise `operator()`.

Runtime (fixed seeds):

- bounds for all eight `int_least*`/`uint_least*` types over their full domains;
- negative-only, straddling, two-value, and degenerate intervals;
- every value of a full 8-bit domain is reached, counts within a wide band;
- dice faces `[1, 6]` are roughly uniform;
- a 32-bit engine fills every bit of a 64-bit result;
- an engine with `[1, 1000]` (non-power-of-two, rejection path) reaches every
  result in `[0, 999]`;
- `generate()` is used, `operator()` is not, when both exist;
- a generate-only engine works;
- a throwing `generate()` propagates to a `catch throws(::std::error)`;
- `fast_io::ibuf_white_hole_engine` integration;
- `std::minstd_rand` (non-power-of-two range, `min() != 0`) coverage;
- draw counts (one draw per 64-bit result, two from a 32-bit engine).

### bernoulli_distribution

Compile-time:

- `result_type` is `bool`; `min() == false`, `max() == true`;
- default `p == 0.5`; `param_type` accessors and equality;
- absence of stream operators;
- `noexcept` follows `generate()` when present, else `operator()`.

Runtime (fixed seeds, 200k trials):

- `p = 0.5`, `0.25`, `0.9`, `0.01`, `1/3` within 5+ sigma;
- `p = 0` always false, `p = 1` always true;
- `p < 0`, `p > 1`, NaN defined as false / true / false;
- 32-bit engine gives the same rates and costs exactly two draws per trial;
- `generate()` preferred, generate-only engine accepted;
- throwing `generate()` propagates; white hole engine integration;
- same seed produces the same sequence;
- under libc++, the sequence is bit-for-bit identical to
  `std::bernoulli_distribution` for the same engine and seed, confirming the
  port is the same algorithm.

Note: UBSan reports a bogus "load of value ... not valid for type bool" when a
`throws` function returning `bool` unwinds. It reproduces with a two-line
`bool f() throws { throw ...; }` program, so it is a toolchain
herbceptions/sanitizer interaction, not distribution logic.

### Other distributions

Compile-time:

- `result_type` for representative real and integer types;
- parameter accessors, `param_type` equality and `param()` round-trip;
- absence of stream insertion/extraction on every distribution;
- `noexcept(distribution(engine))` for a `noexcept generate()` engine across
  real, gamma, Poisson, binomial and geometric distributions.

Runtime:

- default parameters match the standard's defaults for all distributions;
- `discrete` probabilities (`1, 2, 1` -> `0.25, 0.5, 0.25`), bounds;
- `piecewise_constant` / `piecewise_linear` intervals, densities and bounds;
- `normal::reset()` drops the cached second variate;
- under libc++, bit-for-bit equality with `std::` for all twenty
  distributions: `uniform_real` (double and float), `normal`, `lognormal`,
  `exponential`, `cauchy`, `weibull`, `extreme_value`, `gamma` in both the
  `alpha > 1` and `alpha < 1` branches, `chi_squared`, `fisher_f`, `student_t`,
  `poisson` in both the `mean < 10` and `mean >= 10` branches, `binomial`,
  `negative_binomial`, `geometric`, `discrete`, `piecewise_constant`,
  `piecewise_linear`;
- the same equality holds with a 32-bit `mt19937` (forcing the two-draw
  `generate_canonical` path) for `uniform_real`, `normal`, `gamma`, `poisson`,
  `binomial` and `discrete`;
- and with `minstd_rand` (`min() == 1`, non-power-of-two range) for
  `uniform_real` and `poisson`.

## Open questions for review

1. **Resolved: the throw condition is `generate()`'s.** When `generate()`
   exists it is the only spec that matters, so `noexcept(d(g))` is exactly
   `noexcept(g.generate())`. An `operator()`-only engine still follows
   `operator()`'s spec. Consequence to be aware of: `std::mt19937::operator()`
   is not marked `noexcept`, so a std engine makes the distribution call
   `throws` (conservatively; in herbception builds it cannot actually throw
   herbceptions). Since fast_io engines expose `generate()`, this only affects
   mixing with standard engines.
2. **`a <= b` is unchecked.** Follow the standard (precondition), or report a
   violation through fast_io's error channel?
3. **`IntType` domain.** Current constraint is "integral, not bool, <= 64 bits".
   Should `char`/`wchar_t`/`char8_t` be excluded? Should `__int128` be
   supported (needs 128-bit accumulation and a wider `uniform_span`)?
4. **Non-power-of-two engine ranges.** The bucket rejection is unbiased but
   costs up to one extra draw. A division-scaled path (Lemire-style) would use
   the high bits with fewer draws for huge targets; not implemented yet.
5. **Equality only.** `operator==` is provided; `!=` is synthesized. No
   `operator<=>` on purpose (no ordering of distributions).
6. **Stream operators and serialization manipulators are dropped.** After
   reviewing `is >> d` across libc++/libstdc++/MSVC STL, the whole text-form
   idea is out of scope: no `operator<<`/`operator>>`, and no
   `rand_dist_put`/`rand_dist_get` pair. If distribution parameters ever need to
   persist, store the typed fields and validate them when constructing.
7. **Placement.** `fast_io::random` is a placeholder namespace; the final home
   could be `fast_io::random` or `fast_io::containers`-style naming. The
   arithmetic headers include only `<bit>`, `<concepts>`, `<cstdint>`,
   `<cstddef>`, `<initializer_list>`, `<limits>`, `<type_traits>`, the fast_io
   macro header and `fast_io_dsal/vector.h` for the three table-driven
   distributions.
8. **`uniform_int_distribution` is the one non-libc++ algorithm.** It is a
   faster rejection sampler (73 ms vs libc++'s 150 ms for a full 32-bit range)
   with the same standard semantics. Say if it should be swapped for the
   `__independent_bits_engine` copy for strict uniformity with the rest.
