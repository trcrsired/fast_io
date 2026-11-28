// Every fast_io public header is self-fenced under the safe dialect, so it
// can be included directly from safe source with no user-written [[unsafe]].
// The implementation (including all C++ standard library usage) is trusted;
// only APIs that take iterators or pointer+length pairs are call-restricted.
//
// Build with the safe compiler:
//   clang++ --config=$HOME/cfgs/x86_64-linux-gnu-libcxx.cfg \
//     -fsafecxx -fherbceptions -lherbceptions -O2 \
//     -I.../fast_io/include safecxx_examples/include_layers.cc

#include <fast_io_core.h>
#include <fast_io_freestanding.h>
#include <fast_io_hosted.h>
#include <fast_io_crypto.h>
#include <fast_io_i18n.h>
#include <fast_io_legacy.h>
#include <fast_io_dsal/vector.h>

int main()
{
	::fast_io::vector<int> v;
	v.push_back(1);
	v.push_back(2);

	// Range-based algorithms take a range object, not iterators or a
	// pointer+length pair: they stay callable from safe source.
	auto n = ::fast_io::algorithms::count(v, 2);

	return n == 1 ? 0 : 1;
}
