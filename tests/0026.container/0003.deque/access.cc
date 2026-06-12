#include <fast_io.h>
#include <fast_io_dsal/deque.h>
#include <deque>

namespace
{

inline void test_operator_subscript()
{
	::fast_io::io::perr("=== deque operator[] test ===\n");

	::fast_io::deque<::std::size_t> dq;
	::std::deque<::std::size_t> ref;

	// Fill with data
	for (::std::size_t i{}; i != 4096u; ++i)
	{
		dq.push_back(i);
		ref.push_back(i);
	}

	// Test operator[] (non-const)
	for (::std::size_t i{}; i != dq.size(); ++i)
	{
		if (dq[i] != ref[i])
		{
			::fast_io::io::panicln("operator[] mismatch at ", i);
		}
	}

	// Test operator[] (const)
	{
		auto const &cdq = dq;
		for (::std::size_t i{}; i != cdq.size(); ++i)
		{
			if (cdq[i] != ref[i])
			{
				::fast_io::io::panicln("const operator[] mismatch at ", i);
			}
		}
	}

	// Test index_unchecked
	for (::std::size_t i{}; i != dq.size(); ++i)
	{
		if (dq.index_unchecked(i) != ref[i])
		{
			::fast_io::io::panicln("index_unchecked mismatch at ", i);
		}
	}

	// Modify via operator[] and verify
	for (::std::size_t i{}; i != dq.size(); ++i)
	{
		dq[i] = i * 2u;
		ref[i] = i * 2u;
	}

	for (::std::size_t i{}; i != dq.size(); ++i)
	{
		if (dq[i] != ref[i])
		{
			::fast_io::io::panicln("operator[] modification mismatch at ", i);
		}
	}

	::fast_io::io::print("deque operator[] test finished\n");
}

inline void test_front_back()
{
	::fast_io::io::perr("=== deque front() / back() test ===\n");

	::fast_io::deque<::std::size_t> dq;
	::std::deque<::std::size_t> ref;

	for (::std::size_t i{}; i != 4096u; ++i)
	{
		dq.push_back(i);
		ref.push_back(i);
	}

	// front() and back() (non-const)
	if (dq.front() != ref.front())
	{
		::fast_io::io::panic("front() mismatch\n");
	}

	if (dq.back() != ref.back())
	{
		::fast_io::io::panic("back() mismatch\n");
	}

	// front() and back() (const)
	{
		auto const &cdq = dq;
		if (cdq.front() != ref.front())
		{
			::fast_io::io::panic("const front() mismatch\n");
		}

		if (cdq.back() != ref.back())
		{
			::fast_io::io::panic("const back() mismatch\n");
		}
	}

	// front_unchecked and back_unchecked
	if (dq.front_unchecked() != ref.front())
	{
		::fast_io::io::panic("front_unchecked() mismatch\n");
	}

	if (dq.back_unchecked() != ref.back())
	{
		::fast_io::io::panic("back_unchecked() mismatch\n");
	}

	// Modify and verify
	dq.front() = 9999u;
	ref.front() = 9999u;

	if (dq.front() != ref.front())
	{
		::fast_io::io::panic("front() modification mismatch\n");
	}

	dq.back() = 8888u;
	ref.back() = 8888u;

	if (dq.back() != ref.back())
	{
		::fast_io::io::panic("back() modification mismatch\n");
	}

	::fast_io::io::print("deque front() / back() test finished\n");
}

} // namespace

int main()
{
	test_operator_subscript();
	test_front_back();
}
