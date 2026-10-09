#pragma once

/*
basic_tls_io_observer<ch_type> -- a non-owning view of a tls_client,
the TLS counterpart of basic_posix_family_io_observer. Copies share the
underlying client state (secrets included), matching io_observer
semantics for every other family. The stream customization points are
the _bytes operations -- TLS carries an octet stream.
*/

#if defined(__linux__)

namespace fast_io::tls
{

template <::std::integral ch_type>
class basic_tls_io_observer
{
public:
	using char_type = ch_type;
	using input_char_type = char_type;
	using output_char_type = char_type;
	using native_handle_type = tls_client *;
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

template <::std::integral ch_type>
inline constexpr bool operator==(basic_tls_io_observer<ch_type> a, basic_tls_io_observer<ch_type> b) noexcept
{
	return a.handle == b.handle;
}

#if __cpp_impl_three_way_comparison >= 201907L
template <::std::integral ch_type>
inline constexpr auto operator<=>(basic_tls_io_observer<ch_type> a, basic_tls_io_observer<ch_type> b) noexcept
{
	return a.handle <=> b.handle;
}
#endif

template <::std::integral ch_type>
inline constexpr basic_tls_io_observer<ch_type> io_stream_ref_define(basic_tls_io_observer<ch_type> other) noexcept
{
	return other;
}

template <::std::integral ch_type>
inline constexpr basic_tls_io_observer<char>
io_bytes_stream_ref_define(basic_tls_io_observer<ch_type> other) noexcept
{
	return {other.handle};
}

template <::std::integral ch_type>
inline ::std::byte *read_some_bytes_underflow_define(basic_tls_io_observer<ch_type> tob,
													 ::std::byte *first, ::std::size_t count) FAST_IO_HERBCEPTIONS_THROWS
{
	return first + tob.handle->read_some(first, count);
}

template <::std::integral ch_type>
inline ::std::byte const *write_some_bytes_overflow_define(basic_tls_io_observer<ch_type> tob,
														   ::std::byte const *first, ::std::size_t count) FAST_IO_HERBCEPTIONS_THROWS
{
	return first + tob.handle->write_some(first, count);
}

template <::std::integral ch_type>
inline void write_all_bytes_overflow_define(basic_tls_io_observer<ch_type> tob,
											::std::byte const *first, ::std::size_t count) FAST_IO_HERBCEPTIONS_THROWS
{
	tob.handle->write_all(first, count);
}

} // namespace fast_io::tls

#endif
