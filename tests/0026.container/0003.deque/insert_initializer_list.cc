#include <fast_io.h>
#include <fast_io_dsal/deque.h>
#include <deque>
#include <vector>

namespace
{

// fast_io::deque does not have insert(const_iterator, initializer_list).
// This test verifies equivalent behavior using insert_range with a vector
// initialized from an initializer list.

inline void test_insert_range_from_initializer_list()
{
	::fast_io::io::perr("=== deque insert_range from initializer_list test ===\n");

	// Insert at front
	{
		::fast_io::deque<::std::size_t> dq;
		::std::deque<::std::size_t> ref;

		for (::std::size_t i{}; i != 10u; ++i)
		{
			dq.push_back(1u);
			ref.push_back(1u);
		}

		::std::vector<::std::size_t> vec{3u, 4u, 5u, 6u};
		dq.insert_range(dq.cbegin() + 2, vec);
		ref.insert(ref.cbegin() + 2, {3, 4, 5, 6});

		if (dq.size() != ref.size())
		{
			::fast_io::io::panic("size mismatch\n");
		}

		for (::std::size_t i{}; i != dq.size(); ++i)
		{
			if (dq[i] != ref[i])
			{
				::fast_io::io::panicln("value mismatch at ", i);
			}
		}
	}

	// Insert at end
	{
		::fast_io::deque<::std::size_t> dq;
		::std::deque<::std::size_t> ref;

		for (::std::size_t i{}; i != 10u; ++i)
		{
			dq.push_back(i);
			ref.push_back(i);
		}

		::std::vector<::std::size_t> vec{100u, 101u, 102u};
		dq.insert_range(dq.cend(), vec);
		ref.insert(ref.cend(), {100, 101, 102});

		if (dq.size() != ref.size())
		{
			::fast_io::io::panic("size mismatch\n");
		}

		for (::std::size_t i{}; i != dq.size(); ++i)
		{
			if (dq[i] != ref[i])
			{
				::fast_io::io::panicln("value mismatch at ", i);
			}
		}
	}

	// Insert at middle with various sizes
	{
		::std::size_t const sizes[] = {0, 1, 2, 3, 1023, 1024, 1025, 2047, 2048, 2049};

		for (::std::size_t n : sizes)
		{
			::fast_io::deque<::std::size_t> dq;
			::std::deque<::std::size_t> ref;

			for (::std::size_t i{}; i != n; ++i)
			{
				dq.push_back(i);
				ref.push_back(i);
			}

			::std::size_t pos = n / 2;
			::std::vector<::std::size_t> vec{888u, 999u};
			dq.insert_range(dq.cbegin() + pos, vec);
			ref.insert(ref.cbegin() + pos, {888, 999});

			if (dq.size() != ref.size())
			{
				::fast_io::io::panicln("size mismatch for n=", n);
			}

			for (::std::size_t i{}; i != dq.size(); ++i)
			{
				if (dq[i] != ref[i])
				{
					::fast_io::io::panicln("value mismatch at ", i, " for n=", n);
				}
			}
		}
	}

	// Insert into empty deque
	{
		::fast_io::deque<::std::size_t> dq;
		::std::deque<::std::size_t> ref;

		::std::vector<::std::size_t> vec{10u, 20u, 30u};
		dq.insert_range(dq.cbegin(), vec);
		ref.insert(ref.cbegin(), {10, 20, 30});

		if (dq.size() != ref.size())
		{
			::fast_io::io::panic("size mismatch\n");
		}

		for (::std::size_t i{}; i != dq.size(); ++i)
		{
			if (dq[i] != ref[i])
			{
				::fast_io::io::panicln("value mismatch at ", i);
			}
		}
	}

	::fast_io::io::print("deque insert_range from initializer_list test finished\n");
}

} // namespace

int main()
{
	test_insert_range_from_initializer_list();
}
