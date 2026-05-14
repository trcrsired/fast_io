#include <fast_io_dsal/deque.h>
#include <deque>
#include <cstddef>
#include <cstdint>
#include <vector>

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

	size_t i = 0;
	while (i < size)
	{
		uint8_t op = read_u8(i);

		switch (op % 8u)
		{
		case 0:
		{ // assign(count, val)
			std::size_t n = read_u64(i);
			std::size_t v = read_u64(i);
			// Cap n to avoid OOM
			if (n > 1000000)
			{
				n = n % 1000001;
			}
			dq.assign(n, v);
			ref.assign(n, v);
			break;
		}

		case 1:
		{ // assign_range from vector
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

		case 2:
		{ // assign after growth: push_back then assign
			std::size_t pushes = read_u8(i);
			for (std::size_t k = 0; k != pushes; ++k)
			{
				dq.push_back(k);
				ref.push_back(k);
			}
			std::size_t n = read_u64(i);
			std::size_t v = read_u64(i);
			if (n > 1000000)
			{
				n = n % 1000001;
			}
			dq.assign(n, v);
			ref.assign(n, v);
			break;
		}

		case 3:
		{ // assign(0, val) — clear via assign
			std::size_t v = read_u64(i);
			dq.assign(0, v);
			ref.assign(0, v);
			break;
		}

		case 4:
		{ // assign_range from empty range
			std::vector<std::size_t> tmp;
			dq.assign_range(tmp);
			ref.assign(tmp.begin(), tmp.end());
			break;
		}

		case 5:
		{ // assign after push_front (tests front block)
			std::size_t pushes = read_u8(i);
			for (std::size_t k = 0; k != pushes; ++k)
			{
				dq.push_front(k * 7);
				ref.push_front(k * 7);
			}
			std::size_t n = read_u64(i);
			std::size_t v = read_u64(i);
			if (n > 1000000)
			{
				n = n % 1000001;
			}
			dq.assign(n, v);
			ref.assign(n, v);
			break;
		}

		case 6:
		{ // assign_range from std::deque
			std::size_t n = read_u8(i);
			std::deque<std::size_t> src(n);
			for (std::size_t k = 0; k != n; ++k)
			{
				src[k] = read_u64(i);
			}
			dq.assign_range(src);
			ref.assign(src.begin(), src.end());
			break;
		}

		case 7:
		{ // repeated small assigns (stress re-allocation)
			std::size_t rounds = read_u8(i);
			for (std::size_t r = 0; r != rounds; ++r)
			{
				std::size_t n = read_u8(i);
				std::size_t v = read_u64(i);
				dq.assign(n, v);
				ref.assign(n, v);
			}
			break;
		}
		}

		// Validate
		if (dq.size() != ref.size())
		{
			__builtin_trap();
		}

		if (!std::ranges::equal(dq, ref))
		{
			__builtin_trap();
		}
	}

	return 0;
}
