// Safe C++ experiment (fast_io safecxx branch): span owner lifetime — REJECT cases.
//
// This file is expected to be rejected by the safe compiler. It is not built
// as part of the smoke tests; it documents what from_range +
// [[clang::lifetimebound]] now diagnoses:
//
//   clang++ --config=$HOME/cfgs/x86_64-linux-gnu-libcxx.cfg \
//     -std=c++26 -fsafecxx -fherbceptions -fsyntax-only -Iinclude \
//     safecxx_examples/span_dangling_reject.cc
//
// Expected diagnostics:
//   make_dangling: -Werror,-Wlifetime-safety-return-stack-addr
//   make_temporary: no matching constructor (forward R is R&, rvalues do not bind)
//   iter_ctor: call to unsafe 'begin' / unsafe iterator constructor

[[unsafe]] {
#include <fast_io_dsal/impl/misc/push_macros.h>
#include <fast_io_dsal/impl/misc/push_warnings.h>
#include <fast_io_core.h>
#include <fast_io_dsal/impl/freestanding.h>
#include <fast_io_dsal/impl/common.h>
}
#include <fast_io_dsal/vector.h>
#include <fast_io_dsal/span.h>

// Local owner dies at return: the span dangles.
static fast_io::span<int> make_dangling() noexcept
{
	fast_io::vector<int> v;
	v.push_back(1);
	return fast_io::span<int>(fast_io::freestanding::from_range, v);
}

// A temporary owner dies at the end of the full-expression.
static fast_io::span<int> make_temporary() noexcept
{
	return fast_io::span<int>(fast_io::freestanding::from_range,
							 fast_io::vector<int>{1, 2, 3});
}

// Iterator construction is unsafe: an iterator has no owner to borrow from.
static void iter_ctor() noexcept
{
	fast_io::vector<int> v;
	v.push_back(1);
	fast_io::span<int> s(v.begin(), v.size());
	(void)s;
}

int main() { return 0; }
