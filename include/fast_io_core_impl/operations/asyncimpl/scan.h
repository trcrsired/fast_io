#pragma once

namespace fast_io::details
{

/*
 * underflow awaiter for async scanning: submits one async refill of the
 * input buffer's pending window and resumes the scan coroutine with the
 * byte count — 0 on EOF. Mirrors sync scan's ibuffer_underflow step.
 */
template <typename scheduler, typename instmtype>
struct async_scan_underflow_awaiter : async_awaiter_result<::std::size_t>
{
	scheduler sched;
	instmtype instm;
	::fast_io::posix_statx_timestamp_opt timeout;

	inline constexpr bool await_ready() const noexcept
	{
		return false;
	}
	inline bool await_suspend(::std::coroutine_handle<> h) noexcept
	{
		this->coro = h;
		/* unqualified on purpose: the freestanding-layer define is not
		 * visible here at parse time — ADL reaches it through the
		 * stream ref when the awaiter is instantiated */
		async_ibuffer_underflow(
			sched, instm, timeout,
			[this](::std::cxx_std_error e, ::std::size_t got) noexcept {
				this->err = e;
				this->value = got;
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

} // namespace fast_io::details

namespace fast_io::operations::decay
{

/*
 * co_await-able async scan_some: parses each argument from the input
 * stream's buffer window, refilling it through async_ibuffer_underflow
 * when the context parser reports partial. This is the async mirror of
 * scan_some_freestanding_decay — the args are written in place (they
 * live in the awaiting caller's frame, so nothing here has a lifetime
 * problem) and the result reports the arguments left unscanned,
 * exactly like sync scan_some.
 *
 * The stream is the input_stream_ref (a basic_io_buffer_ref for
 * buffered streams): async scanning needs the pending-window machinery
 * plus an async underflow — a byte read past the token cannot be put
 * back on an unbuffered stream, exactly like sync scan needs basic_ibuf.
 */
template <typename async_scheduler_type, typename instmtype, typename... Args>
inline ::fast_io::io_async_task<
	::fast_io::scan_some_result_t,
	/* the scheduler decides coroutine scheduling and frame allocation;
	 * a status-allocator scheduler supplies .alloc_handle and the
	 * coroutine's promise operator new reads it from this first
	 * parameter — schedulers without one fail fast through
	 * native_global_allocator */
	::fast_io::details::async_scheduler_allocator_t<async_scheduler_type>>
async_scan_some_freestanding_decay(async_scheduler_type sched,
								   ::fast_io::posix_statx_timestamp_opt timeout,
								   instmtype instm, Args... args) throws
{
	static_assert(::fast_io::operations::decay::defines::async_scheduler_observer<
				  async_scheduler_type>);
	static_assert(
		::fast_io::operations::decay::defines::has_ibuffer_basic_operations<instmtype>,
		"If you want to scan this type of file, please add ::fast_io::basic_ibuf.");
	static_assert(
		::fast_io::operations::decay::defines::has_async_ibuffer_underflow<
			async_scheduler_type, instmtype,
			decltype([](::std::cxx_std_error, ::std::size_t) noexcept {})>,
		"async scan needs the stream's async_ibuffer_underflow");
	using char_type = typename instmtype::input_char_type;
	static_assert(sizeof(char_type) == 1,
				  "async scan is supported for sizeof(char_type) == 1 only");
	template for (constexpr auto i : ::fast_io::details::index_array_range<0zu, sizeof...(Args)>)
	{
		using argtype = ::std::remove_cvref_t<Args...[i]>;
		if constexpr (::fast_io::precise_reserve_scannable<char_type, argtype>)
		{
			/* fixed-size scan: n pending chars satisfy it in place, else
			 * read_all them out of the stream into the frame buffer and
			 * parse there — the async equivalent of the sync path */
			constexpr ::std::size_t n{
				scan_precise_reserve_size(::fast_io::io_reserve_type<char_type, argtype>)};
			char_type buffer[n == 0 ? 1 : n];
			auto curr{ibuffer_curr(instm)};
			char_type const *p{curr};
			if (static_cast<::std::size_t>(ibuffer_end(instm) - curr) >= n)
			{
				ibuffer_set_curr(instm, curr + n);
			}
			else
			{
				co_await ::fast_io::operations::decay::async_pread_all_bytes_decay(
					sched, instm, reinterpret_cast<::std::byte *>(buffer),
					n * sizeof(char_type), ::fast_io::intfpos_opt{}, timeout);
				p = buffer;
			}
			if constexpr (::fast_io::precise_reserve_scannable_no_error<char_type, argtype>)
			{
				scan_precise_reserve_define(::fast_io::io_reserve_type<char_type, argtype>, p,
											args...[i]);
			}
			else
			{
				auto ec{scan_precise_reserve_define(
					::fast_io::io_reserve_type<char_type, argtype>, p, args...[i])};
				if (ec != ::fast_io::freestanding::parse_errc::ok)
				{
					if (ec == ::fast_io::freestanding::parse_errc::end_of_file)
					{
						co_return ::fast_io::scan_some_result_t{sizeof...(Args) - i};
					}
					::fast_io::herbceptions::throws_parse_errc(ec);
				}
			}
		}
		else if constexpr (::fast_io::context_scannable<char_type, argtype>)
		{
			/* contiguous attempt first over the pending window; only when
			 * the window runs out does the context machine take over
			 * across refills — same split as the sync path */
			bool need_context{true};
			if constexpr (::fast_io::contiguous_scannable<char_type, argtype>)
			{
				auto curr{ibuffer_curr(instm)};
				auto const end{ibuffer_end(instm)};
				auto [it, ec]{scan_contiguous_define(
					::fast_io::io_reserve_type<char_type, argtype>, curr, end, args...[i])};
				if (it != end)
				{
					need_context = false;
					if constexpr (::std::same_as<decltype(curr), decltype(it)>)
					{
						ibuffer_set_curr(instm, it);
					}
					else
					{
						ibuffer_set_curr(instm, it - curr + curr);
					}
					if (ec != ::fast_io::freestanding::parse_errc::ok)
					{
						if (ec == ::fast_io::freestanding::parse_errc::end_of_file)
						{
							co_return ::fast_io::scan_some_result_t{sizeof...(Args) - i};
						}
						::fast_io::herbceptions::throws_parse_errc(ec);
					}
				}
			}
			if (need_context)
			{
				for (typename ::std::remove_cvref_t<decltype(scan_context_type(
						 io_reserve_type<char_type, argtype>))>::type state;
					 ;)
				{
					auto c{ibuffer_curr(instm)};
					auto const e{ibuffer_end(instm)};
					auto [it, ec]{scan_context_define(io_reserve_type<char_type, argtype>,
													  state, c, e, args...[i])};
					if constexpr (::std::same_as<decltype(c), decltype(it)>)
					{
						ibuffer_set_curr(instm, it);
					}
					else
					{
						ibuffer_set_curr(instm, it - c + c);
					}
					if (ec == ::fast_io::freestanding::parse_errc::ok)
					{
						break;
					}
					if (ec != ::fast_io::freestanding::parse_errc::partial)
					{
						::fast_io::herbceptions::throws_parse_errc(ec);
					}
					if (!co_await ::fast_io::details::async_scan_underflow_awaiter<
							async_scheduler_type, instmtype>{{}, sched, instm, timeout})
						[[unlikely]]
					{
						ec = scan_context_eof_define(io_reserve_type<char_type, argtype>,
													 state, args...[i]);
						if (ec == ::fast_io::freestanding::parse_errc::ok)
						{
							break;
						}
						else if (ec == ::fast_io::freestanding::parse_errc::end_of_file)
						{
							co_return ::fast_io::scan_some_result_t{sizeof...(Args) - i};
						}
						::fast_io::herbceptions::throws_parse_errc(ec);
					}
				}
			}
		}
		else
		{
			constexpr bool not_scannable{
				::fast_io::context_scannable<char_type, argtype>};
			static_assert(not_scannable, "type not scannable. need context_scannable");
		}
	}
	co_return ::fast_io::scan_some_result_t{};
}

} // namespace fast_io::operations::decay

namespace fast_io::operations
{

/*
 * async_scan_some_freestanding(sched, timeout, instm, args...) —
 * co_await-able async scan_some: parses each arg from the input
 * stream's buffer window, refilling asynchronously on partial scans.
 * Returns the same scan_some_result_t as sync scan_some: truthy means
 * every argument scanned; a nonzero remained_args reports the arguments
 * left unscanned.
 *
 * The stream must be a buffered input (basic_ibuf et al.) — scanning a
 * byte stream without a buffer cannot put back the byte past the token.
 */
template <typename async_scheduler_type, typename instmtype, typename... Args>
inline auto async_scan_some_freestanding(async_scheduler_type &&scheduler,
										 ::fast_io::posix_statx_timestamp_opt timeout,
										 instmtype &&instm, Args &&...args)
	FAST_IO_HERBCEPTIONS_THROWS
	requires(::fast_io::operations::decay::defines::async_scheduler_observer<
			 ::std::remove_cvref_t<decltype(::fast_io::operations::async_scheduler_ref(scheduler))>>)
{
	using instm_reftype =
		::std::remove_cvref_t<decltype(::fast_io::operations::input_stream_ref(instm))>;
	using char_type = typename instm_reftype::input_char_type;
	return ::fast_io::operations::decay::async_scan_some_freestanding_decay(
		::fast_io::operations::async_scheduler_ref(scheduler), timeout,
		::fast_io::operations::input_stream_ref(instm),
		::fast_io::io_scan_forward<char_type>(::fast_io::io_scan_alias(args))...);
}

} // namespace fast_io::operations

namespace fast_io
{

inline namespace io
{

/*
 * async_scan_some(sched, timeout, instm, args...) — the device-facing
 * entry: instm must be an async input stream carrying a buffer
 * (basic_ibuf et al.) — a byte past a token cannot be put back on an
 * unbuffered stream, so unlike sync scan there is no stdin default.
 * Returns scan_some_result_t — truthy when every argument scanned.
 */
template <typename async_scheduler_type, typename instmtype, typename... Args>
inline auto async_scan_some(async_scheduler_type &&scheduler,
							::fast_io::posix_statx_timestamp_opt timeout,
							instmtype &&instm, Args &&...args) noexcept
{
	static_assert(
		::fast_io::operations::decay::defines::has_ibuffer_basic_operations<
			::std::remove_cvref_t<decltype(::fast_io::operations::input_stream_ref(instm))>>,
		"async scan needs a buffered input stream (basic_ibuf et al.): a byte past "
		"the token cannot be put back on an unbuffered stream");
	return ::fast_io::operations::async_scan_some_freestanding(
		::std::forward<async_scheduler_type>(scheduler), timeout,
		::std::forward<instmtype>(instm), ::std::forward<Args>(args)...);
}

/*
 * async_scan(sched, timeout, instm, args...) — like sync scan: awaits the
 * async_scan_some task and raises end_of_file through the herbception
 * channel when fewer than all args could be scanned.
 */
template <typename async_scheduler_type, typename instmtype, typename... Args>
inline ::fast_io::io_async_task<
	void, ::fast_io::details::async_scheduler_allocator_t<async_scheduler_type>>
async_scan(async_scheduler_type scheduler,
		   ::fast_io::posix_statx_timestamp_opt timeout, instmtype &&instm, Args &&...args) throws
{
	if (!co_await ::fast_io::operations::async_scan_some_freestanding(
			::std::move(scheduler), timeout,
			::std::forward<instmtype>(instm), ::std::forward<Args>(args)...))
	{
		::fast_io::herbceptions::throws_parse_errc(
			::fast_io::freestanding::parse_errc::end_of_file);
	}
}

} // namespace io

} // namespace fast_io
