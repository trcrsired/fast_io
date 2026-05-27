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

		// 0–3 → insert/erase
		// 4–7 → shrink_to_fit
		uint8_t op = b & 0x7u;

		// Position: [0, size]
		std::size_t pos =
			dq.size() == 0 ? 0 : (static_cast<std::size_t>(b) * 131u) % (dq.size() + 1);

		std::size_t value = i * 11400714819323198485ull;

		switch (op)
		{
		case 0: // insert_index
		{
			dq.insert_index(pos, value);
			ref.insert(ref.begin() + pos, value);
			break;
		}

		case 1: // insert(iterator)
		{
			dq.insert(dq.cbegin() + pos, value);
			ref.insert(ref.begin() + pos, value);
			break;
		}

		case 2: // erase single
		{
			if (!ref.empty())
			{
				std::size_t p = pos % ref.size();
				dq.erase_index(p);
				ref.erase(ref.begin() + p);
			}
			break;
		}

		case 3: // emplace_index
		{
			dq.emplace_index(pos, value);
			ref.emplace(ref.begin() + pos, value);
			break;
		}

		case 4: // shrink_to_fit
		case 5:
		case 6:
		case 7:
		{
			dq.shrink_to_fit();
			// std::deque::shrink_to_fit is non-binding but safe to call
			ref.shrink_to_fit();
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
