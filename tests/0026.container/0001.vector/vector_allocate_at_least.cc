#include <fast_io.h>
#include <fast_io_dsal/vector.h>
using namespace fast_io::io;
using namespace fast_io::mnp;

int main()
{
	fast_io::vector<::std::size_t> vec;
	println("Before vec.push_back(50): vec.size()=",vec.size()," vec.capacity()=",vec.capacity());
	if (!(vec.size() == 0)) ::fast_io::fast_terminate();
	if (!(vec.capacity() == 0)) ::fast_io::fast_terminate();
	vec.push_back(50);
	vec.push_back(50);
	println("After vec.push_back(50): vec.size()=",vec.size()," vec.capacity()=",vec.capacity());
	if (!(vec.size() == 2)) ::fast_io::fast_terminate();
	if (!(vec[0] == 50)) ::fast_io::fast_terminate();
	if (!(vec[1] == 50)) ::fast_io::fast_terminate();
}
