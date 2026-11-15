// The comparator contract: `compare` must return a three-way result.
//
// A predicate-shaped `compare` returning bool satisfies every expression the
// algorithms spell -- `0 > false` is always false -- so without this constraint
// sorting compiles and silently does nothing. These checks are compile-time:
// the file fails to build if a bool-returning comparator is accepted again.
#include <fast_io.h>
#include <fast_io_dsal/deque.h>
#include <fast_io_dsal/forward_list.h>
#include <fast_io_dsal/list.h>
#include <fast_io_dsal/priority_queue.h>
#include <fast_io_dsal/vector.h>

struct bool_cmp
{
	template <typename T>
	inline constexpr bool compare(T const &, T const &) const noexcept
	{
		return true;
	}
};

struct cat_cmp
{
	template <typename T>
	inline constexpr ::std::strong_ordering compare(T const &a, T const &b) const noexcept
	{
		return a <=> b;
	}
};

using namespace ::fast_io::details::algorithms;

static_assert(!three_way_result<bool>);
static_assert(three_way_result<::std::strong_ordering>);
static_assert(three_way_result<::std::weak_ordering>);
static_assert(three_way_result<::std::partial_ordering>);
static_assert(!three_way_compare<bool_cmp, int>);
static_assert(!three_way_comparator<bool_cmp, int>);
static_assert(!equality_comparator<bool_cmp, int>);
static_assert(three_way_comparator<cat_cmp, int>);
static_assert(three_way_comparator<::fast_io::algorithms::compare_three_way, int>);
static_assert(equality_comparator<::fast_io::algorithms::equal_to, int>);

// `nullptr` stands in for iterators of the right value type; only the
// constraints are instantiated. The checks are dependent so an unsatisfied
// constraint is a substitution failure rather than an eager diagnostic.
#define FAST_IO_ALGORITHMS_REJECTS(label, ...)                                 \
	template <typename Cmp>                                                    \
	inline constexpr bool label = requires(Cmp cmp) { __VA_ARGS__; };          \
	static_assert(!label<bool_cmp>);                                           \
	static_assert(label<cat_cmp>);

FAST_IO_ALGORITHMS_REJECTS(sort_rejects_bool,
	::fast_io::algorithms::sort_iters(static_cast<int *>(nullptr), static_cast<int *>(nullptr), cmp))
FAST_IO_ALGORITHMS_REJECTS(nth_element_rejects_bool,
	::fast_io::algorithms::nth_element_iters(static_cast<int *>(nullptr), static_cast<int *>(nullptr), static_cast<int *>(nullptr), cmp))
FAST_IO_ALGORITHMS_REJECTS(partial_sort_rejects_bool,
	::fast_io::algorithms::partial_sort_iters(static_cast<int *>(nullptr), static_cast<int *>(nullptr), static_cast<int *>(nullptr), cmp))
FAST_IO_ALGORITHMS_REJECTS(heap_rejects_bool,
	::fast_io::algorithms::make_heap_iters(static_cast<int *>(nullptr), static_cast<int *>(nullptr), cmp))
FAST_IO_ALGORITHMS_REJECTS(is_sorted_rejects_bool,
	::fast_io::algorithms::is_sorted_iters(static_cast<int *>(nullptr), static_cast<int *>(nullptr), cmp))
FAST_IO_ALGORITHMS_REJECTS(binary_search_rejects_bool,
	::fast_io::algorithms::lower_bound_iters(static_cast<int *>(nullptr), static_cast<int *>(nullptr), 1, cmp))
FAST_IO_ALGORITHMS_REJECTS(lexicographical_compare_rejects_bool,
	::fast_io::algorithms::lexicographical_compare_iters(static_cast<int *>(nullptr), static_cast<int *>(nullptr), static_cast<int *>(nullptr), static_cast<int *>(nullptr), cmp))
FAST_IO_ALGORITHMS_REJECTS(merge_rejects_bool,
	::fast_io::algorithms::merge_iters(static_cast<int *>(nullptr), static_cast<int *>(nullptr), static_cast<int *>(nullptr), static_cast<int *>(nullptr), static_cast<int *>(nullptr), cmp))
FAST_IO_ALGORITHMS_REJECTS(inplace_merge_rejects_bool,
	::fast_io::algorithms::inplace_merge_iters(static_cast<int *>(nullptr), static_cast<int *>(nullptr), static_cast<int *>(nullptr), cmp))
FAST_IO_ALGORITHMS_REJECTS(minmax_rejects_bool,
	::fast_io::algorithms::minmax_element_iters(static_cast<int *>(nullptr), static_cast<int *>(nullptr), cmp))
FAST_IO_ALGORITHMS_REJECTS(permutation_rejects_bool,
	::fast_io::algorithms::next_permutation_iters(static_cast<int *>(nullptr), static_cast<int *>(nullptr), cmp))
FAST_IO_ALGORITHMS_REJECTS(set_operations_rejects_bool,
	::fast_io::algorithms::set_union_iters(static_cast<int *>(nullptr), static_cast<int *>(nullptr), static_cast<int *>(nullptr), static_cast<int *>(nullptr), static_cast<int *>(nullptr), cmp))
FAST_IO_ALGORITHMS_REJECTS(equal_rejects_bool,
	::fast_io::algorithms::equal_iters(static_cast<int *>(nullptr), static_cast<int *>(nullptr), static_cast<int *>(nullptr), static_cast<int *>(nullptr), cmp))
FAST_IO_ALGORITHMS_REJECTS(search_rejects_bool,
	::fast_io::algorithms::search_iters(static_cast<int *>(nullptr), static_cast<int *>(nullptr), static_cast<int *>(nullptr), static_cast<int *>(nullptr), cmp))
FAST_IO_ALGORITHMS_REJECTS(unique_rejects_bool,
	::fast_io::algorithms::unique_iters(static_cast<int *>(nullptr), static_cast<int *>(nullptr), cmp))
FAST_IO_ALGORITHMS_REJECTS(list_sort_rejects_bool, ::fast_io::list<int>{}.sort(cmp))
FAST_IO_ALGORITHMS_REJECTS(forward_list_sort_rejects_bool,
	::fast_io::forward_list<int>{}.sort(cmp))
FAST_IO_ALGORITHMS_REJECTS(priority_queue_rejects_bool,
	static_cast<::fast_io::containers::priority_queue<Cmp, ::fast_io::vector<int>> *>(nullptr))

#undef FAST_IO_ALGORITHMS_REJECTS

// A predicate is still a predicate: closures remain valid where the algorithm
// takes an `f`, because that is not a comparator slot.
template <typename F>
inline constexpr bool find_if_accepts =
	requires(::fast_io::vector<int> &v, F f) { ::fast_io::algorithms::find_if(v, f); };
static_assert(find_if_accepts<decltype([](int x) { return x > 0; })>);

int main()
{
	::fast_io::vector<int> v{3, 1, 2};
	::fast_io::algorithms::sort(v, cat_cmp{});
	return ::fast_io::algorithms::is_sorted(v) ? 0 : 1;
}
