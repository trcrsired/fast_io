// Safe C++ experiment (fast_io safecxx branch): span.
//
// fast_io_dsal/impl/span.h classifies its API for safe source: iterator and
// unchecked operations are [[unsafe]] (call-restricted), while checked
// indexing and size queries stay callable. This file exercises that split;
// the owner-lifetime rejection cases live in span_dangling_reject.cc.
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

	// The from_range constructor is declared
	//   forward R range [[clang::lifetimebound]]
	// so the span borrows the range. The lifetime-safety analysis is an error
	// under -fsafecxx, even inside an [[unsafe]] fence — see the rejection
	// cases in span_dangling_reject.cc. Here v outlives s, so this is fine.

	return sum == 6 ? 0 : 1;
}
