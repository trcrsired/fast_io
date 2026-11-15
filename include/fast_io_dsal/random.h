#pragma once

// Public entry point for the fast_io random distributions.
//
// Engine implementations are intentionally not part of this tree: the headers
// consume any uniform random bit generator whose result_type is unsigned and
// that provides operator() and/or generate().
#include "random/random.h"
