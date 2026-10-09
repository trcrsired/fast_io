#pragma once

namespace fast_io
{

template <::std::integral ch_type>
inline void posix_connect(basic_posix_io_observer<ch_type> h, void const *addr, posix_socklen_t addrlen)
	FAST_IO_HERBCEPTIONS_THROWS
{
	details::posix_connect_posix_socket_impl(h.fd, addr, addrlen);
}

template <::std::integral ch_type>
inline void posix_bind(basic_posix_io_observer<ch_type> h, void const *addr, posix_socklen_t addrlen)
	FAST_IO_HERBCEPTIONS_THROWS
{
	details::posix_bind_posix_socket_impl(h.fd, addr, addrlen);
}

template <::std::integral ch_type>
inline void posix_listen(basic_posix_io_observer<ch_type> h, int backlog)
	FAST_IO_HERBCEPTIONS_THROWS
{
	details::posix_listen_posix_socket_impl(h.fd, backlog);
}

template <::std::integral ch_type>
inline ::std::ptrdiff_t posix_recvfrom(basic_posix_io_observer<ch_type> h, void *buf, ::std::size_t len, int flags, void *src_addr, posix_socklen_t *addrlen)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return details::posix_recvfrom_posix_socket_impl(h.fd, buf, len, flags, src_addr, addrlen);
}

template <::std::integral ch_type>
inline ::std::ptrdiff_t posix_sendto(basic_posix_io_observer<ch_type> h, void const *buf, ::std::size_t len, int flags, void const *src_addr, posix_socklen_t addrlen)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return details::posix_sendto_posix_socket_impl(h.fd, buf, len, flags, src_addr, addrlen);
}

namespace details
{

/* a no_block stream still connects synchronously, and an async-scheduled
 * posix socket needs no O_NONBLOCK at all — a would-block io_uring op on a
 * nonblocking socket reports EAGAIN instead of staying pending. Strip the
 * flag entirely for connected sockets; it is purely an async-capability
 * marker (WSA_FLAG_OVERLAPPED) on the win32 side. tcp_listen keeps it —
 * the drain pattern needs nonblocking accept */
inline int posix_tcp_connect_v4_impl(ipv4 v4, open_mode m)
	FAST_IO_HERBCEPTIONS_THROWS
{
	posix_file soc(sock_family::inet, sock_type::stream, m & ~open_mode::no_block,
				   sock_protocol::tcp);
	constexpr auto inet{to_posix_sock_family(sock_family::inet)};
	posix_sockaddr_in in{.sin_family = inet,
						 .sin_port = big_endian(static_cast<::std::uint_least16_t>(v4.port)),
						 .sin_addr = v4.address};
	posix_connect(soc, __builtin_addressof(in), sizeof(in));
	return soc.release();
}

inline int posix_tcp_connect_v6_impl(ipv6 v6, open_mode m)
	FAST_IO_HERBCEPTIONS_THROWS
{
	posix_file soc(sock_family::inet6, sock_type::stream, m & ~open_mode::no_block,
				   sock_protocol::tcp);
	constexpr auto inet6{to_posix_sock_family(sock_family::inet6)};
	posix_sockaddr_in6 in6{.sin6_family = inet6,
						   .sin6_port = big_endian(static_cast<::std::uint_least16_t>(v6.port)),
						   .sin6_addr = v6.address};
	posix_connect(soc, __builtin_addressof(in6), sizeof(in6));
	return soc.release();
}

inline int posix_tcp_connect_ip_impl(ip v, open_mode m)
	FAST_IO_HERBCEPTIONS_THROWS
{
	posix_file soc(v.address.isv4 ? sock_family::inet : sock_family::inet6, sock_type::stream,
				   m & ~open_mode::no_block, sock_protocol::tcp);
	if (v.address.isv4)
	{
		constexpr auto inet{to_posix_sock_family(sock_family::inet)};
		posix_sockaddr_in in{.sin_family = inet, .sin_port = big_endian(v.port), .sin_addr = v.address.address.v4};
		posix_connect(soc, __builtin_addressof(in), sizeof(in));
	}
	else
	{
		constexpr auto inet6{to_posix_sock_family(sock_family::inet6)};
		posix_sockaddr_in6 in6{
			.sin6_family = inet6, .sin6_port = big_endian(v.port), .sin6_addr = v.address.address.v6};
		posix_connect(soc, __builtin_addressof(in6), sizeof(in6));
	}
	return soc.release();
}

inline int posix_tcp_listen_impl(::std::uint_least16_t port, open_mode m)
	FAST_IO_HERBCEPTIONS_THROWS
{
	posix_file soc(sock_family::inet, sock_type::stream, m, sock_protocol::tcp);
	constexpr int one{1};
	/* rebind must succeed even while sessions accepted by a previous
	 * listener on this port linger in TIME_WAIT — otherwise a supervisor
	 * that recreates a dead listener would spin on EADDRINUSE.
	 * SO_REUSEADDR=2 / SOL_SOCKET=1 on linux; BSD/macOS/Solaris use 4 /
	 * 0xffff — prefer the header constants when they are visible */
#ifdef SO_REUSEADDR
	constexpr int sol_socket_v{SOL_SOCKET};
	constexpr int so_reuseaddr_v{SO_REUSEADDR};
#elif defined(__linux__)
	constexpr int sol_socket_v{1};
	constexpr int so_reuseaddr_v{2};
#else
	constexpr int sol_socket_v{0xffff};
	constexpr int so_reuseaddr_v{4};
#endif
	posix_setsockopt_posix_socket_impl(soc.fd, sol_socket_v, so_reuseaddr_v,
									   __builtin_addressof(one), sizeof(one));
	constexpr auto inet{to_posix_sock_family(sock_family::inet)};
	posix_sockaddr_in in{.sin_family = inet, .sin_port = big_endian(port), .sin_addr = {}};
	posix_bind_posix_socket_impl(soc.fd, __builtin_addressof(in), sizeof(in));
	posix_listen_posix_socket_impl(soc.fd, 128);
	return soc.release();
}

} // namespace details

template <::std::integral ch_type>
inline posix_file_factory posix_accept(basic_posix_io_observer<ch_type> h, void *addr, posix_socklen_t *addrlen)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return posix_file_factory{details::posix_accept_posix_socket_impl(h.fd, addr, addrlen)};
}

template <::std::integral ch_type>
inline posix_file_factory tcp_accept(basic_posix_io_observer<ch_type> h)
	FAST_IO_HERBCEPTIONS_THROWS
{
	return posix_file_factory{details::posix_accept_posix_socket_impl(h.fd, nullptr, nullptr)};
}

inline posix_file_factory posix_tcp_connect(ipv4 v4, open_mode m = open_mode{})
	FAST_IO_HERBCEPTIONS_THROWS
{
	return posix_file_factory{details::posix_tcp_connect_v4_impl(v4, m)};
}

inline posix_file_factory posix_tcp_connect(ipv6 v6, open_mode m = open_mode{})
	FAST_IO_HERBCEPTIONS_THROWS
{
	return posix_file_factory{details::posix_tcp_connect_v6_impl(v6, m)};
}

inline posix_file_factory posix_tcp_connect(ip v, open_mode m = open_mode{})
	FAST_IO_HERBCEPTIONS_THROWS
{
	return posix_file_factory{details::posix_tcp_connect_ip_impl(v, m)};
}

inline posix_file_factory tcp_connect(ipv4 v4, open_mode m = open_mode{})
	FAST_IO_HERBCEPTIONS_THROWS
{
	return posix_file_factory{details::posix_tcp_connect_v4_impl(v4, m)};
}
inline posix_file_factory tcp_connect(ipv6 v6, open_mode m = open_mode{})
	FAST_IO_HERBCEPTIONS_THROWS
{
	return posix_file_factory{details::posix_tcp_connect_v6_impl(v6, m)};
}
inline posix_file_factory tcp_connect(ip v, open_mode m = open_mode{})
	FAST_IO_HERBCEPTIONS_THROWS
{
	return posix_file_factory{details::posix_tcp_connect_ip_impl(v, m)};
}
inline posix_file_factory tcp_listen(::std::uint_least16_t port, open_mode m = open_mode{})
	FAST_IO_HERBCEPTIONS_THROWS
{
	return posix_file_factory{details::posix_tcp_listen_impl(port, m)};
}

template <::std::integral ch_type>
using basic_native_socket_io_observer = basic_posix_io_observer<ch_type>;
template <::std::integral ch_type>
using basic_native_socket_file = basic_posix_file<ch_type>;
using net_service = posix_empty_network_service;

namespace operations
{

/*
 * async_connect convenience overloads: build the peer sockaddr from a
 * fast_io address exactly like tcp_connect does, then forward to the raw
 * pointer-and-length form. The backend copies the address at submission,
 * so the local does not need to outlive the call.
 */
template <typename async_scheduler_type, typename streamtype>
inline auto async_connect(async_scheduler_type &&scheduler,
						  ::fast_io::posix_statx_timestamp_opt timeout, streamtype &&stm,
						  ipv4 v4) noexcept
{
	constexpr auto inet{to_posix_sock_family(sock_family::inet)};
	posix_sockaddr_in in{.sin_family = inet,
						 .sin_port = big_endian(static_cast<::std::uint_least16_t>(v4.port)),
						 .sin_addr = v4.address};
	return async_connect(::fast_io::freestanding::forward<async_scheduler_type>(scheduler),
						 timeout, ::fast_io::freestanding::forward<streamtype>(stm),
						 __builtin_addressof(in), sizeof(in));
}

template <typename async_scheduler_type, typename streamtype>
inline auto async_connect(async_scheduler_type &&scheduler,
						  ::fast_io::posix_statx_timestamp_opt timeout, streamtype &&stm,
						  ipv6 v6) noexcept
{
	constexpr auto inet6{to_posix_sock_family(sock_family::inet6)};
	posix_sockaddr_in6 in6{.sin6_family = inet6,
						   .sin6_port = big_endian(static_cast<::std::uint_least16_t>(v6.port)),
						   .sin6_addr = v6.address};
	return async_connect(::fast_io::freestanding::forward<async_scheduler_type>(scheduler),
						 timeout, ::fast_io::freestanding::forward<streamtype>(stm),
						 __builtin_addressof(in6), sizeof(in6));
}

template <typename async_scheduler_type, typename streamtype>
inline auto async_connect(async_scheduler_type &&scheduler,
						  ::fast_io::posix_statx_timestamp_opt timeout, streamtype &&stm,
						  ip v) noexcept
{
	if (v.is_ipv4())
	{
		return async_connect(::fast_io::freestanding::forward<async_scheduler_type>(scheduler),
							 timeout, ::fast_io::freestanding::forward<streamtype>(stm),
							 ipv4{.address = v.address.address.v4, .port = v.port});
	}
	return async_connect(::fast_io::freestanding::forward<async_scheduler_type>(scheduler),
						 timeout, ::fast_io::freestanding::forward<streamtype>(stm),
						 ipv6{.address = v.address.address.v6, .port = v.port});
}

} // namespace operations

#if defined(__HERBCEPTIONS__)
/*
 * The owning file type an async accept yields for a posix listen-stream
 * observer — a property of the stream type, shared by every posix async
 * backend (io_uring, the thread-pool fallback, ...).
 */
template <::fast_io::posix_family family, ::std::integral char_type>
struct ::fast_io::operations::decay::defines::async_accept_file_type<
	::fast_io::basic_posix_family_io_observer<family, char_type>>
{
	using type = ::fast_io::basic_posix_family_file<family, char_type>;
};
#endif
} // namespace fast_io
