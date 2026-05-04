#include <fast_io.h>
#include <fast_io_dsal/deque.h>

template <typename T>
inline void logging(T const &deq, ::std::source_location src = ::std::source_location::current())
{
	::fast_io::io::print(src,
						 "\n\tdeq.front_capacity()=", deq.front_capacity(),
						 "\n\tdeq.back_capacity()=", deq.back_capacity(),
						 "\n\tdeq.capacity()=", deq.capacity(),
						 "\n\tdeq.size()=", deq.size(), "\n\n");
}

int main()
{
	::fast_io::deque<::std::size_t> deq;
	deq.push_back(30);
	logging(deq);
	deq.resize(265, 30);
	logging(deq);
	deq.resize(6000);
	logging(deq);
	deq.resize(6401);
	logging(deq);
}
