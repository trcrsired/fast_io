#pragma once

// The umbrella header supplies the shared declarations before any family
// header. Family headers intentionally do not include `common.h` or
// `compare.h` themselves.

#include <bit>
#include <compare>
#include <utility>

#include "common.h"
#include "customization.h"
#include "compare.h"
#include "operations.h"
#include "swap.h"

#include "advance.h"
#include "all_of.h"
#include "construct.h"
#include "copy.h"
#include "count.h"
#include "destroy.h"
#include "equal.h"
#include "fill.h"
#include "find.h"
#include "for_each.h"
#include "generate.h"

// Numeric family. `scan.h` reuses the `partial_sum` implementation, so it is
// included after `partial_sum.h`.
#include "adjacent_difference.h"
#include "inner_product.h"
#include "integer.h"
#include "iota.h"
#include "reduce.h"
#include "partial_sum.h"
#include "scan.h"

#include "heap.h"
#include "is_sorted.h"
#include "lexicographical_compare.h"
#include "minmax.h"
#include "mismatch.h"
#include "move.h"
#include "remove.h"
#include "relocate.h"
#include "replace.h"
#include "reverse.h"
#include "rotate.h"
#include "search.h"
#include "shift.h"
#include "swap_ranges.h"
#include "transform.h"
#include "unique.h"
#include "binary_search.h"
#include "merge.h"
#include "partition.h"
#include "permutation.h"
#include "sort.h"
#include "nth_element.h"
#include "partial_sort.h"
#include "set_operations.h"

// The deque customization is not part of this umbrella. It needs the deque
// container to be complete, so it is included by fast_io_hosted.h after
// fast_io_dsal/impl/deque.h. Including it here would recurse when the core is
// pulled in from deque.h itself.
