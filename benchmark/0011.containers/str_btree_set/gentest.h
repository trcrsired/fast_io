#pragma once

#include <fast_io_dsal/vector.h>
#include <fast_io_dsal/string.h>
#include <random>

inline ::fast_io::vector<::fast_io::string> gentest()
{
	::fast_io::vector<::fast_io::string> vec;
	::fast_io::mt19937_64 eng;
	::std::uniform_int_distribution<std::size_t> ud(0, 61);
	::std::uniform_int_distribution<std::size_t> rlen(8, 20);
	for (::std::size_t i(0); i != 1000000; ++i)
	{
		::fast_io::string tempstr;
		::std::size_t n{rlen(eng)};
		temp.reserve(n);
		for (std::size_t j(0); j != s; ++j)
		{
			char8_t ch(static_cast<char8_t>(ud(eng)));
			if (ch < 10u)
			{
				ch += u8'0';
			}
			else if (ch < 36u)
			{
				ch = ch - 10u + u8'a';
			}
			else
			{
				ch = ch - 36u + u8'A';
			}
			tempstr.push_back_unchecked(ch);
		}
		vec.emplace_back(::std::move(tempstr));
	}
	return vec;
}
