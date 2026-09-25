#pragma once

#include "../hash/impl.h"
#include <fast_io_dsal/array.h>
#include <fast_io_dsal/index_span.h>
#include <fast_io_dsal/impl/misc/push_macros.h>
#include <fast_io_dsal/impl/misc/push_warnings.h>

#include "field_number.h"
#include "custom_blindings.h"
#include "field.h"
#include "affine.h"
#include "base_folding.h"
#include "folding.h"
#include "order.h"
#include "blinding.h"
#include "create_key_pair.h"
#include "verify.h"
#include "ed25519.h"
#include "x25519.h"

#include <fast_io_dsal/impl/misc/pop_macros.h>
#include <fast_io_dsal/impl/misc/pop_warnings.h>
