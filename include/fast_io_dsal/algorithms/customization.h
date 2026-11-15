#pragma once

namespace fast_io
{

// Tag for the algorithm customization points. The function name selects the
// operation; this tag only makes the call visible to ADL and keeps the
// customization namespace stable, mirroring io_reserve_type_t.
struct io_algorithm_t
{
  inline explicit constexpr io_algorithm_t() noexcept = default;
};
inline constexpr io_algorithm_t io_algorithm{};

} // namespace fast_io

namespace fast_io::algorithms
{

// Probe declarations make each customization name visible during parsing and
// are never viable: no algorithm argument converts to the probe type. Real
// overloads are found by ADL at instantiation time.
#define FAST_IO_ALGORITHMS_DEFINE_PROBE(name) \
  struct name##_probe;                        \
  template<typename... Args>                  \
  void name(name##_probe, Args&&...) = delete;

FAST_IO_ALGORITHMS_DEFINE_PROBE(accumulate_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(adjacent_difference_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(adjacent_find_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(advance_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(all_of_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(any_of_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(binary_search_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(copy_backward_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(copy_if_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(copy_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(copy_n_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(count_if_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(count_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(destroy_at_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(destroy_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(destroy_n_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(equal_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(equal_range_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(exclusive_scan_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(fill_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(fill_n_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(find_end_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(find_first_of_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(find_if_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(find_if_not_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(find_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(for_each_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(for_each_n_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(generate_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(generate_n_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(includes_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(inclusive_scan_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(inner_product_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(inplace_merge_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(iota_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(is_heap_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(is_heap_until_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(is_partitioned_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(is_permutation_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(is_permutation_prefix_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(is_sorted_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(is_sorted_until_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(iter_swap_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(lexicographical_compare_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(lexicographical_compare_three_way_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(lower_bound_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(make_heap_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(max_element_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(merge_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(min_element_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(minmax_element_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(mismatch_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(move_backward_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(move_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(next_permutation_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(non_overlapped_copy_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(non_overlapped_copy_n_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(none_of_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(nth_element_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(partial_sort_copy_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(partial_sort_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(partial_sum_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(partition_copy_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(partition_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(partition_point_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(pop_heap_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(prev_permutation_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(push_heap_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(reduce_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(remove_if_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(remove_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(replace_copy_if_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(replace_copy_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(replace_if_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(replace_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(reverse_copy_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(reverse_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(rotate_copy_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(rotate_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(search_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(search_n_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(set_difference_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(set_intersection_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(set_symmetric_difference_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(set_union_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(shift_left_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(shift_right_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(sort_heap_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(sort_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(stable_partition_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(swap_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(swap_ranges_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(transform_binary_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(transform_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(uninitialized_copy_backward_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(uninitialized_copy_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(uninitialized_copy_n_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(uninitialized_default_construct_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(uninitialized_default_construct_n_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(uninitialized_fill_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(uninitialized_fill_n_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(uninitialized_move_backward_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(uninitialized_move_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(uninitialized_move_n_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(uninitialized_relocate_backward_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(uninitialized_relocate_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(uninitialized_relocate_n_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(uninitialized_value_construct_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(uninitialized_value_construct_n_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(unique_copy_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(unique_iters_define)
FAST_IO_ALGORITHMS_DEFINE_PROBE(upper_bound_iters_define)

#undef FAST_IO_ALGORITHMS_DEFINE_PROBE

} // namespace fast_io::algorithms

// Dispatch to a customization when one exists and is noexcept.
#define FAST_IO_ALGORITHMS_CUSTOMIZE(name, ...)                               \
  if constexpr (requires {                                                    \
                  { name(::fast_io::io_algorithm, __VA_ARGS__) } noexcept;    \
                })                                                            \
  {                                                                           \
    return name(::fast_io::io_algorithm, __VA_ARGS__);                        \
  }

// Same, but only when `cond` is true. Used to keep the contiguous pointer fast
// paths out of the customization mechanism.
#define FAST_IO_ALGORITHMS_CUSTOMIZE_IF(cond, name, ...) \
  if constexpr (cond)                                    \
  {                                                      \
    FAST_IO_ALGORITHMS_CUSTOMIZE(name, __VA_ARGS__)      \
  }
