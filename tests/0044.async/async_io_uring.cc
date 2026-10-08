#include <fast_io.h>
#include <fast_io_device.h>
#include <coroutine>
#include <cerrno>
#include <cstring>
#include <cstdio>
#include <unistd.h>

static int failures{};

#define CHECK(cond)                                                       \
	do                                                                    \
	{                                                                     \
		if (!(cond))                                                      \
		{                                                                 \
			++failures;                                                   \
			::std::fprintf(stderr, "FAIL %s line %d\n", #cond, __LINE__); \
		}                                                                 \
	} while (0)

/* minimal lazy task for the coroutine forms */
struct test_task
{
	struct promise_type
	{
		::std::cxx_std_error error{};
		::std::coroutine_handle<> continuation{};
		test_task get_return_object() noexcept
		{
			return {::std::coroutine_handle<promise_type>::from_promise(*this)};
		}
		static constexpr ::std::suspend_always initial_suspend() noexcept
		{
			return {};
		}
		struct final_awaiter
		{
			static constexpr bool await_ready() noexcept
			{
				return false;
			}
			static ::std::coroutine_handle<> await_suspend(::std::coroutine_handle<promise_type> h) noexcept
			{
				auto c{h.promise().continuation};
				return c ? c : ::std::noop_coroutine();
			}
			static constexpr void await_resume() noexcept
			{}
		};
		static constexpr final_awaiter final_suspend() noexcept
		{
			return {};
		}
		void unhandled_herbception(::std::cxx_std_error e) noexcept
		{
			error = e;
		}
		static constexpr void return_void() noexcept
		{}
	};
	::std::coroutine_handle<promise_type> handle{};
};

namespace fad = ::fast_io::operations;

static test_task coro_main(::fast_io::linux_io_uring_observer sched, int fd_in, int fd_out) throws
{
	::std::byte buf[128]{};
	/* positioned read */
	auto n{co_await fad::async_pread_some_bytes(sched, ::fast_io::posix_io_observer{fd_in}, buf,
												sizeof(buf), ::fast_io::intfpos_opt{0}, {})};
	CHECK(n == 128);
	CHECK(buf[0] == ::std::byte{'A'});
	/* positioned write_all through the coroutine form */
	co_await fad::async_pwrite_all_bytes(sched, ::fast_io::posix_io_observer{fd_out}, buf, 64,
										 ::fast_io::intfpos_opt{0}, {});
}

int main()
{
	namespace fi = fast_io;
	fi::linux_io_uring ring{fi::native_interface, 32, 0};
	fi::linux_io_uring_observer sched{ring.native_handle()};

	::std::byte filedata[128];
	for (::std::size_t i{}; i != sizeof(filedata); ++i)
	{
		filedata[i] = static_cast<::std::byte>('A' + (i & 15));
	}
	fi::posix_file fin{"/tmp/uring_in.bin",
					   fi::open_mode::out | fi::open_mode::in | fi::open_mode::creat | fi::open_mode::trunc};
	fi::posix_file fout{"/tmp/uring_out.bin",
						fi::open_mode::out | fi::open_mode::in | fi::open_mode::creat | fi::open_mode::trunc};
	fi::operations::write_all_bytes(fi::posix_io_observer{fin.native_handle()}, filedata, sizeof(filedata));

	/* coroutine forms */
	{
		auto t{coro_main(sched, fin.native_handle(), fout.native_handle())};
		auto h{t.handle};
		h.resume();
		while (!h.done())
		{
			fi::liburing::io_async_wait(sched);
		}
		CHECK(h.promise().error.domain == nullptr);
		char check[64]{};
		auto got{::pread(fout.native_handle(), check, 64, 0)};
		CHECK(got == 64);
		CHECK(::std::memcmp(check, filedata, 64) == 0);
	}

	/* callback form: read_all at current position */
	{
		CHECK(::lseek(fin.native_handle(), 0, SEEK_SET) == 0);
		bool fired{};
		::std::cxx_std_error err{};
		::std::byte buf[128]{};
		fad::async_pread_all_bytes_callback(
			sched, fi::posix_io_observer{fin.native_handle()}, buf, sizeof(buf), {}, {},
			[&](::std::cxx_std_error e) noexcept {
				fired = true;
				err = e;
			});
		while (!fired)
		{
			fi::liburing::io_async_wait(sched);
		}
		CHECK(err.domain == nullptr);
		CHECK(::std::memcmp(buf, filedata, 128) == 0);
	}

	/* native scatter writev/readv */
	{
		::std::byte a[16], b[16];
		for (::std::size_t i{}; i != 16; ++i)
		{
			a[i] = ::std::byte{0x11};
			b[i] = ::std::byte{0x22};
		}
		fi::io_scatter_t wv[2]{{a, 16}, {b, 16}};
		bool fired{};
		fad::async_scatter_pwrite_all_bytes_callback(
			sched, fi::posix_io_observer{fout.native_handle()}, wv, 2, ::fast_io::intfpos_opt{64}, {},
			[&](::std::cxx_std_error e) noexcept {
				fired = true;
				CHECK(e.domain == nullptr);
			});
		while (!fired)
		{
			fi::liburing::io_async_wait(sched);
		}
		char vbuf[32]{};
		auto got{::pread(fout.native_handle(), vbuf, 32, 64)};
		CHECK(got == 32 && vbuf[0] == 0x11 && vbuf[16] == 0x22);

		fi::io_scatter_t rv[2]{{a, 16}, {b, 16}};
		::std::memset(a, 0, 16);
		::std::memset(b, 0, 16);
		fired = false;
		fi::io_scatter_status_t st{};
		fad::async_scatter_pread_some_bytes_callback(
			sched, fi::posix_io_observer{fout.native_handle()}, rv, 2, ::fast_io::intfpos_opt{64}, {},
			[&](::std::cxx_std_error e, fi::io_scatter_status_t s) noexcept {
				fired = true;
				st = s;
				CHECK(e.domain == nullptr);
			});
		while (!fired)
		{
			fi::liburing::io_async_wait(sched);
		}
		CHECK(st.position == 2 && st.position_in_scatter == 0);
		CHECK(a[0] == ::std::byte{0x11} && b[0] == ::std::byte{0x22});
	}

	/* transmit regular->regular: splice cannot serve it -> bounce fallback */
	{
		bool fired{};
		fad::async_transmit_some_bytes_callback(
			sched, fi::posix_io_observer{fout.native_handle()}, ::fast_io::intfpos_opt{200},
			fi::posix_io_observer{fin.native_handle()}, ::fast_io::intfpos_opt{0},
			fi::size_t_opt{64}, {},
			[&](::std::cxx_std_error e, ::std::size_t n) noexcept {
				fired = true;
				CHECK(e.domain == nullptr);
				CHECK(n == 64);
			});
		while (!fired)
		{
			fi::liburing::io_async_wait(sched);
		}
		char tbuf[64]{};
		auto got{::pread(fout.native_handle(), tbuf, 64, 200)};
		CHECK(got == 64);
		CHECK(::std::memcmp(tbuf, filedata, 64) == 0);
	}

	/* transmit pipe->file: real SPLICE fast path */
	{
		int pfds[2];
		CHECK(::pipe(pfds) == 0);
		char const pipemsg[] = "splice-pipe-payload-0123456789abcdef";
		CHECK(::write(pfds[1], pipemsg, sizeof(pipemsg) - 1) == static_cast<long>(sizeof(pipemsg) - 1));
		bool fired{};
		::std::size_t moved{};
		fad::async_transmit_some_bytes_callback(
			sched, fi::posix_io_observer{fout.native_handle()}, ::fast_io::intfpos_opt{400},
			fi::posix_io_observer{pfds[0]}, {},
			fi::size_t_opt{sizeof(pipemsg) - 1}, {},
			[&](::std::cxx_std_error e, ::std::size_t n) noexcept {
				fired = true;
				moved = n;
				CHECK(e.domain == nullptr);
			});
		while (!fired)
		{
			fi::liburing::io_async_wait(sched);
		}
		CHECK(moved == sizeof(pipemsg) - 1);
		char pbuf[sizeof(pipemsg)]{};
		auto got{::pread(fout.native_handle(), pbuf, sizeof(pipemsg) - 1, 400)};
		CHECK(got == static_cast<long>(sizeof(pipemsg) - 1));
		CHECK(::std::memcmp(pbuf, pipemsg, sizeof(pipemsg) - 1) == 0);
		::close(pfds[0]);
		::close(pfds[1]);
	}

	/* operation timeout: a pipe read that can never complete must come
	 * back errc::timed_out once the armed LINK_TIMEOUT expires */
	{
		int pfds[2];
		CHECK(::pipe(pfds) == 0);
		::std::byte buf[8]{};
		bool fired{};
		::std::cxx_std_error err{};
		fad::async_pread_some_bytes_callback(
			sched, fi::posix_io_observer{pfds[0]}, buf, sizeof(buf), {},
			fi::posix_statx_timestamp_opt{fi::posix_statx_timestamp64{0, 50000000}},
			[&](::std::cxx_std_error e, ::std::size_t) noexcept {
				fired = true;
				err = e;
			});
		while (!fired)
		{
			fi::liburing::io_async_wait(sched);
		}
		CHECK(fired);
		CHECK(err.domain == ::std::error_domain<::std::errc>::domain());
		CHECK(err.code == static_cast<::std::size_t>(::std::errc::timed_out));
		::close(pfds[0]);
		::close(pfds[1]);
	}

	/* buffered input over a real posix fd: window underflow, inline
	 * drain, explicit-offset bypass */
	{
		CHECK(::lseek(fin.native_handle(), 0, SEEK_SET) == 0);
		::fast_io::basic_io_buffer<fi::posix_io_observer,
								   ::fast_io::basic_io_buffer_traits<::fast_io::buffer_mode::in,
																	 ::fast_io::native_global_allocator,
																	 char, void, 64>>
			ibf{fi::posix_io_observer{fin.native_handle()}};
		::std::byte buf[8]{};
		bool fired{};
		fad::async_pread_some_bytes_callback(
			sched, ibf, buf, sizeof(buf), {}, {},
			[&](::std::cxx_std_error e, ::std::size_t n) noexcept {
				fired = true;
				CHECK(e.domain == nullptr);
				CHECK(n == 8);
			});
		while (!fired)
		{
			fi::liburing::io_async_wait(sched);
		}
		CHECK(::std::memcmp(buf, filedata, 8) == 0);
		CHECK(::lseek(fin.native_handle(), 0, SEEK_CUR) == 64);
	}

	/* bad fd: EBADF rides the callback, not an exception */
	{
		bool fired{};
		::std::cxx_std_error err{};
		::std::byte buf[8]{};
		fad::async_pread_some_bytes_callback(
			sched, fi::posix_io_observer{-1}, buf, sizeof(buf), {}, {},
			[&](::std::cxx_std_error e, ::std::size_t) noexcept {
				fired = true;
				err = e;
			});
		while (!fired)
		{
			fi::liburing::io_async_wait(sched);
		}
		CHECK(err.code == static_cast<::std::size_t>(EBADF));
	}

	::std::fprintf(stderr, "io_uring test: %s (failures=%d)\n", failures == 0 ? "all ok" : "FAILED",
				   failures);
	return failures == 0 ? 0 : 1;
}
