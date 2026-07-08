#include <fast_io.h>
#include <fast_io_dsal/str_swiss_set.h>

using namespace fast_io::io;

int main()
{
	// Empty input
	::std::uint_least64_t h0{::fast_io::details::xxh3_64bits_fio(nullptr, 0)};
	println("xxh3_64bits_fio(nullptr, 0) = ", h0);

	// Single byte
	char unsigned c{'A'};
	::std::uint_least64_t h1{::fast_io::details::xxh3_64bits_fio(__builtin_addressof(c), 1)};
	println("xxh3_64bits_fio('A') = ", h1);

	// Short string (< 16 bytes)
	char const *short_str{"Hello, World!"};
	::std::uint_least64_t h2{::fast_io::details::xxh3_64bits_fio(short_str, 13)};
	println("xxh3_64bits_fio(\"Hello, World!\") = ", h2);

	// Exactly 16 bytes (one full block)
	char const *block16{"0123456789ABCDEF"};
	::std::uint_least64_t h3{::fast_io::details::xxh3_64bits_fio(block16, 16)};
	println("xxh3_64bits_fio(16 bytes) = ", h3);

	// 32 bytes (two full blocks)
	char const *block32{"0123456789ABCDEF0123456789ABCDEF"};
	::std::uint_least64_t h4{::fast_io::details::xxh3_64bits_fio(block32, 32)};
	println("xxh3_64bits_fio(32 bytes) = ", h4);

	// 17 bytes (one block + 1 byte tail)
	char const *tail1{"0123456789ABCDEFX"};
	::std::uint_least64_t h5{::fast_io::details::xxh3_64bits_fio(tail1, 17)};
	println("xxh3_64bits_fio(17 bytes) = ", h5);

	// Same input must produce same output (determinism)
	::std::uint_least64_t h6{::fast_io::details::xxh3_64bits_fio(short_str, 13)};
	if (h2 != h6)
	{
		println("ERROR: non-deterministic hash!");
		return 1;
	}

	// Different inputs should (very likely) differ
	if (h0 == h1 || h1 == h2 || h2 == h3 || h3 == h4 || h4 == h5)
	{
		println("ERROR: hash collision on small test set!");
		return 1;
	}

	println("All xxh3_64bits_fio tests passed.");
	return 0;
}
