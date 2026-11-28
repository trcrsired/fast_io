// Range-based and index-based algorithms stay callable from safe source when
// they take a range (or a range + indices) and return a value. Iterator-taking
// and iterator-returning overloads are [[unsafe]] (see algorithms_range_reject).
#include <fast_io_core.h>
#include <fast_io_hosted.h>
#include <fast_io_dsal/vector.h>

int main()
{
	::fast_io::vector<int> v;
	v.push_back(1);
	v.push_back(2);

	auto n = ::fast_io::algorithms::count(v, 2);              // pure range
	auto c = ::fast_io::algorithms::count_index_range(v, 0, 2, 2); // index-based
	return n == 1 && c == 1 ? 0 : 1;
}
