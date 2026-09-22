#include <fast_io.h>
#include <fast_io_legacy.h>
#include <fstream>

/*
wine_file -> std::ofstream, like construct_fstream_from_syscall.cc.

filebuf_file move-constructs straight from wine_file: the chain goes
wine_file -> nt_file -> posix_file -> c_file -> filebuf_file, with the
bottom hop handing the released host_fd to __wine_unix_host_fd_to_nt_handle,
which transfers ownership — a wineserver-registered HANDLE on the unixcall
impl, the same HANDLE bits on the nt impl.
*/

int main()
try
{
	::fast_io::wine_file wf(u8"fstream.txt", ::fast_io::open_mode::out);
	::fast_io::filebuf_file fbf(::std::move(wf), ::fast_io::open_mode::out);
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
