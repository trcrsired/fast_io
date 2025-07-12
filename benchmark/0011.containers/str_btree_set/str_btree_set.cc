#include <fast_io_dsal/str_btree_set.h>
#include <fast_io.h>
#include <fast_io_driver/timer.h>
#include "gentest.h"

int main()
{
	auto vec{::gentest()};
	::fast_io::timer t(u8"str_btree_set");
	::fast_io::str_btree_set bset;
	{
		::fast_io::timer t(u8"insert_key");
		for (auto const &e : vec)
		{
			bset.insert_key(::fast_io::mnp::os_c_str(e));
		}
	}
}