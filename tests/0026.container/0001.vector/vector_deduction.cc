#include <cassert>
#include <fast_io.h>
#include <fast_io_dsal/vector.h>
using namespace fast_io::io;
using namespace fast_io::mnp;

int main()
{
	fast_io::vector vec{static_cast<::std::uint_least32_t>(1), 6, 10, 20};
	println("Before vec.push_back(50): vec.size()=", vec.size(), " vec.capacity()=", vec.capacity());
	assert(vec.size() == 4);
	assert(vec[0] == 1);
	assert(vec[1] == 6);
	assert(vec[2] == 10);
	assert(vec[3] == 20);
	vec.push_back(50);
	vec.push_back(50);
	println("After vec.push_back(50): vec.size()=", vec.size(), " vec.capacity()=", vec.capacity());
	assert(vec.size() == 6);
	assert(vec[4] == 50);
	assert(vec[5] == 50);
}
