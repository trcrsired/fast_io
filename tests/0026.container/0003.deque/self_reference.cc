#include <fast_io.h>
#include <fast_io_dsal/deque.h>
#include <deque>
#include <vector>

namespace
{

inline void test_self_reference_insert_single()
{
	::fast_io::io::perr("=== deque self-reference insert single test ===\n");

	// Insert element from the same deque at various positions
	for (::std::size_t i{}; i != 20u; ++i)
	{
		for (::std::size_t j{}; j != 20u; ++j)
		{
			::fast_io::deque<::std::size_t> dq;
			::std::deque<::std::size_t> ref;

			for (::std::size_t k{}; k != 20u; ++k)
			{
				dq.push_back(k);
				ref.push_back(k);
			}

			auto val = dq[j];
			dq.insert(dq.cbegin() + i, val);
			ref.insert(ref.cbegin() + i, ref[j]);

			if (dq.size() != ref.size())
			{
				::fast_io::io::panicln("size mismatch for i=", i, " j=", j);
			}

			for (::std::size_t k{}; k != dq.size(); ++k)
			{
				if (dq[k] != ref[k])
				{
					::fast_io::io::panicln("value mismatch at ", k, " for i=", i, " j=", j);
				}
			}
		}
	}

	::fast_io::io::print("deque self-reference insert single test finished\n");
}

inline void test_self_reference_insert_count()
{
	::fast_io::io::perr("=== deque self-reference insert count test ===\n");

	// Insert count copies of element from the same deque
	for (::std::size_t i{}; i != 20u; ++i)
	{
		for (::std::size_t j{}; j != 20u; ++j)
		{
			::fast_io::deque<::std::size_t> dq;
			::std::deque<::std::size_t> ref;

			for (::std::size_t k{}; k != 20u; ++k)
			{
				dq.push_back(k);
				ref.push_back(k);
			}

			auto val = dq[j];
			dq.insert(dq.cbegin() + i, 5u, val);
			ref.insert(ref.cbegin() + i, 5, ref[j]);

			if (dq.size() != ref.size())
			{
				::fast_io::io::panicln("size mismatch for i=", i, " j=", j);
			}

			for (::std::size_t k{}; k != dq.size(); ++k)
			{
				if (dq[k] != ref[k])
				{
					::fast_io::io::panicln("value mismatch at ", k, " for i=", i, " j=", j);
				}
			}
		}
	}

	::fast_io::io::print("deque self-reference insert count test finished\n");
}

} // namespace

int main()
{
	test_self_reference_insert_single();
	test_self_reference_insert_count();
}
