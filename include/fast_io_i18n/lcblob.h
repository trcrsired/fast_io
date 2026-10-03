#pragma once

#include "../fast_io_dsal/impl/misc/push_macros.h"
#include "../fast_io_dsal/string_view.h"

// lcblob consumer — read side of the binary locale container emitted by
// fast_io_tools/binfmt (see binfmt/spec.md there for the wire format).
//
// The container is flat, position-independent and ARCHITECTURE-NEUTRAL:
// every reference is an RVA offset, every numeric field is LEB128, and the
// two fixed-width fields (u32 magic, u32 RVA table entries) are stored
// little-endian. One .bin serves 32/64-bit and all platforms alike.
//
// Layout: [header][cat_dir u32*12][pool][slot tables][records]
//
//   header:   u32 magic 'FCL1' | uleb version | uleb total_size | uleb flags
//             | strref name | strref encoding | uleb cat_dir_rva
//   cat_dir:  u32 rva per category (0 = category absent)
//   pool:     string / program / list-body bytes (strref targets)
//   slot tbl: u32 rva per field (0 = slot absent)
//   records:  uleb tag | payload

namespace fast_io::i18n
{

namespace lcblob
{

inline constexpr ::std::uint_least32_t magic{0x314C4346}; // 'FCL1' LE
inline constexpr ::std::uint_least64_t blob_version{1};

// categories — fixed ids
enum lc_cat : ::std::uint_least32_t
{
	cat_identification = 0,
	cat_ctype = 1,
	cat_collate = 2,
	cat_time = 3,
	cat_numeric = 4,
	cat_monetary = 5,
	cat_messages = 6,
	cat_paper = 7,
	cat_name = 8,
	cat_address = 9,
	cat_telephone = 10,
	cat_measurement = 11,
	cat_count = 12,
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

inline constexpr cat_schema cat_schemas[cat_count]{
	{identification_fields, "LC_IDENTIFICATION"}, {ctype_fields, "LC_CTYPE"},
	{collate_fields, "LC_COLLATE"},		  {time_fields, "LC_TIME"},
	{numeric_fields, "LC_NUMERIC"},		  {monetary_fields, "LC_MONETARY"},
	{messages_fields, "LC_MESSAGES"},	  {paper_fields, "LC_PAPER"},
	{name_fields, "LC_NAME"},		  {address_fields, "LC_ADDRESS"},
	{telephone_fields, "LC_TELEPHONE"},	  {measurement_fields, "LC_MEASUREMENT"},
};

enum class blob_charset : ::std::uint_least8_t
{
	utf8 = 0,
	utf16 = 1, // LE
	utf32 = 2, // LE
};

inline constexpr char8_t const *blob_charset_name(blob_charset cs) noexcept
{
	switch (cs)
	{
	case blob_charset::utf8:
		return u8"utf8";
	case blob_charset::utf16:
		return u8"utf16";
	default:
		return u8"utf32";
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

struct blob_header
{
	::std::uint_least64_t version{};
	::std::uint_least64_t total_size{};
	::std::uint_least64_t flags{};
	::fast_io::u8string_view name{};
	::fast_io::u8string_view encoding{};
	::std::uint_least64_t cat_dir_rva{};
	char8_t const *cat_dir{}; // -> u32[cat_count]
};

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
	p = read_leb(p, e, h.cat_dir_rva);
	if (h.cat_dir_rva + cat_count * 4 > blob.size())
	{
		::fast_io::throw_posix_error(EINVAL);
	}
	h.cat_dir = blob.data() + h.cat_dir_rva;
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
	if (cat >= cat_count)
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
	lcblob::blob_charset charset{};
	char8_t const *blob_begin{};
	char8_t const *blob_end{};
	char8_t const *ovr_begin{};
	char8_t const *ovr_end{};
	::std::uint_least32_t const *ovr_index{};
	::std::uint_least32_t ovr_count{};
};

// view over the cached representation — a pointer plus inline accessors
struct locale_view
{
	::fast_io::i18n::locale const *l_{};

	inline constexpr locale_view() noexcept = default;
	inline constexpr locale_view(::fast_io::i18n::locale const *p) noexcept : l_{p}
	{
	}
	inline constexpr explicit operator bool() const noexcept
	{
		return l_ != nullptr;
	}
	inline constexpr ::fast_io::i18n::locale const *get() const noexcept
	{
		return l_;
	}
	inline constexpr ::fast_io::i18n::locale const &operator*() const noexcept
	{
		return *l_;
	}
	inline constexpr ::fast_io::i18n::locale const *operator->() const noexcept
	{
		return l_;
	}
	// raw byte views — for utf16/utf32 blobs the text is little-endian
	// char units, not utf-8
	inline constexpr ::fast_io::u8string_view name() const noexcept
	{
		return l_ == nullptr ? ::fast_io::u8string_view{} : l_->header.name;
	}
	inline constexpr ::fast_io::u8string_view encoding() const noexcept
	{
		return l_ == nullptr ? ::fast_io::u8string_view{} : l_->header.encoding;
	}
	inline constexpr lcblob::blob_charset charset() const noexcept
	{
		return l_ == nullptr ? lcblob::blob_charset::utf8 : l_->charset;
	}
	inline constexpr ::fast_io::u8string_view data() const noexcept
	{
		return l_ == nullptr ? ::fast_io::u8string_view{}
							 : ::fast_io::u8string_view{l_->blob_begin,
														static_cast<::std::size_t>(l_->blob_end - l_->blob_begin)};
	}
	inline constexpr ::std::size_t field_count(lcblob::lc_cat cat) const noexcept
	{
		return static_cast<::std::uint_least32_t>(cat) < lcblob::cat_count
				   ? lcblob::cat_schemas[static_cast<::std::uint_least32_t>(cat)].fields.size()
				   : 0;
	}

	// decode one field: user overrides first (Windows), then the blob.
	// Absent fields return a slot with tag absent, not an error.
	inline locale_slot field(lcblob::lc_cat cat,
							 ::std::uint_least32_t fidx) const FAST_IO_HERBCEPTIONS_THROWS
	{
		if (l_ == nullptr || static_cast<::std::uint_least32_t>(cat) >= lcblob::cat_count ||
			fidx >= lcblob::cat_schemas[static_cast<::std::uint_least32_t>(cat)].fields.size())
		{
			::fast_io::throw_posix_error(EINVAL);
		}
		if (l_->ovr_count != 0)
		{
			::std::uint_least32_t const key{(static_cast<::std::uint_least32_t>(cat) << 8) | fidx};
			for (::std::uint_least32_t i{}; i < l_->ovr_count; ++i)
			{
				if (l_->ovr_index[i * 2] == key)
				{
					locale_slot s;
					lcblob::read_slot(l_->ovr_begin + l_->ovr_index[i * 2 + 1], l_->ovr_end,
									  l_->ovr_begin, s);
					return s;
				}
			}
		}
		::std::uint_least32_t const tbl{
			lcblob::read_u32(l_->header.cat_dir + static_cast<::std::uint_least32_t>(cat) * 4)};
		if (tbl == 0)
		{
			return {};
		}
		::std::uint_least64_t const bsz{
			static_cast<::std::uint_least64_t>(l_->blob_end - l_->blob_begin)};
		::std::uint_least64_t const nf{
			lcblob::cat_schemas[static_cast<::std::uint_least32_t>(cat)].fields.size() * 4};
		if (tbl > bsz || nf > bsz - tbl) // malformed: table must fit in the blob
		{
			::fast_io::throw_posix_error(EINVAL);
		}
		::std::uint_least32_t const srva{lcblob::read_u32(l_->blob_begin + tbl + fidx * 4)};
		if (srva == 0)
		{
			return {};
		}
		locale_slot s;
		lcblob::read_slot(l_->blob_begin + srva, l_->blob_end, l_->blob_begin, s);
		return s;
	}

	inline locale_slot field(lcblob::lc_cat cat,
							 ::fast_io::u8string_view fname) const FAST_IO_HERBCEPTIONS_THROWS
	{
		::std::ptrdiff_t const i{lcblob::field_index(static_cast<::std::uint_least32_t>(cat), fname)};
		if (i < 0)
		{
			::fast_io::throw_posix_error(EINVAL);
		}
		return field(cat, static_cast<::std::uint_least32_t>(i));
	}
};

// ---------------------------------------------------------------------------
// shared-library loading API — implemented in src/locale/lcblob.cc.
// The compiled library owns the process-wide cache: thread-local map first,
// then the global map under a mutex; entries are leaked and never unloaded
// so reloads are pointer lookups, never remaps.
// ---------------------------------------------------------------------------

enum class locale_charset : ::std::uint_least8_t
{
	utf8 = 0,
	utf16 = 1,
	utf32 = 2,
};

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

// Returns a pointer to the cached locale representation (never freed —
// views into it stay valid for the process lifetime). Wrap in
// locale_view for typed access. native_file_loader is an implementation
// detail of the library and is not exposed.
//
//   name is a standard locale name: lang[_TERRITORY][.codeset][@modifier]
//     "de_DE.UTF-8" / "de_DE.utf8" / "de_DE@euro.UTF-16" ...
//     codeset is case/ punctuation-normalized (UTF-8, utf-16le, ...);
//     no codeset defaults to utf8. Unsupported codesets are rejected.
//   name ""      -> system default locale (L10N/LC_ALL/LANG on POSIX;
//                   GetUserDefaultLocaleName then the registry on Windows).
//                   On Windows the user's International settings override
//                   the overridable slots for that locale.
//   name "C"/"POSIX" load their blob files like any other locale.
//   dir          from FAST_IO_LOCALE_PATH env, else the compile-time
//                FAST_IO_I18N_LOCALE_DIR macro, else /usr/lib/fast_io/locale.
FAST_IO_I18N_EXPORT ::fast_io::i18n::locale const *load_locale_blob(::fast_io::u8string_view name)
	FAST_IO_HERBCEPTIONS_THROWS;

// explicit-charset form for programmatic callers
FAST_IO_I18N_EXPORT ::fast_io::i18n::locale const *load_locale_blob(::fast_io::u8string_view name,
									 locale_charset enc)
	FAST_IO_HERBCEPTIONS_THROWS;

} // namespace fast_io::i18n

#include "../fast_io_dsal/impl/misc/pop_macros.h"
