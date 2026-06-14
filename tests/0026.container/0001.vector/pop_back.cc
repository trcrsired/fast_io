#include <cassert>
#include <fast_io.h>
#include <fast_io_dsal/vector.h>
using namespace fast_io::io;
using namespace fast_io::mnp;

template <typename T>
void print_info(T const &xs)
{
	print("[ ", rgvw(xs, " "), " ]\n");
}

int main()
{
	fast_io::vector<int> numbers;

	print_info(numbers);

	numbers.push_back(5);
	numbers.push_back(3);
	numbers.push_back(4);
	assert(numbers.size() == 3);
	assert(numbers[0] == 5);
	assert(numbers[1] == 3);
	assert(numbers[2] == 4);

	print_info(numbers);

	numbers.pop_back();
	assert(numbers.size() == 2);
	assert(numbers.back() == 3);

	print_info(numbers);
}
