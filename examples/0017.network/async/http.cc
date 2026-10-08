#include <fast_io.h>
#include <fast_io_device.h>
#include <string_view>
#include "task.h"

using namespace fast_io::io;

namespace fi = ::fast_io;
namespace fop = ::fast_io::operations;

/*
 * Async HTTP/1.0 GET. The request is print()ed into the socket's output
 * buffer; the input side's tie handling flushes it asynchronously before
 * the first read — no explicit flush needed, same ordering as sync.
 */
static async_task fetch(fi::linux_io_uring_observer sched, fi::u8iobuf_socket_file sock,
						fi::u8native_file out) throws
{
	auto hdr{co_await fop::async_scan_get<fi::u8http_header_buffer>(sched, sock, {})};
	using namespace std::string_view_literals;
	for (auto [key, value] : line_generator(hdr))
	{
		if (::std::u8string_view(key) == u8"Content-Length"sv)
		{
			auto content_length{::fast_io::u8to<::std::uint_least64_t>(value)};
			co_await fop::async_transmit_all_bytes(sched, out, {}, sock, {},
												   fi::size_t_opt{content_length}, {});
			co_return;
		}
	}
}

int main(int argc, char const **argv)
{
	try
	{
		if (argc < 2)
		{
			if (argc == 0)
			{
				return 1;
			}
			fi::perr("Usage: ", ::fast_io::mnp::os_c_str(*argv), " <domain>\n");
			return 1;
		}
		fi::net_service service;
		fi::linux_io_uring ring{fi::native_interface, 32, 0};
		fi::linux_io_uring_observer sched{ring.native_handle()};
		fi::u8iobuf_socket_file sock{
			fi::tcp_connect(fi::to_ip(fi::native_dns_file(::fast_io::mnp::os_c_str(argv[1])), 80))};
		fi::u8native_file out{u8"index.html", fi::open_mode::out};

		print(sock,
			  u8"GET / HTTP/1.1\r\n"
			  "Host:",
			  ::fast_io::mnp::code_cvt_os_c_str(argv[1]),
			  u8"\r\n"
			  "User-agent:whatever\r\n"
			  "Accept-Type:*/*\r\n"
			  "Connection:close\r\n\r\n");

		auto t{fetch(sched, ::std::move(sock), ::std::move(out))};
		t.handle.resume();
		while (!t.handle.done())
		{
			fi::liburing::io_async_wait(sched);
		}
		if (t.handle.promise().error.domain != nullptr)
		{
			fi::perrln("fetch failed, error code: ", t.handle.promise().error.code);
		}
		t.handle.destroy();
	}
	catch throws(::std::error e)
	{
		fi::perrln("fatal: error code: ", e.code());
		return 1;
	}
}
