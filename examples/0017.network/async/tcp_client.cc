#include <fast_io.h>
#include <fast_io_device.h>

namespace fi = ::fast_io;
namespace fop = ::fast_io::operations;

/* one transmit engine per direction: the coroutine owns no buffer at
 * all, the fallback bounce engine manages its own scratch space.
 * Parameters are stream refs BY VALUE — coroutine arguments live in the
 * frame, so nothing here can dangle. */
static fi::io_async_task<> pump(fi::io_async_observer sched, auto to, auto from) throws
{
	co_await fop::async_transmit_all_bytes(sched, to, {}, from, {}, {},
										   {::fast_io::posix_statx_timestamp64{10}});
}

int main()
{
	try
	{
		fi::net_service service;
		fi::io_async_scheduler scheduler{fi::io_async};
		fi::io_async_observer sched{scheduler.native_handle()};
		fi::native_socket_file sock{fi::tcp_connect(fi::ipv4{{127, 0, 0, 1}, 2000},
													fi::open_mode::no_block)};

		/* full duplex: stdin->socket and socket->stdout run concurrently */
		fi::io_async_task<> up{pump(sched, fop::io_stream_ref(sock), fop::input_stream_ref(fi::in()))};
		fi::io_async_task<> down{pump(sched, fop::output_stream_ref(fi::out()), fop::io_stream_ref(sock))};
		up.resume();
		down.resume();
		while (!up.done() || !down.done())
		{
			fi::io_async_wait(sched);
		}
		up.rethrow_if_error();
		down.rethrow_if_error();
	}
	catch throws(::std::error e)
	{
		fi::perrln("fatal: ", e);
		return 1;
	}
}
