#pragma once

#include "../fast_io_dsal/impl/misc/push_macros.h"
#include "lcblob.h"
#include "lc.h"
#include "lc_print_status.h"

// locale-aware printing. imbue(loc, out) wraps an output stream so that
// locale-printable arguments format through the mapped locale:
//   ::fast_io::println(::fast_io::imbue(loc, obf), 1242141242124);
//   ::fast_io::println(::fast_io::imbue(loc, obf), ::fast_io::mnp::d_t_fmt(ts));
//   ::fast_io::println(::fast_io::imbue(loc, obf), ::fast_io::mnp::boolalpha(b));
//
// The locale handle is a ::fast_io::i18n::lcblob::lc_locale const* — an
// lc_ctx built from it at imbue time selects the section for the
// stream's char_type: char follows the file's declared codeset
// (UTF-8/GB18030/UTF-EBCDIC), char8_t/char16_t/char32_t select their
// own always-present sections.

namespace fast_io
{

template <typename stm>
struct lc_imbuer
{
	using handle_type = stm;
	using char_type = typename handle_type::char_type;
	using output_char_type = char_type;
	lc_ctx<char_type> ctx{};
#ifndef __INTELLISENSE__
#if __has_cpp_attribute(msvc::no_unique_address)
	[[msvc::no_unique_address]]
#elif __has_cpp_attribute(no_unique_address) >= 201803
	[[no_unique_address]]
#endif
#endif
	handle_type handle{};
};

template <typename stm>
inline constexpr lc_imbuer<stm> output_stream_ref_define(lc_imbuer<stm> t) noexcept
{
	return t;
}

template <typename stm>
	requires(::std::is_lvalue_reference_v<stm> || ::std::is_trivially_copyable_v<stm>)
inline constexpr auto imbue(::fast_io::i18n::lcblob::lc_locale const *loc,
							stm &&out) noexcept
{
	using reftype = decltype(::fast_io::operations::output_stream_ref(out));
	using char_type = typename reftype::char_type;
	return lc_imbuer<reftype>{::fast_io::lc_load_ctx<char_type>(loc),
							  ::fast_io::operations::output_stream_ref(out)};
}

namespace i18n::details
{

// index of the first arg with an lc hook; sizeof...(Args) when none —
// same template-for consteval pattern as
// details::first_print_define_index_range in the plain path
template <typename char_type, typename... Args>
inline consteval ::std::size_t lc_first_index() noexcept
{
	template for (constexpr auto i : ::fast_io::details::index_array_range<0, sizeof...(Args)>)
	{
		if constexpr (::fast_io::lc_any_printable<char_type, ::std::remove_cvref_t<Args...[i]>>)
		{
			return i;
		}
	}
	return sizeof...(Args);
}

template <typename char_type, typename... Args>
inline constexpr bool lc_any_arg_v{lc_first_index<char_type, Args...>() != sizeof...(Args)};

// args are split at every lc arg: contiguous non-lc segments go through
// print_freestanding_decay (the plain path keeps its reserve / scatter
// batching inside each segment), lc args write through their hooks on
// ctx. The terminal call delegates to the plain path — it also emits
// '\n'.
template <bool line, typename output, typename... Args>
inline constexpr void lc_status_print_impl(lc_ctx<typename output::output_char_type> const &ctx,
										   output optstm, Args... args) FAST_IO_HERBCEPTIONS_THROWS
{
	using char_type = typename output::output_char_type;
	constexpr ::std::size_t fpos{lc_first_index<char_type, Args...>()};
	if constexpr (fpos == sizeof...(Args))
	{
		return ::fast_io::operations::decay::print_freestanding_decay<line>(optstm, args...);
	}
	else
	{
		if constexpr (fpos != 0)
		{
			[&]<::std::size_t... pos>(::std::index_sequence<pos...>) FAST_IO_HERBCEPTIONS_THROWS {
				::fast_io::operations::decay::print_freestanding_decay<false>(optstm, args...[pos]...);
			}(::std::make_index_sequence<fpos>{});
		}
		::fast_io::details::lc_write_lc(ctx, optstm, args...[fpos]);
		return [&]<::std::size_t... pos>(::std::index_sequence<pos...>) FAST_IO_HERBCEPTIONS_THROWS {
			return lc_status_print_impl<line>(ctx, optstm, args...[fpos + 1 + pos]...);
		}(::std::make_index_sequence<sizeof...(Args) - fpos - 1>{});
	}
}

} // namespace i18n::details

template <bool line, typename output, typename... Args>
inline constexpr void status_print_define(::fast_io::lc_imbuer<output> imb, Args... args)
	FAST_IO_HERBCEPTIONS_THROWS
{
	if constexpr (::fast_io::i18n::details::lc_any_arg_v<
					  typename lc_imbuer<output>::char_type, Args...>)
	{
		return ::fast_io::i18n::details::lc_status_print_impl<line>(imb.ctx, imb.handle,
																  args...);
	}
	else
	{
		return ::fast_io::operations::decay::print_freestanding_decay<line>(imb.handle, args...);
	}
}

} // namespace fast_io

#include "../fast_io_dsal/impl/misc/pop_macros.h"
