#pragma once

/*
fast_io_tls.h -- TLS 1.3 client over Linux kernel TLS (kTLS).
Userspace runs the handshake; the kernel does record-layer AEAD after
keys are installed. Hosted Linux only.
*/

#if !defined(__cplusplus)
#error "You are not using a C++ compiler"
#endif

#if !defined(__cpp_concepts)
#error "fast_io requires at least a C++26 standard compiler."
#else

#if ((__STDC_HOSTED__ == 1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED == 1) && \
	  !defined(_LIBCPP_FREESTANDING)) ||                                             \
	 defined(FAST_IO_ENABLE_HOSTED_FEATURES))

#include "fast_io.h"
#include "fast_io_crypto.h"

#if defined(__linux__)

#include "fast_io_dsal/impl/misc/push_warnings.h"
#include "fast_io_dsal/impl/misc/push_macros.h"

#include "fast_io_tls/ktls.h"
#include "fast_io_tls/client.h"
#include "fast_io_tls/roots.h"

#include "fast_io_dsal/impl/misc/pop_macros.h"
#include "fast_io_dsal/impl/misc/pop_warnings.h"

#endif

#endif

#endif
