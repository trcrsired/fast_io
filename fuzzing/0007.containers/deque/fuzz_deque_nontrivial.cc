#include <fast_io_dsal/deque.h>
#include <deque>
#include <cstddef>
#include <cstdint>
#include <vector>
#include <utility>

// Non-trivial type: has destructor, copy constructor, copy assignment operator
struct tracked_object
{
	int value{};
	bool *destroyed{};

	tracked_object() = default;
	explicit tracked_object(int v, bool *flag) : value{v}, destroyed{flag}
	{}
	tracked_object(tracked_object const &other) : value{other.value}, destroyed{other.destroyed}
	{
		if (destroyed && *destroyed)
		{
			__builtin_trap(); // double-destroy / use after destroy
		}
	}
	tracked_object &operator=(tracked_object const &other)
	{
		if (destroyed && *destroyed)
		{
			__builtin_trap();
		}
		if (this != &other)
		{
			value = other.value;
			destroyed = other.destroyed;
		}
		return *this;
	}
	tracked_object(tracked_object &&other) noexcept : value{other.value}, destroyed{other.destroyed}
	{
		other.value = 0;
		other.destroyed = nullptr;
	}
	tracked_object &operator=(tracked_object &&other) noexcept
	{
		if (this != &other)
		{
			value = other.value;
			destroyed = other.destroyed;
			other.value = 0;
			other.destroyed = nullptr;
		}
		return *this;
	}
	~tracked_object()
	{
		if (destroyed && *destroyed)
		{
			__builtin_trap(); // double destroy
		}
		if (destroyed)
		{
			*destroyed = true;
		}
	}
	bool operator==(tracked_object const &other) const noexcept
	{
		return value == other.value;
	}
};

// Same struct for std::deque reference
struct ref_tracked_object
{
	int value{};
	bool *destroyed{};

	ref_tracked_object() = default;
	explicit ref_tracked_object(int v, bool *flag) : value{v}, destroyed{flag}
	{}
	ref_tracked_object(ref_tracked_object const &other) : value{other.value}, destroyed{other.destroyed}
	{
		if (destroyed && *destroyed)
		{
			__builtin_trap();
		}
	}
	ref_tracked_object &operator=(ref_tracked_object const &other)
	{
		if (destroyed && *destroyed)
		{
			__builtin_trap();
		}
		if (this != &other)
		{
			value = other.value;
			destroyed = other.destroyed;
		}
		return *this;
	}
	ref_tracked_object(ref_tracked_object &&other) noexcept : value{other.value}, destroyed{other.destroyed}
	{
		other.value = 0;
		other.destroyed = nullptr;
	}
	ref_tracked_object &operator=(ref_tracked_object &&other) noexcept
	{
		if (this != &other)
		{
			value = other.value;
			destroyed = other.destroyed;
			other.value = 0;
			other.destroyed = nullptr;
		}
		return *this;
	}
	~ref_tracked_object()
	{
		if (destroyed && *destroyed)
		{
			__builtin_trap();
		}
		if (destroyed)
		{
			*destroyed = true;
		}
	}
	bool operator==(ref_tracked_object const &other) const noexcept
	{
		return value == other.value;
	}
};

// Storage for destruction flags so we can verify no double-frees / leaks
static constexpr std::size_t max_tracked = 4096;
static bool destroy_flags_fast[max_tracked];
static bool destroy_flags_ref[max_tracked];

extern "C" int LLVMFuzzerTestOneInput(uint8_t const *data, size_t size)
{
	// Reset destruction flags
	for (std::size_t i = 0; i != max_tracked; ++i)
	{
		destroy_flags_fast[i] = false;
		destroy_flags_ref[i] = false;
	}

	fast_io::deque<tracked_object> dq;
	std::deque<ref_tracked_object> ref;

	auto read_u8 = [&](std::size_t &i) -> uint8_t {
		if (i >= size)
		{
			return 0;
		}
		return data[i++];
	};

	auto read_u64 = [&](std::size_t &i) -> std::size_t {
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

	auto make_fast = [&](int v) -> tracked_object {
		std::size_t idx = static_cast<std::size_t>(v) % max_tracked;
		return tracked_object{v, &destroy_flags_fast[idx]};
	};

	auto make_ref = [&](int v) -> ref_tracked_object {
		std::size_t idx = static_cast<std::size_t>(v) % max_tracked;
		return ref_tracked_object{v, &destroy_flags_ref[idx]};
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
		auto it1 = dq.begin();
		auto it2 = ref.begin();
		for (std::size_t k = 0, sz = dq.size(); k != sz; ++k, ++it1, ++it2)
		{
			if (it1->value != it2->value)
			{
				__builtin_trap();
			}
		}
		if (!ref.empty())
		{
			if (dq.front().value != ref.front().value)
			{
				__builtin_trap();
			}
			if (dq.back().value != ref.back().value)
			{
				__builtin_trap();
			}
		}
	};

	std::size_t i = 0;
	while (i < size)
	{
		uint8_t op = read_u8(i);

		switch (op % 18u)
		{
		case 0:
		{ // push_back
			int v = static_cast<int>(read_u8(i));
			dq.push_back(make_fast(v));
			ref.push_back(make_ref(v));
			break;
		}

		case 1:
		{ // push_front
			int v = static_cast<int>(read_u8(i));
			dq.push_front(make_fast(v));
			ref.push_front(make_ref(v));
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
			int v = static_cast<int>(read_u8(i));
			dq.emplace_back(make_fast(v));
			ref.emplace_back(make_ref(v));
			break;
		}

		case 5:
		{ // emplace_front
			int v = static_cast<int>(read_u8(i));
			dq.emplace_front(make_fast(v));
			ref.emplace_front(make_ref(v));
			break;
		}

		case 6:
		{ // insert (iterator)
			int v = static_cast<int>(read_u8(i));
			if (ref.empty())
			{
				dq.insert(dq.cbegin(), make_fast(v));
				ref.insert(ref.begin(), make_ref(v));
			}
			else
			{
				std::size_t pos = static_cast<std::size_t>(v) % (ref.size() + 1);
				dq.insert(dq.cbegin() + pos, make_fast(v));
				ref.insert(ref.begin() + pos, make_ref(v));
			}
			break;
		}

		case 7:
		{ // insert_index
			int v = static_cast<int>(read_u8(i));
			if (ref.empty())
			{
				dq.insert_index(0, make_fast(v));
				ref.insert(ref.begin(), make_ref(v));
			}
			else
			{
				std::size_t pos = static_cast<std::size_t>(v) % (ref.size() + 1);
				dq.insert_index(pos, make_fast(v));
				ref.insert(ref.begin() + pos, make_ref(v));
			}
			break;
		}

		case 8:
		{ // erase single
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
		{ // erase range
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
			int v = static_cast<int>(read_u8(i));
			if (n > 100000)
			{
				n = n % 100001;
			}
			dq.resize(n, make_fast(v));
			ref.resize(n, make_ref(v));
			break;
		}

		case 12:
		{ // assign(count, val)
			std::size_t n = read_u64(i);
			int v = static_cast<int>(read_u8(i));
			if (n > 100000)
			{
				n = n % 100001;
			}
			dq.assign(n, make_fast(v));
			ref.assign(n, make_ref(v));
			break;
		}

		case 13:
		{ // assign_range
			std::size_t n = read_u8(i);
			std::vector<tracked_object> tmp;
			std::vector<ref_tracked_object> ref_tmp;
			tmp.reserve(n);
			ref_tmp.reserve(n);
			for (std::size_t k = 0; k != n; ++k)
			{
				int v = static_cast<int>(read_u8(i));
				tmp.emplace_back(make_fast(v));
				ref_tmp.emplace_back(make_ref(v));
			}
			dq.assign_range(tmp);
			ref.assign(ref_tmp.begin(), ref_tmp.end());
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
		{ // append_range
			std::size_t n = read_u8(i);
			std::vector<tracked_object> tmp;
			std::vector<ref_tracked_object> ref_tmp;
			tmp.reserve(n);
			ref_tmp.reserve(n);
			for (std::size_t k = 0; k != n; ++k)
			{
				int v = static_cast<int>(read_u8(i));
				tmp.emplace_back(make_fast(v));
				ref_tmp.emplace_back(make_ref(v));
			}
			dq.append_range(tmp);
			ref.insert(ref.end(), ref_tmp.begin(), ref_tmp.end());
			break;
		}

		case 17:
		{ // prepend_range
			std::size_t n = read_u8(i);
			std::vector<tracked_object> tmp;
			std::vector<ref_tracked_object> ref_tmp;
			tmp.reserve(n);
			ref_tmp.reserve(n);
			for (std::size_t k = 0; k != n; ++k)
			{
				int v = static_cast<int>(read_u8(i));
				tmp.emplace_back(make_fast(v));
				ref_tmp.emplace_back(make_ref(v));
			}
			dq.prepend_range(tmp);
			ref.insert(ref.begin(), ref_tmp.begin(), ref_tmp.end());
			break;
		}
		}

		validate();
	}

	// Both deques go out of scope here — destructors fire.
	// The tracked_object destructors already trap on double-destroy above.
	// If we reach return 0, no double-free / use-after-free occurred.

	return 0;
}
