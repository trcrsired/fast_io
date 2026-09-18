#pragma once

#ifdef FAST_IO_PRINT_FREESTANDING_USE_CXX20
#include "print_freestanding_cxx20.h"
#else
#include "print_freestanding_cxx26.h"
#endif
#include "common_post.h"

#include "concat_buffer.h"

#ifdef FAST_IO_PRINT_FREESTANDING_USE_CXX20
#include "concat.h"
#else
#include "concat_cxx26.h"
#endif
