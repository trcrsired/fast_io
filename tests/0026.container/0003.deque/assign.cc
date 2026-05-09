#include <fast_io.h>
#include <fast_io_dsal/deque.h>
#include <vector>
#include <stdexcept>
#include <cassert>

using fast_io::deque;

// A type that throws on copy after N copies
struct ThrowOnCopy
{
	static inline int counter = 0;
	int value;

	ThrowOnCopy(int v = 0) : value(v)
	{}

	ThrowOnCopy(ThrowOnCopy const &other) : value(other.value)
	{
		if (++counter > 3)
		{
			throw std::runtime_error("copy fail");
		}
	}
	ThrowOnCopy &operator=(ThrowOnCopy const &) = default;
};
#if 0
inline void test_assign_basic() {
    deque<int> d;
    d.push_back(1);
    d.push_back(2);
    d.push_back(3);

    d.assign(5, 42);

    assert(d.size() == 5);
    for (auto& x : d) assert(x == 42);
}
#endif
inline void test_assign_range_basic()
{
	deque<int> d;
	std::vector<int> v = {10, 20, 30, 40};

	d.assign_range(v);

	assert(d.size() == 4);
	for (int i = 0; i < 4; ++i)
	{
		assert(d[i] == v[i]);
	}
}
#if 0
inline void test_assign_strong_exception_guarantee() {
    deque<ThrowOnCopy> d;
    d.push_back({1});
    d.push_back({2});
    d.push_back({3});

    auto old = d;  // snapshot

    ThrowOnCopy::counter = 0;

    try {
        d.assign(10, ThrowOnCopy{7});  // will throw after 3 copies
        assert(false); // should not reach
    } catch (...) {
        // strong exception guarantee: unchanged
        assert(d.size() == old.size());
        for (size_t i = 0; i < d.size(); ++i)
            assert(d[i].value == old[i].value);
    }
}
#endif
inline void test_assign_range_exception_guarantee()
{
	deque<ThrowOnCopy> d;
	d.push_back({1});
	d.push_back({2});
	d.push_back({3});

	auto old = d;

	std::vector<ThrowOnCopy> v(10, ThrowOnCopy{9});
	ThrowOnCopy::counter = 0;

	try
	{
		d.assign_range(v); // throws
		assert(false);
	}
	catch (...)
	{
		assert(d.size() == old.size());
		for (size_t i = 0; i < d.size(); ++i)
		{
			assert(d[i].value == old[i].value);
		}
	}
}

inline void test_self_assign()
{
	deque<int> d;
	for (int i = 0; i < 10; ++i)
	{
		d.push_back(i);
	}

	d.assign_range(d); // safe because you use temp

	assert(d.size() == 10);
	for (int i = 0; i < 10; ++i)
	{
		assert(d[i] == i);
	}
}

inline void test_assign_from_subrange()
{
	deque<int> d;
	for (int i = 0; i < 10; ++i)
	{
		d.push_back(i);
	}

	auto first = d.begin() + 2;
	auto last = d.begin() + 7;

	d.assign_range(std::ranges::subrange(first, last));

	assert(d.size() == 5);
	for (int i = 0; i < 5; ++i)
	{
		assert(d[i] == i + 2);
	}
}

int main()
{
#if 0
    test_assign_basic();
#endif
	test_assign_range_basic();
#if 0
    test_assign_strong_exception_guarantee();
    test_assign_range_exception_guarantee();
#endif
	test_self_assign();
	test_assign_from_subrange();

	::fast_io::io::print("All tests passed.\n");
}
