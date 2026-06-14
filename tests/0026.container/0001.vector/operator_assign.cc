#include <cassert>
#include <initializer_list>
#include <iterator>
#include <fast_io.h>
#include <fast_io_dsal/vector.h>
using namespace fast_io::io;
using namespace fast_io::mnp;

int main()
{
	fast_io::vector<int> x{1, 2, 3}, y, z;

	print("Initially:\n");
	print("x = {", rgvw(x, ", "), "}\ny = {", rgvw(y, ", "), "}\nz = {", rgvw(z, ", "), "}\n");
	assert(x.size() == 3);
	assert(y.empty());
	assert(z.empty());

	print("Copy assignment copies data from x to y:\n");
	y = x;
	print("x = {", rgvw(x, ", "), "}\ny = {", rgvw(y, ", "), "}\n");
	assert(y.size() == 3);
	assert(y[0] == 1 && y[1] == 2 && y[2] == 3);

	print("Move assignment moves data from x to z, modifying both x and z:\n");
	z = std::move(x);
	print("x = {", rgvw(x, ", "), "}\nz = {", rgvw(z, ", "), "}\n");
	assert(z.size() == 3);
	assert(z[0] == 1 && z[1] == 2 && z[2] == 3);
	// After move, source vector should be empty
	assert(x.empty());

#if 0
	auto const w = {4, 5, 6, 7};

	print("Assignment of initializer_list w to z:\n");
	z = w;
	print("w = {", rgvw(w, ", "), "}\nz = {", rgvw(z, ", "), "}\n");
#endif
}
