#include <fast_io.h>
#include <fast_io_dsal/deque.h>
#include <deque>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <ranges>

extern "C" int LLVMFuzzerTestOneInput(uint8_t const *data, size_t size)
{
	::fast_io::deque<std::size_t> dq;
	std::deque<std::size_t> ref;

	for (size_t i{}; i != size; ++i)
	{
		uint8_t b = data[i];

		// 4 operations: insert single at various positions
		uint8_t op = b & 0x3u;

		// Position: [0, size]
		std::size_t pos = dq.size() == 0 ? 0 : (static_cast<std::size_t>(b) * 37u) % (dq.size() + 1);

		std::size_t value = i * 1315423911ull;

		switch (op)
		{
		case 0: // insert_index (single element)
		{
			dq.insert_index(pos, value);
			ref.insert(ref.begin() + pos, value);
			break;
		}

		case 1: // insert using iterator (single element)
		{
			auto it = dq.insert(dq.cbegin() + pos, value);
			(void)it;
			ref.insert(ref.begin() + pos, value);
			break;
		}

		case 2: // erase single element (to keep sizes bounded)
		{
			if (!ref.empty())
			{
				std::size_t p = pos % ref.size();
				dq.erase_index(p);
				ref.erase(ref.begin() + p);
			}
			break;
		}

		case 3: // emplace_index (single element)
		{
			dq.emplace_index(pos, value);
			ref.emplace(ref.begin() + pos, value);
			break;
		}
		}

		// Validate correctness
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