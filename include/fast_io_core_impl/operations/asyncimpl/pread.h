#pragma once

namespace fast_io::details
{

/*
 * Awaiter state shared by every async_*_decay coroutine wrapper. Backend
 * callbacks may run in two ways: inline during the submission call
 * (e.g. the op resolves immediately) or later from the scheduler's
 * event-pump context on the same thread. `done`/`suspended` order the two
 * cases: a callback that ran inline records `done` instead of resuming a
 * coroutine that has not suspended yet; await_suspend then returns false
 * and await_resume delivers the result directly.
 */
template <typename T>
struct async_awaiter_result
{
	::std::coroutine_handle<> coro{};
	::std::cxx_std_error err{};
	T value{};
	bool done{};
	bool suspended{};
	/* an awaiter abandoned before resumption still releases a held error
	 * payload (e.g. the coroutine frame destroyed while suspended) */
	inline ~async_awaiter_result() noexcept
	{
		async_dispose_error(err);
	}
	inline constexpr bool await_ready() const noexcept
	{
		return false;
	}
	inline bool async_suspend_done() noexcept
	{
		if (done)
		{
			return false;
		}
		suspended = true;
		return true;
	}
};

template <>
struct async_awaiter_result<void>
{
	::std::coroutine_handle<> coro{};
	::std::cxx_std_error err{};
	bool done{};
	bool suspended{};
	inline ~async_awaiter_result() noexcept
	{
		async_dispose_error(err);
	}
	inline constexpr bool await_ready() const noexcept
	{
		return false;
	}
	inline bool async_suspend_done() noexcept
	{
		if (done)
		{
			return false;
		}
		suspended = true;
		return true;
	}
};

/* Shared state for the async_pread_all_bytes chain: one allocation lives
 * across every partial-read resubmission and is freed when the user's
 * callback is finally invoked. */
template <typename scheduler, typename instmtype, typename alloc_type, typename T>
struct async_pread_all_bytes_state
{
	using allocator_type = alloc_type;
	static inline constexpr bool alloc_with_status{alloc_type::has_status};
	scheduler sched;
	instmtype instm;
	::std::byte *first;
	::std::size_t remaining;
	::fast_io::intfpos_opt off;
	::fast_io::posix_statx_timestamp_opt timeout;
	T callback;
	/* status allocator handle copied from the scheduler at submission so
	 * the state can be freed without it; kept last to preserve the
	 * aggregate initialization order */
	FAST_IO_NO_UNIQUE_ADDRESS ::std::conditional_t<alloc_with_status, typename alloc_type::handle_type,
											   ::fast_io::details::empty>
		alloc_handle{};
};

/*
 * One round of the pread_all chain: submit a pread_some for the remaining
 * bytes; the backend callback receives (err, bytes_read_this_round). On
 * error or completion the user callback runs once as
 * callback(::std::cxx_std_error) noexcept and the state frees itself;
 * otherwise resubmit. A zero-byte read before the buffer is full is EOF:
 * delivered as parse_errc::end_of_file, matching the sync read_all
 * semantics.
 */
template <typename scheduler, typename instmtype, typename alloc_type, typename T>
inline void async_pread_all_bytes_submit(
	async_pread_all_bytes_state<scheduler, instmtype, alloc_type, T> *state) noexcept
{
	async_pread_some_bytes_underflow_callback_define(
		state->sched, state->instm, state->first, state->remaining, state->off, state->timeout,
		[state](::std::cxx_std_error err, ::std::size_t bytesread) noexcept {
			if (err.domain != nullptr) [[unlikely]]
			{
				auto callback{::std::move(state->callback)};
				::fast_io::details::async_delete_state(state);
				callback(err);
				return;
			}
			state->first += bytesread;
			state->remaining -= bytesread;
			if (state->off.has_opt)
			{
				state->off.opt = ::fast_io::fposoffadd_nonegative(
					state->off.opt, static_cast<::std::ptrdiff_t>(bytesread));
			}
			if (state->remaining == 0)
			{
				auto callback{::std::move(state->callback)};
				::fast_io::details::async_delete_state(state);
				callback(::std::cxx_std_error{});
				return;
			}
			if (bytesread == 0) [[unlikely]]
			{
				auto callback{::std::move(state->callback)};
				::fast_io::details::async_delete_state(state);
				callback(::fast_io::details::async_make_error(
					::fast_io::freestanding::parse_errc::end_of_file));
				return;
			}
			async_pread_all_bytes_submit(state);
		});
}

} // namespace fast_io::details

namespace fast_io::operations::decay
{

/*
 * Public entry point for a single async positional read: submits the
 * backend's async_pread_some_bytes_underflow_callback_define, which is
 * found by ADL on the scheduler/stream types — backends implement only
 * the define and callers only ever name this. instm is reduced to its
 * input_stream_ref first, so buffered streams arrive as
 * basic_io_buffer_ref and their buffer-aware define runs instead of the
 * backend's.
 *
 * The functor is invoked once as
 * callback(::std::cxx_std_error, ::std::size_t) noexcept: the size is the
 * bytes read this round. The stream object must outlive the operation.
 */
template <typename async_scheduler_type, typename instmtype, typename callback_type>
	requires ::fast_io::operations::decay::defines::async_bytes_completion_callback<
				 ::std::remove_cvref_t<callback_type>> &&
			 ::fast_io::operations::decay::defines::
				 has_async_pread_some_bytes_underflow_callback_define<
					 async_scheduler_type,
					 instmtype,
					 ::std::remove_cvref_t<callback_type>>
inline void async_pread_some_bytes_decay_callback(async_scheduler_type scheduler, instmtype instm,
												  ::std::byte *first, ::std::size_t count,
												  ::fast_io::intfpos_opt off,
												  ::fast_io::posix_statx_timestamp_opt timeout,
												  callback_type callback) noexcept
{
	async_pread_some_bytes_underflow_callback_define(
		scheduler,
		instm,
		first, count, off, timeout,
		::std::move(callback));
}

/*
 * read_all built on async_pread_some_bytes_decay_callback: partial reads
 * are resubmitted until every byte is read. The functor is invoked once as
 * callback(::std::cxx_std_error) noexcept: err.domain == nullptr means every
 * byte was read, otherwise domain/code describe the failure (a short read at
 * EOF is delivered as parse_errc::end_of_file). `timeout` is forwarded
 * unchanged to every resubmission.
 */
template <typename async_scheduler_type, typename instmtype, typename callback_type>
	requires ::std::is_nothrow_invocable_v<callback_type, ::std::cxx_std_error> &&
			 ::fast_io::operations::decay::defines::
				 has_async_pread_some_bytes_underflow_callback_define<
					 async_scheduler_type,
					 instmtype,
					 ::fast_io::details::async_io_callback>
inline void async_pread_all_bytes_decay_callback(async_scheduler_type scheduler, instmtype instm,
												 ::std::byte *first, ::std::size_t count,
												 ::fast_io::intfpos_opt off,
												 ::fast_io::posix_statx_timestamp_opt timeout,
												 callback_type callback) noexcept
{
	using instm_reftype = instmtype;
	using alloc_type = ::fast_io::details::async_scheduler_allocator_t<async_scheduler_type>;
	using state_type =
		::fast_io::details::async_pread_all_bytes_state<async_scheduler_type, instm_reftype,
														alloc_type,
														::std::remove_cvref_t<callback_type>>;
	::std::cxx_std_error err{};

	if (count) [[likely]]
	{
		try
		{
			::fast_io::details::async_pread_all_bytes_submit(
				::fast_io::details::async_new_state<state_type>(
					scheduler,
					instm,
					first, count, off, timeout, ::std::move(callback)));
			return;
		}
		catch throws(::std::error e)
		{
			err = e.release();
		}
	}
	callback(err);
}

} // namespace fast_io::operations::decay

namespace fast_io::details
{

/* coroutine awaiter for async_pread_some_bytes_decay */
template <typename scheduler, typename instmtype>
struct async_pread_some_bytes_awaiter : async_awaiter_result<::std::size_t>
{
	scheduler sched;
	instmtype instm;
	::std::byte *first;
	::std::size_t count;
	::fast_io::intfpos_opt off;
	::fast_io::posix_statx_timestamp_opt timeout;
	inline bool await_suspend(::std::coroutine_handle<> h) noexcept
	{
		this->coro = h;
		::fast_io::operations::decay::async_pread_some_bytes_decay_callback(
			sched, instm, first, count, off, timeout,
			[this](::std::cxx_std_error e, ::std::size_t n) noexcept {
				this->err = e;
				this->value = n;
				if (this->suspended)
				{
					this->coro.resume();
				}
				else
				{
					this->done = true;
				}
			});
		return this->async_suspend_done();
	}
	inline ::std::size_t await_resume() throws
	{
		if (this->err.domain != nullptr)
		{
			auto e{this->err};
			this->err = {};
			throw throws e;
		}
		return this->value;
	}
};

/* coroutine awaiter for async_pread_all_bytes_decay */
template <typename scheduler, typename instmtype>
struct async_pread_all_bytes_awaiter : async_awaiter_result<void>
{
	scheduler sched;
	instmtype instm;
	::std::byte *first;
	::std::size_t count;
	::fast_io::intfpos_opt off;
	::fast_io::posix_statx_timestamp_opt timeout;
	inline bool await_suspend(::std::coroutine_handle<> h) noexcept
	{
		this->coro = h;
		::fast_io::operations::decay::async_pread_all_bytes_decay_callback(
			sched, instm, first, count, off, timeout,
			[this](::std::cxx_std_error e) noexcept {
				this->err = e;
				if (this->suspended)
				{
					this->coro.resume();
				}
				else
				{
					this->done = true;
				}
			});
		return this->async_suspend_done();
	}
	inline constexpr void await_resume() throws
	{
		if (this->err.domain != nullptr)
		{
			auto e{this->err};
			this->err = {};
			throw throws e;
		}
	}
};

} // namespace fast_io::details

namespace fast_io::operations::decay
{

/* Coroutine forms: co_await submits the operation and suspends until the
 * completion runs. await_resume() rethrows the error through the channel;
 * the some form yields the bytes read. */
template <typename async_scheduler_type, typename instmtype>
inline auto async_pread_some_bytes_decay(async_scheduler_type scheduler, instmtype instm,
										 ::std::byte *first, ::std::size_t count,
										 ::fast_io::intfpos_opt off,
										 ::fast_io::posix_statx_timestamp_opt timeout) noexcept
{
	return ::fast_io::details::async_pread_some_bytes_awaiter<
		async_scheduler_type, instmtype>{
		{}, scheduler, instm, first, count, off, timeout};
}

template <typename async_scheduler_type, typename instmtype>
inline auto async_pread_all_bytes_decay(async_scheduler_type scheduler, instmtype instm,
										::std::byte *first, ::std::size_t count,
										::fast_io::intfpos_opt off,
										::fast_io::posix_statx_timestamp_opt timeout) noexcept
{
	return ::fast_io::details::async_pread_all_bytes_awaiter<
		async_scheduler_type, instmtype>{
		{}, scheduler, instm, first, count, off, timeout};
}

} // namespace fast_io::operations::decay
