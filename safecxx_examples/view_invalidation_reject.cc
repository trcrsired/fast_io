// Safe C++ experiment (fast_io safecxx branch): owner freeze — REJECT cases.
//
// While a live view borrows from an owner, a non-const member call on that
// owner is rejected in safe source. No per-container invalidation list is
// needed: the rule follows provenance and liveness. An [[unsafe]] fence is an
// explicit escape hatch (this is a call restriction, not a lifetime error).
//
//   clang++ --config=$HOME/cfgs/x86_64-linux-gnu-libcxx.cfg \
//     -std=c++26 -fsafecxx -fherbceptions -fsyntax-only -Iinclude \
//     safecxx_examples/view_invalidation_reject.cc
//
// Expected diagnostics:
//   -Werror,-Wlifetime-safety-frozen-owner on each push_back below.

[[unsafe]] {
#include <fast_io_dsal/impl/misc/push_macros.h>
#include <fast_io_dsal/impl/misc/push_warnings.h>
#include <fast_io_core.h>
#include <fast_io_dsal/impl/freestanding.h>
#include <fast_io_dsal/impl/common.h>
}
#include <fast_io_dsal/vector.h>
#include <fast_io_dsal/span.h>

static int bad() noexcept
{
	fast_io::vector<int> v;
	v.push_back(1);
	fast_io::span<int> s(fast_io::freestanding::from_range, v);
	v.push_back(2); // error: v is frozen by s
	return s[0];
}

static int after_last_use() noexcept
{
	fast_io::vector<int> v;
	v.push_back(1);
	{
		fast_io::span<int> s(fast_io::freestanding::from_range, v);
		if (s[0] != 1) return 1;
	}
	v.push_back(2); // OK: the view's last use has passed
	return v[1];
}

static int in_fence() noexcept
{
	fast_io::vector<int> v;
	v.push_back(1);
	fast_io::span<int> s(fast_io::freestanding::from_range, v);
	[[unsafe]] { v.push_back(2); } // OK: explicit fence
	return s[0];
}

int main() { return 0; }
