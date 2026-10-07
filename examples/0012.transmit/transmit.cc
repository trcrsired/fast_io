#include <cstdio>
#include <fast_io.h>

int main()
{
	::fast_io::operations::transmit_all_bytes(fast_io::c_stdout(), {}, fast_io::c_stdin(), {}, {});
}