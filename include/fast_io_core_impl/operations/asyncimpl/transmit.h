#pragma once

namespace fast_io::details
{

/*
 * Generic async transmit emulation state: a bounce buffer lives in the
 * state object; each round submits an async_pread_some into the buffer,
 * then pwrites everything that was read out the other side. Native
 * transmit (e.g. io_uring SPLICE) bypasses this entirely through
 * async_transmit_some_bytes_overflow_underflow_callback_define.
 */
inline constexpr ::std::size_t async_transmit_bounce_size{
	::fast_io::details::transmit_buffer_size_cache<1>};

template <bool all, typename scheduler, typename outstmtype, typename instmtype,
		  typename alloc_type, typename T>
struct async_transmit_bytes_state
{
	static inline constexpr bool transmit_all{all};
	using allocator_type = alloc_type;
	static inline constexpr bool alloc_with_status{alloc_type::has_status};
	scheduler sched;
	outstmtype outstm;
	::fast_io::intfpos_opt off_out;
	instmtype instm;
	::fast_io::intfpos_opt off_in;
	/* bytes still permitted by the caller's bound; SIZE_MAX for "until EOF" */
	::std::size_t remaining;
	::fast_io::posix_statx_timestamp_opt timeout;
	T callback;
	/* staged: bytes read into buf during the current round; moved: total
	 * bytes delivered to the output; round_base: moved at round start, so
	 * moved - round_base is how much of the staged bytes was written */
	::std::size_t staged;
	::std::size_t moved;
	::std::size_t round_base;
	::std::byte buf[async_transmit_bounce_size];
	FAST_IO_NO_UNIQUE_ADDRESS ::std::conditional_t<alloc_with_status, typename alloc_type::handle_type,
											   ::fast_io::details::empty>
		alloc_handle{};
};

template <typename state_type>
inline void async_transmit_bytes_deliver(state_type *state, ::std::cxx_std_error err,
										 ::std::size_t moved) noexcept
{
	auto callback{::std::move(state->callback)};
	::fast_io::details::async_delete_state(state);
	if constexpr (state_type::transmit_all)
	{
		callback(err);
	}
	else
	{
		callback(err, moved);
	}
}

template <typename state_type>
inline void async_transmit_bytes_write_staged(state_type *state) noexcept;

/*
 * The generic engine is shared by transmit_some and transmit_all: `some`
 * delivers after the first fully-written round (bytes moved can be short of
 * the bound — the caller loops); `all` resubmits read rounds until the
 * bound is met or EOF. EOF is a normal stop for both (some reports 0, all
 * reports success).
 */
template <typename state_type>
inline void async_transmit_bytes_read_round(state_type *state) noexcept
{
	::std::size_t const chunk{state->remaining < ::fast_io::details::async_transmit_bounce_size
								  ? state->remaining
								  : ::fast_io::details::async_transmit_bounce_size};
	::fast_io::details::async_submit_catching(
		[state, chunk]() noexcept {
			async_pread_some_bytes_underflow_callback_define(
				state->sched, state->instm, state->buf, chunk, state->off_in, state->timeout,
				[state](::std::cxx_std_error err, ::std::size_t bytesread) noexcept {
					if (err.domain == nullptr)
					{
						if (state->off_in.has_opt)
						{
							state->off_in.opt = ::fast_io::fposoffadd_nonegative(
								state->off_in.opt, static_cast<::std::ptrdiff_t>(bytesread));
						}
						if (bytesread != 0)
						{
							state->staged = bytesread;
							async_transmit_bytes_write_staged(state);
							return;
						}
					}
					::fast_io::details::async_transmit_bytes_deliver(state, err, state->moved);
				});
		},
		[state](::std::cxx_std_error err) noexcept {
			::fast_io::details::async_transmit_bytes_deliver(state, err, state->moved);
		});
}

template <typename state_type>
inline void async_transmit_bytes_write_staged(state_type *state) noexcept
{
	::std::size_t const left{state->staged - (state->moved - state->round_base)};
	::fast_io::details::async_submit_catching(
		[state, left]() noexcept {
			async_pwrite_some_bytes_overflow_callback_define(
				state->sched, state->outstm,
				state->buf + (state->moved - state->round_base), left,
				state->off_out, state->timeout,
				[state](::std::cxx_std_error err, ::std::size_t byteswritten) noexcept {
					if (err.domain == nullptr)
					{
						state->moved += byteswritten;
						state->remaining -= byteswritten;
						if (state->off_out.has_opt)
						{
							state->off_out.opt = ::fast_io::fposoffadd_nonegative(
								state->off_out.opt, static_cast<::std::ptrdiff_t>(byteswritten));
						}
						if (state->moved - state->round_base != state->staged)
						{
							if (byteswritten == 0) [[unlikely]]
							{
								::fast_io::details::async_transmit_bytes_deliver(
									state,
									::fast_io::details::async_make_error(
										::std::errc::no_space_on_device),
									state->moved);
								return;
							}
							async_transmit_bytes_write_staged(state);
							return;
						}
						if constexpr (state_type::transmit_all)
						{
							if (state->remaining != 0)
							{
								state->round_base = state->moved;
								async_transmit_bytes_read_round(state);
								return;
							}
						}
					}
					::fast_io::details::async_transmit_bytes_deliver(state, err, state->moved);
				});
		},
		[state](::std::cxx_std_error err) noexcept {
			::fast_io::details::async_transmit_bytes_deliver(state, err, state->moved);
		});
}

/*
 * transmit_all over a backend's native some define: resubmit until the
 * bound is met or a zero-byte round reports EOF. Shares the loop shape
 * with the p{read,write}_all chains.
 */
template <typename scheduler, typename outstmtype, typename instmtype, typename alloc_type,
		  typename T>
struct async_transmit_all_native_state
{
	using allocator_type = alloc_type;
	static inline constexpr bool alloc_with_status{alloc_type::has_status};
	scheduler sched;
	outstmtype outstm;
	::fast_io::intfpos_opt off_out;
	instmtype instm;
	::fast_io::intfpos_opt off_in;
	::std::size_t remaining;
	::fast_io::posix_statx_timestamp_opt timeout;
	T callback;
	FAST_IO_NO_UNIQUE_ADDRESS ::std::conditional_t<alloc_with_status, typename alloc_type::handle_type,
											   ::fast_io::details::empty>
		alloc_handle{};
};

template <typename scheduler, typename outstmtype, typename instmtype, typename alloc_type,
		  typename T>
inline void async_transmit_all_native_submit(
	async_transmit_all_native_state<scheduler, outstmtype, instmtype, alloc_type, T> *state) noexcept
{
	::std::size_t const bound{state->remaining};
	::fast_io::details::async_submit_catching(
		[state, bound]() noexcept {
			async_transmit_some_bytes_overflow_underflow_callback_define(
				state->sched, state->outstm, state->off_out, state->instm, state->off_in,
				::fast_io::size_t_opt{bound}, state->timeout,
				[state](::std::cxx_std_error err, ::std::size_t moved) noexcept {
					if (err.domain == nullptr)
					{
						state->remaining -= moved;
						if (state->off_out.has_opt)
						{
							state->off_out.opt = ::fast_io::fposoffadd_nonegative(
								state->off_out.opt, static_cast<::std::ptrdiff_t>(moved));
						}
						if (state->off_in.has_opt)
						{
							state->off_in.opt = ::fast_io::fposoffadd_nonegative(
								state->off_in.opt, static_cast<::std::ptrdiff_t>(moved));
						}
						if (moved != 0 && state->remaining != 0)
						{
							async_transmit_all_native_submit(state);
							return;
						}
					}
					auto callback{::std::move(state->callback)};
					::fast_io::details::async_delete_state(state);
					callback(err);
				});
		},
		[state](::std::cxx_std_error err) noexcept {
			auto callback{::std::move(state->callback)};
			::fast_io::details::async_delete_state(state);
			callback(err);
		});
}

} // namespace fast_io::details

namespace fast_io::operations::decay
{

/*
 * async transmit_some: prefers a backend's native
 * async_transmit_some_bytes_overflow_underflow_callback_define (e.g. io_uring SPLICE);
 * otherwise emulates with a read->write chain through a bounce buffer.
 * The functor is invoked once as
 * callback(::std::cxx_std_error, ::std::size_t) noexcept — the size is the
 * bytes delivered to the output.
 */
template <typename async_scheduler_type, typename outstmtype, typename instmtype,
		  typename callback_type>
	requires ::fast_io::operations::decay::defines::async_bytes_completion_callback<
		::std::remove_cvref_t<callback_type>>
inline void async_transmit_some_bytes_decay_callback(
	async_scheduler_type scheduler, outstmtype outstm, ::fast_io::intfpos_opt off_out,
	instmtype instm, ::fast_io::intfpos_opt off_in, ::fast_io::size_t_opt bound,
	::fast_io::posix_statx_timestamp_opt timeout, callback_type callback) noexcept
{
	using outstm_reftype = outstmtype;
	using instm_reftype = instmtype;
	if constexpr (::fast_io::operations::decay::defines::
					  has_async_transmit_some_bytes_overflow_underflow_callback_define<
						  async_scheduler_type, outstm_reftype, instm_reftype,
						  ::std::remove_cvref_t<callback_type>>)
	{
		async_transmit_some_bytes_overflow_underflow_callback_define(
			scheduler,
			outstm,
			off_out,
			instm,
			off_in, bound, timeout,
			::std::move(callback));
		return;
	}
	else
	{
		using alloc_type = ::fast_io::details::async_scheduler_allocator_t<async_scheduler_type>;
		using state_type =
			::fast_io::details::async_transmit_bytes_state<false, async_scheduler_type,
														   outstm_reftype, instm_reftype, alloc_type,
														   ::std::remove_cvref_t<callback_type>>;
		::std::size_t const remaining{bound.has_opt ? bound.opt
													: ::std::numeric_limits<::std::size_t>::max()};
		if (remaining == 0)
		{
			callback(::std::cxx_std_error{}, 0);
			return;
		}
		try
		{
			::fast_io::details::async_transmit_bytes_read_round(
				::fast_io::details::async_new_state<state_type>(
					scheduler,
					outstm,
					off_out,
					instm,
					off_in, remaining, timeout,
					::std::move(callback), 0, 0, 0));
			return;
		}
		catch throws(::std::error e)
		{
			callback(e.release(), 0);
		}
	}
}

/*
 * async transmit_all: loops transmit_some until the bound is met or EOF.
 * With a native backend define it resubmits native rounds; otherwise it
 * uses the bounce-buffer engine. The functor is invoked once as
 * callback(::std::cxx_std_error) noexcept.
 */
template <typename async_scheduler_type, typename outstmtype, typename instmtype,
		  typename callback_type>
	requires ::std::is_nothrow_invocable_v<callback_type, ::std::cxx_std_error>
inline void async_transmit_all_bytes_decay_callback(
	async_scheduler_type scheduler, outstmtype outstm, ::fast_io::intfpos_opt off_out,
	instmtype instm, ::fast_io::intfpos_opt off_in, ::fast_io::size_t_opt bound,
	::fast_io::posix_statx_timestamp_opt timeout, callback_type callback) noexcept
{
	using outstm_reftype = outstmtype;
	using instm_reftype = instmtype;
	using alloc_type = ::fast_io::details::async_scheduler_allocator_t<async_scheduler_type>;
	::std::size_t const remaining{bound.has_opt ? bound.opt
												: ::std::numeric_limits<::std::size_t>::max()};
	if (remaining == 0)
	{
		callback(::std::cxx_std_error{});
		return;
	}
	try
	{
		if constexpr (::fast_io::operations::decay::defines::
						  has_async_transmit_some_bytes_overflow_underflow_callback_define<
							  async_scheduler_type, outstm_reftype, instm_reftype,
							  ::fast_io::details::async_io_callback>)
		{
			using state_type =
				::fast_io::details::async_transmit_all_native_state<async_scheduler_type,
																	outstm_reftype, instm_reftype,
																	alloc_type,
																	::std::remove_cvref_t<callback_type>>;
			::fast_io::details::async_transmit_all_native_submit(
				::fast_io::details::async_new_state<state_type>(
					scheduler,
					outstm,
					off_out,
					instm,
					off_in, remaining, timeout, ::std::move(callback)));
		}
		else
		{
			using state_type =
				::fast_io::details::async_transmit_bytes_state<true, async_scheduler_type,
															   outstm_reftype, instm_reftype,
															   alloc_type,
															   ::std::remove_cvref_t<callback_type>>;
			::fast_io::details::async_transmit_bytes_read_round(
				::fast_io::details::async_new_state<state_type>(
					scheduler,
					outstm,
					off_out,
					instm,
					off_in, remaining, timeout, ::std::move(callback), 0, 0, 0));
		}
		return;
	}
	catch throws(::std::error e)
	{
		callback(e.release());
	}
}

} // namespace fast_io::operations::decay

namespace fast_io::details
{

/* coroutine awaiters for the transmit decays */
template <typename scheduler, typename outstmtype, typename instmtype>
struct async_transmit_some_bytes_awaiter : async_awaiter_result<::std::size_t>
{
	scheduler sched;
	outstmtype outstm;
	::fast_io::intfpos_opt off_out;
	instmtype instm;
	::fast_io::intfpos_opt off_in;
	::fast_io::size_t_opt bound;
	::fast_io::posix_statx_timestamp_opt timeout;
	inline bool await_suspend(::std::coroutine_handle<> h) noexcept
	{
		this->coro = h;
		::fast_io::operations::decay::async_transmit_some_bytes_decay_callback(
			sched, outstm, off_out, instm, off_in, bound, timeout,
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

template <typename scheduler, typename outstmtype, typename instmtype>
struct async_transmit_all_bytes_awaiter : async_awaiter_result<void>
{
	scheduler sched;
	outstmtype outstm;
	::fast_io::intfpos_opt off_out;
	instmtype instm;
	::fast_io::intfpos_opt off_in;
	::fast_io::size_t_opt bound;
	::fast_io::posix_statx_timestamp_opt timeout;
	inline bool await_suspend(::std::coroutine_handle<> h) noexcept
	{
		this->coro = h;
		::fast_io::operations::decay::async_transmit_all_bytes_decay_callback(
			sched, outstm, off_out, instm, off_in, bound, timeout,
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

/* Coroutine forms of the transmit decays. The some form yields the bytes
 * transmitted; the all form yields void. */
template <typename async_scheduler_type, typename outstmtype, typename instmtype>
inline auto async_transmit_some_bytes_decay(async_scheduler_type scheduler, outstmtype outstm,
											::fast_io::intfpos_opt off_out, instmtype instm,
											::fast_io::intfpos_opt off_in,
											::fast_io::size_t_opt bound,
											::fast_io::posix_statx_timestamp_opt timeout) noexcept
{
	return ::fast_io::details::async_transmit_some_bytes_awaiter<
		async_scheduler_type, outstmtype,
		instmtype>{
		{}, scheduler, outstm, off_out, instm, off_in, bound, timeout};
}

template <typename async_scheduler_type, typename outstmtype, typename instmtype>
inline auto async_transmit_all_bytes_decay(async_scheduler_type scheduler, outstmtype outstm,
										   ::fast_io::intfpos_opt off_out, instmtype instm,
										   ::fast_io::intfpos_opt off_in,
										   ::fast_io::size_t_opt bound,
										   ::fast_io::posix_statx_timestamp_opt timeout) noexcept
{
	return ::fast_io::details::async_transmit_all_bytes_awaiter<
		async_scheduler_type, outstmtype,
		instmtype>{
		{}, scheduler, outstm, off_out, instm, off_in, bound, timeout};
}

} // namespace fast_io::operations::decay
