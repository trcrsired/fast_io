// Storing an expression-only manipulator result is rejected in safe source.
#include <fast_io.h>

int main()
{
	auto m = fast_io::mnp::os_c_str_carr("hello world"); // error: expression-only
	fast_io::print(m);
}
