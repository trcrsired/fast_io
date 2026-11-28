// An API that takes an iterator pair carries no owner its result can borrow
// from, so it is declared [[unsafe]] and may only be called inside a fence.
// This file must be rejected by the safe compiler.
//
//   clang++ --config=$HOME/cfgs/x86_64-linux-gnu-libcxx.cfg \
//     -fsafecxx -fsyntax-only -I.../fast_io/include \
//     safecxx_examples/include_layers_reject.cc

#include <fast_io_core.h>

int main()
{
	[[unsafe]] {
		int a[]{1, 2, 3};
		// Inside a fence the call is allowed.
		(void)::fast_io::algorithms::count_iters(a, a + 3, 2);
	}

	int b[]{1, 2, 3};
	// error: count_iters takes iterators, so the call is call-restricted.
	return static_cast<int>(::fast_io::algorithms::count_iters(b, b + 3, 2));
}
