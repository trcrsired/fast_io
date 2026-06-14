#include <string>
#include <fast_io.h>
#include <fast_io_dsal/vector.h>
using namespace fast_io::io;
using namespace fast_io::mnp;

int main()
{
	// C++11 initializer list syntax:
	fast_io::vector<std::string> words1{"the", "frogurt", "is", "also", "cursed"};
	print("1: {", rgvw(words1, ", "), "}\n");
	if (!(words1.size() == 5)) ::fast_io::fast_terminate();
	if (!(words1[0] == "the")) ::fast_io::fast_terminate();
	if (!(words1[1] == "frogurt")) ::fast_io::fast_terminate();
	if (!(words1[2] == "is")) ::fast_io::fast_terminate();
	if (!(words1[3] == "also")) ::fast_io::fast_terminate();
	if (!(words1[4] == "cursed")) ::fast_io::fast_terminate();
#if 0
	// words2 == words1
	fast_io::vector<std::string> words2(::std::from_range, words1);
	print("2: {", rgvw(words2, ", "), "}\n");
#endif
	// words3 == words1
	fast_io::vector<std::string> words3(words1);
	print("3: {", rgvw(words3, ", "), "}\n");
	if (!(words3 == words1)) ::fast_io::fast_terminate();

	// words4 is {"Mo", "Mo", "Mo", "Mo", "Mo"}
	fast_io::vector<std::string> words4(5, "Mo");
	print("4: {", rgvw(words4, ", "), "}\n");
	if (!(words4.size() == 5)) ::fast_io::fast_terminate();
	for (auto const &w : words4) if (!(w == "Mo")) ::fast_io::fast_terminate();
#if 0
	auto const rg = {"cat", "cow", "crow"};
	fast_io::vector<std::string> words5(::std::from_range, rg); // overload (11)
	print("5: {", rgvw(words5, ", "), "}\n");
#endif
}
