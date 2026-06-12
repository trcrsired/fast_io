#include <fast_io.h>
#include <fast_io_dsal/deque.h>
#include <type_traits>
#include <iterator>

int main()
{
	using deque_type = ::fast_io::deque<::std::size_t>;

	// --- value_type ---
	static_assert(::std::same_as<deque_type::value_type, ::std::size_t>);

	// --- reference / const_reference ---
	static_assert(::std::same_as<deque_type::reference, ::std::size_t &>);
	static_assert(::std::same_as<deque_type::const_reference, ::std::size_t const &>);

	// --- pointer / const_pointer ---
	static_assert(::std::same_as<deque_type::pointer, ::std::size_t *>);
	static_assert(::std::same_as<deque_type::const_pointer, ::std::size_t const *>);

	// --- size_type / difference_type ---
	static_assert(::std::same_as<deque_type::size_type, ::std::size_t>);
	static_assert(::std::same_as<deque_type::difference_type, ::std::ptrdiff_t>);
	static_assert(::std::is_signed_v<deque_type::difference_type>);
	static_assert(::std::is_unsigned_v<deque_type::size_type>);

	// --- iterator ---
	static_assert(::std::same_as<deque_type::iterator::value_type, ::std::size_t>);
	static_assert(::std::same_as<deque_type::iterator::difference_type, ::std::ptrdiff_t>);
	static_assert(::std::same_as<deque_type::iterator::reference, ::std::size_t &>);
	static_assert(::std::same_as<deque_type::iterator::pointer, ::std::size_t *>);
	static_assert(::std::same_as<
		::std::iterator_traits<deque_type::iterator>::iterator_category,
		::std::random_access_iterator_tag>);
	static_assert(::std::same_as<
		::std::iterator_traits<deque_type::iterator>::value_type,
		deque_type::value_type>);
	static_assert(::std::same_as<
		::std::iterator_traits<deque_type::iterator>::difference_type,
		deque_type::difference_type>);

	// --- const_iterator ---
	static_assert(::std::same_as<deque_type::const_iterator::value_type, ::std::size_t>);
	static_assert(::std::same_as<deque_type::const_iterator::difference_type, ::std::ptrdiff_t>);
	static_assert(::std::same_as<deque_type::const_iterator::reference, ::std::size_t const &>);
	static_assert(::std::same_as<deque_type::const_iterator::pointer, ::std::size_t const *>);
	static_assert(::std::same_as<
		::std::iterator_traits<deque_type::const_iterator>::iterator_category,
		::std::random_access_iterator_tag>);
	static_assert(::std::same_as<
		::std::iterator_traits<deque_type::const_iterator>::value_type,
		deque_type::value_type>);
	static_assert(::std::same_as<
		::std::iterator_traits<deque_type::const_iterator>::difference_type,
		deque_type::difference_type>);

	// --- difference_type consistency across iterator and const_iterator ---
	static_assert(::std::same_as<
		deque_type::difference_type,
		::std::iterator_traits<deque_type::iterator>::difference_type>);
	static_assert(::std::same_as<
		deque_type::difference_type,
		::std::iterator_traits<deque_type::const_iterator>::difference_type>);

	// --- reverse_iterator ---
	static_assert(::std::same_as<
		deque_type::reverse_iterator,
		::std::reverse_iterator<deque_type::iterator>>);

	// --- const_reverse_iterator ---
	static_assert(::std::same_as<
		deque_type::const_reverse_iterator,
		::std::reverse_iterator<deque_type::const_iterator>>);

	// --- mutable to const iterator conversion ---
	static_assert(::std::convertible_to<deque_type::iterator, deque_type::const_iterator>);
}
