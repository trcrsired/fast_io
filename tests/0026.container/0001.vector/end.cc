#include <cassert>
#include <algorithm>
#include <numeric>
#include <string>
#include <fast_io.h>
#include <fast_io_dsal/vector.h>
using namespace fast_io::io;
using namespace fast_io::mnp;

int main()
{
	fast_io::vector<int> nums{1, 2, 4, 8, 16};
	fast_io::vector<std::string> fruits{"orange", "apple", "raspberry"};
	fast_io::vector<char> empty;

	// Print vector.
	std::for_each(nums.begin(), nums.end(), [](int const n) { print(n, " "); });
	print("\n");

	// Sums all integers in the vector nums (if any), printing only the result.
	auto const sum = std::accumulate(nums.begin(), nums.end(), 0);
	println("Sum of nums: ", sum);
	assert(sum == 31);

	// Prints the first fruit in the vector fruits, checking if there is any.
	if (!fruits.empty())
	{
		println("First fruit: ", *fruits.begin());
		assert(*fruits.begin() == "orange");
	}

	if (empty.begin() == empty.end())
	{
		print("vector 'empty' is indeed empty.\n");
	}
	assert(empty.begin() == empty.end());
}
