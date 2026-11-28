#pragma once
#include "fast_io_core_impl/safecxx_fence.h"

FAST_IO_SAFECXX_IMPL_BEGIN

// Public entry point for the fast_io random distributions.
//
// Engine implementations are intentionally not part of this tree: the headers
// consume any uniform random bit generator whose result_type is unsigned and
// that provides operator() and/or generate().
#include "random/random.h"
FAST_IO_SAFECXX_IMPL_END
