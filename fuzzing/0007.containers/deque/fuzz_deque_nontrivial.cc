#include <fast_io.h>
#if FAST_IO_FUZZ_DEBUG != 0
#include <fast_io_dsal/impl/debug/deque.h>
#endif
#include <fast_io_dsal/deque.h>
#include <deque>
#include <cstddef>
#include <cstdint>
#include <vector>
#include <algorithm>
#include <memory>

using TestType = ::std::shared_ptr<std::size_t>;

extern "C" int LLVMFuzzerTestOneInput(uint8_t const *data, size_t size)
{
	fast_io::deque<TestType> dq;
	std::deque<TestType> ref;

	auto read_u8 = [&](size_t &i) -> uint8_t {
		if (i >= size)
		{
			return 0;
		}
		return data[i++];
	};

	auto read_u64 = [&](size_t &i) -> std::size_t {
		if (i + 7 >= size)
		{
			return 0;
		}
		std::size_t v{};
		for (int k = 0; k < 8; ++k)
		{
			v |= static_cast<std::size_t>(data[i++]) << (k * 8);
		}
		return v;
	};

	auto validate = [&]() {
		if (dq.size() != ref.size())
		{
			__builtin_trap();
		}
		if (dq.empty() != ref.empty())
		{
			__builtin_trap();
		}
		for (std::size_t k = 0, sz = dq.size(); k != sz; ++k)
		{
			if ((dq[k] == nullptr) != (ref[k] == nullptr))
			{
				__builtin_trap();
			}
			if (dq[k] && ref[k] && *dq[k] != *ref[k])
			{
				__builtin_trap();
			}
		}
		if (!ref.empty())
		{
			if ((dq.front() == nullptr) != (ref.front() == nullptr))
			{
				__builtin_trap();
			}
			if (dq.front() && ref.front() && *dq.front() != *ref.front())
			{
				__builtin_trap();
			}
			if ((dq.back() == nullptr) != (ref.back() == nullptr))
			{
				__builtin_trap();
			}
			if (dq.back() && ref.back() && *dq.back() != *ref.back())
			{
				__builtin_trap();
			}
		}
	};

	size_t i = 0;
	while (i < size)
	{
		uint8_t op = read_u8(i);
#if FAST_IO_FUZZ_DEBUG != 0
		::fast_io::io::debug_perrln(::std::source_location::current(), " op%20=", op % 20, " ", ::fast_io::mnp::debug_view(dq));
#endif
		switch (op % 20u)
		{
		case 0:
		{ // push_back (Copy construction via shared_ptr)
			std::size_t val = read_u64(i);
			auto ptr = std::make_shared<std::size_t>(val);
			dq.push_back(ptr); // Tests copy
			ref.push_back(ptr);
			break;
		}

		case 1:
		{ // push_front (Copy construction)
			std::size_t val = read_u64(i);
			auto ptr = std::make_shared<std::size_t>(val);
			dq.push_front(ptr);
			ref.push_front(ptr);
			break;
		}

		case 2:
		{ // pop_back (Tests destruction / reference count decrement)
			if (!ref.empty())
			{
				dq.pop_back();
				ref.pop_back();
			}
			break;
		}

		case 3:
		{ // pop_front (Tests destruction / reference count decrement)
			if (!ref.empty())
			{
				dq.pop_front();
				ref.pop_front();
			}
			break;
		}

		case 4:
		{ // emplace_back (Tests forwarding / move mechanics)
			std::size_t val = read_u64(i);
			dq.emplace_back(std::make_shared<std::size_t>(val));
			ref.emplace_back(std::make_shared<std::size_t>(val));
			break;
		}

		case 5:
		{ // emplace_front (Tests forwarding / move mechanics)
			std::size_t val = read_u64(i);
			dq.emplace_front(std::make_shared<std::size_t>(val));
			ref.emplace_front(std::make_shared<std::size_t>(val));
			break;
		}

		case 6:
		{ // insert (iterator)
			std::size_t val = read_u64(i);
			auto ptr = std::make_shared<std::size_t>(val);
			if (ref.empty())
			{
				dq.insert(dq.cbegin(), ptr);
				ref.insert(ref.begin(), ptr);
			}
			else
			{
				std::size_t pos = (val * 37) % (ref.size() + 1);
				dq.insert(dq.cbegin() + pos, ptr);
				ref.insert(ref.begin() + pos, ptr);
			}
			break;
		}

		case 7:
		{ // insert_index
			std::size_t val = read_u64(i);
			auto ptr = std::make_shared<std::size_t>(val);
			if (ref.empty())
			{
				dq.insert_index(0, ptr);
				ref.insert(ref.begin(), ptr);
			}
			else
			{
				std::size_t pos = (val * 37) % (ref.size() + 1);
				dq.insert_index(pos, ptr);
				ref.insert(ref.begin() + pos, ptr);
			}
			break;
		}

		case 8:
		{ // erase single index
			if (!ref.empty())
			{
				std::size_t seed = read_u8(i);
				std::size_t pos = seed % ref.size();
				dq.erase_index(pos);
				ref.erase(ref.begin() + pos);
			}
			break;
		}

		case 9:
		{ // erase range index
			if (ref.size() >= 2)
			{
				std::size_t a = read_u8(i);
				std::size_t b = read_u8(i);
				std::size_t lo = a % ref.size();
				std::size_t hi = b % (ref.size() - lo) + lo + 1;
				if (hi > ref.size())
				{
					hi = ref.size();
				}
				dq.erase_index(lo, hi);
				ref.erase(ref.begin() + lo, ref.begin() + hi);
			}
			break;
		}

		case 10:
		{ // resize default (creates null shared_ptrs)
			std::size_t n = read_u64(i);
			if (n > 100000)
			{
				n = n % 100001;
			}
			dq.resize(n);
			ref.resize(n);
			break;
		}

		case 11:
		{ // resize with value (copies prototype value)
			std::size_t n = read_u64(i);
			std::size_t val = read_u64(i);
			if (n > 100000)
			{
				n = n % 100001;
			}
			auto prototype = std::make_shared<std::size_t>(val);
			dq.resize(n, prototype);
			ref.resize(n, prototype);
			break;
		}

		case 12:
		{ // assign(count, val)
			std::size_t n = read_u64(i);
			std::size_t val = read_u64(i);
			if (n > 100000)
			{
				n = n % 100001;
			}
			auto prototype = std::make_shared<std::size_t>(val);
			dq.assign(n, prototype);
			ref.assign(n, prototype);
			break;
		}

		case 13:
		{ // assign_range (Triggers true collection duplication copying)
			std::size_t n = read_u8(i);
			std::vector<TestType> tmp(n);
			for (std::size_t k = 0; k != n; ++k)
			{
				tmp[k] = std::make_shared<std::size_t>(read_u64(i));
			}
			dq.assign_range(tmp);
			ref.assign(tmp.begin(), tmp.end());
			break;
		}

		case 14:
		{ // clear
			dq.clear();
			ref.clear();
			break;
		}

		case 15:
		{ // clear_destroy
			dq.clear_destroy();
			ref.clear();
			break;
		}

		case 16:
		{ // swap with another deque
			std::size_t n = read_u8(i);
			fast_io::deque<TestType> other;
			std::deque<TestType> other_ref;
			for (std::size_t k = 0; k != n; ++k)
			{
				auto ptr = std::make_shared<std::size_t>(read_u64(i));
				other.push_back(ptr);
				other_ref.push_back(ptr);
			}
			dq.swap(other);
			std::swap(ref, other_ref);
			break;
		}

		case 17:
		{ // append_range (Triggers copies from input range)
			std::size_t n = read_u8(i);
			std::vector<TestType> tmp(n);
			for (std::size_t k = 0; k != n; ++k)
			{
				tmp[k] = std::make_shared<std::size_t>(read_u64(i));
			}
			dq.append_range(tmp);
			ref.insert(ref.end(), tmp.begin(), tmp.end());
			break;
		}

		case 18:
		{ // prepend_range (Triggers copies from input range)
			std::size_t n = read_u8(i);
			std::vector<TestType> tmp(n);
			for (std::size_t k = 0; k != n; ++k)
			{
				tmp[k] = std::make_shared<std::size_t>(read_u64(i));
			}
			dq.prepend_range(tmp);
			ref.insert(ref.begin(), tmp.begin(), tmp.end());
			break;
		}

		case 19:
		{ // emplace_index
			if (ref.empty())
			{
				std::size_t val = read_u64(i);
				dq.emplace_index(0, std::make_shared<std::size_t>(val));
				ref.emplace(ref.begin(), std::make_shared<std::size_t>(val));
			}
			else
			{
				std::size_t seed = read_u8(i);
				std::size_t val = read_u64(i);
				std::size_t pos = seed % ref.size();
				dq.emplace_index(pos, std::make_shared<std::size_t>(val));
				ref.emplace(ref.begin() + pos, std::make_shared<std::size_t>(val));
			}
			break;
		}
		}
#if FAST_IO_FUZZ_DEBUG != 0
		::fast_io::io::debug_perrln(::std::source_location::current(), " op%20=", op % 20, " ", ::fast_io::mnp::debug_view(dq));
#endif
		validate();
#if FAST_IO_FUZZ_DEBUG != 0
		::fast_io::io::debug_perrln(::std::source_location::current(), " op%20=", op % 20, " ", ::fast_io::mnp::debug_view(dq));
#endif
	}

	// Final iterator validation
	{
		auto it1 = dq.begin();
		auto it2 = ref.begin();
		auto e1 = dq.end();
		auto e2 = ref.end();
		for (; it1 != e1 && it2 != e2; ++it1, ++it2)
		{
			if ((*it1 == nullptr) != (*it2 == nullptr))
			{
				__builtin_trap();
			}
			if (*it1 && *it2 && **it1 != **it2)
			{
				__builtin_trap();
			}
		}
	}

	return 0;
}
