#include <fast_io.h>
#include <fast_io_dsal/deque.h>
#include <deque>

namespace
{

inline void test_pop_front_invalidation()
{
	::fast_io::io::perr("=== deque pop_front invalidation test ===\n");

	::fast_io::deque<::std::size_t> dq;
	for (::std::size_t i{}; i != 4098u; ++i)
	{
		dq.push_back(i);
	}

	while (dq.size() > 1u)
	{
		auto it1 = dq.begin() + 1;   // second element
		auto it2 = dq.end() - 1;     // last element
		::std::size_t val1 = *it1;
		::std::size_t val2 = *it2;

		dq.pop_front();

		// After pop_front, it1 should still point to the same element (now at begin)
		if (*dq.begin() != val1)
		{
			::fast_io::io::panic("pop_front invalidated iterator to second element\n");
		}

		// it2 should still point to the last element
		if (*(dq.end() - 1) != val2)
		{
			::fast_io::io::panic("pop_front invalidated iterator to last element\n");
		}

		// Also verify via direct access
		if (dq.front() != val1)
		{
			::fast_io::io::panic("pop_front changed front value\n");
		}

		dq.pop_back(); // shrink for next iteration
	}

	::fast_io::io::print("deque pop_front invalidation test finished\n");
}

inline void test_pop_back_invalidation()
{
	::fast_io::io::perr("=== deque pop_back invalidation test ===\n");

	::fast_io::deque<::std::size_t> dq;
	for (::std::size_t i{}; i != 4098u; ++i)
	{
		dq.push_back(i);
	}

	while (dq.size() > 1u)
	{
		auto it1 = dq.begin();       // first element
		auto it2 = dq.end() - 2;     // second-to-last element
		::std::size_t val1 = *it1;
		::std::size_t val2 = *it2;

		dq.pop_back();

		// After pop_back, it1 should still point to the same element
		if (*dq.begin() != val1)
		{
			::fast_io::io::panic("pop_back invalidated iterator to first element\n");
		}

		// it2 should still point to what is now the last element
		if (*(dq.end() - 1) != val2)
		{
			::fast_io::io::panic("pop_back invalidated iterator to second-to-last element\n");
		}

		// Also verify via direct access
		if (dq.back() != val2)
		{
			::fast_io::io::panic("pop_back changed back value\n");
		}
	}

	::fast_io::io::print("deque pop_back invalidation test finished\n");
}

inline void test_erase_single_invalidation()
{
	::fast_io::io::perr("=== deque erase single invalidation test ===\n");

	// Erase first element
	{
		::fast_io::deque<::std::size_t> dq;
		for (::std::size_t i{}; i != 20u; ++i)
		{
			dq.push_back(i);
		}

		auto it1 = dq.begin() + 1;   // element 1
		auto it2 = dq.end() - 1;     // last element

		dq.erase(dq.begin());

		// After erasing first element, it1 should still be valid and point to what was index 1
		if (*dq.begin() != 1u)
		{
			::fast_io::io::panic("erase front: begin value wrong\n");
		}

		if (*(dq.end() - 1) != 19u)
		{
			::fast_io::io::panic("erase front: end value wrong\n");
		}
	}

	// Erase last element
	{
		::fast_io::deque<::std::size_t> dq;
		for (::std::size_t i{}; i != 20u; ++i)
		{
			dq.push_back(i);
		}

		auto it1 = dq.begin();       // first element
		auto it2 = dq.end() - 2;     // second-to-last

		dq.erase(dq.end() - 1);

		if (*dq.begin() != 0u)
		{
			::fast_io::io::panic("erase back: begin value wrong\n");
		}

		if (*(dq.end() - 1) != 18u)
		{
			::fast_io::io::panic("erase back: end value wrong\n");
		}
	}

	::fast_io::io::print("deque erase single invalidation test finished\n");
}

inline void test_erase_range_invalidation()
{
	::fast_io::io::perr("=== deque erase range invalidation test ===\n");

	// Erase range at start
	{
		::fast_io::deque<::std::size_t> dq;
		for (::std::size_t i{}; i != 20u; ++i)
		{
			dq.push_back(i);
		}

		::std::size_t num = 5u;
		auto it_beyond = dq.begin() + num; // first element after erased range
		auto it_end = dq.end() - 1;        // last element

		dq.erase(dq.begin(), dq.begin() + num);

		// Elements after the erased range should have been moved down, but iterators
		// to the remaining elements should still be valid
		if (*dq.begin() != num)
		{
			::fast_io::io::panic("erase range front: first element wrong\n");
		}

		if (*(dq.end() - 1) != 19u)
		{
			::fast_io::io::panic("erase range front: last element wrong\n");
		}
	}

	// Erase range at end
	{
		::fast_io::deque<::std::size_t> dq;
		for (::std::size_t i{}; i != 20u; ++i)
		{
			dq.push_back(i);
		}

		::std::size_t num = 5u;
		auto it_first = dq.begin();
		auto it_before = dq.end() - num - 1;

		dq.erase(dq.end() - num, dq.end());

		if (*dq.begin() != 0u)
		{
			::fast_io::io::panic("erase range back: first element wrong\n");
		}

		if (*(dq.end() - 1) != 14u)
		{
			::fast_io::io::panic("erase range back: last element wrong\n");
		}
	}

	// Erase range at start with various sizes around block boundaries
	{
		::std::size_t const sizes[] = {1, 2, 3, 1023, 1024, 1025, 2047, 2048, 2049};

		for (::std::size_t n : sizes)
		{
			::fast_io::deque<::std::size_t> dq;
			::std::deque<::std::size_t> ref;

			for (::std::size_t i{}; i != n; ++i)
			{
				dq.push_back(i);
				ref.push_back(i);
			}

			::std::size_t num = (std::min)(n, static_cast<::std::size_t>(100u));
			if (num == 0u)
			{
				continue;
			}

			dq.erase(dq.cbegin(), dq.cbegin() + num);
			ref.erase(ref.cbegin(), ref.cbegin() + num);

			if (dq.size() != ref.size())
			{
				::fast_io::io::panicln("erase range size mismatch for n=", n);
			}

			for (::std::size_t i{}; i != dq.size(); ++i)
			{
				if (dq[i] != ref[i])
				{
					::fast_io::io::panicln("erase range value mismatch at ", i, " for n=", n);
				}
			}
		}
	}

	::fast_io::io::print("deque erase range invalidation test finished\n");
}

} // namespace

int main()
{
	test_pop_front_invalidation();
	test_pop_back_invalidation();
	test_erase_single_invalidation();
	test_erase_range_invalidation();
}
