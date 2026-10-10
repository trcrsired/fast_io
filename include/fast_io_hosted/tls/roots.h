#pragma once

/*
trust-root store: decodes every CERTIFICATE block from a PEM bundle
into DER and presents them in the flat array form tls13_client_config
wants. Hosted only (reads files via native_file_loader).
*/


#include <cstdlib>

namespace fast_io::tls
{

template <typename allocator_type = ::fast_io::native_global_allocator>
class basic_root_store
{
public:
	using allocator_handle_type = typename allocator_type::handle_type;
	static inline constexpr bool alloc_with_status{
		allocator_type::has_status};

private:
	::fast_io::vector<::std::byte, allocator_type> der_data;
	::fast_io::vector<::std::byte const *, allocator_type> ptrs;
	::fast_io::vector<::std::size_t, allocator_type> sizes;

public:
	inline constexpr basic_root_store() noexcept
		requires(!alloc_with_status)
	= default;
	inline explicit constexpr basic_root_store(allocator_handle_type hdl) noexcept
		requires(alloc_with_status)
		: der_data{hdl}, ptrs{hdl}, sizes{hdl}
	{
	}

	/* append every CERTIFICATE pem block from pem */
	inline void load_pem_text(::fast_io::u8string_view pem) FAST_IO_HERBCEPTIONS_THROWS
	{
		char8_t const *cur{pem.data()};
		char8_t const *const end{cur + pem.size()};
		char8_t const *label, *b64;
		::std::size_t label_size, b64_size;
		while (details::pem_next_block(cur, end, label, label_size, b64, b64_size))
		{
			if (label_size != 11 ||
				::fast_io::freestanding::my_memcmp(label, u8"CERTIFICATE", 11) != 0)
			{
				continue;
			}
			/* worst case decoded size ~ b64_size; grow once */
			::std::size_t const off{der_data.size()};
			der_data.resize(off + b64_size);
			::std::size_t const n{details::pem_decode_block(der_data.data() + off, b64_size,
															b64, b64_size)};
			if (n == 0)
			{
				der_data.resize(off);
				continue;
			}
			der_data.resize(off + n);
			sizes.push_back(n);
			ptrs.push_back(nullptr); /* patched by finalize() */
		}
		finalize();
	}

	/* decode a pem bundle file (e.g. /etc/ssl/certs/ca-certificates.crt) */
	template <::fast_io::constructible_to_os_c_str T>
	inline void load_pem_file(T const &file) FAST_IO_HERBCEPTIONS_THROWS
	{
		::fast_io::native_file_loader f{file};
		load_pem_text(::fast_io::u8string_view{reinterpret_cast<char8_t const *>(f.data()), f.size()});
	}

	/*
	the system trust bundle: $SSL_CERT_FILE first (openssl convention),
	then the well-known per-distro bundle paths.
	*/
	inline void load_system() FAST_IO_HERBCEPTIONS_THROWS
	{
#if defined(_WIN32)
		if constexpr (::fast_io::win32_family::native == ::fast_io::win32_family::ansi_9x)
		{
			char envbuf[32767];
			::std::uint_least32_t const n{::fast_io::win32::GetEnvironmentVariableA(
				"SSL_CERT_FILE", envbuf, sizeof(envbuf))};
			if (n != 0 && n < sizeof(envbuf))
			{
				load_pem_file(::fast_io::mnp::os_c_str_with_known_size(envbuf, n));
				return;
			}
		}
		else
		{
			char16_t envbuf[32767];
			::std::uint_least32_t const n{::fast_io::win32::GetEnvironmentVariableW(
				u"SSL_CERT_FILE", envbuf, 32767)};
			if (n != 0 && n < 32767)
			{
				load_pem_file(::fast_io::mnp::os_c_str_with_known_size(envbuf, n));
				return;
			}
		}
#else
		if (char const *env{::std::getenv("SSL_CERT_FILE")}; env != nullptr && *env != 0)
		{
			load_pem_file(::fast_io::mnp::os_c_str(env));
			return;
		}
#endif
		constexpr char const *paths[]{
			"/etc/ssl/certs/ca-certificates.crt",                /* debian/ubuntu/arch */
			"/etc/pki/tls/certs/ca-bundle.crt",                  /* fedora/rhel */
			"/etc/ssl/ca-bundle.pem",                            /* opensuse */
			"/etc/pki/tls/cacert.pem",                           /* openelec */
			"/etc/pki/ca-trust/extracted/pem/tls-ca-bundle.pem", /* centos/rhel p11-kit */
			"/etc/ca-certificates/extracted/tls-ca-bundle.pem",  /* fedora ca-certificates-ng */
			"/etc/ssl/cert.pem",                                 /* alpine/macos-ish */
		};
		for (char const *p : paths)
		{
			FAST_IO_HERBCEPTIONS_TRY
			{
				load_pem_file(::fast_io::mnp::os_c_str(p));
				return;
			}
			FAST_IO_HERBCEPTIONS_CATCH_ALL
			{
			}
		}
		::fast_io::throw_posix_error(ENOENT);
	}

	/* pointer values depend on der_data's final allocation */
	inline void finalize() noexcept
	{
		for (::std::size_t i{}, off{}; i != sizes.size(); ++i)
		{
			ptrs[i] = der_data.data() + off;
			off += sizes[i];
		}
	}

	inline constexpr ::std::byte const *const *roots() const noexcept
	{
		return ptrs.data();
	}
	inline constexpr ::std::size_t const *root_sizes() const noexcept
	{
		return sizes.data();
	}
	inline constexpr ::std::size_t root_count() const noexcept
	{
		return sizes.size();
	}
};

using root_store = basic_root_store<>;

template <typename allocator_type, typename socket_observer_type>
inline void basic_tls_client<allocator_type, socket_observer_type>::handshake(::fast_io::u8cstring_view hostname) FAST_IO_HERBCEPTIONS_THROWS
{
	auto store{tls_alloc_construct<basic_root_store<allocator_type>, allocator_type>(allocator_handle)};
	store.load_system();
	tls13_client_config cfg{};
	cfg.hostname = hostname;
	cfg.roots = store.roots();
	cfg.root_sizes = store.root_sizes();
	cfg.root_count = store.root_count();
	handshake(cfg);
}

} // namespace fast_io::tls
