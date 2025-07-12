#include <fast_io.h>
#include <fast_io_dsal/str_btree_set.h>

thread_local ::fast_io::containers::basic_str_btree_set<char, ::fast_io::native_global_allocator, 4> bset;
thread_local ::std::size_t n{};

extern "C" int LLVMFuzzerTestOneInput(::std::uint8_t *data, ::std::size_t size)
{
	if (n == 100000)
	{
		bset.clear_destroy();
		n = 0;
	}
	bset.insert_key(::fast_io::containers::basic_string_view<char>(reinterpret_cast<char const *>(data), size));
	++n;
	return 0;
}