#include<fast_io.h>
#define NOMINMAX 1
#define _WIN32_LEAN_AND_MEAN
#include<windows.h>
#undef min
#undef max

int main()
{
	::fast_io::io::perr("keep wine alive: ", utc(::fast_io::posix_clock_gettime(::fast_io::posix_clock_id::realtime)));
	WaitForSingleObject(GetCurrentProcess(), INFINITE);
}
