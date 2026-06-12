#include <fast_io.h>
#include <fast_io_dsal/deque.h>
#include <deque>
#include <algorithm>
#include <vector>

namespace
{

inline void test_copy()
{
	::fast_io::io::perr("=== deque std::copy test ===\n");

	// Test sizes at and around block boundaries
	::std::size_t const sizes[] = {0, 1, 2, 3, 1023, 1024, 1025, 2047, 2048, 2049};

	for (::std::size_t n : sizes)
	{
		::fast_io::deque<::std::size_t> src;
		::fast_io::deque<::std::size_t> dst;

		for (::std::size_t i{}; i != n; ++i)
		{
			src.push_back(i);
			dst.push_back(0u);
		}

		auto result = ::std::copy(src.cbegin(), src.cend(), dst.begin());
		if (result != dst.end())
		{
			::fast_io::io::panicln("copy result not at end for n=", n);
		}

		for (::std::size_t i{}; i != n; ++i)
		{
			if (dst[i] != src[i])
			{
				::fast_io::io::panicln("copy value mismatch at ", i, " for n=", n);
			}
		}
	}

	// Compare with std::deque
	{
		::fast_io::deque<::std::size_t> fsrc, fdst;
		::std::deque<::std::size_t> ssrc, sdst;

		for (::std::size_t i{}; i != 4096u; ++i)
		{
			fsrc.push_back(i);
			fdst.push_back(0u);
			ssrc.push_back(i);
			sdst.push_back(0);
		}

		::std::copy(fsrc.cbegin(), fsrc.cend(), fdst.begin());
		::std::copy(ssrc.cbegin(), ssrc.cend(), sdst.begin());

		for (::std::size_t i{}; i != 4096u; ++i)
		{
			if (fdst[i] != sdst[i])
			{
				::fast_io::io::panicln("copy mismatch with std::deque at ", i);
			}
		}
	}

	::fast_io::io::print("deque std::copy test finished\n");
}

inline void test_copy_backward()
{
	::fast_io::io::perr("=== deque std::copy_backward test ===\n");

	{
		::fast_io::deque<::std::size_t> dq;
		for (::std::size_t i{}; i != 100u; ++i)
		{
			dq.push_back(i);
		}

			dq.resize(110u);
		::std::copy_backward(dq.cbegin(), dq.cbegin() + 100, dq.end());

		// First 10 elements unchanged (still 0..9)
		for (::std::size_t i{}; i != 10u; ++i)
		{
			if (dq[i] != i)
			{
				::fast_io::io::panicln("copy_backward: first elements wrong at ", i);
			}
		}
		for (::std::size_t i{}; i != 100u; ++i)
		{
			if (dq[i + 10u] != i)
			{
				::fast_io::io::panicln("copy_backward: shifted elements wrong at ", i);
			}
		}
	}

	// Compare with std::deque
	{
		::fast_io::deque<::std::size_t> fdq;
		::std::deque<::std::size_t> sdq;

		for (::std::size_t i{}; i != 100u; ++i)
		{
			fdq.push_back(i);
			sdq.push_back(i);
		}

		fdq.resize(110u);
		sdq.resize(110);
		::std::copy_backward(fdq.cbegin(), fdq.cbegin() + 100, fdq.end());
		::std::copy_backward(sdq.cbegin(), sdq.cbegin() + 100, sdq.end());

		for (::std::size_t i{}; i != 110u; ++i)
		{
			if (fdq[i] != sdq[i])
			{
				::fast_io::io::panicln("copy_backward mismatch with std::deque at ", i);
			}
		}
	}

	::fast_io::io::print("deque std::copy_backward test finished\n");
}

inline void test_move()
{
	::fast_io::io::perr("=== deque std::move test ===\n");

	::std::size_t const sizes[] = {0, 1, 2, 3, 1023, 1024, 1025, 2047, 2048, 2049};

	for (::std::size_t n : sizes)
	{
		::fast_io::deque<::std::size_t> src;
		::fast_io::deque<::std::size_t> dst;

		for (::std::size_t i{}; i != n; ++i)
		{
			src.push_back(i);
			dst.push_back(0u);
		}

		auto result = ::std::move(src.begin(), src.end(), dst.begin());
		if (result != dst.end())
		{
			::fast_io::io::panicln("move result not at end for n=", n);
		}

		for (::std::size_t i{}; i != n; ++i)
		{
			if (dst[i] != i)
			{
				::fast_io::io::panicln("move value mismatch at ", i, " for n=", n);
			}
		}
	}

	::fast_io::io::print("deque std::move test finished\n");
}

inline void test_move_backward()
{
	::fast_io::io::perr("=== deque std::move_backward test ===\n");

	::fast_io::deque<::std::size_t> dq;
	for (::std::size_t i{}; i != 100u; ++i)
	{
		dq.push_back(i);
	}

	dq.resize(110u);
	::std::move_backward(dq.begin(), dq.begin() + 100, dq.end());

	for (::std::size_t i{}; i != 10u; ++i)
	{
		if (dq[i] != i)
		{
			::fast_io::io::panicln("move_backward: first elements wrong at ", i);
		}
	}
	for (::std::size_t i{}; i != 100u; ++i)
	{
		if (dq[i + 10u] != i)
		{
			::fast_io::io::panicln("move_backward: shifted elements wrong at ", i);
		}
	}

	::fast_io::io::print("deque std::move_backward test finished\n");
}

} // namespace

int main()
{
	test_copy();
	test_copy_backward();
	test_move();
	test_move_backward();
}
