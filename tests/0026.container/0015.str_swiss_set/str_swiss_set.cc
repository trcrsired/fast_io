#include <fast_io.h>
#include <fast_io_dsal/str_swiss_set.h>

int main()
{
	::fast_io::str_swiss_set sset;
	sset.insert_key("abcd");
	::fast_io::io::println(sset.contains("abcd"));
}
