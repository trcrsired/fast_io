#pragma once

/*
TLS 1.3 client over Linux kernel TLS (kTLS).
Userspace runs the handshake; the kernel does record-layer AEAD after
keys are installed. Hosted Linux only -- the directory is entered only
when __linux__ is defined and TLS 1.3 crypto support is header-only in
fast_io_crypto.
*/

#include "../../fast_io_crypto.h"
#include "../../fast_io_dsal/string_view.h"
#include "ktls.h"
#include "client.h"
#include "observer.h"
#include "file.h"
#include "roots.h"
