#!/bin/sh
# Build and run the fast_io.json usage test.
#
# The module is the canonical artifact, so this imports it instead of including
# a header: precompile once, then compile the test against the PCM.
#
# GCC is not used here: it ICEs on `export module fast_io.json;` with
# herbceptions (cp/module.cc:10395).
set -e
: "${HERBCEPTIONS:=/home/cqwrteur/toolchains_build/gcc_herbceptions_install}"
: "${FAST_IO:=../..}"
: "${CXX:=clang++}"
: "${MODE:=-O2}"

"$CXX" -std=c++26 ${MODE} -fherbceptions -I "$FAST_IO/include" -I "$HERBCEPTIONS/include" \
	-x c++-module "$FAST_IO/share/fast_io/fast_io_json.cppm" \
	-o fast_io.json.pcm --precompile

# -g is not optional on this toolchain: without it the link cannot resolve
# __cxa_error_domain_itanium_exception_ptr for the herbception error payload.
"$CXX" -std=c++26 ${MODE} -fherbceptions -g -I "$FAST_IO/include" -I "$HERBCEPTIONS/include" \
	-fmodule-file=fast_io.json=fast_io.json.pcm \
	parse.cc fast_io.json.pcm \
	-L "$HERBCEPTIONS/lib" -lherbceptions \
	-o json_test

./json_test
