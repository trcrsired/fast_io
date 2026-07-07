#include <algorithm>
#include <list>
#include <fast_io.h>
#include <fast_io_dsal/vector.h>

int main()
{
#if 0
	auto const source = std::list{2, 7, 1};
	auto destination = fast_io::vector{3, 1, 4};
	destination.assign_range(source);
	if (!(std::ranges::equal(source, destination))) ::fast_io::fast_terminate();
#endif
}