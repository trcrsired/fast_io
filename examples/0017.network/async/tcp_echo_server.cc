#include <fast_io.h>
#include <fast_io_device.h>
#include <fast_io_dsal/vector.h>
#include "task.h"

namespace fi = ::fast_io;
namespace fop = ::fast_io::operations;

/*
 * One coroutine per connection: transmit the socket back onto itself
 * until the peer closes. No buffer management anywhere — the transmit
 * engine owns its scratch space. socket->socket cannot splice on Linux,
 * so the io_uring backend transparently falls back to the generic
 * bounce engine.
 */
static async_task echo_session(fi::linux_io_uring_observer sched, fi::native_socket_file sock) throws
{
	fi::posix_io_observer ob{sock.native_handle()};
	co_await fop::async_transmit_all_bytes(sched, ob, {}, ob, {}, {}, {});
}

int main()
{
	try
	{
		fi::net_service service;
		fi::linux_io_uring ring{fi::native_interface, 256, 0};
		fi::linux_io_uring_observer sched{ring.native_handle()};
		fi::native_socket_file listener{fi::tcp_listen(2000, fi::open_mode::no_block)};

		::fast_io::vector<::std::coroutine_handle<async_task::promise_type>> sessions;
		for (;;)
		{
			/* drain pending accepts; the listener is nonblocking so an
			 * empty accept queue reports resource_unavailable_try_again */
			for (;;)
			{
				try
				{
					auto t{echo_session(sched, fi::native_socket_file{fi::tcp_accept(listener)})};
					sessions.push_back(t.handle);
					t.handle.resume();
				}
				catch throws(::std::error e)
				{
					if (e.equivalent(::std::errc::resource_unavailable_try_again))
					{
						break;
					}
					fi::perrln("accept failed, error code: ", e.code());
					break;
				}
			}
			/* reap finished sessions */
			for (::std::size_t i{}; i != sessions.size();)
			{
				auto h{sessions[i]};
				if (h.done())
				{
					if (h.promise().error.domain != nullptr)
					{
						fi::perrln("echo session failed, error code: ", h.promise().error.code);
					}
					h.destroy();
					sessions.erase_index(i);
				}
				else
				{
					++i;
				}
			}
			/* block for one completion, bounded so the accept queue gets
			 * polled even when existing sessions are idle */
			fi::liburing::io_async_wait_timeout(sched, {0, 50000000});
		}
	}
	catch throws(::std::error e)
	{
		fi::perrln("fatal: error code: ", e.code());
		return 1;
	}
}
