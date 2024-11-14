#include <fast_io.h>

int main()
{

	::fast_io::posix_process p{u8"tests.0035.process.test", {}, {}, {}};
	auto ec{wait(p)};
	if (::std::bit_cast<int>(ec) != 114514)
	{
		::fast_io::fast_terminate();
	}
}