// Safe C++ experiment (fast_io safecxx branch): print without [[unsafe]].
//
// fast_io.h now self-fences its implementation under __SAFECXX__, so a normal
// include with -I is enough. The fence is opened in the library header, which
// relaxes syntax for the implementation but does not call-restrict the public
// API. User code has no [[unsafe]] fence:
//
//   clang++ --config=$HOME/cfgs/x86_64-linux-gnu-libcxx.cfg \
//     -std=c++26 -fsafecxx -fherbceptions -lherbceptions -O2 -s -Iinclude \
//     safecxx_examples/print_hello.cc -o /tmp/print_hello
//
// Expected output: hello world

#include <fast_io.h>

int main()
{
	fast_io::print("hello world\n");
}
