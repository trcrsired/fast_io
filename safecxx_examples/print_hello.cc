// Safe C++ experiment (fast_io safecxx branch): print without [[unsafe]].
//
// The only tool the user needs is a trust declaration for fast_io's
// implementation. Two equivalent ways:
//
//   1. Include the safe facade (this file):
//        #include <fast_io_safecxx_safe.h>
//      The facade opens a library-header [[unsafe]] fence around fast_io.h,
//      which makes the implementation trusted without call-restricting the API.
//
//   2. Include <fast_io.h> directly and put fast_io on the system include path:
//        -isystem /path/to/fast_io/include
//      The compiler already exempts system headers from safe-source checks and
//      does not implicitly call-restrict functions declared in them.
//
// In both cases user code has no [[unsafe]] fence at all:
//
//   clang++ --config=$HOME/cfgs/x86_64-linux-gnu-libcxx.cfg \
//     -std=c++26 -fsafecxx -fherbceptions -lherbceptions -O2 -s -Iinclude \
//     safecxx_examples/print_hello.cc -o /tmp/print_hello
//
// Expected output: hello world

#include <fast_io_safecxx_safe.h>

int main()
{
	fast_io::print("hello world\n");
}
