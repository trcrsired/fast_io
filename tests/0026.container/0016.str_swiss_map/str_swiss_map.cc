#include <fast_io.h>
#include <fast_io_dsal/str_swiss_map.h>

int main()
{
	::fast_io::str_swiss_map<::std::size_t> mp;
	//	::fast_io::io::print("::fast_io::str_swiss_map<::std::size_t> size=", mp.size(), ":\n");
	for (auto &e : mp)
	{
		::fast_io::io::println(e);
	}
}