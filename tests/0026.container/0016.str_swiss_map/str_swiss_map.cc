#include <fast_io.h>
#include <fast_io_dsal/str_swiss_map.h>

int main()
{
	::fast_io::str_swiss_map<::std::size_t> mp;
	mp.insert_key("hello", 40);
	//	::fast_io::io::print("::fast_io::str_swiss_map<::std::size_t> size=", mp.size(), ":\n");
#if 1
	for (auto &e : mp)
	{
		::fast_io::io::println(e.key(), " ", e.mapped());
	}
#endif
}