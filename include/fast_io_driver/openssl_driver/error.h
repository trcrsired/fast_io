#pragma once

namespace fast_io
{

/* OpenSSL failures are reported as std::tls_alert through the
   herbceptions channel. */
[[noreturn]] inline void throw_openssl_error() FAST_IO_HERBCEPTIONS_THROWS
{
#if defined(__HERBCEPTIONS__)
	throw throws::std::tls_alert{::std::tls_alert::alert_level::fatal,
								 ::std::tls_alert::alert_description::internal_error};
#else
	::fast_io::fast_terminate();
#endif
}

} // namespace fast_io
