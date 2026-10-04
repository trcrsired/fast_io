#pragma once

#include "../fast_io_dsal/impl/misc/push_macros.h"
#include "lc.h"
#include "lc_numbers/impl.h"

// lc_print_status — the locale-aware print concepts, keyed on lc_ctx
// (the all-equivalent: file pointer + section image + hot fields).
// Master's hooks took basic_lc_all<char_type> const*; these take
// lc_ctx<char_type> const* and the wire fields resolve through
// ctx->sc/ctx->pt.

namespace fast_io
{

template <typename char_type, typename T>
concept lc_scatter_printable = ::std::integral<char_type> &&
	requires(lc_ctx<char_type> const *ctx, T t) {
		{
			print_scatter_define(ctx, t)
		} -> ::std::same_as<::fast_io::basic_io_scatter_t<char_type>>;
	};

template <typename char_type, typename T>
concept lc_dynamic_reserve_printable = ::std::integral<char_type> &&
	requires(lc_ctx<char_type> const *ctx, T t, char_type *ptr, ::std::size_t size) {
		{ print_reserve_size(ctx, t) } -> ::std::convertible_to<::std::size_t>;
		{ print_reserve_define(ctx, ptr, t) } -> ::std::convertible_to<char_type *>;
	};

template <typename char_type, typename T>
concept lc_printable = ::std::integral<char_type> &&
	requires(lc_ctx<char_type> const *ctx,
			 ::fast_io::details::dummy_buffer_output_stream<char_type> out, T t) {
		print_define(ctx, out, t);
	};

// any of the three lc-hook shapes exists for T in char_type
template <typename char_type, typename T>
concept lc_any_printable = lc_scatter_printable<char_type, T> ||
	lc_dynamic_reserve_printable<char_type, T> ||
	lc_printable<char_type, T>;

// write one lc arg to the stream — scatter straight out of the image
// when the hook gives one, the obuffer reserve fast path for
// dynamic-reserve hooks, print_define otherwise
namespace details
{

template <typename output, typename T>
	requires lc_any_printable<typename output::output_char_type, T>
inline constexpr void lc_write_lc(lc_ctx<typename output::output_char_type> const &ctx,
								  output out, T &&t) FAST_IO_HERBCEPTIONS_THROWS
{
	using char_type = typename output::output_char_type;
	using value_type = ::std::remove_cvref_t<T>;
	if constexpr (lc_scatter_printable<char_type, value_type>)
	{
		auto const s{print_scatter_define(__builtin_addressof(ctx), t)};
		if (s.base != nullptr && s.len != 0)
		{
			::fast_io::operations::print_freestanding<false>(out, s);
		}
	}
	else if constexpr (lc_dynamic_reserve_printable<char_type, value_type>)
	{
		auto const *cp{__builtin_addressof(ctx)};
		::std::size_t const need{print_reserve_size(cp, t)};
		if constexpr (::fast_io::operations::decay::defines::
						  has_obuffer_basic_operations<output>)
		{
			auto const curr{obuffer_curr(out)};
			auto const ed{obuffer_end(out)};
			auto const diff{ed - curr};
			if (0 <= diff && need <= static_cast<::std::size_t>(diff)) [[likely]]
			{
				obuffer_set_curr(out, print_reserve_define(cp, curr, t));
				return;
			}
			else if constexpr (::fast_io::operations::decay::defines::
								   has_obuffer_flush_reserve_define<output>)
			{
				obuffer_flush_reserve_define(out, need);
				auto const ncurr{obuffer_curr(out)};
				obuffer_set_curr(out, print_reserve_define(cp, ncurr, t));
				return;
			}
		}
		// non-obuffer output — write into a capped stack buffer, then
		// scatter. Anything larger truncates through the reserve
		// hooks' own bounded writes
		constexpr ::std::size_t cap{512};
		char_type buf[cap];
		auto const it{print_reserve_define(cp, buf, t)};
		::fast_io::operations::print_freestanding<false>(
			out,
			::fast_io::basic_io_scatter_t<char_type>{
				buf, static_cast<::std::size_t>(it - buf)});
	}
	else
	{
		print_define(__builtin_addressof(ctx), out, t);
	}
}

} // namespace details

} // namespace fast_io

#include "../fast_io_dsal/impl/misc/pop_macros.h"
