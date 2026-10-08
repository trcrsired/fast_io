#pragma once

namespace fast_io::details
{

/*
 * async_scan_get awaiter for context_scannable T: the parse state and the
 * destination object live in the awaiter itself — inside the caller's
 * coroutine frame — so both survive every suspension. Each parse round
 * runs over the input buffer's pending window exactly like the sync
 * context loop; on parse_errc::partial the completion submits
 * async_ibuffer_underflow and the loop resumes from its callback. When
 * the buffered window already holds a complete object the operation
 * resolves inside await_ready without any submission.
 */
template <typename scheduler, typename instmtype, typename T>
struct async_scan_get_context_awaiter : async_awaiter_result<T>
{
	using char_type = typename instmtype::input_char_type;
	using arg_type = decltype(::fast_io::io_scan_forward<char_type>(
		::fast_io::io_scan_alias(::std::declval<T &>())));
	using scan_state_type = typename ::std::remove_cvref_t<decltype(
		scan_context_type(::fast_io::io_reserve_type<char_type, arg_type>))>::type;

	scheduler sched;
	instmtype instm;
	::fast_io::posix_statx_timestamp_opt timeout;
	scan_state_type scan_state{};
	bool contiguous_tried{};

	/* store an owned error payload, releasing any previous one */
	inline void set_error(::std::cxx_std_error e) noexcept
	{
		::fast_io::details::async_dispose_error(this->err);
		this->err = e;
	}

	inline void resolve(::std::cxx_std_error e) noexcept
	{
		set_error(e);
		if (this->suspended)
		{
			this->coro.resume();
		}
		else
		{
			this->done = true;
		}
	}

	/* one parse round over the pending window. Returns true when the
	 * operation resolved: this->err records the outcome and this->value
	 * holds the scanned object on success. */
	inline bool pump() noexcept
	{
		try
		{
			auto curr{ibuffer_curr(instm)};
			auto const end{ibuffer_end(instm)};
			arg_type arg{this->value};
			if constexpr (::fast_io::contiguous_scannable<char_type, arg_type>)
			{
				if (!contiguous_tried)
				{
					contiguous_tried = true;
					auto [it, ec]{scan_contiguous_define(
						::fast_io::io_reserve_type<char_type, arg_type>, curr, end, arg)};
					if (it != end)
					{
						if constexpr (::std::same_as<decltype(curr), decltype(it)>)
						{
							ibuffer_set_curr(instm, it);
						}
						else
						{
							ibuffer_set_curr(instm, it - curr + curr);
						}
						if (ec == ::fast_io::freestanding::parse_errc::ok)
						{
							set_error(::std::cxx_std_error{});
						}
						else
						{
							set_error(::fast_io::details::async_make_error(ec));
						}
						return true;
					}
					/* it == end: the contiguous scan exhausted the window;
					 * continue with the context machine below */
				}
			}
			auto [it, ec]{scan_context_define(::fast_io::io_reserve_type<char_type, arg_type>,
											  scan_state, curr, end, arg)};
			if constexpr (::std::same_as<decltype(curr), decltype(it)>)
			{
				ibuffer_set_curr(instm, it);
			}
			else
			{
				ibuffer_set_curr(instm, it - curr + curr);
			}
			if (ec == ::fast_io::freestanding::parse_errc::ok)
			{
				set_error(::std::cxx_std_error{});
				return true;
			}
			if (ec != ::fast_io::freestanding::parse_errc::partial)
			{
				set_error(::fast_io::details::async_make_error(ec));
				return true;
			}
			return false;
		}
		catch throws(::std::error e)
		{
			set_error(e.release());
			return true;
		}
	}

	/* the window cannot supply more bytes — give the scanner its eof
	 * verdict, matching the sync scan_context_eof_define handling */
	inline void eof_resolve() noexcept
	{
		try
		{
			arg_type arg{this->value};
			auto ec{scan_context_eof_define(::fast_io::io_reserve_type<char_type, arg_type>,
											scan_state, arg)};
			if (ec == ::fast_io::freestanding::parse_errc::ok)
			{
				resolve(::std::cxx_std_error{});
			}
			else
			{
				resolve(::fast_io::details::async_make_error(ec));
			}
		}
		catch throws(::std::error e)
		{
			resolve(e.release());
		}
	}

	inline void submit_underflow() noexcept
	{
		async_ibuffer_underflow(
			sched, instm, timeout,
			[this](::std::cxx_std_error e, ::std::size_t got) noexcept {
				if (e.domain != nullptr) [[unlikely]]
				{
					resolve(e);
					return;
				}
				if (got == 0)
				{
					eof_resolve();
					return;
				}
				if (!pump())
				{
					submit_underflow();
				}
				else
				{
					resolve(this->err);
				}
			});
	}

	inline bool await_ready() noexcept
	{
		return pump();
	}
	inline bool await_suspend(::std::coroutine_handle<> h) noexcept
	{
		this->coro = h;
		submit_underflow();
		return this->async_suspend_done();
	}
	inline T await_resume() throws
	{
		if (this->err.domain != nullptr)
		{
			auto e{this->err};
			this->err = {};
			throw throws e;
		}
		return ::std::move(this->value);
	}
};

/*
 * async_scan_get awaiter for precise_reserve_scannable T (fixed-size
 * scans): like the sync path, enough pending buffer satisfies the parse
 * in place; otherwise n bytes are read_all'd asynchronously into the
 * awaiter's own array — which drains the input buffer first — and the
 * result is parsed on completion.
 */
template <typename scheduler, typename instmtype, typename T, ::std::size_t n>
struct async_scan_get_precise_awaiter : async_awaiter_result<T>
{
	using char_type = typename instmtype::input_char_type;
	using arg_type = decltype(::fast_io::io_scan_forward<char_type>(
		::fast_io::io_scan_alias(::std::declval<T &>())));

	scheduler sched;
	instmtype instm;
	::fast_io::posix_statx_timestamp_opt timeout;
	char_type buffer[n == 0 ? 1 : n];

	inline void resolve(::std::cxx_std_error e) noexcept
	{
		::fast_io::details::async_dispose_error(this->err);
		this->err = e;
		if (this->suspended)
		{
			this->coro.resume();
		}
		else
		{
			this->done = true;
		}
	}

	inline void parse_at(char_type const *p) noexcept
	{
		try
		{
			arg_type arg{this->value};
			if constexpr (::fast_io::precise_reserve_scannable_no_error<char_type, arg_type>)
			{
				scan_precise_reserve_define(::fast_io::io_reserve_type<char_type, arg_type>, p, arg);
				resolve(::std::cxx_std_error{});
			}
			else
			{
				resolve(::fast_io::details::async_make_error(
					scan_precise_reserve_define(::fast_io::io_reserve_type<char_type, arg_type>, p,
												arg)));
			}
		}
		catch throws(::std::error e)
		{
			resolve(e.release());
		}
	}

	/* returns true when the pending window already covers n chars and the
	 * parse resolved inline */
	inline bool pump_buffered() noexcept
	{
		auto curr{ibuffer_curr(instm)};
		if (static_cast<::std::size_t>(ibuffer_end(instm) - curr) < n)
		{
			return false;
		}
		parse_at(curr);
		ibuffer_set_curr(instm, curr + n);
		return true;
	}

	inline bool await_ready() noexcept
	{
		return pump_buffered();
	}
	inline bool await_suspend(::std::coroutine_handle<> h) noexcept
	{
		this->coro = h;
		::fast_io::operations::decay::async_pread_all_bytes_decay_callback(
			sched, instm, reinterpret_cast<::std::byte *>(buffer), n * sizeof(char_type),
			::fast_io::intfpos_opt{}, timeout,
			[this](::std::cxx_std_error e) noexcept {
				if (e.domain != nullptr) [[unlikely]]
				{
					resolve(e);
					return;
				}
				parse_at(buffer);
			});
		return this->async_suspend_done();
	}
	inline T await_resume() throws
	{
		if (this->err.domain != nullptr)
		{
			auto e{this->err};
			this->err = {};
			throw throws e;
		}
		return ::std::move(this->value);
	}
};

} // namespace fast_io::details

namespace fast_io::operations::decay
{

/*
 * co_await async_scan_get<T>(scheduler, instm, timeout): parse one T out
 * of a buffered input stream. The stream is reduced to its
 * input_stream_ref (a basic_io_buffer_ref for buffered streams) — async
 * scanning needs the pending-window machinery, so the stream must expose
 * ibuffer operations and an async_ibuffer_underflow, exactly like sync
 * scan needs basic_ibuf. Suspends on parse_errc::partial to refill the
 * buffer asynchronously; resolves inline when the window suffices.
 * Errors — including a truncated object at EOF — come back through
 * await_resume as a real herbception. The stream must outlive the
 * operation.
 */
template <typename T, typename async_scheduler_type, typename instmtype>
	requires ::fast_io::operations::decay::defines::async_scheduler_observer<async_scheduler_type>
inline auto async_scan_get(async_scheduler_type scheduler, instmtype &&instm,
						   ::fast_io::posix_statx_timestamp_opt timeout) noexcept
{
	using instm_reftype = ::fast_io::details::async_input_stream_ref_t<instmtype>;
	using char_type = typename instm_reftype::input_char_type;
	using arg_type = decltype(::fast_io::io_scan_forward<char_type>(
		::fast_io::io_scan_alias(::std::declval<T &>())));
	static_assert(sizeof(char_type) == 1, "async scan is supported for sizeof(char_type) == 1 only");
	static_assert(::fast_io::operations::decay::defines::has_ibuffer_basic_operations<instm_reftype>,
				  "async scan needs a buffered input stream (basic_ibuf et al.)");
	static_assert(::std::default_initializable<T>, "async_scan_get<T> needs a default-initializable T");
	if constexpr (::fast_io::precise_reserve_scannable<char_type, arg_type>)
	{
		constexpr ::std::size_t n{
			scan_precise_reserve_size(::fast_io::io_reserve_type<char_type, arg_type>)};
		static_assert(
			::fast_io::operations::decay::defines::
				has_async_pread_some_bytes_underflow_callback_define<
					async_scheduler_type, instm_reftype,
					decltype([](::std::cxx_std_error, ::std::size_t) noexcept {})>,
			"precise async scan needs the stream's async pread");
		return ::fast_io::details::async_scan_get_precise_awaiter<async_scheduler_type, instm_reftype,
																T, n>{
			{},
			scheduler,
			::fast_io::details::async_input_stream_ref(
				::fast_io::freestanding::forward<instmtype>(instm)),
			timeout};
	}
	else
	{
		static_assert(::fast_io::context_scannable<char_type, arg_type>,
					  "type not scannable. need context_scannable");
		static_assert(
			::fast_io::operations::decay::defines::has_async_ibuffer_underflow<
				async_scheduler_type, instm_reftype,
				decltype([](::std::cxx_std_error, ::std::size_t) noexcept {})>,
			"context async scan needs the stream's async_ibuffer_underflow");
		return ::fast_io::details::async_scan_get_context_awaiter<async_scheduler_type, instm_reftype,
																T>{
			{},
			scheduler,
			::fast_io::details::async_input_stream_ref(
				::fast_io::freestanding::forward<instmtype>(instm)),
			timeout};
	}
}

} // namespace fast_io::operations::decay
