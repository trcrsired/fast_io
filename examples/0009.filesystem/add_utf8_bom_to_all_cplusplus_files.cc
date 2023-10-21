#include<fast_io.h>
#include<fast_io_device.h>
#include<string_view>
#include<algorithm>
#include<cstring>

using namespace ::fast_io::io;

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		if (argc == 0)
		{
			return 1;
		}
		::fast_io::io::perr("Usage: ", ::fast_io::mnp::os_c_str(*argv), " <source directory>\n");
		return 1;
	}
	using namespace ::std::string_view_literals;
	constexpr ::fast_io::freestanding::array<::std::byte, 3> utf8bom{ ::std::byte{0xEF},::std::byte{0xBB},::std::byte{0xBF} };
	::fast_io::dir_file df(::fast_io::mnp::os_c_str(argv[1]));
	for (auto const& ent : recursive(at(df)))
	{
		::std::u8string_view view(u8extension(ent));
		if (view != u8".cc"sv && view != u8".h"sv && view != u8".cpp"sv && view != u8".cxx"sv && view != u8".hpp"sv)
			continue;
		::fast_io::freestanding::array<::std::byte, 3> temp{};
		::fast_io::iobuf_file obf(drt(ent), fast_io::open_mode::in | fast_io::open_mode::out);
		auto ret = ::fast_io::operations::pread_some_bytes(obf, temp.begin(), temp.end(), 0);
		bool should_write{};
		if (ret != temp.end())
		{
			should_write = true;
		}
		else
		{
			if (::fast_io::freestanding::my_memcmp(temp.cbegin(), utf8bom.cbegin(), 3) != 0)
			{
				should_write = true;
			}
		}
		if (should_write)
			::fast_io::operations::pwrite_all_bytes(obf, utf8bom.cbegin(), utf8bom.cend(), 0);
	}
}
