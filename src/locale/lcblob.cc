// lcblob.cc — compiled locale-blob cache for the fast_io_i18n library.
//
// Two-level cache of mmap'd lcblob files; entries are leaked on purpose —
// locale data is never unloaded, so a reload is a pointer lookup:
//
//   thread_local map  (no lock, no syscall)
//        |
//        miss
//        v
//   global map        (native_mutex; one entry per file per process)
//        |
//        miss
//        v
//   native_file_loader mmap -> read_header validate -> [win32 user
//   overrides] -> publish locale
//
// The mapping is private copy-on-write (the default native_file_loader
// mode: PROT_READ|PROT_WRITE|MAP_PRIVATE on POSIX, PAGE_WRITECOPY/
// FILE_MAP_COPY on Windows). The file image is still shared across
// processes through the OS page cache, but no process can write back —
// which is also what allows user settings to be patched in safely.
// On Windows, when the loaded locale is the user's own, overridable
// fields are filled from HKCU\Control Panel\International into a side
// segment so the user's system settings apply; everything else stays
// blob data.
//
// name "" resolves to the system default locale: L10N/LC_ALL/LANG on
// POSIX, GetUserDefaultLocaleName + the International registry key on
// Windows. "C"/"POSIX" are loaded from their blob files like any locale.

#include <fast_io.h>
#include <fast_io_unit/gb18030.h>
#include <fast_io_dsal/string.h>
#include <fast_io_dsal/vector.h>
#include <fast_io_dsal/str_swiss_map.h>
#include <fast_io_i18n/lcblob.h>
#include <fast_io_dsal/impl/misc/push_macros.h>

namespace fast_io::i18n
{

namespace details
{

#if (!defined(_WIN32) || defined(__WINE__)) && !defined(__CYGWIN__)
#if defined(_GNU_SOURCE) && !defined(__ANDROID__)
extern char const *libc_secure_getenv(char const *) noexcept __asm__("secure_getenv");
#else
extern char const *libc_getenv(char const *) noexcept __asm__("getenv");
#endif

inline char const *lc_getenv(char const *env) noexcept
{
	return
#if defined(_GNU_SOURCE) && !defined(__ANDROID__)
		libc_secure_getenv(env);
#else
		libc_getenv(env);
#endif
}
#endif

// One cached locale: the COW mapping (kept private to the process), the
// public representation, and storage for user-override records. The
// loader is an implementation detail — callers only see loc.
struct locale_entry
{
	inline explicit locale_entry(::fast_io::native_file_loader &&l) noexcept
		: loader(static_cast<::fast_io::native_file_loader &&>(l))
	{
	}
	::fast_io::native_file_loader loader;
	::fast_io::i18n::locale loc;
	::fast_io::u8string ovr_storage[lcblob::blob_charset_count];
	::fast_io::vector<::std::uint_least32_t> ovr_idx[lcblob::blob_charset_count];
};

// entries are allocated once through our own allocator and leaked —
// locale data is never unloaded. The maps only keep pointers to them,
// so a rehash can never invalidate anything a caller was handed.
using cache_map = ::fast_io::u8str_swiss_map<::fast_io::i18n::locale const *>;

// allocate + construct one object through fast_io's own allocator —
// cache entries are never freed by design
template <typename T, typename... Args>
inline T *lc_new(Args &&...args) FAST_IO_HERBCEPTIONS_THROWS
{
	return new (::fast_io::native_typed_global_allocator<T>::allocate(1))
		T{::fast_io::freestanding::forward<Args>(args)...};
}

// plain globals. TLS first (no lock), then the process-wide map.
::fast_io::native_mutex global_mtx;
cache_map global_map;
thread_local cache_map tls_map;

struct cache_guard
{
	inline explicit cache_guard(::fast_io::native_mutex &m) FAST_IO_HERBCEPTIONS_THROWS : mtx{m}
	{
		mtx.lock();
	}
	inline ~cache_guard() noexcept
	{
		mtx.unlock();
	}
	::fast_io::native_mutex &mtx;
	cache_guard(cache_guard const &) = delete;
	cache_guard &operator=(cache_guard const &) = delete;
};

[[noreturn]] inline void throw_einval() FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::throw_posix_error(EINVAL);
}

// validate a resolved/normalized locale name against path injection
inline void check_name(::fast_io::u8string_view name) FAST_IO_HERBCEPTIONS_THROWS
{
	constexpr ::std::size_t size_restriction{256u};
	if (name.empty() || name.size() >= size_restriction)
	{
		throw_einval();
	}
	for (char8_t ch : name)
	{
		if (ch == 0 || ch == u8'/' || ch == u8'\\')
		{
			throw_einval();
		}
	}
}

#if defined(_WIN32) && !defined(__WINE__) && !defined(__CYGWIN__)

inline ::fast_io::u8string u16_to_u8(char16_t const *s, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
{
	return ::fast_io::u8concat_fast_io(
		::fast_io::mnp::code_cvt<::fast_io::encoding_scheme::utf_le,
								 ::fast_io::encoding_scheme::utf_le>(
			::fast_io::basic_io_scatter_t<char16_t>{s, n}));
}

inline ::fast_io::u8string default_locale_name_win32() FAST_IO_HERBCEPTIONS_THROWS
{
	constexpr ::std::size_t locale_name_max{85}; // LOCALE_NAME_MAX_LENGTH
	char16_t buf[locale_name_max];
	if (int r{::fast_io::win32::GetUserDefaultLocaleName(buf, locale_name_max)}; r > 1)
	{
		return u16_to_u8(buf, static_cast<::std::size_t>(r - 1));
	}
	// registry fallback: HKCU\Control Panel\International\LocaleName
	constexpr ::std::size_t hkcu{0x80000001u};
	::std::size_t hkey{};
	if (::fast_io::win32::RegOpenKeyW(hkcu, u"Control Panel\\International", __builtin_addressof(hkey)) == 0)
	{
		::std::uint_least32_t bytes{sizeof(buf)};
		auto const res{::fast_io::win32::RegQueryValueExW(
			hkey, u"LocaleName", nullptr, nullptr, buf, __builtin_addressof(bytes))};
		::fast_io::win32::RegCloseKey(hkey);
		if (res != 0)
		{
			::fast_io::throw_win32_error();
		}
		if (bytes >= 4)
		{
			return u16_to_u8(buf, (bytes / 2) - 1);
		}
	}
	return {};
}

// ---------------------------------------------------------------------------
// user-override fill — HKCU\Control Panel\International holds the user's
// Regional-settings customizations. When the loaded locale IS the user's
// own, the overridable slots are serialized into the override segment so
// system settings win over blob data. Only plain values are filled:
// format programs (d_fmt & friends) stay from the blob — the registry
// stores them in the Windows "M/d/yyyy" mini-language, not ours.
// ---------------------------------------------------------------------------

constexpr ::std::uint_least32_t fld_key(::std::uint_least32_t cat, ::std::uint_least32_t f) noexcept
{
	return (cat << 8) | f;
}

// payload encoding of one section slot for the override segment
enum class ovr_enc : ::std::uint_least8_t
{
	utf8,
	utf16,
	utf32,
	gb18030,
	utf_ebcdic,
};

struct ovr_builder
{
	::fast_io::u8string seg;
	::fast_io::vector<::std::uint_least32_t> idx;
	ovr_enc enc;

	inline void put_leb(::std::uint_least64_t v) noexcept
	{
		char8_t buf[10];
		auto *e{::fast_io::details::pr_rsv_leb128_impl(buf, v)};
		seg.append(buf, static_cast<::std::size_t>(e - buf));
	}
	inline void put_sleb(::std::int_least64_t v) noexcept
	{
		char8_t buf[10];
		auto *e{::fast_io::details::pr_rsv_leb128_impl(buf, v)};
		seg.append(buf, static_cast<::std::size_t>(e - buf));
	}
	// utf-16 registry text -> section payload bytes; returns segment offset
	inline ::std::uint_least32_t put_u16(char16_t const *s, ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	{
		::std::uint_least32_t const off{static_cast<::std::uint_least32_t>(seg.size())};
		switch (enc)
		{
		case ovr_enc::utf16:
		{
			for (::std::size_t i{}; i < n; ++i)
			{
				char16_t c{s[i]};
				seg.push_back(static_cast<char8_t>(static_cast<char16_t>(c)));
				seg.push_back(static_cast<char8_t>(static_cast<char16_t>(c) >> 8));
			}
			break;
		}
		case ovr_enc::utf32:
		{
			::fast_io::u32string t{::fast_io::u32concat_fast_io(
				::fast_io::mnp::code_cvt<::fast_io::encoding_scheme::utf_le,
										 ::fast_io::encoding_scheme::utf_le>(
					::fast_io::basic_io_scatter_t<char16_t>{s, n}))};
			for (char32_t c : t)
			{
				::std::uint_least32_t const u{static_cast<::std::uint_least32_t>(c)};
				for (unsigned k{}; k < 4; ++k)
				{
					seg.push_back(static_cast<char8_t>(u >> (k * 8)));
				}
			}
			break;
		}
		case ovr_enc::gb18030:
		{
			::fast_io::string t{::fast_io::concat_fast_io(
				::fast_io::mnp::code_cvt<::fast_io::encoding_scheme::utf_le,
										 ::fast_io::encoding_scheme::gb18030>(
					::fast_io::basic_io_scatter_t<char16_t>{s, n}))};
			seg.append(reinterpret_cast<char8_t const *>(t.data()), t.size());
			break;
		}
		case ovr_enc::utf_ebcdic:
		{
			::fast_io::string t{::fast_io::concat_fast_io(
				::fast_io::mnp::code_cvt<::fast_io::encoding_scheme::utf_le,
										 ::fast_io::encoding_scheme::utf_ebcdic>(
					::fast_io::basic_io_scatter_t<char16_t>{s, n}))};
			seg.append(reinterpret_cast<char8_t const *>(t.data()), t.size());
			break;
		}
		default:
		{
			::fast_io::u8string t{u16_to_u8(s, n)};
			seg.append(t.data(), t.size());
			break;
		}
		}
		return off;
	}
	inline void rec_hdr(::std::uint_least32_t cat, ::std::uint_least32_t f,
						::std::uint_least64_t tag) FAST_IO_HERBCEPTIONS_THROWS
	{
		idx.push_back(fld_key(cat, f));
		idx.push_back(static_cast<::std::uint_least32_t>(seg.size()));
		put_leb(tag);
	}
	inline void add_int(::std::uint_least32_t cat, ::std::uint_least32_t f,
						::std::int_least64_t v) FAST_IO_HERBCEPTIONS_THROWS
	{
		rec_hdr(cat, f, static_cast<::std::uint_least64_t>(lcblob::slot_tag::integer));
		put_sleb(v);
	}
	inline void add_str(::std::uint_least32_t cat, ::std::uint_least32_t f, char16_t const *s,
						::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	{
		::std::uint_least32_t const off{put_u16(s, n)};
		rec_hdr(cat, f, static_cast<::std::uint_least64_t>(lcblob::slot_tag::string));
		put_leb(off);
		put_leb(seg.size() - off);
	}
	inline void add_bytes(::std::uint_least32_t cat, ::std::uint_least32_t f, char8_t const *d,
						  ::std::size_t n) FAST_IO_HERBCEPTIONS_THROWS
	{
		::std::uint_least32_t const off{static_cast<::std::uint_least32_t>(seg.size())};
		seg.append(d, n);
		rec_hdr(cat, f, static_cast<::std::uint_least64_t>(lcblob::slot_tag::bytes));
		put_leb(off);
		put_leb(n);
	}
	inline void add_strlist2(::std::uint_least32_t cat, ::std::uint_least32_t f, char16_t const *a,
							 ::std::size_t an, char16_t const *b, ::std::size_t bn) FAST_IO_HERBCEPTIONS_THROWS
	{
		::std::uint_least32_t const offa{put_u16(a, an)};
		::std::uint_least64_t const lena{seg.size() - offa};
		::std::uint_least32_t const offb{put_u16(b, bn)};
		::std::uint_least64_t const lenb{seg.size() - offb};
		::std::uint_least32_t const body{static_cast<::std::uint_least32_t>(seg.size())};
		put_leb(2);
		put_leb(offa);
		put_leb(lena);
		put_leb(offb);
		put_leb(lenb);
		rec_hdr(cat, f, static_cast<::std::uint_least64_t>(lcblob::slot_tag::strlist));
		put_leb(body);
	}
};

// HKCU\Control Panel\International — all values are REG_SZ utf-16
struct intl_key
{
	::std::size_t hkey{};
	inline intl_key() noexcept
	{
		constexpr ::std::size_t hkcu{0x80000001u};
		::fast_io::win32::RegOpenKeyW(hkcu, u"Control Panel\\International", __builtin_addressof(hkey));
	}
	inline ~intl_key() noexcept
	{
		if (hkey != 0)
		{
			::fast_io::win32::RegCloseKey(hkey);
		}
	}
	intl_key(intl_key const &) = delete;
	intl_key &operator=(intl_key const &) = delete;
	// value length in u16 units; 0 = closed key/not-a-string value
	inline ::std::uint_least32_t get(char16_t const *vname, char16_t *buf,
									 ::std::uint_least32_t cap) FAST_IO_HERBCEPTIONS_THROWS
	{
		if (hkey == 0)
		{
			return 0;
		}
		::std::uint_least32_t bytes{cap * 2};
		::std::uint_least32_t type{};
		if (::fast_io::win32::RegQueryValueExW(hkey, vname, nullptr, __builtin_addressof(type), buf,
											  __builtin_addressof(bytes)) != 0)
		{
			::fast_io::throw_win32_error();
		}
		if (type != 1u || bytes < 2) // want REG_SZ
		{
			return 0;
		}
		::std::uint_least32_t n{bytes / 2};
		if (n != 0 && buf[n - 1] == 0) // REG_SZ terminator is included
		{
			--n;
		}
		return n;
	}
};

inline bool parse_reg_int(char16_t const *s, ::std::size_t n, ::std::int_least64_t &out) noexcept
{
	bool neg{};
	::std::size_t i{};
	if (i < n && s[i] == u'-')
	{
		neg = true;
		++i;
	}
	if (i == n)
	{
		return false;
	}
	::std::int_least64_t v{};
	for (; i < n; ++i)
	{
		char16_t const c{s[i]};
		if (c < u'0' || c > u'9')
		{
			return false;
		}
		v = v * 10 + (c - u'0');
		if (v > 1000000)
		{
			return false;
		}
	}
	out = neg ? -v : v;
	return true;
}

// fill one charset's override segment from the open International key
inline void fill_ovr_charset(ovr_builder &b, intl_key &key) FAST_IO_HERBCEPTIONS_THROWS
{
	char16_t buf[160];
	constexpr ::std::uint_least32_t cap{160};
	// absent key -> RegQueryValueExW throws; override keys are optional,
	// absorb -> 0. Throws are cheap (carry flag), not C++ EH
	auto Q{[&](char16_t const *vn) noexcept -> ::std::uint_least32_t {
		FAST_IO_HERBCEPTIONS_TRY
		{
			return key.get(vn, buf, cap);
		}
		FAST_IO_HERBCEPTIONS_CATCH_ALL
		{
			return 0;
		}
	}};
	auto S{[&](char16_t const *vn, ::std::uint_least32_t cat, ::std::uint_least32_t f)
			   FAST_IO_HERBCEPTIONS_THROWS {
		if (auto const n{Q(vn)}; n != 0)
		{
			b.add_str(cat, f, buf, n);
		}
	}};
	// LC_NUMERIC / LC_MONETARY strings
	S(u"sDecimal", lcblob::lc_numeric, 0);		 // decimal_point
	S(u"sThousand", lcblob::lc_numeric, 1);		 // thousands_sep
	S(u"sCurrency", lcblob::lc_monetary, 1);		 // currency_symbol
	S(u"sMonDecimalSep", lcblob::lc_monetary, 2); // mon_decimal_point
	S(u"sMonThousandSep", lcblob::lc_monetary, 3);
	S(u"sPositiveSign", lcblob::lc_monetary, 5);
	S(u"sNegativeSign", lcblob::lc_monetary, 6);
	S(u"sCountry", lcblob::lc_address, 1); // country_name
	// grouping byte lists: "3;0" -> {3} (win32 trailing 0 = repeat last)
	auto G{[&](char16_t const *vn, ::std::uint_least32_t cat, ::std::uint_least32_t f)
			   FAST_IO_HERBCEPTIONS_THROWS {
		auto const n{Q(vn)};
		if (n == 0)
		{
			return;
		}
		char8_t gb[16];
		::std::size_t m{};
		::std::uint_least32_t v{};
		bool has{};
		for (::std::uint_least32_t i{}; i <= n; ++i)
		{
			if (i == n || buf[i] == u';')
			{
				if (!has)
				{
					return;
				}
				if (m < sizeof(gb))
				{
					gb[m++] = static_cast<char8_t>(v > 127 ? 127 : v);
				}
				v = 0;
				has = false;
			}
			else if (buf[i] >= u'0' && buf[i] <= u'9')
			{
				v = v * 10 + static_cast<::std::uint_least32_t>(buf[i] - u'0');
				has = true;
			}
			else
			{
				return;
			}
		}
		while (m != 0 && gb[m - 1] == 0)
		{
			--m;
		}
		b.add_bytes(cat, f, gb, m);
	}};
	G(u"sGrouping", lcblob::lc_numeric, 2);
	G(u"sMonGrouping", lcblob::lc_monetary, 4);
	// integers
	::std::int_least64_t v{};
	auto I{[&](char16_t const *vn, ::std::int_least64_t &out) FAST_IO_HERBCEPTIONS_THROWS {
		auto const n{Q(vn)};
		return n != 0 && parse_reg_int(buf, n, out);
	}};
	if (I(u"iMeasure", v) && (v == 0 || v == 1))
	{
		b.add_int(lcblob::lc_measurement, 0, v + 1); // win32 0/1 -> glibc 1/2
	}
	if (I(u"iDigits", v))
	{
		b.add_int(lcblob::lc_monetary, 8, v); // frac_digits
	}
	if (I(u"iCurrDigits", v))
	{
		b.add_int(lcblob::lc_monetary, 7, v); // int_frac_digits
	}
	if (I(u"iCurrency", v) && v >= 0 && v <= 3)
	{
		// 0 "$1.1" 1 "1.1$" 2 "$ 1.1" 3 "1.1 $"
		::std::int_least64_t const prec{v == 0 || v == 2};
		::std::int_least64_t const sep{v >= 2};
		b.add_int(lcblob::lc_monetary, 9, prec);	 // p_cs_precedes
		b.add_int(lcblob::lc_monetary, 10, sep);	 // p_sep_by_space
		b.add_int(lcblob::lc_monetary, 13, prec); // int_*
		b.add_int(lcblob::lc_monetary, 14, sep);
	}
	if (I(u"iNegCurr", v) && v >= 0 && v <= 15)
	{
		// -> {n_cs_precedes, n_sep_by_space, n_sign_posn}
		constexpr ::std::int_least8_t tab[16][3]{
			{1, 0, 0}, // ($1.1)
			{1, 0, 3}, // -$1.1
			{1, 0, 4}, // $-1.1
			{1, 0, 2}, // $1.1-
			{0, 0, 0}, // (1.1$)
			{0, 0, 1}, // -1.1$
			{0, 0, 2}, // 1.1-$
			{0, 0, 2}, // 1.1$-
			{0, 1, 1}, // -1.1 $
			{1, 1, 3}, // -$ 1.1
			{0, 1, 2}, // 1.1 $-
			{1, 1, 2}, // $ 1.1-
			{1, 1, 4}, // $ -1.1
			{0, 1, 2}, // 1.1- $
			{1, 1, 0}, // ($ 1.1)
			{0, 1, 0}, // (1.1 $)
		};
		b.add_int(lcblob::lc_monetary, 11, tab[v][0]); // n_cs_precedes
		b.add_int(lcblob::lc_monetary, 12, tab[v][1]); // n_sep_by_space
		b.add_int(lcblob::lc_monetary, 18, tab[v][2]); // n_sign_posn
		b.add_int(lcblob::lc_monetary, 15, tab[v][0]); // int_*
		b.add_int(lcblob::lc_monetary, 16, tab[v][1]);
		b.add_int(lcblob::lc_monetary, 20, tab[v][2]);
	}
	if (I(u"iFirstDayOfWeek", v) && v >= 0 && v <= 6)
	{
		b.add_int(lcblob::lc_time, 18, v + 1); // win32 0=Mon..6=Sun -> 1..7
	}
	if (I(u"iFirstWorkday", v) && v >= 0 && v <= 6)
	{
		b.add_int(lcblob::lc_time, 19, v + 1);
	}
	if (I(u"iPaperSize", v))
	{
		// -> LC_PAPER {height_mm, width_mm}
		::std::int_least64_t hw{};
		switch (v)
		{
		case 1:
			hw = 279 * 1000 + 216;
			break; // Letter
		case 5:
			hw = 356 * 1000 + 216;
			break; // Legal
		case 8:
			hw = 420 * 1000 + 297;
			break; // A3
		case 9:
			hw = 297 * 1000 + 210;
			break; // A4
		default:
			break;
		}
		if (hw != 0)
		{
			b.add_int(lcblob::lc_paper, 0, hw / 1000);
			b.add_int(lcblob::lc_paper, 1, hw % 1000);
		}
	}
	// am/pm designators — one strlist slot, so it is overridden whole
	{
		char16_t am[160], pm[160];
		::std::uint_least32_t nam{}, npm{};
		FAST_IO_HERBCEPTIONS_TRY
		{
			nam = key.get(u"s1159", am, cap);
		}
		FAST_IO_HERBCEPTIONS_CATCH_ALL
		{
		}
		FAST_IO_HERBCEPTIONS_TRY
		{
			npm = key.get(u"s2359", pm, cap);
		}
		FAST_IO_HERBCEPTIONS_CATCH_ALL
		{
		}
		if (nam != 0 || npm != 0)
		{
			b.add_strlist2(lcblob::lc_time, 11, am, nam, pm, npm);
		}
	}
}

inline void fill_user_overrides(locale_entry &e, ::fast_io::u8string_view lname)
	FAST_IO_HERBCEPTIONS_THROWS
{
	// overrides apply only to the user's own locale
	::fast_io::u8string d{default_locale_name_win32()};
	if (d.size() != lname.size())
	{
		return;
	}
	{
		bool eq{true};
		for (::std::size_t i{}; i < d.size(); ++i)
		{
			char8_t c{d[i]};
			if (c == u8'-')
			{
				c = u8'_';
			}
			if (c != lname.data()[i])
			{
				eq = false;
				break;
			}
		}
		if (!eq)
		{
			return;
		}
	}
	intl_key key;
	if (key.hkey == 0)
	{
		return;
	}
	// the same registry values serialized per section — every section
	// of the blob sees user settings in its own encoding
	for (::std::size_t c{}; c < lcblob::blob_charset_count; ++c)
	{
		ovr_enc enc{ovr_enc::utf8};
		switch (static_cast<lcblob::blob_charset>(c))
		{
		case lcblob::blob_charset::utf16:
			enc = ovr_enc::utf16;
			break;
		case lcblob::blob_charset::utf32:
			enc = ovr_enc::utf32;
			break;
		case lcblob::blob_charset::charset:
			enc = e.loc.codeset == lcblob::locale_charset::gb18030 ? ovr_enc::gb18030
				  : e.loc.codeset == lcblob::locale_charset::utf_ebcdic	? ovr_enc::utf_ebcdic
																		: ovr_enc::utf8;
			break;
		default:
			break;
		}
		ovr_builder b{{}, {}, enc};
		fill_ovr_charset(b, key);
		if (!b.idx.empty())
		{
			e.ovr_storage[c] = static_cast<::fast_io::u8string &&>(b.seg);
			e.ovr_idx[c] = static_cast<::fast_io::vector<::std::uint_least32_t> &&>(b.idx);
		}
	}
}

#endif

// codeset text -> locale charset; normalized: lowercase, '-'/'_'/' '
// dropped (UTF-8, gb18030, UTF_EBCDIC ...). Only the three supported
// codesets are accepted.
inline void parse_codeset(::fast_io::u8string_view cs, lcblob::locale_charset &out) FAST_IO_HERBCEPTIONS_THROWS
{
	char buf[16];
	::std::size_t n{};
	for (char8_t ch : cs)
	{
		if (ch == u8'-' || ch == u8'_' || ch == u8' ')
		{
			continue;
		}
		if (n >= sizeof(buf))
		{
			throw_einval();
		}
		buf[n++] = static_cast<char>(ch >= u8'A' && ch <= u8'Z' ? ch + 0x20 : ch);
	}
	::fast_io::string_view sv{buf, n};
	if (sv == "utf8")
	{
		out = lcblob::locale_charset::utf8;
	}
	else if (sv == "gb18030")
	{
		out = lcblob::locale_charset::gb18030;
	}
	else if (sv == "utfebcdic")
	{
		out = lcblob::locale_charset::utf_ebcdic;
	}
	else
	{
		throw_einval();
	}
}

// "" -> system default. Parses the standard
// lang[_TERRITORY][.codeset][@modifier] form: '-' -> '_' (BCP-47 vs posix
// spellings), codeset is normalized into out_cs (absent -> utf8), the
// modifier stays attached to the file basename.
inline void resolve_locale_name(::fast_io::u8string_view name, ::fast_io::u8string &out,
								lcblob::locale_charset &out_cs,
								lcblob::locale_charset const *enc = nullptr) FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::u8string raw;
	if (name.empty())
	{
#if defined(_WIN32) && !defined(__WINE__) && !defined(__CYGWIN__)
		raw = default_locale_name_win32();
#else
		char const *env{};
		for (char const *key : {"L10N", "LC_ALL", "LANG"})
		{
			if (char const *v{lc_getenv(key)}; v != nullptr && *v != 0)
			{
				env = v;
				break;
			}
		}
		if (env == nullptr)
		{
			raw.append(u8"C", 1);
		}
		else
		{
			auto *e8{reinterpret_cast<char8_t const *>(env)};
			raw.append(e8, ::fast_io::cstr_len(env));
		}
#endif
	}
	else
	{
		raw.append(name.data(), name.size());
	}
	for (char8_t &ch : raw) // '-' -> '_' (BCP-47 vs posix spellings)
	{
		if (ch == u8'-')
		{
			ch = u8'_';
		}
	}
	::std::size_t n{raw.size()};
	::std::size_t mod{n}, dot{n};
	for (::std::size_t i{}; i < n; ++i)
	{
		char8_t ch{raw.data()[i]};
		if (ch == u8'@' && mod == n)
		{
			mod = i;
		}
		else if (ch == u8'.' && dot == n && mod == n)
		{
			dot = i;
		}
	}
	out_cs = lcblob::locale_charset::utf8; // no codeset -> UTF-8
	if (dot != n)
	{
		parse_codeset(::fast_io::u8string_view{raw.data() + dot + 1, mod - dot - 1}, out_cs);
	}
	if (enc != nullptr)
	{
		out_cs = *enc; // explicit codeset wins over the name's
	}
	::std::size_t const base_end{dot == n ? mod : dot};
	// C and POSIX are the same UTF-8 locale — canonical name POSIX.UTF-8
	if ((base_end == 1 && raw.data()[0] == u8'C') ||
		(base_end == 5 && raw.data()[0] == u8'P' && raw.data()[1] == u8'O' &&
		 raw.data()[2] == u8'S' && raw.data()[3] == u8'I' && raw.data()[4] == u8'X'))
	{
		out.append(u8"POSIX.UTF-8", 11);
		out_cs = lcblob::locale_charset::utf8;
	}
	else
	{
		auto const *csn{lcblob::locale_charset_name(out_cs)};
		out.append(raw.data(), base_end); // base name
		out.push_back(u8'.');
		out.append(csn, ::fast_io::cstr_len(csn)); // canonical codeset
	}
	out.append(raw.data() + mod, n - mod); // @modifier, if any
	check_name(::fast_io::u8string_view{out.data(), out.size()});
}

inline ::fast_io::u8string_view locale_dir() noexcept
{
#if (!defined(_WIN32) || defined(__WINE__)) && !defined(__CYGWIN__)
	if (char const *v{lc_getenv("FAST_IO_LOCALE_PATH")}; v != nullptr && *v != 0)
	{
		return ::fast_io::u8string_view{reinterpret_cast<char8_t const *>(v),
										::fast_io::cstr_len(v)};
	}
#endif
#ifdef FAST_IO_I18N_LOCALE_DIR
	return ::fast_io::u8string_view{u8"" FAST_IO_I18N_LOCALE_DIR};
#else
#if defined(_WIN32) && !defined(__WINE__) && !defined(__CYGWIN__)
	return ::fast_io::u8string_view{u8""};
#else
	return ::fast_io::u8string_view{u8"/usr/lib/fast_io/locale"};
#endif
#endif
}

} // namespace details

namespace details
{

inline ::fast_io::i18n::locale const *load_blob_impl(::fast_io::u8string_view lname,
													 lcblob::locale_charset cs) FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::u8string_view dir{locale_dir()};
	if (dir.empty())
	{
		throw_einval();
	}
	// one file per locale carries every charset — key = resolved path
	::fast_io::u8string path{::fast_io::u8concat_fast_io(dir, u8"/", lname, u8".bin")};
	::fast_io::u8string_view key{path.data(), path.size()};

	if (auto it{tls_map.find_key(key)}; it != tls_map.end())
	{
		return it->mapped();
	}

	::fast_io::i18n::locale const *p;
	{
		cache_guard g{global_mtx};
		if (auto it{global_map.find_key(key)}; it != global_map.end())
		{
			p = it->mapped();
		}
		else
		{
			// private copy-on-write pages — the default loader mode.
			// validate before allocating: a bad blob never enters the
			// cache, the loader's RAII unmaps on its own
			::fast_io::native_file_loader loader(path);
			::fast_io::u8string_view bv{reinterpret_cast<char8_t const *>(loader.data()),
										loader.size()};
			lcblob::blob_header const hdr{lcblob::read_header(bv)};
			lcblob::section_header secs[lcblob::blob_charset_count];
			for (::std::size_t c{}; c < lcblob::blob_charset_count; ++c)
			{
				if (hdr.sec_rva[c] == 0)
				{
					continue;
				}
				secs[c] = lcblob::read_section_header(bv.data() + hdr.sec_rva[c],
													hdr.sec_size[c]);
			}
			auto *entry{lc_new<locale_entry>(static_cast<::fast_io::native_file_loader &&>(loader))};
			entry->loc.header = hdr;
			entry->loc.blob_begin = bv.data();
			entry->loc.blob_end = bv.data() + bv.size();
			entry->loc.codeset = cs;
			for (::std::size_t c{}; c < lcblob::blob_charset_count; ++c)
			{
				if (hdr.sec_rva[c] == 0)
				{
					continue;
				}
				entry->loc.sec_begin[c] = bv.data() + hdr.sec_rva[c];
				entry->loc.sec_end[c] = bv.data() + hdr.sec_rva[c] + hdr.sec_size[c];
				entry->loc.sec_catdir[c] = secs[c].cat_dir;
			}
#if defined(_WIN32) && !defined(__WINE__) && !defined(__CYGWIN__)
			FAST_IO_HERBCEPTIONS_TRY
			{
				fill_user_overrides(*entry, lname);
			}
#if defined(__HERBCEPTIONS__)
			catch throws (::std::error e)
			{
				// unreadable user settings must not break the load —
				// the blob data still applies on its own
				::fast_io::perrln(e);
			}
#else
			catch (...)
			{
			}
#endif
#endif
			for (::std::size_t c{}; c < lcblob::blob_charset_count; ++c)
			{
				if (!entry->ovr_idx[c].empty())
				{
					entry->loc.ovr_begin[c] = entry->ovr_storage[c].data();
					entry->loc.ovr_end[c] = entry->ovr_storage[c].data() + entry->ovr_storage[c].size();
					entry->loc.ovr_index[c] = entry->ovr_idx[c].data();
					entry->loc.ovr_count[c] =
						static_cast<::std::uint_least32_t>(entry->ovr_idx[c].size() / 2);
				}
			}
			p = __builtin_addressof(entry->loc);
			global_map.insert_key(key, p);
		}
	}
	tls_map.insert_key(key, p);
	return p;
}

} // namespace details

FAST_IO_I18N_EXPORT ::fast_io::i18n::locale const *load_locale_blob(::fast_io::u8string_view name)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::u8string lname;
	lcblob::locale_charset cs{};
	details::resolve_locale_name(name, lname, cs);
	return details::load_blob_impl(::fast_io::u8string_view{lname.data(), lname.size()}, cs);
}

FAST_IO_I18N_EXPORT ::fast_io::i18n::locale const *load_locale_blob(::fast_io::u8string_view name,
																   lcblob::locale_charset enc)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::u8string lname;
	lcblob::locale_charset cs{};
	details::resolve_locale_name(name, lname, cs, __builtin_addressof(enc));
	return details::load_blob_impl(::fast_io::u8string_view{lname.data(), lname.size()}, cs);
}

} // namespace fast_io::i18n

#include <fast_io_dsal/impl/misc/pop_macros.h>
