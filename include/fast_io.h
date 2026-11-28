#pragma once
#if !defined(__cplusplus)
#error "You are not using a C++ compiler"
#endif

#if !defined(__cpp_concepts)
#error "fast_io requires at least a C++26 standard compiler."
#else

// Under the safe dialect the implementation is trusted-unsafe: this fence is
// opened in a library header, so it relaxes syntax for the imported code but
// does not call-restrict the public API (unlike a fence in the main file).
// Users include <fast_io.h> normally and call print/scan without [[unsafe]].
#ifdef __SAFECXX__
[[unsafe]] {
#endif

#include "fast_io_hosted.h"

#include "fast_io_dsal/impl/misc/push_warnings.h"
#include "fast_io_dsal/impl/misc/push_macros.h"

#include "fast_io_legacy_impl/io.h"

#include "fast_io_dsal/impl/misc/pop_macros.h"
#include "fast_io_dsal/impl/misc/pop_warnings.h"

#ifdef __SAFECXX__
}
#endif

#endif
