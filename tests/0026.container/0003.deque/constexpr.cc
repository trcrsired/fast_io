#include <fast_io.h>
#include <fast_io_dsal/deque.h>
#include <utility>

namespace
{

// --- Test 1: basic push_back and iteration ---
constexpr bool test_push_back()
{
	::fast_io::deque<int> dq;
	dq.push_back(1);
	dq.push_back(2);
	dq.push_back(3);
	if (dq.size() != 3)
		return false;
	if (dq[0] != 1 || dq[1] != 2 || dq[2] != 3)
		return false;
	return true;
}
static_assert(test_push_back());

// --- Test 2: push_front and random access ---
constexpr bool test_push_front()
{
	::fast_io::deque<int> dq;
	dq.push_front(10);
	dq.push_front(20);
	dq.push_front(30);
	if (dq.size() != 3)
		return false;
	if (dq[0] != 30 || dq[1] != 20 || dq[2] != 10)
		return false;
	return true;
}
static_assert(test_push_front());

// --- Test 3: pop_back ---
constexpr bool test_pop_back()
{
	::fast_io::deque<int> dq;
	dq.push_back(1);
	dq.push_back(2);
	dq.push_back(3);
	dq.pop_back();
	if (dq.size() != 2)
		return false;
	if (dq[0] != 1 || dq[1] != 2)
		return false;
	dq.pop_back();
	if (dq.size() != 1)
		return false;
	if (dq[0] != 1)
		return false;
	dq.pop_back();
	if (!dq.is_empty())
		return false;
	return true;
}
static_assert(test_pop_back());

// --- Test 4: pop_front ---
constexpr bool test_pop_front()
{
	::fast_io::deque<int> dq;
	dq.push_back(1);
	dq.push_back(2);
	dq.push_back(3);
	dq.pop_front();
	if (dq.size() != 2)
		return false;
	if (dq[0] != 2 || dq[1] != 3)
		return false;
	dq.pop_front();
	if (dq.size() != 1 || dq[0] != 3)
		return false;
	dq.pop_front();
	if (!dq.is_empty())
		return false;
	return true;
}
static_assert(test_pop_front());

// --- Test 5: front() and back() ---
constexpr bool test_front_back()
{
	::fast_io::deque<int> dq;
	dq.push_back(10);
	dq.push_back(20);
	dq.push_back(30);
	if (dq.front() != 10 || dq.back() != 30)
		return false;
	dq.front() = 100;
	dq.back() = 300;
	if (dq[0] != 100 || dq[2] != 300)
		return false;
	return true;
}
static_assert(test_front_back());

// --- Test 6: clear() and reuse ---
constexpr bool test_clear()
{
	::fast_io::deque<int> dq;
	dq.push_back(1);
	dq.push_back(2);
	dq.push_back(3);
	dq.clear();
	if (!dq.is_empty() || dq.size() != 0)
		return false;
	dq.push_back(42);
	if (dq.size() != 1 || dq[0] != 42)
		return false;
	return true;
}
static_assert(test_clear());

// --- Test 7: copy construction ---
constexpr bool test_copy_construct()
{
	::fast_io::deque<int> dq;
	dq.push_back(1);
	dq.push_back(2);
	dq.push_back(3);
	::fast_io::deque<int> dq2(dq);
	if (dq2.size() != 3)
		return false;
	if (dq2[0] != 1 || dq2[1] != 2 || dq2[2] != 3)
		return false;
	if (dq.size() != 3)
		return false;
	if (dq[0] != 1 || dq[1] != 2 || dq[2] != 3)
		return false;
	return true;
}
static_assert(test_copy_construct());

// --- Test 8: move construction ---
constexpr bool test_move_construct()
{
	::fast_io::deque<int> dq;
	dq.push_back(1);
	dq.push_back(2);
	dq.push_back(3);
	::fast_io::deque<int> dq2(::std::move(dq));
	if (dq2.size() != 3)
		return false;
	if (dq2[0] != 1 || dq2[1] != 2 || dq2[2] != 3)
		return false;
	return true;
}
static_assert(test_move_construct());

// --- Test 9: push_back sufficient elements to trigger growth ---
constexpr bool test_push_back_many()
{
	::fast_io::deque<int> dq;
	// push enough to force multiple block allocations
	for (int i{}; i != 5000; ++i)
	{
		dq.push_back(i);
	}
	if (dq.size() != 5000)
		return false;
	for (int i{}; i != 5000; ++i)
	{
		if (dq[i] != i)
			return false;
	}
	return true;
}
static_assert(test_push_back_many());

// --- Test 10: mixed push_front and push_back ---
constexpr bool test_mixed_push()
{
	::fast_io::deque<int> dq;
	dq.push_back(2);
	dq.push_front(1);
	dq.push_back(3);
	dq.push_front(0);
	if (dq.size() != 4)
		return false;
	if (dq[0] != 0 || dq[1] != 1 || dq[2] != 2 || dq[3] != 3)
		return false;
	return true;
}
static_assert(test_mixed_push());

// --- Test 11: copy assignment ---
constexpr bool test_copy_assign()
{
	::fast_io::deque<int> dq;
	dq.push_back(1);
	dq.push_back(2);
	dq.push_back(3);
	::fast_io::deque<int> dq2;
	dq2.push_back(99);
	dq2 = dq;
	if (dq2.size() != 3)
		return false;
	if (dq2[0] != 1 || dq2[1] != 2 || dq2[2] != 3)
		return false;
	return true;
}
static_assert(test_copy_assign());

// --- Test 12: move assignment ---
constexpr bool test_move_assign()
{
	::fast_io::deque<int> dq;
	dq.push_back(1);
	dq.push_back(2);
	dq.push_back(3);
	::fast_io::deque<int> dq2;
	dq2.push_back(99);
	dq2 = ::std::move(dq);
	if (dq2.size() != 3)
		return false;
	if (dq2[0] != 1 || dq2[1] != 2 || dq2[2] != 3)
		return false;
	return true;
}
static_assert(test_move_assign());

// --- Test 13: swap ---
constexpr bool test_swap()
{
	::fast_io::deque<int> dq1;
	dq1.push_back(1);
	dq1.push_back(2);
	::fast_io::deque<int> dq2;
	dq2.push_back(10);
	dq2.push_back(20);
	dq2.push_back(30);
	swap(dq1, dq2);
	if (dq1.size() != 3 || dq2.size() != 2)
		return false;
	if (dq1[0] != 10 || dq1[1] != 20 || dq1[2] != 30)
		return false;
	if (dq2[0] != 1 || dq2[1] != 2)
		return false;
	return true;
}
static_assert(test_swap());

// --- Test 14: iterator operations ---
constexpr bool test_iterators()
{
	::fast_io::deque<int> dq;
	dq.push_back(10);
	dq.push_back(20);
	dq.push_back(30);
	auto it = dq.begin();
	if (*it != 10)
		return false;
	++it;
	if (*it != 20)
		return false;
	it += 1;
	if (*it != 30)
		return false;
	--it;
	if (*it != 20)
		return false;
	if (dq.end() - dq.begin() != 3)
		return false;
	auto rit = dq.rbegin();
	if (*rit != 30)
		return false;
	return true;
}
static_assert(test_iterators());

} // namespace

int main()
{
	return 0;
}
