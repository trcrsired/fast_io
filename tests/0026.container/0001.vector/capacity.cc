#include <cassert>
#include <fast_io.h>
#include <fast_io_dsal/vector.h>
using namespace fast_io::io;
using namespace fast_io::mnp;

int main()
{
	int sz = 100;
	fast_io::vector<int> v;

	auto cap = v.capacity();
	println("Initial size: ", v.size(), ", capacity: ", cap);
	assert(v.size() == 0);
	assert(v.capacity() == 0);

	print("\nDemonstrate the capacity'\''s growth policy."
		  "\nSize:  Capacity:  Ratio:\n");
	while (sz-- > 0)
	{
		v.push_back(sz);
		if (cap != v.capacity())
		{
			println(left(v.size(), 7), left(v.capacity(), 11), left(float(v.capacity()) / static_cast<float>(cap), 10));
			assert(v.capacity() > cap);
			cap = v.capacity();
		}
	}

	println("\nFinal size: ", v.size(), ", capacity: ", v.capacity());
	assert(v.size() == 100);
}
