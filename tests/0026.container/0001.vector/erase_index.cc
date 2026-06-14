#include <fast_io.h>
#include <fast_io_dsal/vector.h>
using namespace fast_io::io;
using namespace fast_io::mnp;

inline void print_container(fast_io::vector<int> const &c)
{
	println(rgvw(c, " "));
}

int main()
{
	fast_io::vector<int> c{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	if (!(c.size() == 10)) ::fast_io::fast_terminate();
	print_container(c);

	c.erase_index(0);
	if (!(c.size() == 9)) ::fast_io::fast_terminate();
	if (!(c[0] == 1)) ::fast_io::fast_terminate();
	print_container(c);

	c.erase_index(2, 5);
	// After erasing index 0: [1,2,3,4,5,6,7,8,9]
	// erase_index(2,5): erase indices 2..4 (values 3,4,5) => [1,2,6,7,8,9]
	if (!(c.size() == 6)) ::fast_io::fast_terminate();
	if (!(c[0] == 1 && c[1] == 2 && c[2] == 6 && c[3] == 7 && c[4] == 8 && c[5] == 9)) ::fast_io::fast_terminate();
	print_container(c);

	// Erase all even numbers
	for (::std::size_t i{}; i!=c.size();)
	{
		if (c[i] % 2 == 0)
		{
			c.erase_index(i);
		}
		else
		{
			++i;
		}
	}
	// After erasing evens from [1,2,6,7,8,9]: [1,7,9]
	if (!(c.size() == 3)) ::fast_io::fast_terminate();
	if (!(c[0] == 1 && c[1] == 7 && c[2] == 9)) ::fast_io::fast_terminate();
	print_container(c);
}
