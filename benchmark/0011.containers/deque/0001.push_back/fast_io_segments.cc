#include <fast_io.h>
#include <fast_io_driver/timer.h>
#include <fast_io_dsal/deque.h>

int main()
{
	fast_io::timer tm(u8"fast_io::deque segments");
	fast_io::deque<std::size_t> deq;
	constexpr std::size_t n{100000000};
	{
		fast_io::timer tm1(u8"push_back");
		for (std::size_t i{}; i != n; ++i)
		{
			deq.push_back(i);
		}
	}
	::std::size_t sum{};
	{
		fast_io::timer tm1(u8"loop");
		for (::std::size_t i{}, n{deq.segments_count()}; i != n; ++i)
		{
			for (auto const e : deq.const_nth_segment(i))
			{
				sum += e;
			}
		}
	}
	::fast_io::io::perrln("sum=", sum);
}
