#pragma once

#include "../fast_io_dsal/impl/misc/push_macros.h"
#include "../fast_io_dsal/string_view.h"

// lcblob consumer — read side of the binary locale container emitted by
// fast_io_tools/binfmt (see binfmt/spec.md there for the wire format).
//
// The container is flat, position-independent and ARCHITECTURE-NEUTRAL:
// every reference is an RVA offset, every numeric field is LEB128, and the
// fixed-width fields (u32 magic, u32 RVA table entries) are stored
// little-endian. One .bin serves 32/64-bit and all platforms alike.
//
// One file carries ALL charsets — no transcoding ever happens at runtime.
//
// Layout: [outer header][section dir][utf8 section][utf16][utf32]
//
//   outer header: u32 magic 'FCL1' | uleb version | uleb total_size | uleb flags
//                 | strref name | strref encoding   (utf8; 'encoding' is the
//                 |                               locale's declared codeset
//                 |                               for char-typed text)
//                 | (uleb rva | uleb size) * 3      (0 rva = section absent)
//   section:      a complete blob of the charset — same body layout:
//                 u32 magic | uleb version | uleb total_size | uleb flags
//                 | strref name | strref encoding | uleb cat_dir_rva
//                 | [cat_dir u32*12][pool][slot tables][records]
//                 all RVAs inside a section are relative to the section base.

namespace fast_io::i18n
{

namespace lcblob
{

inline constexpr ::std::uint_least32_t magic{0x314C4346}; // 'FCL1' LE
inline constexpr ::std::uint_least64_t blob_version{1};

// categories — fixed ids
enum lc_cat : ::std::uint_least32_t
{
	lc_identification = 0,
	lc_ctype = 1,
	lc_collate = 2,
	lc_time = 3,
	lc_numeric = 4,
	lc_monetary = 5,
	lc_messages = 6,
	lc_paper = 7,
	lc_name = 8,
	lc_address = 9,
	lc_telephone = 10,
	lc_measurement = 11,
	lc_cat_count = 12,
};

enum class slot_tag : ::std::uint_least8_t
{
	absent = 0,
	string = 1,
	strlist = 2,
	integer = 3,
	bytes = 4,
	program = 5,
	int3 = 6,
	eralist = 7,
};

struct field_def
{
	char const *name;
	slot_tag tag;
};

// slot schemas — field order is fixed by the emitter (fast_io_tools)
inline constexpr field_def identification_fields[]{
	{"title", slot_tag::string},	{"source", slot_tag::string},
	{"address", slot_tag::string},	{"contact", slot_tag::string},
	{"email", slot_tag::string},	{"tel", slot_tag::string},
	{"fax", slot_tag::string},	{"language", slot_tag::string},
	{"territory", slot_tag::string}, {"audience", slot_tag::string},
	{"application", slot_tag::string},
	{"abbreviation", slot_tag::string}, {"revision", slot_tag::string},
	{"date", slot_tag::string},
};

inline constexpr field_def ctype_fields[]{
	{"codeset", slot_tag::string},
};

inline constexpr field_def collate_fields[]{
	{"collation", slot_tag::integer},
};

inline constexpr field_def time_fields[]{
	{"abday", slot_tag::strlist},	{"day", slot_tag::strlist},
	{"abmon", slot_tag::strlist},	{"ab_alt_mon", slot_tag::strlist},
	{"mon", slot_tag::strlist},	{"alt_mon", slot_tag::strlist},
	{"d_t_fmt", slot_tag::program}, {"d_fmt", slot_tag::program},
	{"t_fmt", slot_tag::program},	{"t_fmt_ampm", slot_tag::program},
	{"date_fmt", slot_tag::program}, {"am_pm", slot_tag::strlist},
	{"era", slot_tag::eralist},	{"era_d_fmt", slot_tag::program},
	{"era_d_t_fmt", slot_tag::program}, {"era_t_fmt", slot_tag::program},
	{"alt_digits", slot_tag::strlist}, {"week", slot_tag::int3},
	{"first_weekday", slot_tag::integer}, {"first_workday", slot_tag::integer},
	{"cal_direction", slot_tag::integer}, {"timezone", slot_tag::strlist},
};

inline constexpr field_def numeric_fields[]{
	{"decimal_point", slot_tag::string},
	{"thousands_sep", slot_tag::string},
	{"grouping", slot_tag::bytes},
};

// LC_NUMERIC field indices — fixed positions in the schema above:
// 0 decimal_point, 1 thousands_sep, 2 grouping
inline constexpr field_def monetary_fields[]{
	{"int_curr_symbol", slot_tag::string}, {"currency_symbol", slot_tag::string},
	{"mon_decimal_point", slot_tag::string}, {"mon_thousands_sep", slot_tag::string},
	{"mon_grouping", slot_tag::bytes},	   {"positive_sign", slot_tag::string},
	{"negative_sign", slot_tag::string},   {"int_frac_digits", slot_tag::integer},
	{"frac_digits", slot_tag::integer},	   {"p_cs_precedes", slot_tag::integer},
	{"p_sep_by_space", slot_tag::integer}, {"n_cs_precedes", slot_tag::integer},
	{"n_sep_by_space", slot_tag::integer}, {"int_p_cs_precedes", slot_tag::integer},
	{"int_p_sep_by_space", slot_tag::integer}, {"int_n_cs_precedes", slot_tag::integer},
	{"int_n_sep_by_space", slot_tag::integer}, {"p_sign_posn", slot_tag::integer},
	{"n_sign_posn", slot_tag::integer},    {"int_p_sign_posn", slot_tag::integer},
	{"int_n_sign_posn", slot_tag::integer},
};

inline constexpr field_def messages_fields[]{
	{"yesexpr", slot_tag::string}, {"noexpr", slot_tag::string},
	{"yesstr", slot_tag::string},  {"nostr", slot_tag::string},
};

inline constexpr field_def paper_fields[]{
	{"height", slot_tag::integer}, {"width", slot_tag::integer},
};

inline constexpr field_def name_fields[]{
	{"name_fmt", slot_tag::program}, {"name_gen", slot_tag::string},
	{"name_miss", slot_tag::string}, {"name_mr", slot_tag::string},
	{"name_mrs", slot_tag::string},	 {"name_ms", slot_tag::string},
};

inline constexpr field_def address_fields[]{
	{"postal_fmt", slot_tag::program}, {"country_name", slot_tag::string},
	{"country_post", slot_tag::string}, {"country_ab2", slot_tag::string},
	{"country_ab3", slot_tag::string},  {"country_num", slot_tag::integer},
	{"country_car", slot_tag::string},  {"country_isbn", slot_tag::string},
	{"lang_name", slot_tag::string},    {"lang_ab", slot_tag::string},
	{"lang_term", slot_tag::string},    {"lang_lib", slot_tag::string},
};

inline constexpr field_def telephone_fields[]{
	{"tel_int_fmt", slot_tag::program}, {"tel_dom_fmt", slot_tag::program},
	{"int_select", slot_tag::string},   {"int_prefix", slot_tag::string},
};

inline constexpr field_def measurement_fields[]{
	{"measurement", slot_tag::integer},
};

struct cat_schema
{
	::std::span<field_def const> fields;
	char const *glibc_name;
};

inline constexpr cat_schema cat_schemas[lc_cat_count]{
	{identification_fields, "LC_IDENTIFICATION"}, {ctype_fields, "LC_CTYPE"},
	{collate_fields, "LC_COLLATE"},		  {time_fields, "LC_TIME"},
	{numeric_fields, "LC_NUMERIC"},		  {monetary_fields, "LC_MONETARY"},
	{messages_fields, "LC_MESSAGES"},	  {paper_fields, "LC_PAPER"},
	{name_fields, "LC_NAME"},		  {address_fields, "LC_ADDRESS"},
	{telephone_fields, "LC_TELEPHONE"},	  {measurement_fields, "LC_MEASUREMENT"},
};

// section selector. Slot charset is the locale's declared codeset view
// for char (utf8 / gb18030 / utf_ebcdic — see header.encoding); it is
// always present and aliases the utf8 section when the codeset IS utf8.
enum class blob_charset : ::std::uint_least8_t
{
	charset = 0,
	utf8 = 1,
	utf16 = 2, // LE
	utf32 = 3, // LE
};

inline constexpr ::std::size_t blob_charset_count{4};

// the codesets a locale file can declare for its char view — the only
// three supported
enum class locale_charset : ::std::uint_least8_t
{
	utf8 = 0,
	gb18030 = 1,
	utf_ebcdic = 2,
};

inline constexpr char8_t const *locale_charset_name(locale_charset cs) noexcept
{
	switch (cs)
	{
	case locale_charset::gb18030:
		return u8"GB18030";
	case locale_charset::utf_ebcdic:
		return u8"UTF-EBCDIC";
	default:
		return u8"UTF-8";
	}
}

// fixed-width file fields are little-endian — swap on big-endian hosts
inline ::std::uint_least32_t read_u32(char8_t const *p) noexcept
{
	::std::uint_least32_t v{};
	::fast_io::details::my_memcpy(__builtin_addressof(v), p, 4);
	if constexpr (::std::endian::native == ::std::endian::big)
	{
		v = ::std::byteswap(v);
	}
	return v;
}

template <::std::integral T>
inline char8_t const *read_leb(char8_t const *p, char8_t const *e, T &v) FAST_IO_HERBCEPTIONS_THROWS
{
	auto [it, ec]{::fast_io::parse_by_scan(p, e, ::fast_io::manipulators::leb128_get(v))};
	if (ec != ::fast_io::freestanding::parse_errc::ok)
	{
		::fast_io::herbceptions::throws_parse_errc(ec);
	}
	return it;
}

// section sub-header — what a section's own blob header contributes
struct section_header
{
	char8_t const *cat_dir{}; // -> u32[lc_cat_count] within the section
};

struct blob_header
{
	::std::uint_least64_t version{};
	::std::uint_least64_t total_size{};
	::std::uint_least64_t flags{};
	::fast_io::u8string_view name{};
	::fast_io::u8string_view encoding{};
	::std::uint_least64_t sec_rva[blob_charset_count]{};
	::std::uint_least64_t sec_size[blob_charset_count]{};
};

// one embedded section header — validates and returns its cat_dir
inline section_header read_section_header(char8_t const *sec,
										  ::std::uint_least64_t sec_size) FAST_IO_HERBCEPTIONS_THROWS
{
	if (sec_size < 4 || read_u32(sec) != magic)
	{
		::fast_io::throw_posix_error(EINVAL);
	}
	char8_t const *p{sec + 4};
	char8_t const *e{sec + sec_size};
	::std::uint_least64_t v{};
	p = read_leb(p, e, v); // version
	if (v > blob_version)
	{
		::fast_io::throw_posix_error(EINVAL);
	}
	p = read_leb(p, e, v); // total_size
	if (v > sec_size)
	{
		::fast_io::throw_posix_error(EINVAL);
	}
	p = read_leb(p, e, v); // flags
	::std::uint_least64_t rva{}, len{};
	p = read_leb(p, e, rva); // name
	p = read_leb(p, e, len);
	if (rva > sec_size || len > sec_size - rva)
	{
		::fast_io::throw_posix_error(EINVAL);
	}
	p = read_leb(p, e, rva); // encoding
	p = read_leb(p, e, len);
	if (rva > sec_size || len > sec_size - rva)
	{
		::fast_io::throw_posix_error(EINVAL);
	}
	::std::uint_least64_t cat_dir_rva{};
	p = read_leb(p, e, cat_dir_rva);
	if (cat_dir_rva > sec_size || lc_cat_count * 4 > sec_size - cat_dir_rva)
	{
		::fast_io::throw_posix_error(EINVAL);
	}
	return {sec + cat_dir_rva};
}

inline blob_header read_header(::fast_io::u8string_view blob) FAST_IO_HERBCEPTIONS_THROWS
{
	if (blob.size() < 4)
	{
		::fast_io::throw_posix_error(EINVAL);
	}
	if (read_u32(blob.data()) != magic)
	{
		::fast_io::throw_posix_error(EINVAL);
	}
	char8_t const *p{blob.data() + 4};
	char8_t const *e{blob.data() + blob.size()};
	blob_header h;
	p = read_leb(p, e, h.version);
	if (h.version > blob_version)
	{
		::fast_io::throw_posix_error(EINVAL);
	}
	p = read_leb(p, e, h.total_size);
	p = read_leb(p, e, h.flags);
	::std::uint_least64_t rva{}, len{};
	p = read_leb(p, e, rva);
	p = read_leb(p, e, len);
	if (rva + len > blob.size())
	{
		::fast_io::throw_posix_error(EINVAL);
	}
	h.name = ::fast_io::u8string_view{blob.data() + rva, len};
	p = read_leb(p, e, rva);
	p = read_leb(p, e, len);
	if (rva + len > blob.size())
	{
		::fast_io::throw_posix_error(EINVAL);
	}
	h.encoding = ::fast_io::u8string_view{blob.data() + rva, len};
	for (::std::size_t c{}; c < blob_charset_count; ++c)
	{
		p = read_leb(p, e, rva);
		p = read_leb(p, e, len);
		if (rva != 0)
		{
			if (rva > blob.size() || len > blob.size() - rva || len < 4)
			{
				::fast_io::throw_posix_error(EINVAL);
			}
		}
		h.sec_rva[c] = rva;
		h.sec_size[c] = len;
	}
	return h;
}

} // namespace lcblob

// ---------------------------------------------------------------------------
// decoded slot — result of looking up one field in a locale.
//
// base..seg_end is the segment RVAs resolve against (the blob itself, or
// the per-user override segment). For strlist/eralist, ptr points at the
// first strref/era record (the count leb already consumed into count).
// ---------------------------------------------------------------------------

struct locale_slot
{
	char8_t const *base{};
	char8_t const *ptr{};
	char8_t const *seg_end{};
	::std::uint_least64_t len{};	// string/program/bytes payload size
	::std::uint_least64_t count{}; // strlist/eralist element count
	::std::int_least64_t ints[3]{}; // integer/int3 payloads
	lcblob::slot_tag tag{lcblob::slot_tag::absent};

	inline constexpr bool absent() const noexcept
	{
		return tag == lcblob::slot_tag::absent;
	}
	// string/program/bytes payload; for utf16/utf32 blobs these are
	// little-endian char units, not utf-8.
	inline constexpr ::fast_io::u8string_view bytes() const noexcept
	{
		return ::fast_io::u8string_view{ptr, len};
	}
};

namespace lcblob
{

// decode one slot record at rec within the segment [base, seg_end)
inline void read_slot(char8_t const *rec, char8_t const *seg_end, char8_t const *base,
					  ::fast_io::i18n::locale_slot &out) FAST_IO_HERBCEPTIONS_THROWS
{
	if (rec == nullptr || rec < base || rec >= seg_end)
	{
		::fast_io::throw_posix_error(EINVAL);
	}
	::std::uint_least64_t tag{};
	rec = read_leb(rec, seg_end, tag);
	out = {};
	out.base = base;
	out.seg_end = seg_end;
	out.tag = static_cast<slot_tag>(tag);
	::std::uint_least64_t const seg{static_cast<::std::uint_least64_t>(seg_end - base)};
	switch (out.tag)
	{
	case slot_tag::string:
	case slot_tag::bytes:
	case slot_tag::program:
	{
		::std::uint_least64_t rva{}, len{};
		rec = read_leb(rec, seg_end, rva);
		rec = read_leb(rec, seg_end, len);
		if (rva > seg || len > seg - rva)
		{
			::fast_io::throw_posix_error(EINVAL);
		}
		out.ptr = base + rva;
		out.len = len;
		break;
	}
	case slot_tag::strlist:
	case slot_tag::eralist:
	{
		::std::uint_least64_t rva{};
		rec = read_leb(rec, seg_end, rva);
		if (rva >= seg)
		{
			::fast_io::throw_posix_error(EINVAL);
		}
		char8_t const *q{base + rva};
		q = read_leb(q, seg_end, out.count);
		out.ptr = q;
		break;
	}
	case slot_tag::integer:
	{
		rec = read_leb(rec, seg_end, out.ints[0]);
		break;
	}
	case slot_tag::int3:
	{
		rec = read_leb(rec, seg_end, out.ints[0]);
		rec = read_leb(rec, seg_end, out.ints[1]);
		rec = read_leb(rec, seg_end, out.ints[2]);
		break;
	}
	default:
	{
		out.tag = slot_tag::absent;
		break;
	}
	}
}

// element i of a strlist slot (sequential strref walk; lists are small)
inline void strlist_elem(::fast_io::i18n::locale_slot const &s, ::std::uint_least64_t i,
						 ::fast_io::u8string_view &out) FAST_IO_HERBCEPTIONS_THROWS
{
	if (s.tag != slot_tag::strlist || i >= s.count)
	{
		::fast_io::throw_posix_error(EINVAL);
	}
	char8_t const *p{s.ptr};
	for (::std::uint_least64_t k{};; ++k)
	{
		::std::uint_least64_t rva{}, len{};
		p = read_leb(p, s.seg_end, rva);
		p = read_leb(p, s.seg_end, len);
		::std::uint_least64_t const seg{static_cast<::std::uint_least64_t>(s.seg_end - s.base)};
		if (rva > seg || len > seg - rva)
		{
			::fast_io::throw_posix_error(EINVAL);
		}
		if (k == i)
		{
			out = ::fast_io::u8string_view{s.base + rva, len};
			return;
		}
	}
}

// raw record bytes of era element i (era_rec decode lives with the
// consumer; the rec layout is in fast_io_tools/binfmt/spec.md)
inline void era_elem(::fast_io::i18n::locale_slot const &s, ::std::uint_least64_t i,
					 ::fast_io::u8string_view &out) FAST_IO_HERBCEPTIONS_THROWS
{
	if (s.tag != slot_tag::eralist || i >= s.count)
	{
		::fast_io::throw_posix_error(EINVAL);
	}
	char8_t const *p{s.ptr};
	for (::std::uint_least64_t k{};; ++k)
	{
		char8_t const *rec{p};
		::std::int_least64_t iv{};
		::std::uint_least64_t uv{}, uv2{};
		p = read_leb(p, s.seg_end, iv);	 // direction
		p = read_leb(p, s.seg_end, iv);	 // offset
		p = read_leb(p, s.seg_end, iv);	 // start_year
		p = read_leb(p, s.seg_end, uv);	 // start_month
		p = read_leb(p, s.seg_end, uv);	 // start_day
		p = read_leb(p, s.seg_end, iv);	 // end_year
		p = read_leb(p, s.seg_end, uv);	 // end_month
		p = read_leb(p, s.seg_end, uv);	 // end_day
		p = read_leb(p, s.seg_end, uv);	 // name rva
		p = read_leb(p, s.seg_end, uv2); // name len
		p = read_leb(p, s.seg_end, uv);	 // fmt rva
		p = read_leb(p, s.seg_end, uv2); // fmt len
		if (k == i)
		{
			out = ::fast_io::u8string_view{rec, static_cast<::std::size_t>(p - rec)};
			return;
		}
	}
}

// field index by schema name (linear; field counts are ~20 max)
inline ::std::ptrdiff_t field_index(::std::uint_least32_t cat,
									::fast_io::u8string_view fname) noexcept
{
	if (cat >= lc_cat_count)
	{
		return -1;
	}
	auto const &fs{cat_schemas[cat].fields};
	for (::std::size_t i{}; i < fs.size(); ++i)
	{
		char const *n{fs[i].name};
		::std::size_t nl{::std::char_traits<char>::length(n)};
		if (nl != fname.size())
		{
			continue;
		}
		bool eq{true};
		for (::std::size_t j{}; j < nl; ++j)
		{
			if (static_cast<char8_t>(n[j]) != fname.data()[j])
			{
				eq = false;
				break;
			}
		}
		if (eq)
		{
			return static_cast<::std::ptrdiff_t>(i);
		}
	}
	return -1;
}

} // namespace lcblob

// ---------------------------------------------------------------------------
// The locale representation. Storage is owned by the fast_io_i18n cache
// and never unloaded — a pointer stays valid for the process lifetime.
//
// The blob is mapped on private (copy-on-write) pages: PROT_READ|
// PROT_WRITE|MAP_PRIVATE on POSIX, PAGE_WRITECOPY/FILE_MAP_COPY on
// Windows — the default native_file_loader mode. The file image is
// shared through the OS page cache but a process can never write back.
//
// ovr_* is the per-user override segment (Windows user settings from
// HKCU\Control Panel\International — empty elsewhere). Records use the
// same encoding as blob slots with RVAs relative to ovr_begin; ovr_index
// is {key = cat<<8 | field_index, record_off} pairs. Only slots present
// in the index are overridden — everything else comes from the blob.
// ---------------------------------------------------------------------------

struct locale
{
	lcblob::blob_header header{};
	lcblob::locale_charset codeset{}; // the char-view codeset the caller asked for
	char8_t const *blob_begin{};
	char8_t const *blob_end{};
	// per-charset section spans + validated cat_dir (null = absent)
	char8_t const *sec_begin[lcblob::blob_charset_count]{};
	char8_t const *sec_end[lcblob::blob_charset_count]{};
	char8_t const *sec_catdir[lcblob::blob_charset_count]{};
	// per-charset override segment (Windows user settings)
	char8_t const *ovr_begin[lcblob::blob_charset_count]{};
	char8_t const *ovr_end[lcblob::blob_charset_count]{};
	::std::uint_least32_t const *ovr_index[lcblob::blob_charset_count]{};
	::std::uint_least32_t ovr_count[lcblob::blob_charset_count]{};
};

// where one slot record lives: rec is 'uleb tag | payload' (nullptr =
// absent); string/strref RVAs inside the record are relative to
// seg_begin, and seg_end bounds it. Feed rec/seg_begin/seg_end to
// lcblob::read_slot / strlist_elem / era_elem.
struct locale_field_ref
{
	char8_t const *rec{};
	char8_t const *seg_begin{};
	char8_t const *seg_end{};
};

// locate the slot record for (cat,fidx) in charset section cs:
// user overrides first (Windows), then the blob section. Returns {}
// when absent or out of range — no exceptions.
inline locale_field_ref locale_field(::fast_io::i18n::locale const *l, lcblob::lc_cat cat,
									 ::std::uint_least32_t fidx, lcblob::blob_charset cs) noexcept
{
	if (l == nullptr || static_cast<::std::uint_least32_t>(cat) >= lcblob::lc_cat_count ||
		fidx >= lcblob::cat_schemas[static_cast<::std::uint_least32_t>(cat)].fields.size() ||
		static_cast<::std::size_t>(cs) >= lcblob::blob_charset_count)
	{
		return {};
	}
	::std::size_t const ci{static_cast<::std::size_t>(cs)};
	if (l->ovr_count[ci] != 0)
	{
		::std::uint_least32_t const key{(static_cast<::std::uint_least32_t>(cat) << 8) | fidx};
		for (::std::uint_least32_t i{}; i < l->ovr_count[ci]; ++i)
		{
			if (l->ovr_index[ci][i * 2] == key)
			{
				return {l->ovr_begin[ci] + l->ovr_index[ci][i * 2 + 1], l->ovr_begin[ci],
						l->ovr_end[ci]};
			}
		}
	}
	char8_t const *sbase{l->sec_begin[ci]};
	if (sbase == nullptr)
	{
		return {};
	}
	::std::uint_least32_t const tbl{
		lcblob::read_u32(l->sec_catdir[ci] + static_cast<::std::uint_least32_t>(cat) * 4)};
	if (tbl == 0)
	{
		return {};
	}
	::std::uint_least64_t const bsz{static_cast<::std::uint_least64_t>(l->sec_end[ci] - sbase)};
	::std::uint_least64_t const nf{
		lcblob::cat_schemas[static_cast<::std::uint_least32_t>(cat)].fields.size() * 4};
	if (tbl > bsz || nf > bsz - tbl)
	{
		return {};
	}
	::std::uint_least32_t const srva{lcblob::read_u32(sbase + tbl + fidx * 4)};
	if (srva == 0 || srva >= bsz)
	{
		return {};
	}
	return {sbase + srva, sbase, l->sec_end[ci]};
}

// same, in the char view (slot charset — the locale's declared codeset)
inline locale_field_ref locale_field(::fast_io::i18n::locale const *l, lcblob::lc_cat cat,
									 ::std::uint_least32_t fidx) noexcept
{
	return locale_field(l, cat, fidx, lcblob::blob_charset::charset);
}

// ---------------------------------------------------------------------------
// shared-library loading API — implemented in src/locale/lcblob.cc.
// The compiled library owns the process-wide cache: thread-local map first,
// then the global map under a mutex; entries are leaked and never unloaded
// so reloads are pointer lookups, never remaps.
// ---------------------------------------------------------------------------

#if defined(FAST_IO_I18N_SHARED)
#if defined(_WIN32) || defined(__CYGWIN__)
#if defined(FAST_IO_I18N_BUILDING)
#define FAST_IO_I18N_EXPORT __declspec(dllexport)
#else
#define FAST_IO_I18N_EXPORT __declspec(dllimport)
#endif
#else
#define FAST_IO_I18N_EXPORT __attribute__((__visibility__("default")))
#endif
#else
#define FAST_IO_I18N_EXPORT
#endif

// Returns a pointer to the cached locale representation — a plain
// C-style struct, never freed (locale data is never unloaded). Every
// pointer inside it targets mmap'd blob content or cache-owned buffers
// and stays valid for the process lifetime. native_file_loader is an
// implementation detail of the library and is not exposed.
//
//   name is a standard locale name: lang[_TERRITORY][.codeset][@modifier]
//     "de_DE.UTF-8" / "de_DE.gb18030" / "de_DE.UTF-EBCDIC" ...
//     the codeset names the char view only — char8_t/char16_t/char32_t
//     sections are always present in the same file. Supported codesets:
//     UTF-8, GB18030, UTF-EBCDIC; no codeset defaults to UTF-8.
//   name ""      -> system default locale (L10N/LC_ALL/LANG on POSIX;
//                   GetUserDefaultLocaleName then the registry on Windows).
//                   On Windows the user's International settings override
//                   the overridable slots for that locale.
//   name "C"/"POSIX" resolve to the canonical POSIX.UTF-8 locale.
//   dir          from FAST_IO_LOCALE_PATH env, else the compile-time
//                FAST_IO_I18N_LOCALE_DIR macro, else /usr/lib/fast_io/locale.
FAST_IO_I18N_EXPORT ::fast_io::i18n::locale const *load_locale_blob(::fast_io::u8string_view name)
	FAST_IO_HERBCEPTIONS_THROWS;

// explicit-charset form for programmatic callers
FAST_IO_I18N_EXPORT ::fast_io::i18n::locale const *load_locale_blob(::fast_io::u8string_view name,
									 lcblob::locale_charset enc)
	FAST_IO_HERBCEPTIONS_THROWS;

} // namespace fast_io::i18n

#include "../fast_io_dsal/impl/misc/pop_macros.h"
