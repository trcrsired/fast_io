#pragma once

#include "../fast_io_dsal/impl/misc/push_macros.h"
#include "lcblob.h"

// lc — the runtime locale context every lc_* print hook is keyed on.
// Master's hooks take `basic_lc_all<char_type> const*`; here the key is
// lc_ctx: the lc_locale file pointer (needed to resolve the wire
// format's RVAs) plus the selected section image plus the hot
// LC_NUMERIC fields decoded once per imbue.
//
// Field access in ported code is mechanical:
//   master:  all->numeric.thousands_sep          (basic_io_scatter_t)
//   here:    ctx->sc(ctx->all->numeric.thousands_sep)
//            ^ sc resolves the wire lc_scatter to a real scatter
//   master:  all->time.era.base                  (pointer)
//   here:    ctx->pt(ctx->all->time.era.ref)     (lc_rva -> pointer)

namespace fast_io
{

inline constexpr ::std::size_t lc_sep_capacity{16};

template <::std::integral char_type>
struct lc_ctx
{
	::fast_io::i18n::lcblob::lc_locale const *loc{};
	// the section image — wire-layout identical for every char_type,
	// so it is read through the char instantiation
	::fast_io::i18n::lcblob::basic_lc_all<char> const *all{};
	// hot LC_NUMERIC fields decoded once per imbue: thousands_sep as
	// char_type units and grouping as u8 group sizes (both capped — a
	// longer field disables grouping rather than allocating)
	char_type sep[lc_sep_capacity]{};
	::std::size_t sep_len{};
	char8_t grouping[lc_sep_capacity]{};
	::std::size_t grouping_len{};

	// resolve a wire scatter member to a real scatter of char_type
	// units — the member's declared T is the charset tag (char for
	// every string field); the actual units are this section's
	// char_type. rva==0 gives {nullptr,0}
	template <typename T>
	inline constexpr ::fast_io::basic_io_scatter_t<char_type> sc(
		::fast_io::i18n::lcblob::lc_scatter<T> s) const noexcept
	{
		auto const r{::fast_io::i18n::lcblob::lc_get_scatter(loc, s)};
		return {reinterpret_cast<char_type const *>(r.base), r.len};
	}
	// resolve a wire rva member to a pointer; rva==0 gives nullptr
	template <typename T>
	inline constexpr T const *pt(::fast_io::i18n::lcblob::lc_rva<T> r) const noexcept
	{
		return ::fast_io::i18n::lcblob::lc_get_rva(loc, r);
	}
	// a strref-table element of a dynamic string list (alt_digits and
	// friends): member is {tbl_rva,count}, tbl = lc_scatter[count]
	template <typename T>
	inline constexpr ::fast_io::basic_io_scatter_t<char_type> sc_elem(
		::fast_io::i18n::lcblob::lc_scatter<::fast_io::i18n::lcblob::lc_scatter<T>> s,
		::std::size_t i) const noexcept
	{
		auto const *tbl{pt(s.ref)};
		if (tbl == nullptr || i >= ::fast_io::i18n::lcblob::lc_u32(s.len))
		{
			return {};
		}
		auto const e{::fast_io::i18n::lcblob::lc_get_scatter(loc, tbl[i])};
		return {reinterpret_cast<char_type const *>(e.base), e.len};
	}
};

template <::std::integral char_type>
inline lc_ctx<char_type>
lc_load_ctx(::fast_io::i18n::lcblob::lc_locale const *v) noexcept
{
	namespace lc = ::fast_io::i18n::lcblob;
	lc_ctx<char_type> ctx{};
	auto const *all{lc::lc_get_all<char_type>(v)};
	if (all == nullptr)
	{
		return ctx;
	}
	ctx.loc = v;
	ctx.all = reinterpret_cast<lc::basic_lc_all<char> const *>(all);
	// payload units already are char_type — no codecvt anywhere
	if (auto const sep{lc::lc_get_scatter(v, all->numeric.thousands_sep)};
		sep.base != nullptr && sep.len != 0 && sep.len <= lc_sep_capacity)
	{
		::fast_io::details::my_memcpy(ctx.sep, sep.base, sep.len * sizeof(char_type));
		ctx.sep_len = sep.len;
	}
	if (auto const g{lc::lc_get_scatter(v, all->numeric.grouping)};
		g.base != nullptr && g.len != 0 && g.len <= lc_sep_capacity)
	{
		::fast_io::details::my_memcpy(ctx.grouping, g.base, g.len);
		ctx.grouping_len = g.len;
	}
	return ctx;
}

} // namespace fast_io

#include "../fast_io_dsal/impl/misc/pop_macros.h"
