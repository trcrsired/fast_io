#include <fast_io.h>
#include <fast_io_driver/timer.h>
#include <fast_io_dsal/deque.h>

int main()
{
	fast_io::timer tm(u8"fast_io::deque resize_for_overwrite");
	fast_io::deque<std::size_t> deq;
	constexpr std::size_t n{100000000};
	{
		fast_io::timer tm1(u8"push_back");
		deq.resize(n, ::fast_io::for_overwrite);
		::std::size_t idx{};
		for (::std::size_t i{}, segs{deq.segments_count()}; i != segs; ++i)
		{
			for (auto &e : deq.nth_segment(i))
			{
				e = idx;
				++idx;
			}
		}
	}
	::std::size_t sum{};
	{
		fast_io::timer tm1(u8"loop");
		for (::std::size_t i{}, segs{deq.segments_count()}; i != segs; ++i)
		{
			for (auto const e : deq.const_nth_segment(i))
			{
				sum += e;
			}
		}
	}
	::fast_io::io::perrln("sum=", sum);
}
