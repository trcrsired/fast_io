#pragma once
#if !defined(__cplusplus)
#error "You are not using a C++ compiler"
#endif

#if !defined(__cpp_concepts)
#error "fast_io requires at least a C++26 standard compiler."
#else
#include "fast_io_hosted.h"

#include "fast_io_dsal/impl/misc/push_warnings.h"

#include "fast_io_i18n/lcblob.h"
#include "fast_io_i18n/lc.h"
#include "fast_io_i18n/imbuer.h"
#include "fast_io_i18n/lc_print_status.h"
#include "fast_io_i18n/lc_numbers/impl.h"
#include "fast_io_i18n/lc_concat.h"

// locale-aware floating print hooks live in
// fast_io_unit/floating/lc_impl.h — they exist only when the
// fast_io.floating module provides the conversion routines, so they
// are not pulled in here. Include it explicitly (or via the module)
// when locale-aware floating output is wanted.

#include "fast_io_dsal/impl/misc/pop_warnings.h"

#endif
