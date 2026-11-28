// Raw byte/seek operations are [[unsafe]] in the safe dialect; the high-level
// formatted and streaming operations stay safe. This example exercises the
// safe side: print, scan and transmit need no user-written [[unsafe]] fence.
//
// See operations_classify_reject.cc for the raw operations that a fence is
// required for.
#include <fast_io.h>

int main()
{
	int n{};
	fast_io::scan(n);
	fast_io::print("n=", n, "\n");
	fast_io::posix_file src(u8"/tmp/safecxx_ops_src", fast_io::open_mode::in | fast_io::open_mode::creat);
	fast_io::posix_file dst(u8"/tmp/safecxx_ops_dst", fast_io::open_mode::out | fast_io::open_mode::creat);
	fast_io::operations::transmit_all_bytes(dst, {}, src, {}, {});
}
