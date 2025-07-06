#include<fast_io_dsal/vector.h>
#include<fast_io_dsal/deque.h>
#include<fast_io.h>

#include <cassert>
#include <ranges>
#include <vector>
#include <deque>

#if !defined(__cpp_size_t_suffix) || __cpp_size_t_suffix < 202011L
// make IntelliSence happy
// _uz is a reserved suffix, but this is for a test file and IntelliSence.
// A production-safe version would be operator"" uz, without the underscore.
inline constexpr std::size_t operator"" _uz(unsigned long long const value) noexcept {
    return value;
}
#define uz _uz
#else
#define uz uz
#endif

namespace std {
// The from_range_t fallback is no longer needed as we use preprocessor checks.

#ifndef __cpp_lib_ranges_enumerate
    // --- Fallback for C++20/17 ---
    // This is a generator function, not a view.
    template<ranges::range R>
    auto enumerate(R &&r) {
        using ValueType = ranges::range_value_t<R>;
        vector<pair<size_t, ValueType> > result;

        if constexpr (ranges::sized_range<R>) {
            result.reserve(ranges::size(r));
        }

        size_t index = 0;
        for (const auto &value: r) {
            result.emplace_back(index++, value);
        }
        return result;
    }
#endif
}


template<std::size_t Size>
class ele {
    unsigned char data[Size]{};

public:
    ele() = default;

    ele(const std::size_t num) {
        data[0] = static_cast<unsigned char>(num);
    }

    ele(ele const &other) = default;

    ele &operator=(ele const &other) = default;

    ~ele() = default;

    bool operator==(const std::size_t num) const {
        return data[0] == static_cast<unsigned char>(num);
    }
};

template<typename deque>
void test_constructor(std::size_t count = 1000uz) {
    using namespace std::ranges; {
        // (1) zero construct
        {
            deque d{};
            assert(d.size() == 0uz);
            assert(d.empty());
        }
        // (2) skip
        // (3)
        for (auto i = 0uz; i != count; ++i) {
            deque d(i + 1uz);
            assert(d.size() == (i + 1uz));
            assert(!d.empty());
            for (auto [idx, v]: std::enumerate(d)) {
                assert(d[idx] == 0uz);
                assert(v == 0uz);
            }
            d.clear();
            assert(d.empty());
        }
        // (4)
        for (auto i = 0uz; i != count; ++i) {
            deque d(i + 1uz, 0x7Euz);
            assert(d.size() == (i + 1uz));
            for (auto [idx, v]: std::enumerate(d)) {
                assert(d[idx] == 0x7Euz);
                assert(v == 0x7Euz);
            }
        }
        // (5)
        for (auto i = 0uz; i != count; ++i) {
            std::vector<typename deque::value_type> v(i + 1uz, 0x7Euz);
            deque d(v.begin(), v.end());
            assert(d.size() == (i + 1uz));
            deque d1(d.begin(), d.end());
            assert(d1.size() == (i + 1uz));
            for (auto [idx, v]: std::enumerate(d)) {
                assert(d[idx] == 0x7Euz);
                assert(v == 0x7Euz);
            }
            for (auto [idx, v]: std::enumerate(d1)) {
                assert(d1[idx] == 0x7Euz);
                assert(v == 0x7Euz);
            }
            // forward/bidirectional iterator variant are equivalent to emplace_back
        }
        // (6) equivalent to (4), (5) and operator= (1)
        {
#if __cplusplus >= 202302L
            std::vector<typename deque::value_type> v{std::from_range, std::ranges::iota_view(0uz, count)};
            deque d(std::from_range, v);
            assert(d.size() == count);
            deque d1(std::from_range, std::ranges::subrange(v.begin(), v.end()));
            assert(d1.size() == count);
#else
            auto iota_r = std::ranges::iota_view(0uz, count);
            std::vector<typename deque::value_type> v(iota_r.begin(), iota_r.end());
            deque d(v.begin(), v.end());
            assert(d.size() == count);

            auto sub_r = std::ranges::subrange(v.begin(), v.end());
            deque d1(sub_r.begin(), sub_r.end());
            assert(d1.size() == count);
#endif
        }
        // (7) equivalent to (5)
        {
            deque d{};
            auto d1{d};
        }
        // (8) equivalent to swap
        {
            deque d{};
            auto d1{std::move(d)};
        }
        // (9, 10) not support
        // (11) equivalent to (5.1)
        {
            deque d{1uz, 2uz, 3uz, 4uz};
        }
    }
}

template<typename deque>
void test_operator_assign(std::size_t count = 1000uz) {
    using namespace std::ranges;
    // (1) equivalent to copy constructor (7)
    {
        for (auto i = 0uz; i != count; ++i) {
#if __cplusplus >= 202302L
            deque d(std::from_range, std::ranges::iota_view(0uz, i + 1uz));
#else
            auto iota_r = std::ranges::iota_view(0uz, i + 1uz);
            deque d(iota_r.begin(), iota_r.end());
#endif

            deque d1(100uz);
            d1 = d;
            assert(d.size() == (i + 1uz));
            for (auto [idx, v]: std::enumerate(d1)) {
                assert(d[idx] == idx);
                assert(v == idx);
            }
        }
    }
    // (2) equivalent to swap
    {
        deque d(100uz);
        deque d1{};
        d1 = d;
        assert(d1.size() == 100uz);
        d = d1;
        assert(d.size() == 100uz);
    }
    // (3) equivalent to (1) and constructor (5.1)
    {
        deque d(100uz);
        d = {0uz, 1uz, 2uz, 3uz};
        assert(d.size() == 4uz);
    }
}

// assign
template<typename deque>
void test_assign(std::size_t count = 1000uz) {
    using namespace std::ranges;
    // (1) equivalent to constructor (4)
    {
        deque d(100uz);
        d.assign(100uz, 1uz);
        assert(d.size() == 100uz);
    }
    // (2) equivalent to constructor (5)
    {
        deque d(100uz);
        deque d1(100uz);
        d1.assign(d.begin(), d.end());
        assert(d1.size() == 100uz);
    }
    // (3) equivalent to constructor (11)
    {
        deque d(100uz);
        deque d1(100uz);
        d1.assign({0uz, 1uz, 2uz, 3uz});
        assert(d1.size() == 4uz);
    }
}

// assign_range
template<typename deque>
void test_assign_range(std::size_t count = 1000uz) {
    deque d{};
    auto ilist = {0uz, 1uz, 2uz, 3uz};
#if __cplusplus >= 202302L
    d.assign_range(ilist);
#else
    d.assign(ilist.begin(), ilist.end());
#endif
    assert(d.size() == 4uz);
}

// operator at tests in other tests above
// at equivalent to operator at

// front/back never fail
template<typename deque>
void test_front_back(std::size_t count = 1000uz) {
    deque d(1uz);
    auto &&ignore = d.front();
    auto &&ignore1 = d.back();
    deque const &d1 = d;
    auto &&ignore2 = d1.front();
    auto &&ignore3 = d1.back();
}

// size/empty tests in constructor's above and others

// shrink_to_fit never fail
template<typename deque>
void test_shrink(std::size_t count = 1000uz) {
    deque d{};
    d.shrink_to_fit();
}

// clear tests in constructor (3) and others
template<typename deque>
void test_clear(std::size_t count = 1000uz) {
    deque d{100uz};
    d.clear();
    assert(d.empty());
}

// todo: emplace、insert、insert_range、emplace、erase

template<typename deque>
void test_emplace_back(std::size_t count = 1000uz) {
    using namespace std::ranges; {
        for (auto i = 0uz; i != count; ++i) {
            deque d{};
            for (auto j = 0uz; j != i; ++j) {
                d.emplace_back(j);
                assert(d[j] == j);
                assert(d.size() == (j + 1uz));
            }
            for (auto j = 0uz; j != i; ++j) {
                auto const size = i - j;
                assert(d.size() == size);
                assert(d[size - 1uz] == size - 1uz);
                if (i) {
                    d.pop_back();
                }
            }
        }
        for (auto i = 0uz; i != count; ++i) {
            auto const half_head = (i + 1uz) / 2uz;
#if  __cplusplus >= 202302L
            deque d{std::from_range, std::ranges::iota_view(0uz, half_head)};
#else
            auto iota_r = std::ranges::iota_view(0uz, half_head);
            deque d(iota_r.begin(), iota_r.end());
#endif

            for (auto j = half_head; j != i; ++j) {
                d.emplace_back(j);
                assert(d.size() == (j + 1uz));
                assert(d[j] == j);
            }
            for (auto j = 0uz; j != i; ++j) {
                auto const size = i - j;
                auto si = d.size();
                assert(d.size() == size);
                assert(d[0] == j);
                if (i) {
                    d.pop_front();
                }
            }
        }
    }
}

template<typename deque>
void test_emplace_front(const std::size_t count = 1000uz) {
    using namespace std::ranges; {
        for (auto i = 0uz; i != count; ++i) {
            deque d{};
            for (auto j = 0uz; j != i; ++j) {
                d.emplace_front(i - j - 1uz);
                assert(d[0uz] == i - j - 1uz);
                assert(d.size() == (j + 1uz));
            }
            for (auto [idx, v]: std::enumerate(d)) {
                assert(d[idx] == idx);
                assert(v == idx);
            }
            for (auto j = 0uz; j != i; ++j) {
                auto const size = i - j;
                assert(d.size() == size);
                assert(d[size - 1uz] == size - 1uz);
                if (i) {
                    d.pop_back();
                }
            }
        }
        for (auto i = 0uz; i != count; ++i) {
            auto const half_head = (i + 1uz) / 2uz;
#if __cplusplus >= 202302L
            deque d{std::from_range, std::ranges::iota_view(half_head, i)};
#else
            auto iota_r = std::ranges::iota_view(half_head, i);
            deque d(iota_r.begin(), iota_r.end());
#endif

            for (auto j = 0; j != half_head; ++j) {
                d.emplace_front(half_head - j - 1uz);
                assert(d.size() == ((i - half_head) + j + 1uz));
                assert(d[0uz] == half_head - j - 1uz);
            }
            for (auto [idx, v]: std::enumerate(d)) {
                assert(d[idx] == idx);
                assert(v == idx);
            }
            for (auto j = 0uz; j != i; ++j) {
                auto const size = i - j;
                auto si = d.size();
                assert(d.size() == size);
                assert(d[0] == j);
                if (i) {
                    d.pop_front();
                }
            }
        }
    }
}

// pop_back/pop_front tests in emplace_back/emplace_front

template<typename deque>
void test_prep_app_end_range(std::size_t count = 1000uz) {
    // equivlent to emplace_back/emplace_front
    {
        deque d{};
        auto range_to_append = std::views::iota(0uz, 100uz);
#if __cplusplus >= 202302L
        d.append_range(range_to_append);
#else
        d.insert(d.end(), range_to_append.begin(), range_to_append.end());
#endif
        assert(d.size() == 100uz);
    } {
        deque d{};
        auto range_to_prepend = std::views::iota(0uz, 100uz);
#if __cplusplus >= 202302L
        d.prepend_range(range_to_prepend);
#else
        d.insert(d.begin(), range_to_prepend.begin(), range_to_prepend.end());
#endif
        assert(d.size() == 100uz);
    }
}

template<typename deque>
void test_resize(std::size_t count = 1000uz) {
    {
        deque d{};
        d.resize(100uz);
        assert(d.size() == 100uz);
        d.resize(0uz);
        assert(d.size() == 0uz);
        d.resize(100uz, typename deque::value_type(0uz));
        assert(d.size() == 100uz);
    }
}

template<typename deque>
void test_emplace_insert(std::size_t count = 1000uz) { {
        deque d;
        d.emplace(d.begin());
        assert(d.size() == 1uz);
        assert(d[0uz] == 0uz);
        d.emplace(d.end(), 5uz);
        assert(d.size() == 2uz);
        assert(d[1uz] == 5uz);
        d.emplace(d.begin() + 1uz, 1uz);
        assert(d.size() == 3uz);
        assert(d[1uz] == 1uz);
        d.emplace(d.begin() + 2uz, 4uz);
        assert(d.size() == 4uz);
        assert(d[2uz] == 4uz);
        d.emplace(d.begin() + 2uz, 3uz);
        assert(d.size() == 5uz);
        assert(d[2uz] == 3uz);
        d.emplace(d.begin() + 2uz, 2uz);
        assert(d.size() == 6uz);
        assert(d[2uz] == 2uz);
    } {
        deque d;
        d.insert(d.begin(), typename deque::value_type{});
    } {
        deque d;
        typename deque::value_type v{};
        d.insert(d.begin(), v);
    }
}

template<typename Type>
void test_all(std::size_t count = 1000uz) {
    test_constructor<std::deque<Type> >(count);
    test_operator_assign<std::deque<Type> >(count);
    test_assign<std::deque<Type> >(count);
    test_assign_range<std::deque<Type> >(count);
    test_front_back<std::deque<Type> >(count);
    test_shrink<std::deque<Type> >(count);
    test_clear<std::deque<Type> >(count);
    test_emplace_back<std::deque<Type> >(count);
    test_emplace_front<std::deque<Type> >(count);
    test_prep_app_end_range<std::deque<Type> >(count);
    test_resize<std::deque<Type> >(count);
    test_emplace_insert<std::deque<Type> >(count);
    using namespace fast_io;
    test_constructor<deque<Type> >(count);
    test_operator_assign<deque<Type> >(count);
    test_assign<deque<Type> >(count);
    test_assign_range<deque<Type> >(count);
    test_front_back<deque<Type> >(count);
    test_shrink<deque<Type> >(count);
    test_clear<deque<Type> >(count);
    test_emplace_back<deque<Type> >(count);
    test_emplace_front<deque<Type> >(count);
    test_prep_app_end_range<deque<Type> >(count);
    test_resize<deque<Type> >(count);
    test_emplace_insert<deque<Type> >(count);
}

struct metaindex
{
	::std::size_t modulepos{SIZE_MAX},moduleroutinepos{SIZE_MAX};
};

int main()
{
	::fast_io::vector<::fast_io::vector<metaindex>> vec;
	vec.emplace_back(30);

	auto& vec20{vec[0]};
	vec20.push_back(metaindex{20,30});

    test_all<ele<1uz> >();
    test_all<ele<2uz> >();
    test_all<ele<3uz> >();
    test_all<ele<4uz> >();
    test_all<ele<5uz> >();
    test_all<ele<6uz> >();
    test_all<ele<7uz> >();
    test_all<ele<8uz> >();
    test_all<ele<9uz> >();
}
