#include <fast_io.h>
#include <fast_io_dsal/deque.h>
#include <fast_io_dsal/string.h>
#include <deque>

namespace
{

inline void test_operator_equal()
{
	::fast_io::io::perr("=== deque operator== test ===\n");

	// Equal empty deques
	{
		::fast_io::deque<::std::size_t> a, b;
		if (!(a == b))
		{
			::fast_io::io::panic("empty deques should be equal\n");
		}
	}

	// Equal non-empty deques
	{
		::fast_io::deque<::std::size_t> a, b;
		for (::std::size_t i{}; i != 100u; ++i)
		{
			a.push_back(i);
			b.push_back(i);
		}
		if (!(a == b))
		{
			::fast_io::io::panic("identical deques should be equal\n");
		}
	}

	// Unequal deques (different size)
	{
		::fast_io::deque<::std::size_t> a, b;
		a.push_back(1u);
		a.push_back(2u);
		b.push_back(1u);
		if (a == b)
		{
			::fast_io::io::panic("deques with different sizes should not be equal\n");
		}
	}

	// Unequal deques (same size, different values)
	{
		::fast_io::deque<::std::size_t> a, b;
		a.push_back(1u);
		a.push_back(2u);
		b.push_back(1u);
		b.push_back(3u);
		if (a == b)
		{
			::fast_io::io::panic("deques with different values should not be equal\n");
		}
	}

	// Self comparison
	{
		::fast_io::deque<::std::size_t> a;
		for (::std::size_t i{}; i != 100u; ++i)
		{
			a.push_back(i);
		}
		if (!(a == a))
		{
			::fast_io::io::panic("deque should equal itself\n");
		}
	}

	// Compare with std::deque
	{
		::fast_io::deque<::std::size_t> a;
		::std::deque<::std::size_t> ref;
		for (::std::size_t i{}; i != 4096u; ++i)
		{
			a.push_back(i);
			ref.push_back(i);
		}

		// Verify fast_io deque == fast_io deque consistency
		::fast_io::deque<::std::size_t> b = a;
		if (!(a == b))
		{
			::fast_io::io::panic("copy should be equal\n");
		}
	}

	::fast_io::io::print("deque operator== test finished\n");
}

inline void test_spaceship()
{
	::fast_io::io::perr("=== deque operator<=> test ===\n");

	// Equal deques
	{
		::fast_io::deque<::std::size_t> a, b;
		for (::std::size_t i{}; i != 100u; ++i)
		{
			a.push_back(i);
			b.push_back(i);
		}
		auto cmp = (a <=> b);
		if (!(cmp == 0))
		{
			::fast_io::io::panic("equal deques should compare equal\n");
		}
	}

	// a < b (a is prefix)
	{
		::fast_io::deque<::std::size_t> a, b;
		a.push_back(1u);
		a.push_back(2u);
		b.push_back(1u);
		b.push_back(2u);
		b.push_back(3u);

		auto cmp = (a <=> b);
		if (!(cmp < 0))
		{
			::fast_io::io::panic("prefix deque should be less\n");
		}
	}

	// a < b (first differing element)
	{
		::fast_io::deque<::std::size_t> a, b;
		a.push_back(1u);
		a.push_back(2u);
		b.push_back(1u);
		b.push_back(3u);

		auto cmp = (a <=> b);
		if (!(cmp < 0))
		{
			::fast_io::io::panic("deque with smaller element should be less\n");
		}
	}

	// a > b
	{
		::fast_io::deque<::std::size_t> a, b;
		a.push_back(1u);
		a.push_back(3u);
		b.push_back(1u);
		b.push_back(2u);

		auto cmp = (a <=> b);
		if (!(cmp > 0))
		{
			::fast_io::io::panic("deque with larger element should be greater\n");
		}
	}

	// Empty deques
	{
		::fast_io::deque<::std::size_t> a, b;
		auto cmp = (a <=> b);
		if (!(cmp == 0))
		{
			::fast_io::io::panic("empty deques should compare equal\n");
		}
	}

	// Deque with non-trivial type
	{
		::fast_io::deque<::fast_io::string> a, b;
		a.emplace_back("apple");
		a.emplace_back("banana");
		b.emplace_back("apple");
		b.emplace_back("cherry");

		auto cmp = (a <=> b);
		if (!(cmp < 0))
		{
			::fast_io::io::panic("string spaceship should work\n");
		}
	}

	::fast_io::io::print("deque operator<=> test finished\n");
}

} // namespace

int main()
{
	test_operator_equal();
	test_spaceship();
}
