#include <fast_io.h>
#include <fast_io_dsal/deque.h>
#include <fast_io_dsal/string.h>
#include <deque>
#include <algorithm>

namespace
{

inline void test_swap_member()
{
	::fast_io::io::perr("=== deque swap member test ===\n");

	::std::size_t const sizes[] = {0, 1, 2, 3, 1023, 1024, 1025, 2047, 2048, 2049};

	for (::std::size_t n : sizes)
	{
		for (::std::size_t m : sizes)
		{
			::fast_io::deque<::std::size_t> a;
			::fast_io::deque<::std::size_t> b;

			for (::std::size_t i{}; i != n; ++i)
			{
				a.push_back(i);
			}
			for (::std::size_t i{}; i != m; ++i)
			{
				b.push_back(i + 1000u);
			}

			::fast_io::deque<::std::size_t> a_save = a;
			::fast_io::deque<::std::size_t> b_save = b;

			a.swap(b);

			// a should now equal b_save
			if (a.size() != b_save.size())
			{
				::fast_io::io::panicln("a.size mismatch after swap for n=", n, " m=", m);
			}
			for (::std::size_t i{}; i != a.size(); ++i)
			{
				if (a[i] != b_save[i])
				{
					::fast_io::io::panicln("a[", i, "] mismatch after swap for n=", n, " m=", m);
				}
			}

			// b should now equal a_save
			if (b.size() != a_save.size())
			{
				::fast_io::io::panicln("b.size mismatch after swap for n=", n, " m=", m);
			}
			for (::std::size_t i{}; i != b.size(); ++i)
			{
				if (b[i] != a_save[i])
				{
					::fast_io::io::panicln("b[", i, "] mismatch after swap for n=", n, " m=", m);
				}
			}
		}
	}

	::fast_io::io::print("deque swap member test finished\n");
}

inline void test_swap_free_function()
{
	::fast_io::io::perr("=== deque swap free function test ===\n");

	::fast_io::deque<::std::size_t> a;
	::fast_io::deque<::std::size_t> b;

	for (::std::size_t i{}; i != 100u; ++i)
	{
		a.push_back(i);
	}
	for (::std::size_t i{}; i != 200u; ++i)
	{
		b.push_back(i + 500u);
	}

	::fast_io::deque<::std::size_t> a_save = a;
	::fast_io::deque<::std::size_t> b_save = b;

	swap(a, b);

	if (a.size() != b_save.size() || b.size() != a_save.size())
	{
		::fast_io::io::panic("size mismatch after free function swap\n");
	}

	for (::std::size_t i{}; i != a.size(); ++i)
	{
		if (a[i] != b_save[i])
		{
			::fast_io::io::panic("a value mismatch after free function swap\n");
		}
	}

	for (::std::size_t i{}; i != b.size(); ++i)
	{
		if (b[i] != a_save[i])
		{
			::fast_io::io::panic("b value mismatch after free function swap\n");
		}
	}

	// Self swap
	a.swap(a);
	if (a.size() != b_save.size())
	{
		::fast_io::io::panic("self-swap changed size\n");
	}
	for (::std::size_t i{}; i != a.size(); ++i)
	{
		if (a[i] != b_save[i])
		{
			::fast_io::io::panic("self-swap changed values\n");
		}
	}

	::fast_io::io::print("deque swap free function test finished\n");
}

inline void test_swap_non_trivial()
{
	::fast_io::io::perr("=== deque swap non-trivial type test ===\n");

	::fast_io::deque<::fast_io::string> a;
	::fast_io::deque<::fast_io::string> b;

	for (::std::size_t i{}; i != 100u; ++i)
	{
		a.emplace_back(::fast_io::concat_fast_io("hello_", i));
	}
	for (::std::size_t i{}; i != 50u; ++i)
	{
		b.emplace_back(::fast_io::concat_fast_io("world_", i));
	}

	::fast_io::deque<::fast_io::string> a_save = a;
	::fast_io::deque<::fast_io::string> b_save = b;

	a.swap(b);

	if (a.size() != b_save.size() || b.size() != a_save.size())
	{
		::fast_io::io::panic("size mismatch after non-trivial swap\n");
	}

	for (::std::size_t i{}; i != a.size(); ++i)
	{
		if (a[i] != b_save[i])
		{
			::fast_io::io::panic("value mismatch after non-trivial swap\n");
		}
	}

	::fast_io::io::print("deque swap non-trivial type test finished\n");
}

} // namespace

int main()
{
	test_swap_member();
	test_swap_free_function();
	test_swap_non_trivial();
}
