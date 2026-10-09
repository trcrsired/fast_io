#pragma once

/*
trust-root store: decodes every CERTIFICATE block from a PEM bundle
into DER and presents them in the flat array form tls13_client_config
wants. Hosted only (reads files via native_file_loader).
*/

#if defined(__linux__)

namespace fast_io::tls
{

class root_store
{
	::fast_io::vector<::std::byte> der_data{};
	::fast_io::vector<::std::byte const *> ptrs{};
	::fast_io::vector<::std::size_t> sizes{};

public:
	/* append every CERTIFICATE pem block from [pem, pem+pem_size) */
	inline void load_pem_text(char8_t const *pem, ::std::size_t pem_size) FAST_IO_HERBCEPTIONS_THROWS
	{
		char8_t const *cur{pem};
		char8_t const *const end{pem + pem_size};
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
	inline void load_pem_file(char const *path) FAST_IO_HERBCEPTIONS_THROWS
	{
		::fast_io::native_file_loader f{::fast_io::mnp::os_c_str(path)};
		load_pem_text(reinterpret_cast<char8_t const *>(f.data()), f.size());
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

} // namespace fast_io::tls

#endif
