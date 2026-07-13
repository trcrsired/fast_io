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
	if (!(numbers.size() == 3)) ::fast_io::fast_terminate();
	if (!(numbers[0] == 5)) ::fast_io::fast_terminate();
	if (!(numbers[1] == 3)) ::fast_io::fast_terminate();
	if (!(numbers[2] == 4)) ::fast_io::fast_terminate();

	print_info(numbers);

	numbers.pop_back();
	if (!(numbers.size() == 2)) ::fast_io::fast_terminate();
	if (!(numbers.back() == 3)) ::fast_io::fast_terminate();

	print_info(numbers);
}
