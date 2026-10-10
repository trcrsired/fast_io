# fast_io conventions

## TLS module (include/fast_io_crypto/tls/)

- Free functions and public structs only. No OOP encapsulation, no
  private state, no convenience methods. RAII and moves are fine.
- Herbceptions only. `catch throws(::std::error)` is the only handler;
  `catch(...)` is forbidden and must never appear.
- Name qualification: inside `fast_io::tls{::details}` scope bare
  names resolve locally and stay bare; any code outside that scope
  must write `::fast_io::tls::` / `::fast_io::tls::details::` fully.
- Windows-specific code: `#if (defined(_WIN32) && !defined(__WINE__)) ||
  defined(__CYGWIN__)`. wine-gcc defines _WIN32 on a POSIX-hosted
  compiler, so Wine takes POSIX paths; Cygwin uses posix_thread_pool.
- FAST_IO_TLS_FORCE_FAST_IO=1 pins everything to the fast_io userspace
  implementations (defined in defs.h).

## Crypto backend seam

`basic_tls_client` takes a defaulted `crypto` backend supplying seven
ops: sha256/sha384 contexts (transcript+HKDF), x25519_keypair /
x25519_shared_secret, record_open/seal_inner/seal (suite-dispatched
AEAD), cert_sig_verify / cert_cv_verify (issuer/leaf cert passed whole
so providers can import spki_der). Protocol machinery -- record
framing, key schedule, X.509 parse, chain walk, SAN -- always stays in
fast_io.

Default backend resolves: openssl EVP > gnutls > builtin on non-windows;
builtin on windows and under FORCE_FAST_IO.
## Freestanding discipline

This library is freestanding. Do not use C++ hosted standard library
facilities anywhere in include/.

- No `::std::pair`, `::std::tuple`, `::std::unique_ptr`,
  `::std::shared_ptr`, or any standard smart pointer.
- No `new`/`delete` expressions and no `::std::construct_at` --
  use placement new (`::new (ptr) T{...}`) or the fast_io construct
  helpers.
- No `::std::exchange`, `::std::move` where a fast_io utility exists,
  and no standard-library containers at all -- `vector`, `string`,
  `deque`, `list`, `map`, `unordered_map` etc. come from
  `fast_io::containers` (`basic_vector`, `basic_string`, ...), not
  `::std`.
- No `iostream`, `fstream`, `sstream`, `algorithm` heap helpers, or
  any other hosted-only standard header.
## Herbceptions

All error handling uses Herbceptions, not legacy C++ exceptions.

- `throw throws(::std::error{...})` to raise; `try {}` +
  `catch throws(::std::error e) {}` to handle.
- When a handler cannot call throws-marked code inside its body, the
  empty handler form is correct: `catch throws(::std::error){}`.
- `catch(...)` is forbidden everywhere -- it is legacy-EH-only, catches
  nothing that can exist under `-fherbceptions`, and lies that an
  unknown C++ exception means a protocol error.
- `FAST_IO_HERBCEPTIONS_TRY`/`FAST_IO_HERBCEPTIONS_CATCH`/`FAST_IO_-
  HERBCEPTIONS_CATCH_ALL` expand to the right form per build config;
  `FAST_IO_HERBCEPTIONS_CATCH_ALL` is the only sanctioned catch-all
  and is for C-ABI / thread-entry boundaries only.
- Throwing functions are marked `FAST_IO_HERBCEPTIONS_THROWS`
  (or the explicit `throws` spec); noexcept helpers stay noexcept.
