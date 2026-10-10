#pragma once

/*
TLS 1.3 client: userspace handshake + record layer, optional Linux kTLS
offload. The client, observer and file composites are transport-generic
-- anywhere fast_io's native socket types exist. ktls.h carries the
Linux SOL_TLS plumbing; async.h carries the io_uring/pthread-pool ops --
both stay linux-gated (windows async would come from an iocp backend).
*/

#include "client.h"
#include "observer.h"
#include "file.h"
#include "roots.h"
#include "swasync.h"
#if defined(__linux__)
#include "async.h"
#endif
#include "openssl.h"
#include "schannel.h"
#include "native_backend.h"
