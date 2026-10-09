#pragma once
/*
 * IOCP backend for fast_io's generic async layer, on the raw win32 API
 * surface declared in win32/api/apis.h.
 *
 * The scheduler is a kernel completion port: the owning object is
 * io_async_scheduler (win32_file) opened with fi::io_async — its ctor
 * runs CreateIoCompletionPort(INVALID_HANDLE_VALUE, ...); the
 * trivially-copyable io_async_observer (win32_io_observer) is what every
 * async operation takes.
 *
 * Streams must be opened with open_mode::no_block — it maps to
 * FILE_FLAG_OVERLAPPED for files/pipes and WSA_FLAG_OVERLAPPED for
 * sockets. A stream is associated with the port lazily at submission
 * (CreateIoCompletionPort on an already-associated handle reports
 * ERROR_INVALID_PARAMETER and is left alone).
 *
 * Backend contract (operations/refs/async.h):
 * - async_pread_some_bytes_underflow_callback_define /
 *   async_pwrite_some_bytes_overflow_callback_define: required.
 *   Everything else (all forms, scatter, transmit, buffered) is built on
 *   them generically.
 * All defines are noexcept; every error — including submission
 * failures — is delivered through the callback as ::std::cxx_std_error.
 *
 * Timeouts are per-operation: posix_statx_timestamp_opt is a relative
 * deadline. There is no linked-timeout primitive here, so each timed op
 * arms a threadpool timer; on expiry the callback flags the cookie and
 * runs CancelIoEx, and the aborted completion reports errc::timed_out.
 * The timer callback never owns or frees state — the cookie lives until
 * its op completion arrives, and the completion path joins outstanding
 * timer callbacks before freeing.
 */

#if !defined(_WIN32_WINDOWS)

namespace fast_io
{

/*
 * Owning-scheduler reduction: a win32 file holding a completion port
 * decays to its observer for the decay layer.
 */
template <win32_family family, ::std::integral char_type>
inline constexpr basic_win32_family_io_observer<family, char_type>
async_scheduler_ref_define(basic_win32_family_file<family, char_type> &file) noexcept
{
	return {file.native_handle()};
}

/* every win32 stream family (nt, win32, socket) schedules on a port
 * observer regardless of its own char type */
template <nt_family family, ::std::integral char_type>
inline constexpr ::fast_io::io_type_t<win32_io_observer>
async_scheduler_type(basic_nt_family_io_observer<family, char_type>) noexcept
{
	return {};
}

template <win32_family family, ::std::integral char_type>
inline constexpr ::fast_io::io_type_t<win32_io_observer>
async_scheduler_type(basic_win32_family_io_observer<family, char_type>) noexcept
{
	return {};
}

template <win32_family family, ::std::integral char_type>
inline constexpr ::fast_io::io_type_t<win32_io_observer>
async_scheduler_type(basic_win32_family_socket_io_observer<family, char_type>) noexcept
{
	return {};
}

namespace details
{

/*
 * Internal completion ABI: GetQueuedCompletionStatus hands back the
 * OVERLAPPED* the op was submitted with. Every IOCP state object embeds
 * win32_iocp_state_base FIRST so the completion pointer IS the state
 * pointer; invoke() is the per-op finisher. The timer thunk also reaches
 * file_handle/timed_out through this base, so it needs no knowledge of
 * the concrete cookie type. Not part of the public API.
 */
using win32_iocp_invoke_func = void (*)(void *, ::std::size_t, ::std::uint_least32_t) noexcept;

struct win32_iocp_state_base
{
	::fast_io::win32::overlapped ovl;
	win32_iocp_invoke_func invoke;
	void *file_handle{}; /* stream handle, for CancelIoEx */
	void *timer{};       /* PTP_TIMER when a deadline is armed */
	bool timed_out{};
};

inline void win32_iocp_dispatch(::fast_io::win32::overlapped *over, ::std::size_t transferred,
								::std::uint_least32_t err) noexcept
{
	auto *base{reinterpret_cast<win32_iocp_state_base *>(over)};
	base->invoke(base, transferred, err);
}

/*
 * Threadpool-timer thunk: runs on a pool worker. Only touches the state
 * base — flags the cookie and cancels the pending op; the aborted
 * completion is what frees it.
 */
inline void FAST_IO_WINSTDCALL win32_iocp_timer_thunk(void *, void *context, void *) noexcept
{
	auto *state{static_cast<win32_iocp_state_base *>(context)};
	state->timed_out = true;
	::fast_io::win32::CancelIoEx(state->file_handle, __builtin_addressof(state->ovl));
}

/*
 * Cookie for one pending read or write; both ops share the layout —
 * win32_iocp_state_base stays the first member because
 * win32_iocp_dispatch dereferences the OVERLAPPED* as it. The wsabuf
 * slot feeds socket submissions (WSARecv/WSASend take a wsabuf, not a
 * raw byte pointer); file submissions leave it unused.
 */
template <typename alloc_type, typename T>
struct win32_iocp_rw_cookie
{
	using allocator_type = alloc_type;
	static inline constexpr bool alloc_with_status{alloc_type::has_status};
	win32_iocp_state_base base;
	::fast_io::win32::wsabuf wsa{};
	T callback;
	FAST_IO_NO_UNIQUE_ADDRESS ::std::conditional_t<alloc_with_status, typename alloc_type::handle_type,
											   ::fast_io::details::empty>
		alloc_handle{};
};

/*
 * Completion finisher for a read or write cookie: joins the deadline
 * timer, frees the cookie, invokes the user callback exactly once.
 */
template <typename alloc_type, typename T, bool is_read>
inline void win32_iocp_rw_deliver(void *self, ::std::size_t transferred,
								  ::std::uint_least32_t err) noexcept
{
	using cookie_type = win32_iocp_rw_cookie<alloc_type, T>;
	auto *cookie{static_cast<cookie_type *>(self)};
	if (auto *timer{cookie->base.timer}; timer != nullptr)
	{
		::fast_io::win32::SetThreadpoolTimer(timer, nullptr, 0, 0);
		::fast_io::win32::WaitForThreadpoolTimerCallbacks(timer, 1);
		::fast_io::win32::CloseThreadpoolTimer(timer);
	}
	bool const timed_out{cookie->base.timed_out};
	auto callback{::std::move(cookie->callback)};
	::fast_io::details::async_delete_state(cookie);
	::std::cxx_std_error e{};
	if (timed_out && err != 0) [[unlikely]]
	{
		/* the timer ran CancelIoEx and the completion is the aborted op;
		 * a completion that beat the deadline arrives with err == 0 and
		 * still reports success */
		e = ::fast_io::details::async_make_error(::std::errc::timed_out);
	}
	else if constexpr (is_read)
	{
		/* ERROR_HANDLE_EOF (38) / ERROR_BROKEN_PIPE (109) on a read are
		 * stream EOF, not errors: a zero-byte completion matches posix
		 * read() semantics */
		if (err == 38u || err == 109u)
		{
			transferred = 0;
			err = 0;
		}
	}
	if (err != 0) [[unlikely]]
	{
		e = ::fast_io::details::async_make_error(
			static_cast<::fast_io::freestanding::win32_errc>(err));
	}
	callback(e, transferred);
}

/*
 * Associate the stream handle with the port. Re-association of an
 * already-associated handle fails with ERROR_INVALID_PARAMETER (87) —
 * benign, the handle rides whichever port it already has.
 */
inline void win32_iocp_associate(void *port, void *handle) throws
{
	if (::fast_io::win32::CreateIoCompletionPort(handle, port, 0, 0) == nullptr) [[unlikely]]
	{
		auto err{::fast_io::win32::GetLastError()};
		if (err != 87u) // ERROR_INVALID_PARAMETER
		{
			throw_win32_error(err);
		}
	}
}

/* posix_statx_timestamp64 -> relative FILETIME due time (negative
 * 100ns units), rounded up and clamped away from the disarm value 0 */
inline ::fast_io::win32::filetime
win32_iocp_relative_deadline(::fast_io::posix_statx_timestamp64 timeout) noexcept
{
	::std::int_least64_t due{-(
		static_cast<::std::int_least64_t>(timeout.tv_sec) * 10000000 +
		static_cast<::std::int_least64_t>((timeout.tv_nsec + 99u) / 100u))};
	if (due == 0)
	{
		due = -1;
	}
	auto u{static_cast<::std::uint_least64_t>(due)};
	return {static_cast<::std::uint_least32_t>(u), static_cast<::std::uint_least32_t>(u >> 32)};
}

/*
 * One read/write submission against either a file-family handle
 * (ReadFile/WriteFile) or a socket (WSARecv/WSASend); is_socket is the
 * only branch difference. Every failure — including submission
 * failures — is delivered through the callback exactly once.
 */
template <bool is_write, typename sched_type, typename stream_type, typename T>
inline void win32_iocp_rw_submit(sched_type sched, stream_type stream, void *first,
								 ::std::size_t count, ::fast_io::intfpos_opt off,
								 ::fast_io::posix_statx_timestamp_opt timeout, T &&callback) noexcept
{
	using callback_type = ::std::remove_cvref_t<T>;
	using alloc_type = ::fast_io::details::async_scheduler_allocator_t<sched_type>;
	using cookie_type = win32_iocp_rw_cookie<alloc_type, callback_type>;
	constexpr bool is_socket{
		::std::same_as<typename ::std::remove_cvref_t<stream_type>::native_handle_type,
					   ::std::size_t>};

	::std::cxx_std_error early_err{};
	void *file_handle{};
	if constexpr (is_socket)
	{
		if (off.has_opt) [[unlikely]]
		{
			early_err = ::fast_io::details::async_make_error(::std::errc::invalid_seek);
		}
		file_handle = reinterpret_cast<void *>(stream.native_handle());
	}
	else
	{
		file_handle = stream.native_handle();
	}
	if (early_err.domain == nullptr)
	{
		try
		{
			win32_iocp_associate(sched.native_handle(), file_handle);
		}
		catch throws(::std::error e)
		{
			early_err = e.release();
		}
	}
	if (early_err.domain != nullptr)
	{
		auto e{early_err};
		early_err = {};
		callback(e, 0zu);
		return;
	}

	cookie_type *cookie;
	try
	{
		cookie = ::fast_io::details::async_new_state_plain<cookie_type>(
			sched,
			win32_iocp_state_base{{},
								  &win32_iocp_rw_deliver<alloc_type, callback_type, !is_write>,
								  file_handle},
			::fast_io::win32::wsabuf{}, callback_type{::std::forward<T>(callback)});
	}
	catch throws(::std::error e)
	{
		callback(e.release(), 0zu);
		return;
	}

	if (off.has_opt)
	{
		try
		{
			::fast_io::win32::details::win32_calculate_offset_impl(file_handle, cookie->base.ovl,
																   off.opt);
		}
		catch throws(::std::error e)
		{
			auto cb{::std::move(cookie->callback)};
			::fast_io::details::async_delete_state(cookie);
			cb(e.release(), 0zu);
			return;
		}
	}
	else if constexpr (!is_socket)
	{
		/* empty offset = current file position: FILE_USE_FILE_POINTER_POSITION */
		cookie->base.ovl.dummy_union_name.dummy_struct_name = {~0u, ~0u};
	}
	::std::uint_least32_t const len{
		::fast_io::details::read_write_bytes_compute<::std::uint_least32_t>(
			static_cast<::std::byte *>(first), count)};
	if constexpr (is_socket)
	{
		cookie->wsa = {len, static_cast<char *>(first)};
	}

	/* the deadline timer is created before the op goes out so a
	 * submission failure tears down cleanly; it is armed only once the
	 * kernel owns the op. A zero-length timeout fails immediately. */
	if (timeout.has_opt)
	{
		if (timeout.opt.tv_sec == 0 && timeout.opt.tv_nsec == 0) [[unlikely]]
		{
			auto cb{::std::move(cookie->callback)};
			::fast_io::details::async_delete_state(cookie);
			cb(::fast_io::details::async_make_error(::std::errc::timed_out), 0zu);
			return;
		}
		cookie->base.timer = ::fast_io::win32::CreateThreadpoolTimer(
			win32_iocp_timer_thunk, cookie, nullptr);
		if (cookie->base.timer == nullptr) [[unlikely]]
		{
			auto err{::fast_io::win32::GetLastError()};
			auto cb{::std::move(cookie->callback)};
			::fast_io::details::async_delete_state(cookie);
			cb(::fast_io::details::async_make_error(
				   static_cast<::fast_io::freestanding::win32_errc>(err)),
			   0zu);
			return;
		}
	}

	::std::uint_least32_t err{};
	bool pending{};
	if constexpr (is_socket)
	{
		auto const sock{stream.native_handle()};
		if constexpr (is_write)
		{
			pending = ::fast_io::win32::WSASend(sock, __builtin_addressof(cookie->wsa), 1u, nullptr,
												0u, __builtin_addressof(cookie->base.ovl),
												nullptr) == 0;
			if (!pending)
			{
				err = ::fast_io::win32::WSAGetLastError();
				pending = err == 997u; // WSA_IO_PENDING
			}
		}
		else
		{
			::std::uint_least32_t flags{};
			pending = ::fast_io::win32::WSARecv(sock, __builtin_addressof(cookie->wsa), 1u, nullptr,
												__builtin_addressof(flags),
												__builtin_addressof(cookie->base.ovl),
												nullptr) == 0;
			if (!pending)
			{
				err = ::fast_io::win32::WSAGetLastError();
				pending = err == 997u;
			}
		}
	}
	else if constexpr (is_write)
	{
		pending = ::fast_io::win32::WriteFile(file_handle, first, len, nullptr,
											  __builtin_addressof(cookie->base.ovl)) != 0;
		if (!pending)
		{
			err = ::fast_io::win32::GetLastError();
			pending = err == 997u; // ERROR_IO_PENDING
		}
	}
	else
	{
		pending = ::fast_io::win32::ReadFile(file_handle, first, len, nullptr,
											 __builtin_addressof(cookie->base.ovl)) != 0;
		if (!pending)
		{
			err = ::fast_io::win32::GetLastError();
			pending = err == 997u;
		}
	}

	if (pending)
	{
		/* synchronous success and ERROR_IO_PENDING both end here: the
		 * kernel queues the completion packet either way */
		if (auto *timer{cookie->base.timer}; timer != nullptr)
		{
			auto due{win32_iocp_relative_deadline(timeout.opt)};
			::fast_io::win32::SetThreadpoolTimer(timer, __builtin_addressof(due), 0, 0);
		}
		return;
	}

	if (auto *timer{cookie->base.timer}; timer != nullptr)
	{
		::fast_io::win32::CloseThreadpoolTimer(timer);
	}
	auto cb{::std::move(cookie->callback)};
	::fast_io::details::async_delete_state(cookie);
	cb(::fast_io::details::async_make_error(
		   static_cast<::fast_io::freestanding::win32_errc>(err)),
	   0zu);
}

/*
 * Cookie for one pending AcceptEx: the accept socket is created at
 * submission (AcceptEx takes a pre-created socket), closed on any
 * failure, and SO_UPDATE_ACCEPT_CONTEXT'd on success so it can receive
 * and send. file_handle in the base stays the LISTEN socket — CancelIoEx
 * targets the op's owner and SO_UPDATE_ACCEPT_CONTEXT needs it.
 */
template <typename alloc_type, typename T>
struct win32_iocp_accept_cookie
{
	using allocator_type = alloc_type;
	static inline constexpr bool alloc_with_status{alloc_type::has_status};
	win32_iocp_state_base base;
	::std::size_t accept_sock{};
	::std::uint_least32_t received{};
	T callback;
	/* AcceptEx writes the local and remote addresses into the output
	 * buffer: each slot is sockaddr+16 per MSDN. Wine rejects a null
	 * lpOutputBuffer outright (WSAEINVAL) even when no addresses are
	 * wanted, so the buffer must always be real. */
	::std::byte addrbuf[2 * (sizeof(::fast_io::posix_sockaddr_in6) + 16)]{};
	FAST_IO_NO_UNIQUE_ADDRESS ::std::conditional_t<alloc_with_status, typename alloc_type::handle_type,
											   ::fast_io::details::empty>
		alloc_handle{};
};

/*
 * Resolves AcceptEx through the socket extension-function ioctl; the
 * entry point lives in mswsock.dll so there is no import to pragma-map.
 * WSAID_ACCEPTEX = {b5367df1-cbac-11cf-95ca-00805f48a192} in the
 * wire-layout byte order GUIDs marshal in.
 */
inline ::fast_io::win32::acceptex_func win32_iocp_acceptex(::std::size_t listen_sock) throws
{
	static constexpr ::std::byte const guid[16]{
		::std::byte{0xf1}, ::std::byte{0x7d}, ::std::byte{0x36}, ::std::byte{0xb5},
		::std::byte{0xac}, ::std::byte{0xcb}, ::std::byte{0xcf}, ::std::byte{0x11},
		::std::byte{0x95}, ::std::byte{0xca}, ::std::byte{0x00}, ::std::byte{0x80},
		::std::byte{0x5f}, ::std::byte{0x48}, ::std::byte{0xa1}, ::std::byte{0x92}};
	::fast_io::win32::acceptex_func fp{};
	::std::uint_least32_t nbytes{};
	/* SIO_GET_EXTENSION_FUNCTION_POINTER */
	if (::fast_io::win32::WSAIoctl(listen_sock, 0xc8000006u,
								   const_cast<::std::byte *>(guid),
								   static_cast<::std::uint_least32_t>(sizeof(guid)),
								   __builtin_addressof(fp),
								   static_cast<::std::uint_least32_t>(sizeof(fp)),
								   __builtin_addressof(nbytes), nullptr, nullptr) != 0 ||
		fp == nullptr) [[unlikely]]
	{
		throw_win32_error(static_cast<::std::uint_least32_t>(::fast_io::win32::WSAGetLastError()));
	}
	return fp;
}

/*
 * Accepted-socket teardown + success fixup. On success the socket still
 * lacks a transport context until SO_UPDATE_ACCEPT_CONTEXT stamps the
 * listener's — the kernel rejects recv/send on it otherwise.
 */
template <typename alloc_type, typename T>
inline void win32_iocp_accept_deliver(void *self, ::std::size_t,
									  ::std::uint_least32_t err) noexcept
{
	using cookie_type = win32_iocp_accept_cookie<alloc_type, T>;
	auto *cookie{static_cast<cookie_type *>(self)};
	if (auto *timer{cookie->base.timer}; timer != nullptr)
	{
		::fast_io::win32::SetThreadpoolTimer(timer, nullptr, 0, 0);
		::fast_io::win32::WaitForThreadpoolTimerCallbacks(timer, 1);
		::fast_io::win32::CloseThreadpoolTimer(timer);
	}
	bool const timed_out{cookie->base.timed_out};
	::std::size_t const accept_sock{cookie->accept_sock};
	::std::size_t const listen_sock{
		reinterpret_cast<::std::size_t>(cookie->base.file_handle)};
	auto callback{::std::move(cookie->callback)};
	::fast_io::details::async_delete_state(cookie);

	::std::cxx_std_error e{};
	if (timed_out && err != 0) [[unlikely]]
	{
		/* the timer ran CancelIoEx and this completion is the aborted op;
		 * a completion that beat the deadline arrives with err == 0 and
		 * still reports success */
		e = ::fast_io::details::async_make_error(::std::errc::timed_out);
	}
	else if (err != 0) [[unlikely]]
	{
		e = ::fast_io::details::async_make_error(
			static_cast<::fast_io::freestanding::win32_errc>(err));
	}
	else if (::fast_io::win32::setsockopt(accept_sock, 0xffffu /* SOL_SOCKET */,
										  0x700bu /* SO_UPDATE_ACCEPT_CONTEXT */,
										  __builtin_addressof(listen_sock),
										  static_cast<int>(sizeof(listen_sock))) != 0) [[unlikely]]
	{
		e = ::fast_io::details::async_make_error(
			static_cast<::fast_io::freestanding::win32_errc>(
				static_cast<::std::uint_least32_t>(::fast_io::win32::WSAGetLastError())));
	}
	if (e.domain != nullptr) [[unlikely]]
	{
		::fast_io::win32::closesocket(accept_sock);
		callback(e, 0zu);
		return;
	}
	callback(e, accept_sock);
}

/*
 * One AcceptEx submission: resolve the extension pointer, mint the
 * accept socket with the listener's family (getsockname), associate the
 * listener with the port, submit. Every failure goes through the
 * callback exactly once.
 */
template <::fast_io::win32_family family, typename sched_type, typename stream_type, typename T>
inline void win32_iocp_accept_submit(sched_type sched, stream_type stream,
									 ::fast_io::open_mode m,
									 ::fast_io::posix_statx_timestamp_opt timeout,
									 T &&callback) noexcept
{
	using callback_type = ::std::remove_cvref_t<T>;
	using alloc_type = ::fast_io::details::async_scheduler_allocator_t<sched_type>;
	using cookie_type = win32_iocp_accept_cookie<alloc_type, callback_type>;

	::std::size_t const listen_sock{stream.native_handle()};
	::fast_io::win32::acceptex_func acceptex{};
	::std::size_t accept_sock{};
	try
	{
		acceptex = win32_iocp_acceptex(listen_sock);
		/* the accept socket's family has to match the listener's;
		 * getsockname reports the bound address's family */
		::fast_io::posix_sockaddr_in6 addrbuf{};
		int addrlen{static_cast<int>(sizeof(addrbuf))};
		if (::fast_io::win32::getsockname(listen_sock, __builtin_addressof(addrbuf),
										  __builtin_addressof(addrlen)) != 0) [[unlikely]]
		{
			throw_win32_error(
				static_cast<::std::uint_least32_t>(::fast_io::win32::WSAGetLastError()));
		}
		::std::uint_least32_t dwflags{
			(family == ::fast_io::win32_family::wide_nt
				 ? ::fast_io::to_win32_sock_open_mode(m)
				 : ::fast_io::to_win32_sock_open_mode_9xa(m)) |
			/* WSA_FLAG_OVERLAPPED: async sockets always need it */
			0x01u};
		accept_sock = ::fast_io::win32::details::open_win32_socket_raw_impl<family>(
			addrbuf.sin6_family, ::fast_io::to_win32_sock_type(::fast_io::sock_type::stream),
			::fast_io::to_win32_sock_protocol(::fast_io::sock_protocol::tcp), dwflags);
		if ((m & ::fast_io::open_mode::no_block) == ::fast_io::open_mode::no_block)
		{
			::fast_io::win32::details::win32_socket_apply_no_block(accept_sock);
		}
		win32_iocp_associate(sched.native_handle(), reinterpret_cast<void *>(listen_sock));
	}
	catch throws(::std::error e)
	{
		if (accept_sock != 0)
		{
			::fast_io::win32::closesocket(accept_sock);
		}
		callback(e.release(), 0zu);
		return;
	}

	cookie_type *cookie;
	try
	{
		cookie = ::fast_io::details::async_new_state_plain<cookie_type>(
			sched,
			win32_iocp_state_base{{},
								  &win32_iocp_accept_deliver<alloc_type, callback_type>,
								  reinterpret_cast<void *>(listen_sock)},
			accept_sock, 0u, callback_type{::std::forward<T>(callback)});
	}
	catch throws(::std::error e)
	{
		::fast_io::win32::closesocket(accept_sock);
		callback(e.release(), 0zu);
		return;
	}

	/* the deadline timer is created before the op goes out so a
	 * submission failure tears down cleanly; it is armed only once the
	 * kernel owns the op */
	if (timeout.has_opt)
	{
		if (timeout.opt.tv_sec == 0 && timeout.opt.tv_nsec == 0) [[unlikely]]
		{
			auto cb{::std::move(cookie->callback)};
			::fast_io::details::async_delete_state(cookie);
			::fast_io::win32::closesocket(accept_sock);
			cb(::fast_io::details::async_make_error(::std::errc::timed_out), 0zu);
			return;
		}
		cookie->base.timer = ::fast_io::win32::CreateThreadpoolTimer(
			win32_iocp_timer_thunk, cookie, nullptr);
		if (cookie->base.timer == nullptr) [[unlikely]]
		{
			auto err{::fast_io::win32::GetLastError()};
			auto cb{::std::move(cookie->callback)};
			::fast_io::details::async_delete_state(cookie);
			::fast_io::win32::closesocket(accept_sock);
			cb(::fast_io::details::async_make_error(
				   static_cast<::fast_io::freestanding::win32_errc>(err)),
			   0zu);
			return;
		}
	}

	/* dwReceiveDataLength 0: the accepted socket is returned without
	 * waiting for payload. The address buffer stays real — wine's
	 * AcceptEx rejects a null lpOutputBuffer with WSAEINVAL, and real
	 * Windows wants sockaddr_storage+16 per address slot anyway.
	 * Synchronous success and WSA_IO_PENDING both queue a completion */
	constexpr ::std::uint_least32_t addrslotlen{
		static_cast<::std::uint_least32_t>(sizeof(::fast_io::posix_sockaddr_in6) + 16)};
	if (acceptex(listen_sock, accept_sock, cookie->addrbuf, 0u, addrslotlen, addrslotlen,
				 __builtin_addressof(cookie->received),
				 __builtin_addressof(cookie->base.ovl)) == 0)
	{
		auto const err{
			static_cast<::std::uint_least32_t>(::fast_io::win32::WSAGetLastError())};
		if (err != 997u) [[unlikely]] // WSA_IO_PENDING
		{
			if (auto *timer{cookie->base.timer}; timer != nullptr)
			{
				::fast_io::win32::CloseThreadpoolTimer(timer);
			}
			auto cb{::std::move(cookie->callback)};
			::fast_io::details::async_delete_state(cookie);
			::fast_io::win32::closesocket(accept_sock);
			cb(::fast_io::details::async_make_error(
				   static_cast<::fast_io::freestanding::win32_errc>(err)),
			   0zu);
			return;
		}
	}
	if (auto *timer{cookie->base.timer}; timer != nullptr)
	{
		auto due{win32_iocp_relative_deadline(timeout.opt)};
		::fast_io::win32::SetThreadpoolTimer(timer, __builtin_addressof(due), 0, 0);
	}
}

} // namespace details

/*
 * Event pump: dequeue one completion and dispatch it to its cookie.
 * io_async_wait blocks; io_async_peek returns false when nothing is
 * queued; io_async_wait_timeout returns false when the deadline elapsed.
 * A completion carrying no OVERLAPPED (PostQueuedCompletionStatus) is an
 * external wakeup — consumed, nothing dispatched.
 */
inline void io_async_wait(win32_io_observer port) throws
{
	::std::uint_least32_t transferred{};
	::std::size_t completionkey{};
	::fast_io::win32::overlapped *over{};
	::std::uint_least32_t err{};
	if (!::fast_io::win32::GetQueuedCompletionStatus(port.native_handle(),
													 __builtin_addressof(transferred),
													 __builtin_addressof(completionkey),
													 __builtin_addressof(over),
													 ~0u)) [[unlikely]]
	{
		if (over == nullptr)
		{
			throw_win32_error();
		}
		err = ::fast_io::win32::GetLastError();
	}
	if (over != nullptr)
	{
		::fast_io::details::win32_iocp_dispatch(over, transferred, err);
	}
}

inline bool io_async_peek(win32_io_observer port) throws
{
	::std::uint_least32_t transferred{};
	::std::size_t completionkey{};
	::fast_io::win32::overlapped *over{};
	::std::uint_least32_t err{};
	if (!::fast_io::win32::GetQueuedCompletionStatus(port.native_handle(),
													 __builtin_addressof(transferred),
													 __builtin_addressof(completionkey),
													 __builtin_addressof(over),
													 0u))
	{
		if (over == nullptr)
		{
			auto code{::fast_io::win32::GetLastError()};
			if (code == 258u) // WAIT_TIMEOUT
			{
				return false;
			}
			throw_win32_error(code);
		}
		err = ::fast_io::win32::GetLastError();
	}
	if (over != nullptr)
	{
		::fast_io::details::win32_iocp_dispatch(over, transferred, err);
	}
	return true;
}

inline bool io_async_wait_timeout(win32_io_observer port,
								  ::fast_io::posix_statx_timestamp64 timeout) throws
{
	/* the port bound is a relative duration here, same as the uring
	 * backend — convert {sec, nsec} to GQCS milliseconds */
	auto ms{static_cast<::std::uint_least64_t>(timeout.tv_sec) * 1000u +
			timeout.tv_nsec / 1000000u};
	if (ms > 0xFFFFFFFEu) [[unlikely]]
	{
		ms = 0xFFFFFFFEu; // anything longer saturates below INFINITE
	}
	::std::uint_least32_t transferred{};
	::std::size_t completionkey{};
	::fast_io::win32::overlapped *over{};
	::std::uint_least32_t err{};
	if (!::fast_io::win32::GetQueuedCompletionStatus(port.native_handle(),
													 __builtin_addressof(transferred),
													 __builtin_addressof(completionkey),
													 __builtin_addressof(over),
													 static_cast<::std::uint_least32_t>(ms)))
	{
		if (over == nullptr)
		{
			auto code{::fast_io::win32::GetLastError()};
			if (code == 258u) // WAIT_TIMEOUT
			{
				return false;
			}
			throw_win32_error(code);
		}
		err = ::fast_io::win32::GetLastError();
	}
	if (over != nullptr)
	{
		::fast_io::details::win32_iocp_dispatch(over, transferred, err);
	}
	return true;
}

/* ======================= backend defines ======================= */

/*
 * async_pread_some_bytes_underflow_callback_define: required low-level
 * define. cb is invoked once as cb(::std::cxx_std_error, ::std::size_t)
 * noexcept; a fired timeout reports errc::timed_out. Files and pipes
 * ride ReadFile; sockets ride WSARecv (posix_statx offsets are
 * meaningless there and are rejected). Handle family observers (nt and
 * win32) both arrive through the void* handle path.
 */
template <nt_family family, ::std::integral char_type, typename func>
	requires ::fast_io::operations::decay::defines::async_bytes_completion_callback<
		::std::remove_cvref_t<func>>
inline void async_pread_some_bytes_underflow_callback_define(
	::fast_io::win32_io_observer sched,
	::fast_io::basic_nt_family_io_observer<family, char_type> ntiob, ::std::byte *first,
	::std::size_t count, ::fast_io::intfpos_opt off,
	::fast_io::posix_statx_timestamp_opt timeout, func callback) noexcept
{
	::fast_io::details::win32_iocp_rw_submit<false>(
		sched, ntiob, first, count, off, timeout, ::std::move(callback));
}

template <win32_family family, ::std::integral char_type, typename func>
	requires ::fast_io::operations::decay::defines::async_bytes_completion_callback<
		::std::remove_cvref_t<func>>
inline void async_pread_some_bytes_underflow_callback_define(
	::fast_io::win32_io_observer sched,
	::fast_io::basic_win32_family_io_observer<family, char_type> wiob, ::std::byte *first,
	::std::size_t count, ::fast_io::intfpos_opt off,
	::fast_io::posix_statx_timestamp_opt timeout, func callback) noexcept
{
	::fast_io::details::win32_iocp_rw_submit<false>(
		sched, wiob, first, count, off, timeout, ::std::move(callback));
}

template <win32_family family, ::std::integral char_type, typename func>
	requires ::fast_io::operations::decay::defines::async_bytes_completion_callback<
		::std::remove_cvref_t<func>>
inline void async_pread_some_bytes_underflow_callback_define(
	::fast_io::win32_io_observer sched,
	::fast_io::basic_win32_family_socket_io_observer<family, char_type> wsiob, ::std::byte *first,
	::std::size_t count, ::fast_io::intfpos_opt off,
	::fast_io::posix_statx_timestamp_opt timeout, func callback) noexcept
{
	::fast_io::details::win32_iocp_rw_submit<false>(
		sched, wsiob, first, count, off, timeout, ::std::move(callback));
}

template <nt_family family, ::std::integral char_type, typename func>
	requires ::fast_io::operations::decay::defines::async_bytes_completion_callback<
		::std::remove_cvref_t<func>>
inline void async_pwrite_some_bytes_overflow_callback_define(
	::fast_io::win32_io_observer sched,
	::fast_io::basic_nt_family_io_observer<family, char_type> ntiob, ::std::byte const *first,
	::std::size_t count, ::fast_io::intfpos_opt off,
	::fast_io::posix_statx_timestamp_opt timeout, func callback) noexcept
{
	::fast_io::details::win32_iocp_rw_submit<true>(
		sched, ntiob, const_cast<::std::byte *>(first), count, off, timeout,
		::std::move(callback));
}

template <win32_family family, ::std::integral char_type, typename func>
	requires ::fast_io::operations::decay::defines::async_bytes_completion_callback<
		::std::remove_cvref_t<func>>
inline void async_pwrite_some_bytes_overflow_callback_define(
	::fast_io::win32_io_observer sched,
	::fast_io::basic_win32_family_io_observer<family, char_type> wiob, ::std::byte const *first,
	::std::size_t count, ::fast_io::intfpos_opt off,
	::fast_io::posix_statx_timestamp_opt timeout, func callback) noexcept
{
	::fast_io::details::win32_iocp_rw_submit<true>(
		sched, wiob, const_cast<::std::byte *>(first), count, off, timeout,
		::std::move(callback));
}

template <win32_family family, ::std::integral char_type, typename func>
	requires ::fast_io::operations::decay::defines::async_bytes_completion_callback<
		::std::remove_cvref_t<func>>
inline void async_pwrite_some_bytes_overflow_callback_define(
	::fast_io::win32_io_observer sched,
	::fast_io::basic_win32_family_socket_io_observer<family, char_type> wsiob,
	::std::byte const *first, ::std::size_t count, ::fast_io::intfpos_opt off,
	::fast_io::posix_statx_timestamp_opt timeout, func callback) noexcept
{
	::fast_io::details::win32_iocp_rw_submit<true>(
		sched, wsiob, const_cast<::std::byte *>(first), count, off, timeout,
		::std::move(callback));
}

template <win32_family family, ::std::integral char_type>
struct ::fast_io::operations::decay::defines::async_accept_file_type<
	::fast_io::basic_win32_family_socket_io_observer<family, char_type>>
{
	using type = ::fast_io::basic_win32_family_socket_file<family, char_type>;
};

/*
 * async_accept_callback_define: AcceptEx on the listen socket; the
 * callback receives the accepted SOCKET through
 * cb(::std::cxx_std_error, ::std::size_t). The accept socket is created
 * overlapped-capable (WSA_FLAG_OVERLAPPED forced on) and gets
 * SO_UPDATE_ACCEPT_CONTEXT on completion; no_block additionally applies
 * FIONBIO so sync calls report WSAEWOULDBLOCK like posix O_NONBLOCK.
 */
template <win32_family family, ::std::integral char_type, typename func>
	requires ::std::is_nothrow_invocable_v<func, ::std::cxx_std_error, ::std::size_t>
inline void async_accept_callback_define(
	::fast_io::win32_io_observer sched,
	::fast_io::basic_win32_family_socket_io_observer<family, char_type> wsiob,
	::fast_io::open_mode m, ::fast_io::posix_statx_timestamp_opt timeout,
	func callback) noexcept
{
	::fast_io::details::win32_iocp_accept_submit<family>(
		sched, wsiob, m, timeout, ::std::move(callback));
}

} // namespace fast_io

#endif
