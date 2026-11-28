// Raw byte/seek operations are [[unsafe]] in the safe dialect. Each call
// below is rejected in safe source; put it inside a [[unsafe]] fence if the
// program really needs to move raw bytes.
#include <fast_io.h>

int main()
{
	fast_io::posix_file f(u8"/tmp/safecxx_ops_reject",
						  fast_io::open_mode::in | fast_io::open_mode::out | fast_io::open_mode::creat);
	[[unsafe]] char buf[16]; // the raw buffer itself needs a fence
	fast_io::operations::read_some(f, buf, 16); // expected-error: unsafe
	fast_io::operations::write_some(f, buf, 16); // expected-error: unsafe
	fast_io::operations::pread_some(f, buf, 16, 0); // expected-error: unsafe
	fast_io::operations::pwrite_some(f, buf, 16, 0); // expected-error: unsafe
	// seek/rewind/buffer-flush stay safe: they take a stream, no raw pointer.
}
