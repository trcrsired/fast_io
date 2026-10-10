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
## Detemplatization

Template parameters must pay for themselves. Code inside a template
that does not touch the template parameter is generated per
instantiation -- N template args means N copies of identical machine
code.

```cpp
// BAD: the before/after work is re-emitted for every T
template <typename T>
void foo(T t) noexcept
{
	// big block that never touches t
	t.bar();
	// big block that never touches t
}

// GOOD: hoist the invariant blocks; only the glue stays templated
inline void foo_before() noexcept { /* ... */ }
inline void foo_after() noexcept { /* ... */ }

template <typename T>
void foo(T t) noexcept
{
	foo_before();
	t.bar();
	foo_after();
}
```

- Split functions at the points where the template parameter is
  actually used; the extracted pieces take the client's concrete
  fields, not the template param.
- Prefer extracting to `details::` free functions on the non-templated
  client/observer aggregate over adding more parameters.
- A param used only at the boundary (e.g. `ch_type` on an observer
  whose handle points at a ch_type-free client) means the whole
  operation can drop that parameter -- delegate straight to the
  client-level function.
- Same rule for `crypto` backends: wire-format, transcript-buffer and
  X.509 work is type-independent -- hoist it; only the primitive calls
  belong in the templated layer.
- Lambdas are particularly bad here: every lambda is a unique type,
  so a lambda passed into a templated pump/state machine instantiates
  the entire machine per call site even when the body differs by one
  line. Factor shared lambda bodies into named functions and pass a
  plain function pointer or a small functor -- one instantiation, not
  one per call site.

## Dedup near-identical code

A big block repeated with small variations is still one copy too many:
extract the common shape into a function and let call sites pass the
difference.

```cpp
// BAD
void foo()
{
	/* somewhat similar code */
}

void bar()
{
	/* somewhat similar code */
}

// GOOD
void baz(/* the differing bits as params */)
{
	/* the common shape */
}

void foo() { baz(...); }
void bar() { baz(...); }
```

- "Somewhat similar" counts -- do not wait for byte-identical copies;
  parameterize the differing constants/branches instead.
- The extracted function is `details::` or file-local; call sites keep
  their own names.
