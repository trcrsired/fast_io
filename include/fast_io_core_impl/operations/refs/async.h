#pragma once

namespace fast_io::operations::decay::defines
{

/*
A scheduler observer: the non-owning view of an async submission/completion
engine (io_uring observer, IOCP handle, ...). Every async operation takes it
as the first parameter.
*/
template <typename T>
concept async_scheduler_observer = requires(::std::remove_cvref_t<T> sched) {
	typename ::std::remove_cvref_t<T>::native_handle_type;
	{
		sched.native_handle()
	} -> ::std::same_as<typename ::std::remove_cvref_t<T>::native_handle_type>;
};

/*
Completion callback contracts. `some` operations report bytes (or scatter
progress); `all` operations report only the terminal error. Callbacks are
invoked exactly once and are noexcept.
*/
template <typename func>
concept async_bytes_completion_callback =
	::std::is_nothrow_invocable_v<func, ::std::cxx_std_error, ::std::size_t>;

template <typename func>
concept async_completion_callback = ::std::is_nothrow_invocable_v<func, ::std::cxx_std_error>;

template <typename func>
concept async_scatter_completion_callback =
	::std::is_nothrow_invocable_v<func, ::std::cxx_std_error, ::fast_io::io_scatter_status_t>;

/*
Backend defines. Each scheduler provides these in an associated namespace of
its scheduler type so generic decay functions find them by ADL:

- async_pread_some_bytes_underflow_callback_define /
  async_pwrite_some_bytes_overflow_callback_define are required; everything
  else is built on them generically.
- the scatter defines are optional fast paths (single kernel operation, e.g.
  io_uring READV/WRITEV); the generic layer emulates them element-wise.
- async_transmit_some_bytes_overflow_underflow_callback_define is an
  optional fast path (e.g. io_uring SPLICE); the generic layer emulates it
  with read/write chains.

The _underflow/_overflow infix marks that these defines sit beneath the
buffering layer — for a buffered stream type the same-named define on the
buffered reference (io_buffer/{input,output}_async.h) handles the buffering
itself and delegates to the handle's define only when the device must be
touched.

Defines are noexcept: every error — including submission failures — is
delivered through the callback as ::std::cxx_std_error{domain, code};
domain == nullptr means success (not an error at all). `off` is
intfpos_opt (empty = current file position); `timeout` is
posix_statx_timestamp_opt (empty = none) — the timestamp type doubles as a
duration — and generic `all` loops forward it unchanged to every
resubmission.

Error ownership: a cxx_std_error parameter is an owning handle — the
callee takes ownership of the {domain, code} payload, exactly as
std::error::release() hands it out across the ABI boundary. A callback
that forwards the value transfers ownership onward; the terminal owner
either `throw throws`es it (the error channel reclaims it) or runs its
domain's do_cleanup — details::async_dispose_error / the
cxx_std_error_guard RAII helper do this. This matters for resource-
carrying domains (e.g. a boxed C++ exception handle): dropping such an
error without cleanup leaks the payload.
*/

template <typename schedulertype, typename instmtype, typename functype>
concept has_async_pread_some_bytes_underflow_callback_define =
	requires(schedulertype sched, instmtype instm, functype func) {
		{
			async_pread_some_bytes_underflow_callback_define(
				sched, instm, static_cast<::std::byte *>(nullptr), ::std::size_t{0},
				::fast_io::intfpos_opt{}, ::fast_io::posix_statx_timestamp_opt{}, func)
		} noexcept;
	};

template <typename schedulertype, typename outstmtype, typename functype>
concept has_async_pwrite_some_bytes_overflow_callback_define =
	requires(schedulertype sched, outstmtype outstm, functype func) {
		{
			async_pwrite_some_bytes_overflow_callback_define(
				sched, outstm, static_cast<::std::byte const *>(nullptr), ::std::size_t{0},
				::fast_io::intfpos_opt{}, ::fast_io::posix_statx_timestamp_opt{}, func)
		} noexcept;
	};

template <typename schedulertype, typename instmtype, typename functype>
concept has_async_scatter_pread_some_bytes_underflow_callback_define =
	requires(schedulertype sched, instmtype instm, functype func) {
		{
			async_scatter_pread_some_bytes_underflow_callback_define(
				sched, instm, static_cast<::fast_io::io_scatter_t const *>(nullptr), ::std::size_t{0},
				::fast_io::intfpos_opt{}, ::fast_io::posix_statx_timestamp_opt{}, func)
		} noexcept;
	};

template <typename schedulertype, typename outstmtype, typename functype>
concept has_async_scatter_pwrite_some_bytes_overflow_callback_define =
	requires(schedulertype sched, outstmtype outstm, functype func) {
		{
			async_scatter_pwrite_some_bytes_overflow_callback_define(
				sched, outstm, static_cast<::fast_io::io_scatter_t const *>(nullptr), ::std::size_t{0},
				::fast_io::intfpos_opt{}, ::fast_io::posix_statx_timestamp_opt{}, func)
		} noexcept;
	};

template <typename schedulertype, typename outstmtype, typename instmtype, typename functype>
concept has_async_transmit_some_bytes_overflow_underflow_callback_define =
	requires(schedulertype sched, outstmtype outstm, instmtype instm, functype func) {
		{
			async_transmit_some_bytes_overflow_underflow_callback_define(
				sched, outstm, ::fast_io::intfpos_opt{}, instm, ::fast_io::intfpos_opt{},
				::fast_io::size_t_opt{}, ::fast_io::posix_statx_timestamp_opt{}, func)
		} noexcept;
	};

/*
Buffered-stream defines — both live beneath their public wrappers and are
likewise noexcept with callback-delivered errors:

- async_ibuffer_underflow(sched, instm, timeout, cb): refill the input
  buffer's pending window; cb(err, n) reports the bytes made available
  (n == 0 = end of file). The buffering equivalent of
  ibuffer_underflow(iobref).
- async_output_stream_buffer_flush_define(sched, outstm, timeout, cb):
  write every pending output byte to the underlying handle; cb(err).
*/
template <typename schedulertype, typename instmtype, typename functype>
concept has_async_ibuffer_underflow = requires(schedulertype sched, instmtype instm, functype func) {
	{
		async_ibuffer_underflow(sched, instm, ::fast_io::posix_statx_timestamp_opt{}, func)
	} noexcept;
};

template <typename schedulertype, typename outstmtype, typename functype>
concept has_async_output_stream_buffer_flush_define =
	requires(schedulertype sched, outstmtype outstm, functype func) {
		{
			async_output_stream_buffer_flush_define(sched, outstm,
													::fast_io::posix_statx_timestamp_opt{}, func)
		} noexcept;
	};

template <typename schedulertype, typename instmtype>
concept async_preadable =
	::fast_io::operations::decay::defines::async_scheduler_observer<schedulertype> &&
	::fast_io::operations::decay::defines::has_async_pread_some_bytes_underflow_callback_define<
		schedulertype, instmtype,
		decltype([](::std::cxx_std_error, ::std::size_t) noexcept {})>;

template <typename schedulertype, typename outstmtype>
concept async_pwritable =
	::fast_io::operations::decay::defines::async_scheduler_observer<schedulertype> &&
	::fast_io::operations::decay::defines::has_async_pwrite_some_bytes_overflow_callback_define<
		schedulertype, outstmtype,
		decltype([](::std::cxx_std_error, ::std::size_t) noexcept {})>;

} // namespace fast_io::operations::decay::defines
