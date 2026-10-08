#include <fast_io.h>
#include "task.h"

namespace fi = ::fast_io;
namespace fop = ::fast_io::operations;

/* one transmit engine per direction: the coroutine owns no buffer at
 * all, the fallback bounce engine manages its own scratch space */
static async_task pump(fi::linux_io_uring_observer sched, fi::posix_io_observer to,
					   fi::posix_io_observer from) throws
{
	co_await fop::async_transmit_all_bytes(sched, to, {}, from, {}, {}, {});
}

int main()
{
	try
	{
		fi::net_service service;
		fi::linux_io_uring ring{fi::native_interface, 32, 0};
		fi::linux_io_uring_observer sched{ring.native_handle()};
		fi::native_socket_file sock{fi::tcp_connect(fi::ipv4{{127, 0, 0, 1}, 2000})};
		fi::posix_io_observer sob{sock.native_handle()};

		/* full duplex: stdin->socket and socket->stdout run concurrently */
		auto up{pump(sched, sob, fi::in())};
		auto down{pump(sched, fi::out(), sob)};
		up.handle.resume();
		down.handle.resume();
		while (!up.handle.done() || !down.handle.done())
		{
			fi::liburing::io_async_wait(sched);
		}
		for (auto h : {up.handle, down.handle})
		{
			if (h.promise().error.domain != nullptr)
			{
				fi::perrln("transfer failed, error code: ", h.promise().error.code);
			}
		}
		up.handle.destroy();
		down.handle.destroy();
	}
	catch throws(::std::error e)
	{
		fi::perrln("fatal: error code: ", e.code());
		return 1;
	}
}
