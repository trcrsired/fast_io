#include <cassert>
#include <fast_io.h>
#include <fast_io_dsal/vector.h>
using namespace fast_io::io;
using namespace fast_io::mnp;

int main()
{
	::fast_io::vector<::std::size_t> vec;
	vec.push_back(4);
	vec.emplace_index(0,6);
	vec.emplace_index(0,8);
	vec.erase_index(0);
	// After push_back(4): [4]
	// After emplace_index(0,6): [6,4]
	// After emplace_index(0,8): [8,6,4]
	// After erase_index(0): [6,4]
	assert(vec.size() == 2);
	assert(vec[0] == 6);
	assert(vec[1] == 4);
	for(auto const & e : vec)
	{
		::fast_io::io::println(e);
	}
}
