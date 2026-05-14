#include <fast_io_dsal/string.h>

// Tests that default construction (reset_imp) works in constexpr context.
constexpr bool default_construct()
{
	::fast_io::u8string s{};
	return s.empty();
}
static_assert(default_construct());

int main()
{
	return 0;
}
