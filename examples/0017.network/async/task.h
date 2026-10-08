#pragma once

#include <fast_io.h>
#include <coroutine>

/*
 * Minimal lazy task for the async examples: a fire-and-forget coroutine
 * whose errors arrive through the herbception channel as an owning
 * ::std::cxx_std_error in the promise. The caller resumes the handle and
 * the ring pump drives it to completion.
 */
struct async_task
{
	struct promise_type
	{
		::std::cxx_std_error error{};
		::std::coroutine_handle<> continuation{};

		inline constexpr async_task get_return_object() noexcept
		{
			return {::std::coroutine_handle<promise_type>::from_promise(*this)};
		}
		static inline constexpr ::std::suspend_always initial_suspend() noexcept
		{
			return {};
		}
		struct final_awaiter
		{
			static inline constexpr bool await_ready() noexcept
			{
				return false;
			}
			static inline ::std::coroutine_handle<> await_suspend(::std::coroutine_handle<promise_type> h) noexcept
			{
				auto c{h.promise().continuation};
				return c ? c : ::std::noop_coroutine();
			}
			static inline constexpr void await_resume() noexcept
			{}
		};
		static inline constexpr final_awaiter final_suspend() noexcept
		{
			return {};
		}
		inline void unhandled_herbception(::std::cxx_std_error e) noexcept
		{
			error = e;
		}
		static inline constexpr void return_void() noexcept
		{}
		/* a retained error is an owning handle — release its payload if
		 * nobody rethrew it before the frame went away */
		inline ~promise_type()
		{
			::fast_io::details::async_dispose_error(error);
		}
	};
	::std::coroutine_handle<promise_type> handle{};
};
