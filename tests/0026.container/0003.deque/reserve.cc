#include <fast_io.h>
#include <fast_io_dsal/deque.h>

int main()
{
	::fast_io::deque<::std::size_t> deq;
	::fast_io::io::println(::std::source_location::current(), " ", deq.back_capacity(), " ", deq.size());
	deq.push_back(30);
	::fast_io::io::println(::std::source_location::current(), " ", deq.back_capacity(), " ", deq.size());
	deq.reserve_back(6000);
	::fast_io::io::println(::std::source_location::current(), " ", deq.back_capacity(), " ", deq.size());
}
