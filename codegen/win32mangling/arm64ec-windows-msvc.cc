#include "common.h"

inline auto gen_pragmas(char8_t const *asmpath, ::fast_io::vector<::fast_io::u8string> const &win32_symbols, ::fast_io::vector<::fast_io::u8string> const &nt_symbols) noexcept
{
	::fast_io::u8string result{};
	bool is_win32_symbols{true};
	::std::size_t symbols_index{};
	auto asmfile = ::fast_io::native_file_loader{::fast_io::mnp::os_c_str(asmpath)};
	auto const &&asmfile_size = asmfile.size();
	for (::std::size_t i{}; i < asmfile_size; ++i)
	{
		while (asmfile[i] == u8'\t' || asmfile[i] == u8' ' || asmfile[i] == u8'\n')
		{
			++i;
		}
		if (asmfile[i] == u8'.' && asmfile[i + 1] == u8'a' && asmfile[i + 2] == u8'd' && asmfile[i + 3] == u8'd' && asmfile[i + 4] == u8'r' && asmfile[i + 5] == u8's' && asmfile[i + 6] == u8'i' && asmfile[i + 7] == u8'g' && asmfile[i + 8] == u8'_' && asmfile[i + 9] == u8's' && asmfile[i + 10] == u8'y' && asmfile[i + 11] == u8'm')
		{
			i += 12;
			while (asmfile[i] != '\"')
			{
				if (asmfile[i] == '\n')
				{
					goto deep_continue;
				}
				else
				{
					++i;
				}
			}
			++i;
			result.append(u8"#pragma comment(linker,\"/alternatename:");
			while (asmfile[i] != '\"')
			{
				result.push_back(asmfile[i++]);
			}
			result.push_back('=');
			if (is_win32_symbols)
			{
				result.append(::fast_io::u8concat_fast_io(u8"#", win32_symbols[symbols_index++]));
				result.append(u8"\")\n");
				if (symbols_index == win32_symbols.size())
				{
					is_win32_symbols = false;
					symbols_index = 0;
					result.append(u8"// NT\n");
				}
			}
			else
			{
				result.append(::fast_io::u8concat_fast_io("#", nt_symbols[symbols_index++]));
				result.append(u8"\")\n");
			}
		}
		while (asmfile[i] != '\n')
		{
			++i;
		}
	deep_continue:;
	}

	return result;
}

int main()
{
	auto &&nt_symbols = parse_symbols(u8"nt_symbols.txt");
	auto &&win32_symbols = parse_symbols(u8"win32_symbols.txt");

	::fast_io::u8string mangling{u8"#pragma once\n// This file have been generated by codegen/win32mangling, therefore, do NOT edit this file directely!\n// clang-format off\n// WIN32\n"};
	mangling.append(gen_pragmas(u8"arm64ec-windows-msvc.s", win32_symbols, nt_symbols));

	mangling.append(u8R"(// msvc
#pragma comment(linker,"/alternatename:?msvc__RTtypeid@msvc@fast_io@@$$hYAPEAXPEAX@Z=#__RTtypeid")
// clang-format on
)");

	::fast_io::native_file output{u8"../../../include/fast_io_hosted/platforms/win32/msvc_linker_arm64ec.h", ::fast_io::open_mode::out};
	::fast_io::print(output, ::fast_io::mnp::code_cvt(mangling));

	return 0;
}
