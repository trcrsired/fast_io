#include <fast_io.h>
#include <fast_io_dsal/vector.h>
using namespace fast_io::io;
using namespace fast_io::mnp;

int main()
{
	fast_io::vector vec{static_cast<::std::uint_least32_t>(1), 6, 10, 20};
	println("Before vec.push_back(50): vec.size()=", vec.size(), " vec.capacity()=", vec.capacity());
	if (!(vec.size() == 4)) ::fast_io::fast_terminate();
	if (!(vec[0] == 1)) ::fast_io::fast_terminate();
	if (!(vec[1] == 6)) ::fast_io::fast_terminate();
	if (!(vec[2] == 10)) ::fast_io::fast_terminate();
	if (!(vec[3] == 20)) ::fast_io::fast_terminate();
	vec.push_back(50);
	vec.push_back(50);
	println("After vec.push_back(50): vec.size()=", vec.size(), " vec.capacity()=", vec.capacity());
	if (!(vec.size() == 6)) ::fast_io::fast_terminate();
	if (!(vec[4] == 50)) ::fast_io::fast_terminate();
	if (!(vec[5] == 50)) ::fast_io::fast_terminate();
}
