#pragma once

namespace fast_io
{

namespace details::io_buffer
{

/*
 * Buffered async write state for the overflow path: the user range does
 * not fit the remaining window, so pending bytes are flushed to the
 * handle asynchronously first; the completion then either buffers the
 * range or submits it to the device directly.
 */
template <typename scheduler, typename io_buffer_type, typename alloc_type, typename T>
struct async_iobuffer_pwrite_state
{
	using allocator_type = alloc_type;
	static inline constexpr bool alloc_with_status{alloc_type::has_status};
	scheduler sched;
	::fast_io::basic_io_buffer_ref<io_buffer_type> iobref;
	::std::byte const *first;
	::std::size_t count;
	::fast_io::posix_statx_timestamp_opt timeout;
	T callback;
	FAST_IO_NO_UNIQUE_ADDRESS ::std::conditional_t<alloc_with_status, typename alloc_type::handle_type,
											 ::fast_io::details::empty>
		alloc_handle{};
};

template <typename state_type>
inline void async_iobuffer_pwrite_deliver(state_type *state, ::std::cxx_std_error err,
										  ::std::size_t n) noexcept
{
	auto callback{::std::move(state->callback)};
	::fast_io::details::async_delete_state(state);
	callback(err, n);
}

template <typename scheduler, typename io_buffer_type, typename alloc_type, typename T>
inline void async_iobuffer_pwrite_submit(
	async_iobuffer_pwrite_state<scheduler, io_buffer_type, alloc_type, T> *state) noexcept
{
	auto iobref{state->iobref};
	auto &obuffer{iobref.iobptr->output_buffer};
	::std::size_t const pending{
		static_cast<::std::size_t>(obuffer.buffer_curr - obuffer.buffer_begin)};
	if (pending == 0)
	{
		/* nothing to flush: count >= buffer_size here, so the range goes
		 * straight to the device */
		auto sched{state->sched};
		auto outstm{::fast_io::operations::output_stream_ref(iobref.iobptr->handle)};
		auto first{state->first};
		auto count{state->count};
		auto timeout{state->timeout};
		auto callback{::std::move(state->callback)};
		::fast_io::details::async_delete_state(state);
		async_pwrite_some_bytes_overflow_callback_define(sched, timeout, outstm, first, count,
														 ::fast_io::intfpos_opt{},
														 ::std::move(callback));
		return;
	}
	::fast_io::operations::decay::async_pwrite_all_bytes_decay_callback(
		state->sched, state->timeout,
		::fast_io::operations::output_stream_ref(iobref.iobptr->handle),
		reinterpret_cast<::std::byte const *>(obuffer.buffer_begin), pending,
		::fast_io::intfpos_opt{},
		[state](::std::cxx_std_error err) noexcept {
			if (err.domain != nullptr) [[unlikely]]
			{
				async_iobuffer_pwrite_deliver(state, err, 0);
				return;
			}
			auto iobref{state->iobref};
			auto &obuffer{iobref.iobptr->output_buffer};
			obuffer.buffer_curr = obuffer.buffer_begin;
			if (state->count <= io_buffer_type::traits_type::output_buffer_size)
			{
				/* the now-empty window takes the whole range without
				 * touching the device — same as a fresh sync write */
				::fast_io::details::non_overlapped_copy_n(
					state->first, state->count,
					reinterpret_cast<::std::byte *>(obuffer.buffer_begin));
				obuffer.buffer_curr = obuffer.buffer_begin + state->count;
				async_iobuffer_pwrite_deliver(state, ::std::cxx_std_error{}, state->count);
				return;
			}
			auto sched{state->sched};
			auto outstm{::fast_io::operations::output_stream_ref(iobref.iobptr->handle)};
			auto first{state->first};
			auto count{state->count};
			auto timeout{state->timeout};
			auto callback{::std::move(state->callback)};
			::fast_io::details::async_delete_state(state);
			async_pwrite_some_bytes_overflow_callback_define(sched, timeout, outstm, first, count,
															 ::fast_io::intfpos_opt{},
															 ::std::move(callback));
		});
}

/*
 * Buffered async transmit state: pending output must reach the device
 * before the transfer runs — the sync transmit prelude flushes the
 * obuffer and writes the payload straight to the handle; never stage the
 * payload in the buffer.
 */
template <typename scheduler, typename io_buffer_type, typename instmtype, typename alloc_type,
		  typename T>
struct async_iobuffer_transmit_state
{
	using allocator_type = alloc_type;
	static inline constexpr bool alloc_with_status{alloc_type::has_status};
	scheduler sched;
	::fast_io::basic_io_buffer_ref<io_buffer_type> iobref;
	::fast_io::intfpos_opt off_out;
	instmtype instm;
	::fast_io::intfpos_opt off_in;
	::fast_io::size_t_opt bound;
	::fast_io::posix_statx_timestamp_opt timeout;
	T callback;
	FAST_IO_NO_UNIQUE_ADDRESS ::std::conditional_t<alloc_with_status, typename alloc_type::handle_type,
											 ::fast_io::details::empty>
		alloc_handle{};
};

template <typename state_type>
inline void async_iobuffer_transmit_dispatch(state_type *state) noexcept
{
	auto iobref{state->iobref};
	auto &obuffer{iobref.iobptr->output_buffer};
	::std::size_t const pending{
		static_cast<::std::size_t>(obuffer.buffer_curr - obuffer.buffer_begin)};
	if (pending == 0)
	{
		/* obuffer empty on the device — run the transfer on the handle */
		auto sched{state->sched};
		auto outstm{::fast_io::operations::output_stream_ref(iobref.iobptr->handle)};
		auto off_out{state->off_out};
		auto instm{::std::move(state->instm)};
		auto off_in{state->off_in};
		auto bound{state->bound};
		auto timeout{state->timeout};
		auto callback{::std::move(state->callback)};
		::fast_io::details::async_delete_state(state);
		::fast_io::operations::decay::async_transmit_some_bytes_decay_callback(
			sched, timeout, outstm, off_out, instm, off_in, bound, ::std::move(callback));
		return;
	}
	::fast_io::operations::decay::async_pwrite_all_bytes_decay_callback(
		state->sched, state->timeout,
		::fast_io::operations::output_stream_ref(iobref.iobptr->handle),
		reinterpret_cast<::std::byte const *>(obuffer.buffer_begin), pending,
		::fast_io::intfpos_opt{},
		[state](::std::cxx_std_error err) noexcept {
			if (err.domain != nullptr) [[unlikely]]
			{
				auto callback{::std::move(state->callback)};
				::fast_io::details::async_delete_state(state);
				callback(err, 0);
				return;
			}
			auto &ob{state->iobref.iobptr->output_buffer};
			ob.buffer_curr = ob.buffer_begin;
			async_iobuffer_transmit_dispatch(state);
		});
}

} // namespace details::io_buffer

/*
 * async_output_stream_buffer_flush_define for basic_io_buffer_ref: write
 * every pending output byte to the handle with the generic pwrite_all
 * chain, then reset the window and report. cb(::std::cxx_std_error) —
 * err.domain == nullptr means the buffer is now empty on the device.
 */
template <typename io_buffer_type, typename scheduler, typename T>
	requires((io_buffer_type::traits_type::mode & ::fast_io::buffer_mode::out) ==
				 ::fast_io::buffer_mode::out &&
			 sizeof(typename io_buffer_type::output_char_type) == 1 &&
			 ::fast_io::operations::decay::defines::async_completion_callback<T>)
inline void async_output_stream_buffer_flush_define(
	scheduler sched, ::fast_io::posix_statx_timestamp_opt timeout,
	::fast_io::basic_io_buffer_ref<io_buffer_type> iobref, T callback) noexcept
{
	auto &obuffer{iobref.iobptr->output_buffer};
	::std::size_t const pending{
		static_cast<::std::size_t>(obuffer.buffer_curr - obuffer.buffer_begin)};
	if (pending == 0)
	{
		callback(::std::cxx_std_error{});
		return;
	}
	::fast_io::operations::decay::async_pwrite_all_bytes_decay_callback(
		sched, timeout, ::fast_io::operations::output_stream_ref(iobref.iobptr->handle),
		reinterpret_cast<::std::byte const *>(obuffer.buffer_begin), pending,
		::fast_io::intfpos_opt{},
		[iobref, callback{::std::move(callback)}](::std::cxx_std_error err) noexcept {
			if (err.domain == nullptr)
			{
				auto &obuffer{iobref.iobptr->output_buffer};
				obuffer.buffer_curr = obuffer.buffer_begin;
			}
			callback(err);
		});
}

/*
 * Buffered async pwrite define — the async analog of
 * write_some_overflow_define: when the window can take the range the
 * bytes are copied into the output buffer and the callback reports them
 * immediately; the device is not touched. Overflow flushes pending bytes
 * to the handle asynchronously, then buffers or writes the range.
 * Explicit offsets delegate to the handle without disturbing the window,
 * like the sync pwrite overflow define. Bytes sitting in the buffer are
 * only guaranteed on the device after async_output_stream_flush (or a
 * tied read — iobuf flushes before reading).
 */
template <typename io_buffer_type, typename scheduler, typename T>
	requires((io_buffer_type::traits_type::mode & ::fast_io::buffer_mode::out) ==
				 ::fast_io::buffer_mode::out &&
			 sizeof(typename io_buffer_type::output_char_type) == 1 &&
			 ::fast_io::operations::decay::defines::async_bytes_completion_callback<T>)
inline void async_pwrite_some_bytes_overflow_callback_define(
	scheduler sched, ::fast_io::posix_statx_timestamp_opt timeout,
	::fast_io::basic_io_buffer_ref<io_buffer_type> iobref,
	::std::byte const *first, ::std::size_t count, ::fast_io::intfpos_opt off,
	T callback) noexcept
{
	if (off.has_opt)
	{
		async_pwrite_some_bytes_overflow_callback_define(
			sched, timeout, ::fast_io::operations::output_stream_ref(iobref.iobptr->handle),
			first, count, off, ::std::move(callback));
		return;
	}
	if (count == 0)
	{
		callback(::std::cxx_std_error{}, 0);
		return;
	}
	auto &obuffer{iobref.iobptr->output_buffer};
	constexpr ::std::size_t bfsz{io_buffer_type::traits_type::output_buffer_size};
	if (obuffer.buffer_begin == nullptr)
	{
		if (count < bfsz)
		{
			try
			{
				using char_type = typename io_buffer_type::output_char_type;
				auto begin{::fast_io::details::io_buffer::iobuffer_allocate<
					char_type, typename io_buffer_type::traits_type::allocator_type>(
					iobref.iobptr->allocator_handle, bfsz)};
				::fast_io::details::non_overlapped_copy_n(first, count,
														reinterpret_cast<::std::byte *>(begin));
				obuffer.buffer_begin = begin;
				obuffer.buffer_curr = begin + count;
				obuffer.buffer_end = begin + bfsz;
				callback(::std::cxx_std_error{}, count);
			}
			catch throws(::std::error e)
			{
				callback(e.release(), 0);
			}
			return;
		}
		async_pwrite_some_bytes_overflow_callback_define(
			sched, timeout, ::fast_io::operations::output_stream_ref(iobref.iobptr->handle),
			first, count, off, ::std::move(callback));
		return;
	}
	::std::size_t const space{
		static_cast<::std::size_t>(obuffer.buffer_end - obuffer.buffer_curr)};
	if (space >= count)
	{
		::fast_io::details::non_overlapped_copy_n(
			first, count, reinterpret_cast<::std::byte *>(obuffer.buffer_curr));
		obuffer.buffer_curr += count;
		callback(::std::cxx_std_error{}, count);
		return;
	}
	if (obuffer.buffer_curr == obuffer.buffer_begin)
	{
		/* window empty and the range cannot fit — straight to device */
		async_pwrite_some_bytes_overflow_callback_define(
			sched, timeout, ::fast_io::operations::output_stream_ref(iobref.iobptr->handle),
			first, count, off, ::std::move(callback));
		return;
	}
	using alloc_type = ::fast_io::details::async_scheduler_allocator_t<scheduler>;
	try
	{
		::fast_io::details::io_buffer::async_iobuffer_pwrite_submit(
			::fast_io::details::async_new_state<
				::fast_io::details::io_buffer::async_iobuffer_pwrite_state<
					scheduler, io_buffer_type, alloc_type, ::std::remove_cvref_t<T>>>(
				sched, iobref, first, count, timeout, ::std::move(callback)));
	}
	catch throws(::std::error e)
	{
		callback(e.release(), 0);
	}
}

/*
 * Buffered async transmit define: pending output is flushed to the handle
 * first — the sync transmit prelude flushes the obuffer and then writes
 * the payload straight to the device; the transfer never stages through
 * the buffer. Input buffering is handled by the input side's own
 * pread-underflow define during the delegated transfer.
 */
template <typename io_buffer_type, typename scheduler, typename instmtype, typename T>
	requires((io_buffer_type::traits_type::mode & ::fast_io::buffer_mode::out) ==
				 ::fast_io::buffer_mode::out &&
			 sizeof(typename io_buffer_type::output_char_type) == 1 &&
			 ::fast_io::operations::decay::defines::async_bytes_completion_callback<T>)
inline void async_transmit_some_bytes_overflow_underflow_callback_define(
	scheduler sched, ::fast_io::posix_statx_timestamp_opt timeout,
	::fast_io::basic_io_buffer_ref<io_buffer_type> iobref,
	::fast_io::intfpos_opt off_out, instmtype instm, ::fast_io::intfpos_opt off_in,
	::fast_io::size_t_opt bound, T callback) noexcept
{
	auto &obuffer{iobref.iobptr->output_buffer};
	if (obuffer.buffer_curr == obuffer.buffer_begin)
	{
		::fast_io::operations::decay::async_transmit_some_bytes_decay_callback(
			sched, timeout, ::fast_io::operations::output_stream_ref(iobref.iobptr->handle), off_out,
			::std::move(instm), off_in, bound, ::std::move(callback));
		return;
	}
	using alloc_type = ::fast_io::details::async_scheduler_allocator_t<scheduler>;
	try
	{
		::fast_io::details::io_buffer::async_iobuffer_transmit_dispatch(
			::fast_io::details::async_new_state<
				::fast_io::details::io_buffer::async_iobuffer_transmit_state<
					scheduler, io_buffer_type, instmtype, alloc_type, ::std::remove_cvref_t<T>>>(
				sched, iobref, off_out, ::std::move(instm), off_in, bound, timeout,
				::std::move(callback)));
	}
	catch throws(::std::error e)
	{
		callback(e.release(), 0);
	}
}

} // namespace fast_io
