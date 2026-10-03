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
	auto get_leb{[&p, e](auto &v) FAST_IO_HERBCEPTIONS_THROWS {
		auto [it, ec]{::fast_io::details::scn_cnt_define_leb128_impl(p, e, v)};
		if (ec != ::fast_io::freestanding::parse_errc::ok)
		{
			::fast_io::throw_posix_error(EINVAL);
		}
		p = it;
	}};
	get_leb(h.version);
	if (h.version > blob_version)
	{
		::fast_io::throw_posix_error(EINVAL);
	}
	get_leb(h.total_size);
	get_leb(h.flags);
	::std::uint_least64_t rva{}, len{};
	get_leb(rva);
	get_leb(len);
	if (rva + len > blob.size())
	{
		::fast_io::throw_posix_error(EINVAL);
	}
	h.name = ::fast_io::u8string_view{blob.data() + rva, len};
	get_leb(rva);
	get_leb(len);
	if (rva + len > blob.size())
	{
		::fast_io::throw_posix_error(EINVAL);
	}
	h.encoding = ::fast_io::u8string_view{blob.data() + rva, len};
	get_leb(h.cat_dir_rva);
	if (h.cat_dir_rva + cat_count * 4 > blob.size())
	{
		::fast_io::throw_posix_error(EINVAL);
	}
	h.cat_dir = blob.data() + h.cat_dir_rva;
	return h;
}

} // namespace lcblob

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

// Returns a never-freed pointer to the mapped lcblob for
// <dir>/<name>.<utf8|utf16|utf32>.bin.
//   name ""      -> system default locale (L10N/LC_ALL/LANG on POSIX;
//                   GetUserDefaultLocaleName then the registry on Windows).
//   name "C"/"POSIX" load their blob files like any other locale.
//   dir          from FAST_IO_LOCALE_PATH env, else the compile-time
//                FAST_IO_I18N_LOCALE_DIR macro, else /usr/lib/fast_io/locale.
FAST_IO_I18N_EXPORT ::fast_io::native_file_loader const *load_locale_blob(::fast_io::u8string_view name,
									 locale_charset enc)
	FAST_IO_HERBCEPTIONS_THROWS;

} // namespace fast_io::i18n

#include "../fast_io_dsal/impl/misc/pop_macros.h"
