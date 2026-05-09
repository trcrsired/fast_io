#include <fast_io.h>
#include <fast_io_dsal/deque.h>
#include <deque>
#include <cstdint>
#include <vector>

extern "C" int LLVMFuzzerTestOneInput(uint8_t const *data, size_t size)
{
	fast_io::deque<int> fq;
	std::deque<int> sq;

	auto read_u8 = [&](size_t &i) -> uint8_t {
		if (i >= size)
		{
			return 0;
		}
		return data[i++];
	};

	size_t i = 0;
	while (i < size)
	{
		uint8_t op = read_u8(i);

		switch (op % 8)
		{
		case 0:
		{ // push_back
			int v = read_u8(i);
			fq.push_back(v);
			sq.push_back(v);
			break;
		}
		case 1:
		{ // push_front
			int v = read_u8(i);
			fq.push_front(v);
			sq.push_front(v);
			break;
		}
		case 2:
		{ // reserve_back
			size_t n = static_cast<size_t>(read_u8(i)) * 256;
			fq.reserve_back(n);

			// capacity checks
			if (fq.back_capacity() < n)
			{
				__builtin_trap();
			}
			break;
		}
		case 3:
		{ // reserve_front
			size_t n = static_cast<size_t>(read_u8(i)) * 256;
			fq.reserve_front(n);

			if (fq.front_capacity() < n)
			{
				__builtin_trap();
			}
			break;
		}
		case 4:
		{ // resize
			size_t n = read_u8(i);
			fq.resize(n);
			sq.resize(n);
			break;
		}
		case 5:
		{ // resize overwrite
			size_t n = read_u8(i);
			int v = read_u8(i);
			fq.resize(n, v);
			sq.resize(n, v);
			break;
		}
		case 6:
		{ // assign_range
			size_t n = read_u8(i);
			std::vector<int> tmp(n);
			for (size_t k = 0; k < n; ++k)
			{
				tmp[k] = read_u8(i);
			}

			fq.assign_range(tmp);
			sq.assign(tmp.begin(), tmp.end());
			break;
		}
		case 7:
		{ // pop ops
			if (!fq.empty())
			{
				fq.pop_back();
				sq.pop_back();
			}
			if (!fq.empty())
			{
				fq.pop_front();
				sq.pop_front();
			}
			break;
		}
		}

		// Validate element correctness
		if (fq.size() != sq.size())
		{
			__builtin_trap();
		}

		for (size_t k = 0; k < fq.size(); ++k)
		{
			if (fq[k] != sq[k])
			{
				__builtin_trap();
			}
		}

		// capacity must always be >= size
		if (fq.capacity() < fq.size())
		{
			__builtin_trap();
		}
	}

	return 0;
}
