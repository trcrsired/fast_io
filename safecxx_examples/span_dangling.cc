// Safe C++ experiment (fast_io safecxx branch): span.
//
// fast_io_dsal/impl/span.h classifies its API for safe source: iterator and
// unchecked operations are [[unsafe]] (call-restricted), while checked
// indexing and size queries stay callable. This file exercises that split and
// probes the two ways a span can outlive its owner.
//
// Build with the safe compiler:
//   clang++ --config=$HOME/cfgs/x86_64-linux-gnu-libcxx.cfg \
//     -fsafecxx -fherbceptions -lherbceptions -O3 -s -flto=thin \
//     -Iinclude safecxx_examples/span_dangling.cc -o /tmp/span_dangling

[[unsafe]] {
#include <fast_io_dsal/impl/misc/push_macros.h>
#include <fast_io_dsal/impl/misc/push_warnings.h>
#include <fast_io_core.h>
#include <fast_io_dsal/impl/freestanding.h>
#include <fast_io_dsal/impl/common.h>
}

#include <fast_io_dsal/vector.h>
#include <fast_io_dsal/span.h>

int main()
{
	fast_io::vector<int> v;
	v.push_back(1);
	v.push_back(2);
	v.push_back(3);

	// Checked construction and indexing are safe source.
	fast_io::span<int> s(fast_io::freestanding::from_range, v);
	int sum = s[0] + s[1] + s[2];

	// Iterator/pointer/unchecked access needs a fence.
	[[unsafe]] {
		(void)s.data();
		(void)s.begin();
		(void)s.front_unchecked();
	}

	// A span over a temporary owner is rejected by span's own constraint
	// (`!is_rvalue_reference_v<R&&>`), so it cannot be formed in safe source:
	//
	//   fast_io::span<int> bad(fast_io::freestanding::from_range,
	//                          fast_io::vector<int>{1, 2, 3});  // error

	// A span over a *local* owner that dies before the span is used is NOT
	// diagnosed by the compiler today: the span stores an opaque pointer, and
	// no lifetime provenance connects it back to `v`. Fence it explicitly.
	[[unsafe]] {
		auto local = []() noexcept {
			fast_io::vector<int> tmp;
			tmp.push_back(9);
			return fast_io::span<int>(fast_io::freestanding::from_range, tmp);
		};
		(void)local;
	}

	return sum == 6 ? 0 : 1;
}
