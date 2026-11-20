// Safe C++ experiment (fast_io safecxx branch).
//
// fast_io's support headers are imported as legacy through a main-file
// [[unsafe]] fence. `vector.h` itself is then compiled under safe-source
// rules: its kernel lives in a header-local fence, and its API is classified:
//   - index / checked operations are safe;
//   - iterator / unchecked operations are [[unsafe]] (call-restricted).
//
// Build with the safe compiler:
//   clang++ --config=$HOME/cfgs/x86_64-linux-gnu-libcxx.cfg \
//     -fsafecxx -fherbceptions -lherbceptions -O3 -s -flto=thin \
//     safecxx_examples/vector_index.cc -o /tmp/vector_index

[[unsafe]] {
#include <fast_io_dsal/impl/misc/push_macros.h>
#include <fast_io_dsal/impl/misc/push_warnings.h>
#include <fast_io_core.h>
#include <fast_io_dsal/impl/freestanding.h>
#include <fast_io_dsal/impl/common.h>
}

#include <fast_io_dsal/vector.h>

int main()
{
	::fast_io::vector<int> v;
	v.push_back(1);
	v.emplace_back(2);
	v.push_back(3);
	v.insert_index(1, 42);
	v.resize(5, 9);

	// Bounds-checked index API: callable from safe code.
	int sum = 0;
	for (::std::size_t i{}; i != v.size(); ++i)
		sum += v[i];
	in auto f = v.front();
	in auto b = v.back();
	(void)f;
	(void)b;
	(void)v.capacity();
	(void)v.max_size();
	(void)v.is_empty();

	// Iterator API: begin()/end() are [[unsafe]], but a range-for's
	// compiler-generated calls are exempt and the loop variable is a checked
	// `in` borrow, so no fence is needed for the loop itself. The range object
	// v is frozen while the loop runs: touching v inside the body would
	// require an [[unsafe]] block.
	for (in auto x : v)
		sum += x;

	// Expected: 1+42+2+3+9 = 57, plus the range-for pass over the same 5
	// elements = 114.
	return sum == 114 ? 0 : 1;
}
