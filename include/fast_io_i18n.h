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
#include "fast_io_i18n/imbuer.h"

#include "fast_io_dsal/impl/misc/pop_warnings.h"

#endif
