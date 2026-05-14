#include <fast_io.h>
#include <fast_io_dsal/deque.h>

int main()
{
	::fast_io::deque<::std::size_t> b(1000);
	::fast_io::io::println("before assign size()=", b.size());
	b.assign(100000, 10);
	::fast_io::io::println("after assign size()", b.size());
}
