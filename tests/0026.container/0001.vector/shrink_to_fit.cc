#include <fast_io.h>
#include <fast_io_dsal/vector.h>
using namespace fast_io::io;
using namespace fast_io::mnp;

int main()
{
	fast_io::vector<int> v;
	println("Default-constructed capacity is ", v.capacity());
	if (!(v.capacity() == 0)) ::fast_io::fast_terminate();
	v.resize(100);
	println("Capacity of a 100-element vector is ", v.capacity());
	if (!(v.capacity() >= 100)) ::fast_io::fast_terminate();
	auto old_cap = v.capacity();
	v.resize(50);
	println("Capacity after resize(50) is ", v.capacity());
	if (!(v.capacity() == old_cap)) ::fast_io::fast_terminate();
	v.shrink_to_fit();
	println("Capacity after shrink_to_fit() is ", v.capacity());
	if (!(v.size() == 50)) ::fast_io::fast_terminate();
	if (!(v.capacity() >= 50)) ::fast_io::fast_terminate();
	v.clear();
	println("Capacity after clear() is ", v.capacity());
	v.shrink_to_fit();
	println("Capacity after shrink_to_fit() is ", v.capacity());
	for (int i = 1000; i < 1300; ++i)
	{
		v.push_back(i);
	}
	println("Capacity after adding 300 elements is ", v.capacity());
	if (!(v.size() == 300)) ::fast_io::fast_terminate();
	v.shrink_to_fit();
	println("Capacity after shrink_to_fit() is ", v.capacity());
	if (!(v.capacity() >= 300)) ::fast_io::fast_terminate();
}
