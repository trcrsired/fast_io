#include <fast_io.h>
#include <fast_io_driver/timer.h>
#include <fast_io_dsal/deque.h>
#include <cstddef>

int main()
{
	fast_io::timer tm(u8"fast_io::deque insert iterator");
	fast_io::deque<std::size_t> dq;
	constexpr std::size_t n{50000};

	{
		fast_io::timer t(u8"insert(iterator)");
		for (std::size_t i{}; i != n; ++i)
		{
			::std::size_t dqsz{dq.size()};
			std::size_t pos = dqsz ? (i % dqsz) : 0;
			dq.insert(dq.begin() + pos, i);
		}
	}

	std::size_t sum{};
	{
		fast_io::timer t(u8"loop");
		for (auto const e : dq)
		{
			sum += e;
		}
	}

	fast_io::io::perrln("sum=", sum);
}