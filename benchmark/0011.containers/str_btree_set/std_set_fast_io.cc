#include <set>
#include <string>
#include <fast_io.h>
#include <fast_io_driver/timer.h>
#include "gentest.h"

int main()
{
	auto vec{::gentest()};
	::fast_io::timer t(u8"str_btree_set");
	::std::set<::fast_io::string> bset;
	{
		::fast_io::timer t(u8"insert");
		for (auto const &e : vec)
		{
			bset.insert(e);
		}
	}
	{
		::std::size_t count{};
		{
			::fast_io::timer t(u8"contains");
			for (auto const &e : vec)
			{
				count += static_cast<::std::size_t>(bset.contains(e));
			}
		}
		::fast_io::io::perrln("count=", count);
	}
}