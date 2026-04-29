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

		// 4 operations: insert counts, erase, etc.
		uint8_t op = b & 0x3u;

		// Position: [0, size]
		std::size_t pos =
			dq.size() == 0 ? 0 : (static_cast<std::size_t>(b) * 131u) % (dq.size() + 1);

		// Count: small to avoid explosion
		std::size_t count = (b >> 2) % 4; // 0–3
		if (count == 0)
		{
			count = 1; // avoid no‑op
		}

		std::size_t value = i * 2654435761ull;

		switch (op)
		{
		case 0: // insert_index(count, value)
		{
			dq.insert_index(pos, count, value);
			ref.insert(ref.begin() + pos, count, value);
			break;
		}

		case 1: // insert(iterator, count, value)
		{
			auto it = dq.insert(dq.cbegin() + pos, count, value);
			(void)it;
			ref.insert(ref.begin() + pos, count, value);
			break;
		}

		case 2: // erase single element
		{
			if (!ref.empty())
			{
				std::size_t p = pos % ref.size();
				dq.erase_index(p);
				ref.erase(ref.begin() + p);
			}
			break;
		}

		case 3: // mix: insert_index with larger count
		{
			std::size_t big_count = (b % 5) + 1; // 1–5
			dq.insert_index(pos, big_count, value ^ 0x9E3779B97F4A7C15ull);
			ref.insert(ref.begin() + pos, big_count,
					   value ^ 0x9E3779B97F4A7C15ull);
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
