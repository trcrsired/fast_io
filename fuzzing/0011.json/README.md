# 0011.json

Fuzz target for the `fast_io.json` module (`share/fast_io/fast_io_json.cppm`).

```sh
./build.sh                      # precompiles the module, links ASan + libFuzzer
mkdir -p corpus
cp /path/to/a/real/document.json corpus/    # seed with something big and real
./json_fuzz -max_total_time=60 corpus
```

`build.sh` defaults `HERBCEPTIONS` to the herbception toolchain install and
`FAST_IO` to `../..`; both can be overridden. `LIBCXX_LIB` is needed because
the libFuzzer runtime clang ships is built against libc++.

## What is checked

Every input is parsed three ways, and the three must agree:

* `parse()` - the DOM builder,
* `parse_into()` - the streaming callback parser, which must also consume the
  whole buffer whenever the DOM accepted it,
* `parse<json_policy::none>()` - the FPU-free arm used by kernel code. It may
  only reject *more* than the default policy (documents holding a fraction or
  an exponent), never less, and it must never accept a document the default
  policy rejected.

The oracle is structural rather than "it did not crash". The flat node array
promises that every child index is in range and that sibling chains terminate,
so the walker checks both against the node/member counts - a leaked cycle or an
out-of-range index still returns normally, and only this walk catches it. String
views are hashed so that ASan sees a read through a view that overruns its
buffer or points into freed memory.

Crashes land in `./crash-*`; replay one with `./json_fuzz crash-<hash>`.
