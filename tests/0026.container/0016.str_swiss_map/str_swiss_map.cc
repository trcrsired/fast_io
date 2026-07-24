#include <fast_io.h>
#include <fast_io_dsal/str_swiss_map.h>

int main()
{
	::fast_io::str_swiss_map<::std::size_t> mp;
	mp.insert_key("hello", 40);
	mp.insert_key("hello", 60);
	mp.insert_key("helloworld", 60);
	::fast_io::io::print("::fast_io::str_swiss_map<::std::size_t> insert_key size=", mp.size(), ":\n");
	for (auto &e : mp)
	{
		::fast_io::io::println(e.key(), " ", e.mapped());
	}
	mp.clear_destroy();
	::fast_io::io::print("::fast_io::str_swiss_map<::std::size_t> clear_destroy size=", mp.size(), ":\n");
	for (auto &e : mp)
	{
		::fast_io::io::println(e.key(), " ", e.mapped());
	}
}