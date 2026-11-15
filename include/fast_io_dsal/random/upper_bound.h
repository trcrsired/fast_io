#pragma once

#include <cstddef>

namespace fast_io::random::details
{

// Index of the first element strictly greater than `value`, i.e.
// upper_bound(first, first + count, value) - first. Reproduced locally because
// the distributions are freestanding and must not pull in the hosted
// <algorithm>.
template <typename T>
inline constexpr ::std::size_t upper_bound_index(
	T const *first, ::std::size_t count, T const &value) noexcept
{
	::std::size_t low{};
	::std::size_t high{count};
	while (low < high)
	{
		::std::size_t const mid{low + (high - low) / 2};
		if (value < first[mid])
		{
			high = mid;
		}
		else
		{
			low = mid + 1;
		}
	}
	return low;
}

} // namespace fast_io::random::details
