/*
https example: TLS 1.3 over Linux kernel TLS (kTLS).
	./https www.google.com
The handshake runs in userspace (x25519 + hkdf + cert verification
against the system trust bundle); after that the kernel does record
encryption/decryption and the tls stream behaves like a plain tcp stream.
*/
#include <fast_io.h>

int main(int argc, char const **argv)
{
	using namespace ::fast_io::io;
	if (argc < 2)
	{
		if (argc == 0)
		{
			return 1;
		}
		perr("Usage: ", ::fast_io::mnp::os_c_str(*argv), " <domain>\n");
		return 1;
	}
	try
	{
		::fast_io::u8cstring_view const host{
			::fast_io::mnp::os_c_str(reinterpret_cast<char8_t const *>(argv[1]))};

		::fast_io::net_service service;
		::fast_io::native_socket_file socket{
			::fast_io::tcp_connect(::fast_io::to_ip(::fast_io::native_dns_file{::fast_io::mnp::os_c_str(argv[1])}, 443))};

		::fast_io::tls::tls_file tls{socket.fd};
		tls.handshake(host);

		char8_t const req_head[]{u8"GET / HTTP/1.1\r\nHost: "};
		char8_t const req_tail[]{u8"\r\nUser-agent:fast_io\r\n"
								 u8"Accept:*/*\r\n"
								 u8"Connection:close\r\n\r\n"};
		::fast_io::operations::write_all_bytes(tls, reinterpret_cast<::std::byte const *>(req_head), sizeof(req_head) - 1);
		::fast_io::operations::write_all_bytes(tls, reinterpret_cast<::std::byte const *>(host.data()), host.size());
		::fast_io::operations::write_all_bytes(tls, reinterpret_cast<::std::byte const *>(req_tail), sizeof(req_tail) - 1);

		/* Connection: close -- transmit until the peer's close_notify (eof) */
		::fast_io::operations::transmit_all_bytes(::fast_io::out(), {}, tls, {}, {});
		tls.send_close_notify();
	}
	catch throws(::std::error e)
	{
		perrln(e);
		return 1;
	}
	return 0;
}
