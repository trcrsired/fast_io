#include <fast_io.h>
#include <fast_io_dsal/deque.h>
#include <fast_io_dsal/impl/debug/deque.h>

template <typename T>
inline void logging(T const &deq, ::std::source_location src = ::std::source_location::current())
{
	::fast_io::io::perr(src,
						"\n\tdeq.front_capacity()=", deq.front_capacity(),
						"\n\tdeq.back_capacity()=", deq.back_capacity(),
						"\n\tdeq.capacity()=", deq.capacity(),
						"\n\tdeq.size()=", deq.size(), "\n\n");
	::fast_io::io::debug_perr(src,
							  "\n", ::fast_io::manipulators::debug_view(deq), "\n\n");
}

int main()
{
	::fast_io::deque<::std::size_t> deq;
	deq.reserve_back(10000000);
	logging(deq);
	deq.clear_destroy();
	deq.push_back(30);
	logging(deq);
	deq.reserve_back(265);
	logging(deq);
	deq.reserve_back(6000);
	logging(deq);
	deq.reserve_back(6401);
	logging(deq);
	deq.clear_destroy();
	logging(deq);
	deq.push_front(30);
	logging(deq);
	deq.reserve_front(265);
	logging(deq);
	deq.reserve_front(6000);
	logging(deq);
	deq.reserve_front(6401);
	logging(deq);
	deq.clear_destroy();
	logging(deq);
	deq.reserve_front(512);
	logging(deq);
	deq.push_front(30);
	deq.pop_back();
	logging(deq);
	for (auto e : deq)
	{
		::fast_io::io::println(e);
	}
}
