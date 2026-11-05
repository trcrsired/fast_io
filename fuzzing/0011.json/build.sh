#!/bin/sh
# Build the fast_io.json fuzz target.
#
# The module is the canonical artifact, so the fuzzer imports it instead of
# textually including a header: precompile once, then link the target against
# the PCM together with ASan + libFuzzer and the herbception runtime.
set -e
: "${HERBCEPTIONS:=/home/cqwrteur/toolchains_build/gcc_herbceptions_install}"
: "${FAST_IO:=../..}"
# The libFuzzer runtime shipped with clang is built against libc++, so its
# thread/error helpers have to be resolved even though the target itself is
# compiled against the default standard library.
: "${LIBCXX_LIB:=/home/cqwrteur/toolchains/llvm/x86_64-linux-gnu/x86_64-linux-gnu/usr/lib}"

clang++ -std=c++26 -fherbceptions -O1 -g \
	-I "$FAST_IO/include" -I "$HERBCEPTIONS/include" \
	-x c++-module "$FAST_IO/share/fast_io/fast_io_json.cppm" \
	-o fast_io.json.pcm --precompile

clang++ -std=c++26 -fherbceptions -O1 -g -fsanitize=fuzzer,address \
	-I "$FAST_IO/include" -I "$HERBCEPTIONS/include" \
	-fmodule-file=fast_io.json=fast_io.json.pcm \
	parse.cc fast_io.json.pcm \
	-L "$HERBCEPTIONS/lib" -lherbceptions \
	-L "$LIBCXX_LIB" -lc++ -lc++abi \
	-o json_fuzz

echo "built ./json_fuzz - seed it with real documents, e.g."
echo "  mkdir -p corpus && cp \$FAST_IO/../pwa-store/docs/store/store.json corpus/ 2>/dev/null || true"
