#include <fast_io.h>
#include <fast_io_dsal/deque.h>
#include <iterator>
#include <ranges>
#include <concepts>

int main()
{
	using deque_type = ::fast_io::deque<int>;
	using iterator = deque_type::iterator;
	using const_iterator = deque_type::const_iterator;
	using reverse_iterator = deque_type::reverse_iterator;
	using const_reverse_iterator = deque_type::const_reverse_iterator;
	using value_type = int;

	// ===== Iterator concept conformance =====

	// random_access_iterator (NOT contiguous_iterator)
	static_assert(::std::random_access_iterator<iterator>);
	static_assert(!::std::contiguous_iterator<iterator>);

	static_assert(::std::random_access_iterator<const_iterator>);
	static_assert(!::std::contiguous_iterator<const_iterator>);

	// indirectly_writable
	static_assert(::std::indirectly_writable<iterator, value_type>);
	static_assert(!::std::indirectly_writable<const_iterator, value_type>);

	// sentinel_for
	static_assert(::std::sentinel_for<iterator, iterator>);
	static_assert(::std::sentinel_for<iterator, const_iterator>);
	static_assert(!::std::sentinel_for<iterator, reverse_iterator>);
	static_assert(!::std::sentinel_for<iterator, const_reverse_iterator>);

	static_assert(::std::sentinel_for<const_iterator, iterator>);
	static_assert(::std::sentinel_for<const_iterator, const_iterator>);
	static_assert(!::std::sentinel_for<const_iterator, reverse_iterator>);
	static_assert(!::std::sentinel_for<const_iterator, const_reverse_iterator>);

	// sized_sentinel_for
	static_assert(::std::sized_sentinel_for<iterator, iterator>);
	static_assert(::std::sized_sentinel_for<iterator, const_iterator>);
	static_assert(!::std::sized_sentinel_for<iterator, reverse_iterator>);
	static_assert(!::std::sized_sentinel_for<iterator, const_reverse_iterator>);

	static_assert(::std::sized_sentinel_for<const_iterator, iterator>);
	static_assert(::std::sized_sentinel_for<const_iterator, const_iterator>);
	static_assert(!::std::sized_sentinel_for<const_iterator, reverse_iterator>);
	static_assert(!::std::sized_sentinel_for<const_iterator, const_reverse_iterator>);

	// indirectly_movable / indirectly_movable_storable
	static_assert(::std::indirectly_movable<iterator, iterator>);
	static_assert(::std::indirectly_movable_storable<iterator, iterator>);
	static_assert(!::std::indirectly_movable<iterator, const_iterator>);
	static_assert(!::std::indirectly_movable_storable<iterator, const_iterator>);
	static_assert(::std::indirectly_movable<iterator, reverse_iterator>);
	static_assert(::std::indirectly_movable_storable<iterator, reverse_iterator>);
	static_assert(!::std::indirectly_movable<iterator, const_reverse_iterator>);
	static_assert(!::std::indirectly_movable_storable<iterator, const_reverse_iterator>);

	static_assert(::std::indirectly_movable<const_iterator, iterator>);
	static_assert(::std::indirectly_movable_storable<const_iterator, iterator>);
	static_assert(!::std::indirectly_movable<const_iterator, const_iterator>);
	static_assert(!::std::indirectly_movable_storable<const_iterator, const_iterator>);
	static_assert(::std::indirectly_movable<const_iterator, reverse_iterator>);
	static_assert(::std::indirectly_movable_storable<const_iterator, reverse_iterator>);
	static_assert(!::std::indirectly_movable<const_iterator, const_reverse_iterator>);
	static_assert(!::std::indirectly_movable_storable<const_iterator, const_reverse_iterator>);

	// indirectly_copyable / indirectly_copyable_storable
	static_assert(::std::indirectly_copyable<iterator, iterator>);
	static_assert(::std::indirectly_copyable_storable<iterator, iterator>);
	static_assert(!::std::indirectly_copyable<iterator, const_iterator>);
	static_assert(!::std::indirectly_copyable_storable<iterator, const_iterator>);
	static_assert(::std::indirectly_copyable<iterator, reverse_iterator>);
	static_assert(::std::indirectly_copyable_storable<iterator, reverse_iterator>);
	static_assert(!::std::indirectly_copyable<iterator, const_reverse_iterator>);
	static_assert(!::std::indirectly_copyable_storable<iterator, const_reverse_iterator>);

	static_assert(::std::indirectly_copyable<const_iterator, iterator>);
	static_assert(::std::indirectly_copyable_storable<const_iterator, iterator>);
	static_assert(!::std::indirectly_copyable<const_iterator, const_iterator>);
	static_assert(!::std::indirectly_copyable_storable<const_iterator, const_iterator>);
	static_assert(::std::indirectly_copyable<const_iterator, reverse_iterator>);
	static_assert(::std::indirectly_copyable_storable<const_iterator, reverse_iterator>);
	static_assert(!::std::indirectly_copyable<const_iterator, const_reverse_iterator>);
	static_assert(!::std::indirectly_copyable_storable<const_iterator, const_reverse_iterator>);

	// indirectly_swappable
	static_assert(::std::indirectly_swappable<iterator, iterator>);
	static_assert(!::std::indirectly_swappable<const_iterator, const_iterator>);

	// ===== Range concept conformance =====
	using range = deque_type;

	static_assert(::std::same_as<::std::ranges::iterator_t<range>, range::iterator>);
	static_assert(::std::ranges::common_range<range>);
	static_assert(::std::ranges::random_access_range<range>);
	static_assert(!::std::ranges::contiguous_range<range>);
	static_assert(!::std::ranges::view<range>);
	static_assert(::std::ranges::sized_range<range>);
	static_assert(!::std::ranges::borrowed_range<range>);
	static_assert(::std::ranges::viewable_range<range>);

	static_assert(::std::same_as<::std::ranges::iterator_t<range const>, range::const_iterator>);
	static_assert(::std::ranges::common_range<range const>);
	static_assert(::std::ranges::random_access_range<range const>);
	static_assert(!::std::ranges::contiguous_range<range const>);
	static_assert(!::std::ranges::view<range const>);
	static_assert(::std::ranges::sized_range<range const>);
	static_assert(!::std::ranges::borrowed_range<range const>);
	static_assert(!::std::ranges::viewable_range<range const>);
}
