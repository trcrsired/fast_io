/*
https example: TLS 1.3 over Linux kernel TLS (kTLS).
	./https www.google.com
The handshake runs in userspace (x25519 + hkdf + cert verification
against the system trust bundle); after that the kernel does record
encryption/decryption and the socket behaves like a plain tcp stream.
*/
#include <fast_io.h>
#include <fast_io_tls.h>
#include <cstring>

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
		::std::size_t const host_len{::std::strlen(argv[1])};

		/* trust roots from the system CA bundle */
		::fast_io::tls::root_store roots{};
		roots.load_pem_file("/etc/ca-certificates/extracted/tls-ca-bundle.pem");

		::fast_io::net_service service;
		::fast_io::native_socket_file socket{
			::fast_io::tcp_connect(::fast_io::to_ip(::fast_io::native_dns_file{::fast_io::mnp::os_c_str(argv[1])}, 443))};

		::fast_io::tls::ktls_client tls{socket.fd};
		::fast_io::tls::tls13_client_config cfg{};
		cfg.hostname = reinterpret_cast<char8_t const *>(argv[1]);
		cfg.hostname_size = host_len;
		cfg.roots = roots.roots();
		cfg.root_sizes = roots.root_sizes();
		cfg.root_count = roots.root_count();
		cfg.now = ::std::time(nullptr);
		tls.handshake(cfg);

		char8_t const req_hdr[]{u8"GET / HTTP/1.1\r\nHost: "};
		char8_t const req_tail[]{u8"\r\nUser-Agent: fast_io\r\n"
								 u8"Accept: */*\r\n"
								 u8"Connection: close\r\n\r\n"};
		tls.write_all(reinterpret_cast<::std::byte const *>(req_hdr), sizeof(req_hdr) - 1);
		tls.write_all(reinterpret_cast<::std::byte const *>(argv[1]), host_len);
		tls.write_all(reinterpret_cast<::std::byte const *>(req_tail), sizeof(req_tail) - 1);

		::std::byte buf[16384];
		for (;;)
		{
			::std::size_t const n{tls.read_some(buf, sizeof(buf))};
			if (n == 0)
			{
				break;
			}
			::fast_io::operations::write_all_bytes(::fast_io::out(), buf, n);
		}
		tls.send_close_notify();
	}
	catch throws(::std::error e)
	{
		if (e.is_code_of<::fast_io::tls::handshake_error>())
		{
			::std::size_t const code{e.code()};
			perrln("tls alert ", code >> 16u, " at stage ", code & 0xffffu);
		}
		else
		{
			perrln(e);
		}
		return 1;
	}
	return 0;
}
