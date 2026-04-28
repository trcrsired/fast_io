#include <fast_io.h>
#include <fast_io_dsal/deque.h>
#include <cstdint>
#include <deque>
#include <algorithm>

namespace
{
inline void test_deque(::std::uint_least8_t flag)
{
	bool const toback{static_cast<bool>(flag & 0x1)};
	bool const useeraseindex{static_cast<bool>(flag & 0x3)};
	::std::deque<::std::uint_least32_t> ref;
	::fast_io::deque<::std::uint_least32_t> deq;
	for (::std::uint_least32_t i{}; i != 1024u; ++i)
	{
		if (toback)
		{
			deq.push_back(i);
			ref.push_back(i);
		}
		else
		{
			deq.push_front(i);
			ref.push_front(i);
		}
	}
	if (!::std::equal(deq.cbegin(), deq.cend(), ref.cbegin(), ref.cend()))
	{
		__builtin_trap();
	}

	using namespace ::fast_io::manipulators;
	if (useeraseindex)
	{
		::std::size_t first{};
		::std::size_t last{deq.size()};
		::std::size_t mid{last >> 1};
		if (toback)
		{
			deq.erase_index(mid, last);
			deq.erase_index(first, mid - 1u);
		}
		else
		{
			deq.erase_index(first, mid);
			deq.erase_index(1u, mid);
		}
		deq.erase_index(0u);
	}
	else
	{
		::std::size_t mid{deq.size() >> 1u};
		if (toback)
		{
			deq.erase(deq.cbegin() + mid, deq.cend());
			deq.erase(deq.cbegin(), deq.cbegin() + (mid - 1u));
		}
		else
		{
			deq.erase(deq.cbegin(), deq.cbegin() + mid);
			deq.erase(++deq.cbegin(), deq.cbegin() + mid);
		}
		deq.erase(deq.cbegin());
	}
	{
		auto first{ref.cbegin()};
		auto last{ref.cend()};
		ref.erase(first, last);
	}

	if (!::std::equal(deq.cbegin(), deq.cend(), ref.cbegin(), ref.cend()))
	{
		__builtin_trap();
	}
}
} // namespace

int main()
{
	for (::std::uint_least8_t i{}; i != 4u; ++i)
	{
		test_deque(i);
	}
}
