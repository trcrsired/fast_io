#include <fast_io.h>
#include <fast_io_legacy.h>
#include <fstream>

/*
wine_file -> std::ofstream, like construct_fstream_from_syscall.cc.

__wine_unix_host_fd_to_nt_handle hands out an independently-owned HANDLE on
either wineunix.dll implementation: the nt impl NtDuplicateObject's the handle
it wraps, the unixcall impl registers the host fd with the wineserver inside
libwineunix.so. wine_file keeps owning its host_fd either way.
*/

int main()
try
{
	::fast_io::wine_file wf(u8"fstream.txt", ::fast_io::open_mode::out);
	::fast_io::nt_file nf(
		reinterpret_cast<void *>(static_cast<::std::uintptr_t>(
			try(__wine_unix_host_fd_to_nt_handle(wf.native_handle())))));
	::fast_io::filebuf_file fbf(::std::move(nf), ::fast_io::open_mode::out);
	::std::ofstream fout;
	*fout.rdbuf() = ::std::move(*fbf.fb);
	::fast_io::filebuf_io_observer fiob{fout.rdbuf()};
	fout << "Hello World from std::ofstream\n";
	::fast_io::print(fiob, "Hello World from fast_io::filebuf_io_observer\n");
}
catch throws(::std::error e)
{
	::fast_io::perrln(e);
	return 1;
}
