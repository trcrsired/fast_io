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

extern "C" int LLVMFuzzerTestOneInput(uint8_t const *data, size_t size)
{
	fast_io::deque<std::size_t> dq;
	std::deque<std::size_t> ref;

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
			if (dq[k] != ref[k])
			{
				__builtin_trap();
			}
		}
		if (!ref.empty())
		{
			if (dq.front() != ref.front())
			{
				__builtin_trap();
			}
			if (dq.back() != ref.back())
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
		{ // push_back
			std::size_t v = read_u64(i);
			dq.push_back(v);
			ref.push_back(v);
			break;
		}

		case 1:
		{ // push_front
			std::size_t v = read_u64(i);
			dq.push_front(v);
			ref.push_front(v);
			break;
		}

		case 2:
		{ // pop_back
			if (!ref.empty())
			{
				dq.pop_back();
				ref.pop_back();
			}
			break;
		}

		case 3:
		{ // pop_front
			if (!ref.empty())
			{
				dq.pop_front();
				ref.pop_front();
			}
			break;
		}

		case 4:
		{ // emplace_back
			std::size_t v = read_u64(i);
			dq.emplace_back(v);
			ref.emplace_back(v);
			break;
		}

		case 5:
		{ // emplace_front
			std::size_t v = read_u64(i);
			dq.emplace_front(v);
			ref.emplace_front(v);
			break;
		}

		case 6:
		{ // insert (iterator)
			std::size_t v = read_u64(i);
			if (ref.empty())
			{
				auto it = dq.insert(dq.cbegin(), v);
				(void)it;
				ref.insert(ref.begin(), v);
			}
			else
			{
				std::size_t pos = (v * 37) % (ref.size() + 1);
				dq.insert(dq.cbegin() + pos, v);
				ref.insert(ref.begin() + pos, v);
			}
			break;
		}

		case 7:
		{ // insert_index
			std::size_t v = read_u64(i);
			if (ref.empty())
			{
				dq.insert_index(0, v);
				ref.insert(ref.begin(), v);
			}
			else
			{
				std::size_t pos = (v * 37) % (ref.size() + 1);
				dq.insert_index(pos, v);
				ref.insert(ref.begin() + pos, v);
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
		{ // resize default
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
		{ // resize with value
			std::size_t n = read_u64(i);
			std::size_t v = read_u64(i);
			if (n > 100000)
			{
				n = n % 100001;
			}
			dq.resize(n, v);
			ref.resize(n, v);
			break;
		}

		case 12:
		{ // assign(count, val)
			std::size_t n = read_u64(i);
			std::size_t v = read_u64(i);
			if (n > 100000)
			{
				n = n % 100001;
			}
			dq.assign(n, v);
			ref.assign(n, v);
			break;
		}

		case 13:
		{ // assign_range
			std::size_t n = read_u8(i);
			std::vector<std::size_t> tmp(n);
			for (std::size_t k = 0; k != n; ++k)
			{
				tmp[k] = read_u64(i);
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
			fast_io::deque<std::size_t> other(n);
			std::deque<std::size_t> other_ref(n);
			for (std::size_t k = 0; k != n; ++k)
			{
				other[k] = read_u64(i);
				other_ref[k] = other[k];
			}
			dq.swap(other);
			std::swap(ref, other_ref);
			break;
		}

		case 17:
		{ // append_range
			std::size_t n = read_u8(i);
			std::vector<std::size_t> tmp(n);
			for (std::size_t k = 0; k != n; ++k)
			{
				tmp[k] = read_u64(i);
			}
			dq.append_range(tmp);
			ref.insert(ref.end(), tmp.begin(), tmp.end());
			break;
		}

		case 18:
		{ // prepend_range
			std::size_t n = read_u8(i);
			std::vector<std::size_t> tmp(n);
			for (std::size_t k = 0; k != n; ++k)
			{
				tmp[k] = read_u64(i);
			}
			dq.prepend_range(tmp);
			ref.insert(ref.begin(), tmp.begin(), tmp.end());
			break;
		}

		case 19:
		{ // emplace_index
			if (ref.empty())
			{
				std::size_t v = read_u64(i);
				dq.emplace_index(0, v);
				ref.emplace(ref.begin(), v);
			}
			else
			{
				std::size_t seed = read_u8(i);
				std::size_t v = read_u64(i);
				std::size_t pos = seed % ref.size();
				dq.emplace_index(pos, v);
				ref.emplace(ref.begin() + pos, v);
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
		for (;
			 it1 != e1 && it2 != e2; ++it1, ++it2)
		{
			if (*it1 != *it2)
			{
				__builtin_trap();
			}
		}
	}

	return 0;
}
