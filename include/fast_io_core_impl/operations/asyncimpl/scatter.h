#pragma once

namespace fast_io::details
{

/*
 * Generic scatter emulation (lio_listio style, sequential): submit one
 * scalar op per scatter element. Sequential submission is required for
 * correctness when off is empty — elements then ride the object's own
 * file position, which only advances in submission order. A backend may
 * instead provide async_scatter_*_some_bytes_*_callback_define for a true
 * single-operation vectored transfer; the decay function prefers it.
 *
 * `some` semantics mirror the synchronous scatter ops: walk elements in
 * order, stop at the first short transfer or error, and report
 * io_scatter_status_t{position, position_in_scatter} — position == n
 * means every element was transferred completely.
 */
template <typename scheduler, typename stmtype, typename alloc_type, typename T>
struct async_scatter_pread_some_bytes_state
{
	using allocator_type = alloc_type;
	static inline constexpr bool alloc_with_status{alloc_type::has_status};
	scheduler sched;
	stmtype instm;
	::fast_io::io_scatter_t const *scatters;
	::std::size_t n;
	::std::size_t index;
	::std::size_t elem_off;
	::fast_io::intfpos_opt off;
	::fast_io::posix_statx_timestamp_opt timeout;
	T callback;
	[[no_unique_address]] ::std::conditional_t<alloc_with_status, typename alloc_type::handle_type,
											 ::fast_io::details::empty>
		alloc_handle{};
};

template <typename scheduler, typename stmtype, typename alloc_type, typename T>
inline void async_scatter_pread_some_bytes_submit(
	async_scatter_pread_some_bytes_state<scheduler, stmtype, alloc_type, T> *state) noexcept
{
	while (state->elem_off == state->scatters[state->index].len)
	{
		++state->index;
		state->elem_off = 0;
		if (state->index == state->n)
		{
			auto callback{::std::move(state->callback)};
			auto status{::fast_io::io_scatter_status_t{state->index, 0}};
			::fast_io::details::async_delete_state(state);
			callback(::std::cxx_std_error{}, status);
			return;
		}
	}
	auto const elem{state->scatters[state->index]};
	::std::byte *base{const_cast<::std::byte *>(
		static_cast<::std::byte const *>(elem.base)) + state->elem_off};
	::std::size_t const elem_remaining{elem.len - state->elem_off};
	async_pread_some_bytes_underflow_callback_define(
		state->sched, state->instm, base, elem_remaining, state->off, state->timeout,
		[state](::std::cxx_std_error err, ::std::size_t bytesread) noexcept {
			state->elem_off += bytesread;
			if (err.domain != nullptr) [[unlikely]]
			{
				goto report;
			}
			if (state->off.has_opt)
			{
				state->off.opt = ::fast_io::fposoffadd_nonegative(
					state->off.opt, static_cast<::std::ptrdiff_t>(bytesread));
			}
			if (state->elem_off == state->scatters[state->index].len)
			{
				++state->index;
				state->elem_off = 0;
				if (state->index == state->n)
				{
					goto report;
				}
				async_scatter_pread_some_bytes_submit(state);
				return;
			}
			/* a short element transfer ends the some operation */
			goto report;
		report:
			auto callback{::std::move(state->callback)};
			auto status{::fast_io::io_scatter_status_t{state->index, state->elem_off}};
			::fast_io::details::async_delete_state(state);
			callback(err, status);
		});
}

/*
 * scatter pread_all: like the some chain, but a short element transfer
 * keeps resubmitting and a zero-byte read short of the goal is EOF
 * (parse_errc::end_of_file, matching sync scatter_read_all_bytes).
 */
template <typename scheduler, typename stmtype, typename alloc_type, typename T>
inline void async_scatter_pread_all_bytes_submit(
	async_scatter_pread_some_bytes_state<scheduler, stmtype, alloc_type, T> *state) noexcept
{
	while (state->elem_off == state->scatters[state->index].len)
	{
		++state->index;
		state->elem_off = 0;
		if (state->index == state->n)
		{
			auto callback{::std::move(state->callback)};

			::fast_io::details::async_delete_state(state);
			callback(::std::cxx_std_error{});
			return;
		}
	}
	auto const elem{state->scatters[state->index]};
	::std::byte *base{const_cast<::std::byte *>(
		static_cast<::std::byte const *>(elem.base)) + state->elem_off};
	::std::size_t const elem_remaining{elem.len - state->elem_off};
	async_pread_some_bytes_underflow_callback_define(
		state->sched, state->instm, base, elem_remaining, state->off, state->timeout,
		[state](::std::cxx_std_error err, ::std::size_t bytesread) noexcept {
			if (err.domain == nullptr)
			{
				if (bytesread == 0) [[unlikely]]
				{
					err = ::fast_io::details::async_make_error(
						::fast_io::freestanding::parse_errc::end_of_file);
					goto report;
				}
				state->elem_off += bytesread;
				if (state->off.has_opt)
				{
					state->off.opt = ::fast_io::fposoffadd_nonegative(
						state->off.opt, static_cast<::std::ptrdiff_t>(bytesread));
				}
				if (state->elem_off == state->scatters[state->index].len)
				{
					++state->index;
					state->elem_off = 0;
					if (state->index == state->n)
					{
						goto report;
					}
				}
				async_scatter_pread_all_bytes_submit(state);
				return;
			}
		report:
			auto callback{::std::move(state->callback)};
			::fast_io::details::async_delete_state(state);
			callback(err);
		});
}

template <typename scheduler, typename stmtype, typename alloc_type, typename T>
struct async_scatter_pwrite_some_bytes_state
{
	using allocator_type = alloc_type;
	static inline constexpr bool alloc_with_status{alloc_type::has_status};
	scheduler sched;
	stmtype outstm;
	::fast_io::io_scatter_t const *scatters;
	::std::size_t n;
	::std::size_t index;
	::std::size_t elem_off;
	::fast_io::intfpos_opt off;
	::fast_io::posix_statx_timestamp_opt timeout;
	T callback;
	[[no_unique_address]] ::std::conditional_t<alloc_with_status, typename alloc_type::handle_type,
											 ::fast_io::details::empty>
		alloc_handle{};
};

template <typename scheduler, typename stmtype, typename alloc_type, typename T>
inline void async_scatter_pwrite_some_bytes_submit(
	async_scatter_pwrite_some_bytes_state<scheduler, stmtype, alloc_type, T> *state) noexcept
{
	while (state->elem_off == state->scatters[state->index].len)
	{
		++state->index;
		state->elem_off = 0;
		if (state->index == state->n)
		{
			auto callback{::std::move(state->callback)};
			auto status{::fast_io::io_scatter_status_t{state->index, 0}};
			::fast_io::details::async_delete_state(state);
			callback(::std::cxx_std_error{}, status);
			return;
		}
	}
	auto const elem{state->scatters[state->index]};
	::std::byte const *base{static_cast<::std::byte const *>(elem.base) + state->elem_off};
	::std::size_t const elem_remaining{elem.len - state->elem_off};
	async_pwrite_some_bytes_overflow_callback_define(
		state->sched, state->outstm, base, elem_remaining, state->off, state->timeout,
		[state](::std::cxx_std_error err, ::std::size_t byteswritten) noexcept {
			state->elem_off += byteswritten;
			if (err.domain != nullptr) [[unlikely]]
			{
				goto report;
			}
			if (state->off.has_opt)
			{
				state->off.opt = ::fast_io::fposoffadd_nonegative(
					state->off.opt, static_cast<::std::ptrdiff_t>(byteswritten));
			}
			if (state->elem_off == state->scatters[state->index].len)
			{
				++state->index;
				state->elem_off = 0;
				if (state->index == state->n)
				{
					goto report;
				}
				async_scatter_pwrite_some_bytes_submit(state);
				return;
			}
			goto report;
		report:
			auto callback{::std::move(state->callback)};
			auto status{::fast_io::io_scatter_status_t{state->index, state->elem_off}};
			::fast_io::details::async_delete_state(state);
			callback(err, status);
		});
}

template <typename scheduler, typename stmtype, typename alloc_type, typename T>
inline void async_scatter_pwrite_all_bytes_submit(
	async_scatter_pwrite_some_bytes_state<scheduler, stmtype, alloc_type, T> *state) noexcept
{
	while (state->elem_off == state->scatters[state->index].len)
	{
		++state->index;
		state->elem_off = 0;
		if (state->index == state->n)
		{
			auto callback{::std::move(state->callback)};

			::fast_io::details::async_delete_state(state);
			callback(::std::cxx_std_error{});
			return;
		}
	}
	auto const elem{state->scatters[state->index]};
	::std::byte const *base{static_cast<::std::byte const *>(elem.base) + state->elem_off};
	::std::size_t const elem_remaining{elem.len - state->elem_off};
	async_pwrite_some_bytes_overflow_callback_define(
		state->sched, state->outstm, base, elem_remaining, state->off, state->timeout,
		[state](::std::cxx_std_error err, ::std::size_t byteswritten) noexcept {
			if (err.domain == nullptr)
			{
				if (byteswritten == 0) [[unlikely]]
				{
					err = ::fast_io::details::async_make_error(
						::std::errc::no_space_on_device);
					goto report;
				}
				state->elem_off += byteswritten;
				if (state->off.has_opt)
				{
					state->off.opt = ::fast_io::fposoffadd_nonegative(
						state->off.opt, static_cast<::std::ptrdiff_t>(byteswritten));
				}
				if (state->elem_off == state->scatters[state->index].len)
				{
					++state->index;
					state->elem_off = 0;
					if (state->index == state->n)
					{
						goto report;
					}
				}
				async_scatter_pwrite_all_bytes_submit(state);
				return;
			}
		report:
			auto callback{::std::move(state->callback)};
			::fast_io::details::async_delete_state(state);
			callback(err);
		});
}

} // namespace fast_io::details

namespace fast_io::operations::decay
{

/*
 * scatter pread_some: prefers a backend's single-operation vectored
 * define; otherwise walks the scatter array with sequential scalar
 * submissions. The functor is invoked once as
 * callback(::std::cxx_std_error, ::fast_io::io_scatter_status_t) noexcept.
 */
template <typename async_scheduler_type, typename instmtype, typename callback_type>
	requires ::fast_io::operations::decay::defines::async_scatter_completion_callback<
				 ::std::remove_cvref_t<callback_type>>
inline void async_scatter_pread_some_bytes_decay_callback(
	async_scheduler_type scheduler, instmtype &&instm, ::fast_io::io_scatter_t const *pscatters,
	::std::size_t n, ::fast_io::intfpos_opt off, ::fast_io::posix_statx_timestamp_opt timeout,
	callback_type callback) noexcept
{
	using instm_reftype = ::fast_io::details::async_input_stream_ref_t<instmtype>;
	if constexpr (::fast_io::operations::decay::defines::
					  has_async_scatter_pread_some_bytes_underflow_callback_define<
						  async_scheduler_type, instm_reftype,
						  ::std::remove_cvref_t<callback_type>>)
	{
		async_scatter_pread_some_bytes_underflow_callback_define(
			scheduler,
			::fast_io::details::async_input_stream_ref(
				::fast_io::freestanding::forward<instmtype>(instm)),
			pscatters, n, off, timeout,
			::std::move(callback));
		return;
	}
	else
	{
		static_assert(
			::fast_io::operations::decay::defines::
				has_async_pread_some_bytes_underflow_callback_define<
					async_scheduler_type, instm_reftype,
					decltype([](::std::cxx_std_error, ::std::size_t) noexcept {})>,
			"no scatter fast path and no scalar pread define to emulate with");
		using alloc_type = ::fast_io::details::async_scheduler_allocator_t<async_scheduler_type>;
		using state_type =
			::fast_io::details::async_scatter_pread_some_bytes_state<async_scheduler_type,
																   instm_reftype, alloc_type,
																   ::std::remove_cvref_t<callback_type>>;
		if (n == 0)
		{
			callback(::std::cxx_std_error{}, ::fast_io::io_scatter_status_t{0, 0});
			return;
		}
		try
		{
			::fast_io::details::async_scatter_pread_some_bytes_submit(
				::fast_io::details::async_new_state<state_type>(
					scheduler,
					::fast_io::details::async_input_stream_ref(
						::fast_io::freestanding::forward<instmtype>(instm)),
					pscatters, n, 0, 0, off, timeout,
					::std::move(callback)));
			return;
		}
		catch throws(::std::error e)
		{
			callback(e.release(), ::fast_io::io_scatter_status_t{});
		}
	}
}

template <typename async_scheduler_type, typename instmtype, typename callback_type>
	requires ::std::is_nothrow_invocable_v<callback_type, ::std::cxx_std_error> &&
			 ::fast_io::operations::decay::defines::
				 has_async_pread_some_bytes_underflow_callback_define<
					 async_scheduler_type,
					 ::fast_io::details::async_input_stream_ref_t<instmtype>,
					 decltype([](::std::cxx_std_error, ::std::size_t) noexcept {})>
inline void async_scatter_pread_all_bytes_decay_callback(
	async_scheduler_type scheduler, instmtype &&instm, ::fast_io::io_scatter_t const *pscatters,
	::std::size_t n, ::fast_io::intfpos_opt off, ::fast_io::posix_statx_timestamp_opt timeout,
	callback_type callback) noexcept
{
	using instm_reftype = ::fast_io::details::async_input_stream_ref_t<instmtype>;
	using alloc_type = ::fast_io::details::async_scheduler_allocator_t<async_scheduler_type>;
	using state_type =
		::fast_io::details::async_scatter_pread_some_bytes_state<async_scheduler_type,
															   instm_reftype, alloc_type,
															   ::std::remove_cvref_t<callback_type>>;
	if (n == 0)
	{
		callback(::std::cxx_std_error{});
		return;
	}
	try
	{
		::fast_io::details::async_scatter_pread_all_bytes_submit(
			::fast_io::details::async_new_state<state_type>(
				scheduler,
				::fast_io::details::async_input_stream_ref(
					::fast_io::freestanding::forward<instmtype>(instm)),
				pscatters, n, 0, 0, off, timeout, ::std::move(callback)));
		return;
	}
	catch throws(::std::error e)
	{
		callback(e.release());
	}
}

template <typename async_scheduler_type, typename outstmtype, typename callback_type>
	requires ::fast_io::operations::decay::defines::async_scatter_completion_callback<
				 ::std::remove_cvref_t<callback_type>>
inline void async_scatter_pwrite_some_bytes_decay_callback(
	async_scheduler_type scheduler, outstmtype &&outstm, ::fast_io::io_scatter_t const *pscatters,
	::std::size_t n, ::fast_io::intfpos_opt off, ::fast_io::posix_statx_timestamp_opt timeout,
	callback_type callback) noexcept
{
	using outstm_reftype = ::fast_io::details::async_output_stream_ref_t<outstmtype>;
	if constexpr (::fast_io::operations::decay::defines::
					  has_async_scatter_pwrite_some_bytes_overflow_callback_define<
						  async_scheduler_type, outstm_reftype,
						  ::std::remove_cvref_t<callback_type>>)
	{
		async_scatter_pwrite_some_bytes_overflow_callback_define(
			scheduler,
			::fast_io::details::async_output_stream_ref(
				::fast_io::freestanding::forward<outstmtype>(outstm)),
			pscatters, n, off, timeout,
			::std::move(callback));
		return;
	}
	else
	{
		static_assert(
			::fast_io::operations::decay::defines::
				has_async_pwrite_some_bytes_overflow_callback_define<
					async_scheduler_type, outstm_reftype,
					decltype([](::std::cxx_std_error, ::std::size_t) noexcept {})>,
			"no scatter fast path and no scalar pwrite define to emulate with");
		using alloc_type = ::fast_io::details::async_scheduler_allocator_t<async_scheduler_type>;
		using state_type =
			::fast_io::details::async_scatter_pwrite_some_bytes_state<async_scheduler_type,
																	outstm_reftype, alloc_type,
																	::std::remove_cvref_t<callback_type>>;
		if (n == 0)
		{
			callback(::std::cxx_std_error{}, ::fast_io::io_scatter_status_t{0, 0});
			return;
		}
		try
		{
			::fast_io::details::async_scatter_pwrite_some_bytes_submit(
				::fast_io::details::async_new_state<state_type>(
					scheduler,
					::fast_io::details::async_output_stream_ref(
						::fast_io::freestanding::forward<outstmtype>(outstm)),
					pscatters, n, 0, 0, off, timeout,
					::std::move(callback)));
			return;
		}
		catch throws(::std::error e)
		{
			callback(e.release(), ::fast_io::io_scatter_status_t{});
		}
	}
}

template <typename async_scheduler_type, typename outstmtype, typename callback_type>
	requires ::std::is_nothrow_invocable_v<callback_type, ::std::cxx_std_error> &&
			 ::fast_io::operations::decay::defines::
				 has_async_pwrite_some_bytes_overflow_callback_define<
					 async_scheduler_type,
					 ::fast_io::details::async_output_stream_ref_t<outstmtype>,
					 decltype([](::std::cxx_std_error, ::std::size_t) noexcept {})>
inline void async_scatter_pwrite_all_bytes_decay_callback(
	async_scheduler_type scheduler, outstmtype &&outstm, ::fast_io::io_scatter_t const *pscatters,
	::std::size_t n, ::fast_io::intfpos_opt off, ::fast_io::posix_statx_timestamp_opt timeout,
	callback_type callback) noexcept
{
	using outstm_reftype = ::fast_io::details::async_output_stream_ref_t<outstmtype>;
	using alloc_type = ::fast_io::details::async_scheduler_allocator_t<async_scheduler_type>;
	using state_type =
		::fast_io::details::async_scatter_pwrite_some_bytes_state<async_scheduler_type,
																outstm_reftype, alloc_type,
																::std::remove_cvref_t<callback_type>>;
	if (n == 0)
	{
		callback(::std::cxx_std_error{});
		return;
	}
	try
	{
		::fast_io::details::async_scatter_pwrite_all_bytes_submit(
			::fast_io::details::async_new_state<state_type>(
				scheduler,
				::fast_io::details::async_output_stream_ref(
					::fast_io::freestanding::forward<outstmtype>(outstm)),
				pscatters, n, 0, 0, off, timeout, ::std::move(callback)));
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

/* coroutine awaiters for the scatter decays */
template <typename scheduler, typename instmtype>
struct async_scatter_pread_some_bytes_awaiter : async_awaiter_result<::fast_io::io_scatter_status_t>
{
	scheduler sched;
	instmtype instm;
	::fast_io::io_scatter_t const *scatters;
	::std::size_t n;
	::fast_io::intfpos_opt off;
	::fast_io::posix_statx_timestamp_opt timeout;
	inline bool await_suspend(::std::coroutine_handle<> h) noexcept
	{
		this->coro = h;
		::fast_io::operations::decay::async_scatter_pread_some_bytes_decay_callback(
			sched, instm, scatters, n, off, timeout,
			[this](::std::cxx_std_error e, ::fast_io::io_scatter_status_t status) noexcept {
				this->err = e;
				this->value = status;
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
	inline ::fast_io::io_scatter_status_t await_resume() throws
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

template <typename scheduler, typename instmtype>
struct async_scatter_pread_all_bytes_awaiter : async_awaiter_result<void>
{
	scheduler sched;
	instmtype instm;
	::fast_io::io_scatter_t const *scatters;
	::std::size_t n;
	::fast_io::intfpos_opt off;
	::fast_io::posix_statx_timestamp_opt timeout;
	inline bool await_suspend(::std::coroutine_handle<> h) noexcept
	{
		this->coro = h;
		::fast_io::operations::decay::async_scatter_pread_all_bytes_decay_callback(
			sched, instm, scatters, n, off, timeout,
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

template <typename scheduler, typename outstmtype>
struct async_scatter_pwrite_some_bytes_awaiter : async_awaiter_result<::fast_io::io_scatter_status_t>
{
	scheduler sched;
	outstmtype outstm;
	::fast_io::io_scatter_t const *scatters;
	::std::size_t n;
	::fast_io::intfpos_opt off;
	::fast_io::posix_statx_timestamp_opt timeout;
	inline bool await_suspend(::std::coroutine_handle<> h) noexcept
	{
		this->coro = h;
		::fast_io::operations::decay::async_scatter_pwrite_some_bytes_decay_callback(
			sched, outstm, scatters, n, off, timeout,
			[this](::std::cxx_std_error e, ::fast_io::io_scatter_status_t status) noexcept {
				this->err = e;
				this->value = status;
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
	inline ::fast_io::io_scatter_status_t await_resume() throws
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

template <typename scheduler, typename outstmtype>
struct async_scatter_pwrite_all_bytes_awaiter : async_awaiter_result<void>
{
	scheduler sched;
	outstmtype outstm;
	::fast_io::io_scatter_t const *scatters;
	::std::size_t n;
	::fast_io::intfpos_opt off;
	::fast_io::posix_statx_timestamp_opt timeout;
	inline bool await_suspend(::std::coroutine_handle<> h) noexcept
	{
		this->coro = h;
		::fast_io::operations::decay::async_scatter_pwrite_all_bytes_decay_callback(
			sched, outstm, scatters, n, off, timeout,
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

/* Coroutine forms of the scatter decays. The some forms yield
 * io_scatter_status_t; the all forms yield void. */
template <typename async_scheduler_type, typename instmtype>
inline auto async_scatter_pread_some_bytes_decay(async_scheduler_type scheduler, instmtype &&instm,
												 ::fast_io::io_scatter_t const *pscatters,
												 ::std::size_t n, ::fast_io::intfpos_opt off,
												 ::fast_io::posix_statx_timestamp_opt timeout) noexcept
{
	return ::fast_io::details::async_scatter_pread_some_bytes_awaiter<
		async_scheduler_type, ::fast_io::details::async_input_stream_ref_t<instmtype>>{
		{}, scheduler,
		::fast_io::details::async_input_stream_ref(::fast_io::freestanding::forward<instmtype>(instm)),
		pscatters, n, off, timeout};
}

template <typename async_scheduler_type, typename instmtype>
inline auto async_scatter_pread_all_bytes_decay(async_scheduler_type scheduler, instmtype &&instm,
												::fast_io::io_scatter_t const *pscatters,
												::std::size_t n, ::fast_io::intfpos_opt off,
												::fast_io::posix_statx_timestamp_opt timeout) noexcept
{
	return ::fast_io::details::async_scatter_pread_all_bytes_awaiter<
		async_scheduler_type, ::fast_io::details::async_input_stream_ref_t<instmtype>>{
		{}, scheduler,
		::fast_io::details::async_input_stream_ref(::fast_io::freestanding::forward<instmtype>(instm)),
		pscatters, n, off, timeout};
}

template <typename async_scheduler_type, typename outstmtype>
inline auto async_scatter_pwrite_some_bytes_decay(async_scheduler_type scheduler,
												  outstmtype &&outstm,
												  ::fast_io::io_scatter_t const *pscatters,
												  ::std::size_t n, ::fast_io::intfpos_opt off,
												  ::fast_io::posix_statx_timestamp_opt timeout) noexcept
{
	return ::fast_io::details::async_scatter_pwrite_some_bytes_awaiter<
		async_scheduler_type, ::fast_io::details::async_output_stream_ref_t<outstmtype>>{
		{}, scheduler,
		::fast_io::details::async_output_stream_ref(
			::fast_io::freestanding::forward<outstmtype>(outstm)),
		pscatters, n, off, timeout};
}

template <typename async_scheduler_type, typename outstmtype>
inline auto async_scatter_pwrite_all_bytes_decay(async_scheduler_type scheduler,
												 outstmtype &&outstm,
												 ::fast_io::io_scatter_t const *pscatters,
												 ::std::size_t n, ::fast_io::intfpos_opt off,
												 ::fast_io::posix_statx_timestamp_opt timeout) noexcept
{
	return ::fast_io::details::async_scatter_pwrite_all_bytes_awaiter<
		async_scheduler_type, ::fast_io::details::async_output_stream_ref_t<outstmtype>>{
		{}, scheduler,
		::fast_io::details::async_output_stream_ref(
			::fast_io::freestanding::forward<outstmtype>(outstm)),
		pscatters, n, off, timeout};
}

} // namespace fast_io::operations::decay
