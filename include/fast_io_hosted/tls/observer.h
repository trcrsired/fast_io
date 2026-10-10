#pragma once

/*
basic_tls_io_observer<ch_type, allocator_type, socket_observer_type> --
a non-owning view of a basic_tls_client, the TLS counterpart of
basic_posix_family_io_observer / basic_win32_family_socket_io_observer.
Copies share the underlying client state (secrets included), matching
io_observer semantics for every other family. The stream customization
points are the _bytes operations -- TLS carries an octet stream.
*/

namespace fast_io::tls
{

template <::std::integral ch_type, typename allocator_type = ::fast_io::native_global_allocator,
		  typename socket_observer_type = ::fast_io::native_socket_io_observer>
class basic_tls_io_observer
{
public:
	using char_type = ch_type;
	using input_char_type = char_type;
	using output_char_type = char_type;
	using native_handle_type = basic_tls_client<allocator_type, socket_observer_type> *;
	native_handle_type handle{};

	inline constexpr native_handle_type native_handle() const noexcept
	{
		return handle;
	}
	inline explicit constexpr operator bool() const noexcept
	{
		return handle != nullptr;
	}
	inline constexpr native_handle_type release() noexcept
	{
		auto temp{handle};
		handle = nullptr;
		return temp;
	}
};

template <::std::integral ch_type, typename allocator_type, typename socket_observer_type>
inline constexpr bool operator==(basic_tls_io_observer<ch_type, allocator_type, socket_observer_type> a,
								 basic_tls_io_observer<ch_type, allocator_type, socket_observer_type> b) noexcept
{
	return a.handle == b.handle;
}

#if __cpp_impl_three_way_comparison >= 201907L
template <::std::integral ch_type, typename allocator_type, typename socket_observer_type>
inline constexpr auto operator<=>(basic_tls_io_observer<ch_type, allocator_type, socket_observer_type> a,
								  basic_tls_io_observer<ch_type, allocator_type, socket_observer_type> b) noexcept
{
	return a.handle <=> b.handle;
}
#endif

template <::std::integral ch_type, typename allocator_type, typename socket_observer_type>
inline constexpr basic_tls_io_observer<ch_type, allocator_type, socket_observer_type>
io_stream_ref_define(basic_tls_io_observer<ch_type, allocator_type, socket_observer_type> other) noexcept
{
	return other;
}

template <::std::integral ch_type, typename allocator_type, typename socket_observer_type>
inline constexpr basic_tls_io_observer<char, allocator_type, socket_observer_type>
io_bytes_stream_ref_define(basic_tls_io_observer<ch_type, allocator_type, socket_observer_type> other) noexcept
{
	return {other.handle};
}

template <::std::integral ch_type, typename allocator_type, typename socket_observer_type>
inline ::std::byte *read_some_bytes_underflow_define(basic_tls_io_observer<ch_type, allocator_type, socket_observer_type> tob,
													 ::std::byte *first, ::std::size_t count) FAST_IO_HERBCEPTIONS_THROWS
{
	return first + tob.handle->read_some(first, count);
}

template <::std::integral ch_type, typename allocator_type, typename socket_observer_type>
inline ::std::byte const *write_some_bytes_overflow_define(basic_tls_io_observer<ch_type, allocator_type, socket_observer_type> tob,
														   ::std::byte const *first, ::std::size_t count) FAST_IO_HERBCEPTIONS_THROWS
{
	return first + tob.handle->write_some(first, count);
}

template <::std::integral ch_type, typename allocator_type, typename socket_observer_type>
inline void write_all_bytes_overflow_define(basic_tls_io_observer<ch_type, allocator_type, socket_observer_type> tob,
											::std::byte const *first, ::std::size_t count) FAST_IO_HERBCEPTIONS_THROWS
{
	tob.handle->write_all(first, count);
}

/*
Positional byte ops: TLS records are not seekable, but a socket's
transport ops still answer them (ESPIPE-style) -- forwarding keeps the
pool's generic async-rw path and buffered positional helpers
well-formed.
*/
template <::std::integral ch_type, typename allocator_type, typename socket_observer_type>
inline ::std::byte *pread_some_bytes_underflow_define(basic_tls_io_observer<ch_type, allocator_type, socket_observer_type> tob,
													  ::std::byte *first, ::std::size_t count,
													  ::fast_io::intfpos_t off) FAST_IO_HERBCEPTIONS_THROWS
{
	(void)off;
	return first + tob.handle->read_some(first, count);
}

template <::std::integral ch_type, typename allocator_type, typename socket_observer_type>
inline ::std::byte const *pwrite_some_bytes_overflow_define(basic_tls_io_observer<ch_type, allocator_type, socket_observer_type> tob,
															::std::byte const *first, ::std::size_t count,
															::fast_io::intfpos_t off) FAST_IO_HERBCEPTIONS_THROWS
{
	(void)off;
	return first + tob.handle->write_some(first, count);
}

template <::std::integral ch_type, typename allocator_type, typename socket_observer_type>
inline ::fast_io::io_scatter_status_t
scatter_pread_some_bytes_underflow_define(basic_tls_io_observer<ch_type, allocator_type, socket_observer_type> tob,
										  ::fast_io::io_scatter_t const *pscatters, ::std::size_t n,
										  ::fast_io::intfpos_t off) FAST_IO_HERBCEPTIONS_THROWS
{
	return scatter_pread_some_bytes_underflow_define(tob.handle->socket(), pscatters, n, off);
}

template <::std::integral ch_type, typename allocator_type, typename socket_observer_type>
inline ::fast_io::io_scatter_status_t
scatter_pwrite_some_bytes_overflow_define(basic_tls_io_observer<ch_type, allocator_type, socket_observer_type> tob,
										  ::fast_io::io_scatter_t const *pscatters, ::std::size_t n,
										  ::fast_io::intfpos_t off) FAST_IO_HERBCEPTIONS_THROWS
{
	return scatter_pwrite_some_bytes_overflow_define(tob.handle->socket(), pscatters, n, off);
}

#if defined(__linux__)
/*
fd-level zero-copy transmit: splice() on the input side and sendfile()
on the output side ride the kTLS socket directly -- only while the
client reports offloaded(); userspace-mode streams get the generic
bounce-buffer path from operations-level transmit.
*/
template <::std::integral ch_type, typename allocator_type, typename socket_observer_type>
inline constexpr ::fast_io::posix_transmit_entry
input_transmit_handle_define(basic_tls_io_observer<ch_type, allocator_type, socket_observer_type> tob) noexcept
	requires requires { tob.handle->fd(); }
{
	return ::fast_io::posix_transmit_entry{tob.handle->fd()};
}

template <::std::integral ch_type, typename allocator_type, typename socket_observer_type>
inline constexpr ::fast_io::posix_transmit_entry
output_transmit_handle_define(basic_tls_io_observer<ch_type, allocator_type, socket_observer_type> tob) noexcept
	requires requires { tob.handle->fd(); }
{
	return ::fast_io::posix_transmit_entry{tob.handle->fd()};
}
#endif

} // namespace fast_io::tls
