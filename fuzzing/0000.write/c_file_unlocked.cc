﻿#include <fast_io.h>

fast_io::c_file_unlocked obf(
#if defined(_WIN32) && !defined(__WINE__)
	"nul"
#else
	"/dev/null"
#endif
	,
	fast_io::open_mode::out);

extern "C" int LLVMFuzzerTestOneInput(std::uint8_t const *pptr, std::size_t n) noexcept
{
	::std::byte const *ptr{reinterpret_cast<::std::byte const *>(pptr)};
	::fast_io::operations::write_all_bytes(obf, ptr, ptr + n);
	return 0;
}
