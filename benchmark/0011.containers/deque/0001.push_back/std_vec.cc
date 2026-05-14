#include <fast_io.h>
#include <fast_io_driver/timer.h>
#include <vector>

int main()
{
	fast_io::timer tm(u8"std::vector");
	std::vector<std::size_t> vec;
	constexpr std::size_t n{100000000};
	{
		fast_io::timer tm1(u8"push_back");
		for (std::size_t i{}; i != n; ++i)
		{
			vec.push_back(i);
		}
	}
	::std::size_t sum{};
	{
		fast_io::timer tm1(u8"loop");
		for (auto const e : vec)
		{
			sum += e;
		}
	}
	::fast_io::io::perrln("sum=", sum);
}
