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
