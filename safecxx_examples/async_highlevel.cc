// The high-level async operations stay safe in the safe dialect: async_print,
// async_scan and async_transmit take dialect parameter kinds (in/inout) and
// borrow only from owned streams. The raw byte layer (async_pread/async_pwrite/
// async_scatter_*) is [[unsafe]]; see operations_classify_reject.cc.
//
// Build (syntax check):
//   clang++ --config=$HOME/cfgs/x86_64-linux-gnu-libcxx.cfg \
//     -std=c++26 -fsafecxx -fherbceptions -fsyntax-only -Iinclude \
//     safecxx_examples/async_highlevel.cc
#include <fast_io.h>
#include <fast_io_device.h>

static fast_io::io_async_task<> session(fast_io::io_async_observer sched) throws
{
	fast_io::u8iobuf_file out(u8"/tmp/safecxx_async_out",
							  fast_io::open_mode::out | fast_io::open_mode::creat);
	fast_io::u8iobuf_file in(u8"/tmp/safecxx_async_in",
							 fast_io::open_mode::in | fast_io::open_mode::creat);
	co_await fast_io::io::async_print(sched, {}, out, u8"42\n");
	int value{};
	co_await fast_io::io::async_scan(sched, {}, in, value);
	co_await fast_io::operations::async_transmit_all_bytes(sched, {}, out, {}, in, {}, {});
	fast_io::print("value=", value, "\n");
}

int main()
{
	fast_io::io_async_scheduler sched{fast_io::io_async};
	auto task{session(sched)};
	task.resume();
	task.rethrow_if_error();
}
