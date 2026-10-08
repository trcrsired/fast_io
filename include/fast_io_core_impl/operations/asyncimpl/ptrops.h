#pragma once

namespace fast_io::operations
{

/*
 * Public async surface. Users name these; the decay infix names in
 * fast_io::operations::decay are for implementations that already hold
 * reduced types. Exactly like the sync layer, the handles are reduced
 * here — scheduler through async_scheduler_ref (owning ring → its
 * trivially-copyable observer), streams through input_stream_ref/
 * output_stream_ref (buffered stream → basic_io_buffer_ref) — and the
 * decay layer sees only small by-value types, so && qualifications never
 * multiply template instantiations.
 */

/* ------------------------------ pread ------------------------------ */

template <typename async_scheduler_type, typename instmtype, typename callback_type>
inline void async_pread_some_bytes_callback(async_scheduler_type &&scheduler, instmtype &&instm,
											::std::byte *first, ::std::size_t count,
											::fast_io::intfpos_opt off,
											::fast_io::posix_statx_timestamp_opt timeout,
											callback_type callback) noexcept
	requires(::fast_io::operations::decay::defines::
				 has_async_pread_some_bytes_underflow_callback_define<
					 ::std::remove_cvref_t<decltype(::fast_io::operations::async_scheduler_ref(scheduler))>,
					 ::std::remove_cvref_t<decltype(::fast_io::operations::input_stream_ref(instm))>,
					 ::std::remove_cvref_t<callback_type>>)
{
	::fast_io::operations::decay::async_pread_some_bytes_decay_callback(
		::fast_io::operations::async_scheduler_ref(scheduler),
		::fast_io::operations::input_stream_ref(instm), first, count, off, timeout,
		::std::move(callback));
}

template <typename async_scheduler_type, typename instmtype, typename callback_type>
inline void async_pread_all_bytes_callback(async_scheduler_type &&scheduler, instmtype &&instm,
										   ::std::byte *first, ::std::size_t count,
										   ::fast_io::intfpos_opt off,
										   ::fast_io::posix_statx_timestamp_opt timeout,
										   callback_type callback) noexcept
	requires(::fast_io::operations::decay::defines::
				 has_async_pread_some_bytes_underflow_callback_define<
					 ::std::remove_cvref_t<decltype(::fast_io::operations::async_scheduler_ref(scheduler))>,
					 ::std::remove_cvref_t<decltype(::fast_io::operations::input_stream_ref(instm))>,
					 decltype([](::std::cxx_std_error, ::std::size_t) noexcept {})>)
{
	::fast_io::operations::decay::async_pread_all_bytes_decay_callback(
		::fast_io::operations::async_scheduler_ref(scheduler),
		::fast_io::operations::input_stream_ref(instm), first, count, off, timeout,
		::std::move(callback));
}

template <typename async_scheduler_type, typename instmtype>
inline auto async_pread_some_bytes(async_scheduler_type &&scheduler, instmtype &&instm,
								   ::std::byte *first, ::std::size_t count,
								   ::fast_io::intfpos_opt off,
								   ::fast_io::posix_statx_timestamp_opt timeout) noexcept
	requires(::fast_io::operations::decay::defines::
				 has_async_pread_some_bytes_underflow_callback_define<
					 ::std::remove_cvref_t<decltype(::fast_io::operations::async_scheduler_ref(scheduler))>,
					 ::std::remove_cvref_t<decltype(::fast_io::operations::input_stream_ref(instm))>,
					 decltype([](::std::cxx_std_error, ::std::size_t) noexcept {})>)
{
	return ::fast_io::operations::decay::async_pread_some_bytes_decay(
		::fast_io::operations::async_scheduler_ref(scheduler),
		::fast_io::operations::input_stream_ref(instm), first, count, off, timeout);
}

template <typename async_scheduler_type, typename instmtype>
inline auto async_pread_all_bytes(async_scheduler_type &&scheduler, instmtype &&instm,
								  ::std::byte *first, ::std::size_t count,
								  ::fast_io::intfpos_opt off,
								  ::fast_io::posix_statx_timestamp_opt timeout) noexcept
	requires(::fast_io::operations::decay::defines::
				 has_async_pread_some_bytes_underflow_callback_define<
					 ::std::remove_cvref_t<decltype(::fast_io::operations::async_scheduler_ref(scheduler))>,
					 ::std::remove_cvref_t<decltype(::fast_io::operations::input_stream_ref(instm))>,
					 decltype([](::std::cxx_std_error, ::std::size_t) noexcept {})>)
{
	return ::fast_io::operations::decay::async_pread_all_bytes_decay(
		::fast_io::operations::async_scheduler_ref(scheduler),
		::fast_io::operations::input_stream_ref(instm), first, count, off, timeout);
}

/* ------------------------------ pwrite ----------------------------- */

template <typename async_scheduler_type, typename outstmtype, typename callback_type>
inline void async_pwrite_some_bytes_callback(async_scheduler_type &&scheduler, outstmtype &&outstm,
											 ::std::byte const *first, ::std::size_t count,
											 ::fast_io::intfpos_opt off,
											 ::fast_io::posix_statx_timestamp_opt timeout,
											 callback_type callback) noexcept
	requires(::fast_io::operations::decay::defines::
				 has_async_pwrite_some_bytes_overflow_callback_define<
					 ::std::remove_cvref_t<decltype(::fast_io::operations::async_scheduler_ref(scheduler))>,
					 ::std::remove_cvref_t<decltype(::fast_io::operations::output_stream_ref(outstm))>,
					 ::std::remove_cvref_t<callback_type>>)
{
	::fast_io::operations::decay::async_pwrite_some_bytes_decay_callback(
		::fast_io::operations::async_scheduler_ref(scheduler),
		::fast_io::operations::output_stream_ref(outstm), first, count, off, timeout,
		::std::move(callback));
}

template <typename async_scheduler_type, typename outstmtype, typename callback_type>
inline void async_pwrite_all_bytes_callback(async_scheduler_type &&scheduler, outstmtype &&outstm,
											::std::byte const *first, ::std::size_t count,
											::fast_io::intfpos_opt off,
											::fast_io::posix_statx_timestamp_opt timeout,
											callback_type callback) noexcept
	requires(::fast_io::operations::decay::defines::
				 has_async_pwrite_some_bytes_overflow_callback_define<
					 ::std::remove_cvref_t<decltype(::fast_io::operations::async_scheduler_ref(scheduler))>,
					 ::std::remove_cvref_t<decltype(::fast_io::operations::output_stream_ref(outstm))>,
					 decltype([](::std::cxx_std_error, ::std::size_t) noexcept {})>)
{
	::fast_io::operations::decay::async_pwrite_all_bytes_decay_callback(
		::fast_io::operations::async_scheduler_ref(scheduler),
		::fast_io::operations::output_stream_ref(outstm), first, count, off, timeout,
		::std::move(callback));
}

template <typename async_scheduler_type, typename outstmtype>
inline auto async_pwrite_some_bytes(async_scheduler_type &&scheduler, outstmtype &&outstm,
									::std::byte const *first, ::std::size_t count,
									::fast_io::intfpos_opt off,
									::fast_io::posix_statx_timestamp_opt timeout) noexcept
	requires(::fast_io::operations::decay::defines::
				 has_async_pwrite_some_bytes_overflow_callback_define<
					 ::std::remove_cvref_t<decltype(::fast_io::operations::async_scheduler_ref(scheduler))>,
					 ::std::remove_cvref_t<decltype(::fast_io::operations::output_stream_ref(outstm))>,
					 decltype([](::std::cxx_std_error, ::std::size_t) noexcept {})>)
{
	return ::fast_io::operations::decay::async_pwrite_some_bytes_decay(
		::fast_io::operations::async_scheduler_ref(scheduler),
		::fast_io::operations::output_stream_ref(outstm), first, count, off, timeout);
}

template <typename async_scheduler_type, typename outstmtype>
inline auto async_pwrite_all_bytes(async_scheduler_type &&scheduler, outstmtype &&outstm,
								   ::std::byte const *first, ::std::size_t count,
								   ::fast_io::intfpos_opt off,
								   ::fast_io::posix_statx_timestamp_opt timeout) noexcept
	requires(::fast_io::operations::decay::defines::
				 has_async_pwrite_some_bytes_overflow_callback_define<
					 ::std::remove_cvref_t<decltype(::fast_io::operations::async_scheduler_ref(scheduler))>,
					 ::std::remove_cvref_t<decltype(::fast_io::operations::output_stream_ref(outstm))>,
					 decltype([](::std::cxx_std_error, ::std::size_t) noexcept {})>)
{
	return ::fast_io::operations::decay::async_pwrite_all_bytes_decay(
		::fast_io::operations::async_scheduler_ref(scheduler),
		::fast_io::operations::output_stream_ref(outstm), first, count, off, timeout);
}

/* ------------------------------ scatter ---------------------------- */

template <typename async_scheduler_type, typename instmtype, typename callback_type>
inline void async_scatter_pread_some_bytes_callback(async_scheduler_type &&scheduler,
													instmtype &&instm,
													::fast_io::io_scatter_t const *pscatters,
													::std::size_t n, ::fast_io::intfpos_opt off,
													::fast_io::posix_statx_timestamp_opt timeout,
													callback_type callback) noexcept
	requires(::fast_io::operations::decay::defines::
				 has_async_scatter_pread_some_bytes_underflow_callback_define<
					 ::std::remove_cvref_t<decltype(::fast_io::operations::async_scheduler_ref(scheduler))>,
					 ::std::remove_cvref_t<decltype(::fast_io::operations::input_stream_ref(instm))>,
					 ::std::remove_cvref_t<callback_type>> ||
			 ::fast_io::operations::decay::defines::
				 has_async_pread_some_bytes_underflow_callback_define<
					 ::std::remove_cvref_t<decltype(::fast_io::operations::async_scheduler_ref(scheduler))>,
					 ::std::remove_cvref_t<decltype(::fast_io::operations::input_stream_ref(instm))>,
					 decltype([](::std::cxx_std_error, ::std::size_t) noexcept {})>)
{
	::fast_io::operations::decay::async_scatter_pread_some_bytes_decay_callback(
		::fast_io::operations::async_scheduler_ref(scheduler),
		::fast_io::operations::input_stream_ref(instm), pscatters, n, off, timeout,
		::std::move(callback));
}

template <typename async_scheduler_type, typename instmtype, typename callback_type>
inline void async_scatter_pread_all_bytes_callback(async_scheduler_type &&scheduler,
												   instmtype &&instm,
												   ::fast_io::io_scatter_t const *pscatters,
												   ::std::size_t n, ::fast_io::intfpos_opt off,
												   ::fast_io::posix_statx_timestamp_opt timeout,
												   callback_type callback) noexcept
	requires(::fast_io::operations::decay::defines::
				 has_async_pread_some_bytes_underflow_callback_define<
					 ::std::remove_cvref_t<decltype(::fast_io::operations::async_scheduler_ref(scheduler))>,
					 ::std::remove_cvref_t<decltype(::fast_io::operations::input_stream_ref(instm))>,
					 decltype([](::std::cxx_std_error, ::std::size_t) noexcept {})>)
{
	::fast_io::operations::decay::async_scatter_pread_all_bytes_decay_callback(
		::fast_io::operations::async_scheduler_ref(scheduler),
		::fast_io::operations::input_stream_ref(instm), pscatters, n, off, timeout,
		::std::move(callback));
}

template <typename async_scheduler_type, typename outstmtype, typename callback_type>
inline void async_scatter_pwrite_some_bytes_callback(async_scheduler_type &&scheduler,
													 outstmtype &&outstm,
													 ::fast_io::io_scatter_t const *pscatters,
													 ::std::size_t n, ::fast_io::intfpos_opt off,
													 ::fast_io::posix_statx_timestamp_opt timeout,
													 callback_type callback) noexcept
	requires(::fast_io::operations::decay::defines::
				 has_async_scatter_pwrite_some_bytes_overflow_callback_define<
					 ::std::remove_cvref_t<decltype(::fast_io::operations::async_scheduler_ref(scheduler))>,
					 ::std::remove_cvref_t<decltype(::fast_io::operations::output_stream_ref(outstm))>,
					 ::std::remove_cvref_t<callback_type>> ||
			 ::fast_io::operations::decay::defines::
				 has_async_pwrite_some_bytes_overflow_callback_define<
					 ::std::remove_cvref_t<decltype(::fast_io::operations::async_scheduler_ref(scheduler))>,
					 ::std::remove_cvref_t<decltype(::fast_io::operations::output_stream_ref(outstm))>,
					 decltype([](::std::cxx_std_error, ::std::size_t) noexcept {})>)
{
	::fast_io::operations::decay::async_scatter_pwrite_some_bytes_decay_callback(
		::fast_io::operations::async_scheduler_ref(scheduler),
		::fast_io::operations::output_stream_ref(outstm), pscatters, n, off, timeout,
		::std::move(callback));
}

template <typename async_scheduler_type, typename outstmtype, typename callback_type>
inline void async_scatter_pwrite_all_bytes_callback(async_scheduler_type &&scheduler,
													outstmtype &&outstm,
													::fast_io::io_scatter_t const *pscatters,
													::std::size_t n, ::fast_io::intfpos_opt off,
													::fast_io::posix_statx_timestamp_opt timeout,
													callback_type callback) noexcept
	requires(::fast_io::operations::decay::defines::
				 has_async_pwrite_some_bytes_overflow_callback_define<
					 ::std::remove_cvref_t<decltype(::fast_io::operations::async_scheduler_ref(scheduler))>,
					 ::std::remove_cvref_t<decltype(::fast_io::operations::output_stream_ref(outstm))>,
					 decltype([](::std::cxx_std_error, ::std::size_t) noexcept {})>)
{
	::fast_io::operations::decay::async_scatter_pwrite_all_bytes_decay_callback(
		::fast_io::operations::async_scheduler_ref(scheduler),
		::fast_io::operations::output_stream_ref(outstm), pscatters, n, off, timeout,
		::std::move(callback));
}

template <typename async_scheduler_type, typename instmtype>
inline auto async_scatter_pread_some_bytes(async_scheduler_type &&scheduler, instmtype &&instm,
										   ::fast_io::io_scatter_t const *pscatters,
										   ::std::size_t n, ::fast_io::intfpos_opt off,
										   ::fast_io::posix_statx_timestamp_opt timeout) noexcept
	requires(::fast_io::operations::decay::defines::
				 has_async_scatter_pread_some_bytes_underflow_callback_define<
					 ::std::remove_cvref_t<decltype(::fast_io::operations::async_scheduler_ref(scheduler))>,
					 ::std::remove_cvref_t<decltype(::fast_io::operations::input_stream_ref(instm))>,
					 decltype([](::std::cxx_std_error, ::fast_io::io_scatter_status_t) noexcept {})> ||
			 ::fast_io::operations::decay::defines::
				 has_async_pread_some_bytes_underflow_callback_define<
					 ::std::remove_cvref_t<decltype(::fast_io::operations::async_scheduler_ref(scheduler))>,
					 ::std::remove_cvref_t<decltype(::fast_io::operations::input_stream_ref(instm))>,
					 decltype([](::std::cxx_std_error, ::std::size_t) noexcept {})>)
{
	return ::fast_io::operations::decay::async_scatter_pread_some_bytes_decay(
		::fast_io::operations::async_scheduler_ref(scheduler),
		::fast_io::operations::input_stream_ref(instm), pscatters, n, off, timeout);
}

template <typename async_scheduler_type, typename instmtype>
inline auto async_scatter_pread_all_bytes(async_scheduler_type &&scheduler, instmtype &&instm,
										  ::fast_io::io_scatter_t const *pscatters,
										  ::std::size_t n, ::fast_io::intfpos_opt off,
										  ::fast_io::posix_statx_timestamp_opt timeout) noexcept
	requires(::fast_io::operations::decay::defines::
				 has_async_pread_some_bytes_underflow_callback_define<
					 ::std::remove_cvref_t<decltype(::fast_io::operations::async_scheduler_ref(scheduler))>,
					 ::std::remove_cvref_t<decltype(::fast_io::operations::input_stream_ref(instm))>,
					 decltype([](::std::cxx_std_error, ::std::size_t) noexcept {})>)
{
	return ::fast_io::operations::decay::async_scatter_pread_all_bytes_decay(
		::fast_io::operations::async_scheduler_ref(scheduler),
		::fast_io::operations::input_stream_ref(instm), pscatters, n, off, timeout);
}

template <typename async_scheduler_type, typename outstmtype>
inline auto async_scatter_pwrite_some_bytes(async_scheduler_type &&scheduler, outstmtype &&outstm,
											::fast_io::io_scatter_t const *pscatters,
											::std::size_t n, ::fast_io::intfpos_opt off,
											::fast_io::posix_statx_timestamp_opt timeout) noexcept
	requires(::fast_io::operations::decay::defines::
				 has_async_scatter_pwrite_some_bytes_overflow_callback_define<
					 ::std::remove_cvref_t<decltype(::fast_io::operations::async_scheduler_ref(scheduler))>,
					 ::std::remove_cvref_t<decltype(::fast_io::operations::output_stream_ref(outstm))>,
					 decltype([](::std::cxx_std_error, ::fast_io::io_scatter_status_t) noexcept {})> ||
			 ::fast_io::operations::decay::defines::
				 has_async_pwrite_some_bytes_overflow_callback_define<
					 ::std::remove_cvref_t<decltype(::fast_io::operations::async_scheduler_ref(scheduler))>,
					 ::std::remove_cvref_t<decltype(::fast_io::operations::output_stream_ref(outstm))>,
					 decltype([](::std::cxx_std_error, ::std::size_t) noexcept {})>)
{
	return ::fast_io::operations::decay::async_scatter_pwrite_some_bytes_decay(
		::fast_io::operations::async_scheduler_ref(scheduler),
		::fast_io::operations::output_stream_ref(outstm), pscatters, n, off, timeout);
}

template <typename async_scheduler_type, typename outstmtype>
inline auto async_scatter_pwrite_all_bytes(async_scheduler_type &&scheduler, outstmtype &&outstm,
										   ::fast_io::io_scatter_t const *pscatters,
										   ::std::size_t n, ::fast_io::intfpos_opt off,
										   ::fast_io::posix_statx_timestamp_opt timeout) noexcept
	requires(::fast_io::operations::decay::defines::
				 has_async_pwrite_some_bytes_overflow_callback_define<
					 ::std::remove_cvref_t<decltype(::fast_io::operations::async_scheduler_ref(scheduler))>,
					 ::std::remove_cvref_t<decltype(::fast_io::operations::output_stream_ref(outstm))>,
					 decltype([](::std::cxx_std_error, ::std::size_t) noexcept {})>)
{
	return ::fast_io::operations::decay::async_scatter_pwrite_all_bytes_decay(
		::fast_io::operations::async_scheduler_ref(scheduler),
		::fast_io::operations::output_stream_ref(outstm), pscatters, n, off, timeout);
}

/* ----------------------------- transmit ---------------------------- */

template <typename async_scheduler_type, typename outstmtype, typename instmtype,
		  typename callback_type>
inline void async_transmit_some_bytes_callback(async_scheduler_type &&scheduler, outstmtype &&outstm,
											   ::fast_io::intfpos_opt off_out, instmtype &&instm,
											   ::fast_io::intfpos_opt off_in,
											   ::fast_io::size_t_opt bound,
											   ::fast_io::posix_statx_timestamp_opt timeout,
											   callback_type callback) noexcept
{
	::fast_io::operations::decay::async_transmit_some_bytes_decay_callback(
		::fast_io::operations::async_scheduler_ref(scheduler),
		::fast_io::operations::output_stream_ref(outstm), off_out,
		::fast_io::operations::input_stream_ref(instm), off_in, bound, timeout,
		::std::move(callback));
}

template <typename async_scheduler_type, typename outstmtype, typename instmtype,
		  typename callback_type>
inline void async_transmit_all_bytes_callback(async_scheduler_type &&scheduler, outstmtype &&outstm,
											  ::fast_io::intfpos_opt off_out, instmtype &&instm,
											  ::fast_io::intfpos_opt off_in,
											  ::fast_io::size_t_opt bound,
											  ::fast_io::posix_statx_timestamp_opt timeout,
											  callback_type callback) noexcept
{
	::fast_io::operations::decay::async_transmit_all_bytes_decay_callback(
		::fast_io::operations::async_scheduler_ref(scheduler),
		::fast_io::operations::output_stream_ref(outstm), off_out,
		::fast_io::operations::input_stream_ref(instm), off_in, bound, timeout,
		::std::move(callback));
}

template <typename async_scheduler_type, typename outstmtype, typename instmtype>
inline auto async_transmit_some_bytes(async_scheduler_type &&scheduler, outstmtype &&outstm,
									  ::fast_io::intfpos_opt off_out, instmtype &&instm,
									  ::fast_io::intfpos_opt off_in, ::fast_io::size_t_opt bound,
									  ::fast_io::posix_statx_timestamp_opt timeout) noexcept
{
	return ::fast_io::operations::decay::async_transmit_some_bytes_decay(
		::fast_io::operations::async_scheduler_ref(scheduler),
		::fast_io::operations::output_stream_ref(outstm), off_out,
		::fast_io::operations::input_stream_ref(instm), off_in, bound, timeout);
}

template <typename async_scheduler_type, typename outstmtype, typename instmtype>
inline auto async_transmit_all_bytes(async_scheduler_type &&scheduler, outstmtype &&outstm,
									 ::fast_io::intfpos_opt off_out, instmtype &&instm,
									 ::fast_io::intfpos_opt off_in, ::fast_io::size_t_opt bound,
									 ::fast_io::posix_statx_timestamp_opt timeout) noexcept
{
	return ::fast_io::operations::decay::async_transmit_all_bytes_decay(
		::fast_io::operations::async_scheduler_ref(scheduler),
		::fast_io::operations::output_stream_ref(outstm), off_out,
		::fast_io::operations::input_stream_ref(instm), off_in, bound, timeout);
}

/* ------------------------ flush / scan_get ------------------------- */

template <typename async_scheduler_type, typename outstmtype, typename callback_type>
inline void async_output_stream_flush_callback(async_scheduler_type &&scheduler, outstmtype &&outstm,
											   ::fast_io::posix_statx_timestamp_opt timeout,
											   callback_type callback) noexcept
	requires(::fast_io::operations::decay::defines::
				 has_async_output_stream_buffer_flush_define<
					 ::std::remove_cvref_t<decltype(::fast_io::operations::async_scheduler_ref(scheduler))>,
					 ::std::remove_cvref_t<decltype(::fast_io::operations::output_stream_ref(outstm))>,
					 ::std::remove_cvref_t<callback_type>>)
{
	::fast_io::operations::decay::async_output_stream_flush_decay_callback(
		::fast_io::operations::async_scheduler_ref(scheduler),
		::fast_io::operations::output_stream_ref(outstm), timeout, ::std::move(callback));
}

template <typename async_scheduler_type, typename outstmtype>
inline auto async_output_stream_flush(async_scheduler_type &&scheduler, outstmtype &&outstm,
									  ::fast_io::posix_statx_timestamp_opt timeout) noexcept
	requires(::fast_io::operations::decay::defines::
				 has_async_output_stream_buffer_flush_define<
					 ::std::remove_cvref_t<decltype(::fast_io::operations::async_scheduler_ref(scheduler))>,
					 ::std::remove_cvref_t<decltype(::fast_io::operations::output_stream_ref(outstm))>,
					 decltype([](::std::cxx_std_error) noexcept {})>)
{
	return ::fast_io::operations::decay::async_output_stream_flush_decay(
		::fast_io::operations::async_scheduler_ref(scheduler),
		::fast_io::operations::output_stream_ref(outstm), timeout);
}

template <typename T, typename async_scheduler_type, typename instmtype>
inline auto async_scan_get(async_scheduler_type &&scheduler, instmtype &&instm,
						   ::fast_io::posix_statx_timestamp_opt timeout) noexcept
	requires(::fast_io::operations::decay::defines::async_scheduler_observer<
			 ::std::remove_cvref_t<decltype(::fast_io::operations::async_scheduler_ref(scheduler))>>)
{
	return ::fast_io::operations::decay::async_scan_get<T>(
		::fast_io::operations::async_scheduler_ref(scheduler),
		::fast_io::operations::input_stream_ref(instm), timeout);
}

/* ------------------------------ accept ------------------------------ */

/*
 * async_accept: co_await suspends until a connection arrives and yields
 * the accepted socket's native handle — int on posix, SOCKET on win32.
 * Wrap it in native_socket_file:
 *     fi::native_socket_file s{co_await fop::async_accept(sched, listener)};
 * mode describes the accepted socket (no_block marks it async-capable;
 * required for IOCP sockets, ignored on posix — io_uring needs no flag).
 */
template <typename async_scheduler_type, typename streamtype>
inline auto async_accept(async_scheduler_type &&scheduler, streamtype &&listenstm,
						 ::fast_io::open_mode mode = ::fast_io::open_mode{},
						 ::fast_io::posix_statx_timestamp_opt timeout = {}) noexcept
	requires(::fast_io::operations::decay::defines::has_async_accept_callback_define<
			 ::std::remove_cvref_t<decltype(::fast_io::operations::async_scheduler_ref(scheduler))>,
			 ::std::remove_cvref_t<decltype(::fast_io::operations::io_stream_ref(listenstm))>,
			 decltype([](::std::cxx_std_error,
						 typename ::std::remove_cvref_t<
							 decltype(::fast_io::operations::io_stream_ref(listenstm))>::
							 native_handle_type) noexcept {})>)
{
	return ::fast_io::operations::decay::async_accept_decay(
		::fast_io::operations::async_scheduler_ref(scheduler),
		::fast_io::operations::io_stream_ref(listenstm), mode, timeout);
}

template <typename async_scheduler_type, typename streamtype, typename callback_type>
inline void async_accept_callback(async_scheduler_type &&scheduler, streamtype &&listenstm,
								  ::fast_io::open_mode mode,
								  ::fast_io::posix_statx_timestamp_opt timeout,
								  callback_type callback) noexcept
	requires(::fast_io::operations::decay::defines::has_async_accept_callback_define<
			 ::std::remove_cvref_t<decltype(::fast_io::operations::async_scheduler_ref(scheduler))>,
			 ::std::remove_cvref_t<decltype(::fast_io::operations::io_stream_ref(listenstm))>,
			 ::std::remove_cvref_t<callback_type>>)
{
	::fast_io::operations::decay::async_accept_decay_callback(
		::fast_io::operations::async_scheduler_ref(scheduler),
		::fast_io::operations::io_stream_ref(listenstm), mode, timeout, ::std::move(callback));
}

} // namespace fast_io::operations
