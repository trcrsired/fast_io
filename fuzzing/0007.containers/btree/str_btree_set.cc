#include <fast_io.h>
#include <fast_io_dsal/impl/associative_string.h>
#include <fast_io_dsal/impl/str_btree_set.h>
#include <fast_io_dsal/string_view.h>

thread_local ::fast_io::containers::basic_str_btree_set<char, ::fast_io::native_global_allocator, 4> bset;
thread_local ::std::size_t n{};

extern "C" int LLVMFuzzerTestOneInput(::std::uint8_t *data, ::std::size_t size)
{
	if (n == 100000)
	{
		bset.clear_destroy();
		n = 0;
	}
	bset.insert_key(::fast_io::string_view(reinterpret_cast<char const *>(data), size));
	++n;
	return 0;
}