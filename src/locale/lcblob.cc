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
//   native_file_loader mmap -> magic+version check -> publish the
//   lc_locale pointer straight into the mapping
//
// The mapping is private copy-on-write (the default native_file_loader
// mode: PROT_READ|PROT_WRITE|MAP_PRIVATE on POSIX, PAGE_WRITECOPY/
// FILE_MAP_COPY on Windows). The file image is still shared across
// processes through the OS page cache, but no process can write back —
// users may also patch rva fields in their own mapping to customize
// fields without touching the shared image.
//
// name "" resolves to the system default locale: L10N/LC_ALL/LANG on
// POSIX, GetUserDefaultLocaleName + the International registry key on
// Windows. "C"/"POSIX" are loaded from their blob files like any locale.

#include <fast_io.h>
#include <fast_io_device.h>
#include <fast_io_dsal/string.h>
#include <fast_io_dsal/str_swiss_map.h>
#include <fast_io_i18n/lcblob.h>
#include <fast_io_dsal/impl/misc/push_macros.h>

namespace fast_io::l10n
{

namespace lcblob = ::fast_io::l10n;


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

// One cached locale: the COW mapping. The published pointer targets the
// mapping itself — the loader just keeps it alive. Entries are
// allocated once through our own allocator and leaked on purpose:
// locale data is never unloaded, so a rehash can never invalidate
// anything a caller was handed.
struct locale_entry
{
	inline explicit locale_entry(::fast_io::native_file_loader &&l) noexcept
		: loader(static_cast<::fast_io::native_file_loader &&>(l))
	{
	}
	::fast_io::native_file_loader loader;
};

using cache_map = ::fast_io::u8str_swiss_map<::fast_io::l10n::lc_locale const *>;

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

inline ::fast_io::u8string default_locale_name_win32(bool ignore_system_settings) FAST_IO_HERBCEPTIONS_THROWS
{
	constexpr ::std::size_t locale_name_max{85}; // LOCALE_NAME_MAX_LENGTH
	char16_t buf[locale_name_max];
	if (int r{::fast_io::win32::GetUserDefaultLocaleName(buf, locale_name_max)}; r > 1)
	{
		return u16_to_u8(buf, static_cast<::std::size_t>(r - 1));
	}
	// registry fallback — skipped under ignore_system_settings
	if (!ignore_system_settings)
	{
		constexpr ::std::size_t hkcu{0x80000001u};
		::std::size_t hkey{};
		if (::fast_io::win32::RegOpenKeyW(hkcu, u"Control Panel\\International",
										__builtin_addressof(hkey)) == 0)
		{
			::std::uint_least32_t bytes{sizeof(buf)};
			auto const res{::fast_io::win32::RegQueryValueExW(
				hkey, u"LocaleName", nullptr, nullptr, buf, __builtin_addressof(bytes))};
			::fast_io::win32::RegCloseKey(hkey);
			if (res == 0 && bytes >= 4)
			{
				return u16_to_u8(buf, (bytes / 2) - 1);
			}
		}
	}
	return {}; // "" resolution falls back to C like other platforms
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
// spellings), codeset is normalized (absent -> utf8), the modifier stays
// attached to the file basename.
// out = canonical file basename  base.codeset[@mod]
inline void resolve_locale_name(::fast_io::u8string_view name, ::fast_io::u8string &out,
								lcblob::locale_charset &out_cs,
								lcblob::locale_charset const *enc = nullptr,
								bool ignore_system_settings = false) FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::u8string raw;
	if (name.empty())
	{
#if defined(_WIN32) && !defined(__WINE__) && !defined(__CYGWIN__)
		// ignore_system_settings only gates the registry fallback — the
		// GetUserDefaultLocaleName API is the standard OS lookup
		raw = default_locale_name_win32(ignore_system_settings);
		if (raw.empty())
		{
			raw.append(u8"C", 1);
		}
#else
		char const *env{};
		for (char const *var : {"L10N", "LC_ALL", "LANG"})
		{
			if (char const *v{lc_getenv(var)}; v != nullptr && *v != 0)
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

// the locale data directory: FAST_IO_LOCALE_PATH, or the build-time
// FAST_IO_I18N_LOCALE_DIR default. Nothing configured is an error —
// there is no system-wide default to silently guess.
inline ::fast_io::u8string_view locale_dir() FAST_IO_HERBCEPTIONS_THROWS
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
	throw_einval();
#endif
}

// the locale directory fd — opened lazily on the first miss and kept
// open for the process's lifetime; every locale file is opened relative
// to it (openat semantics), never by string concatenation
inline ::fast_io::dir_file *locale_dir_file{};

inline ::fast_io::l10n::lc_locale const *load_blob_impl(::fast_io::u8string_view lname)
	FAST_IO_HERBCEPTIONS_THROWS
{
	// one file per locale+codeset — key = canonical file name
	::fast_io::u8string path{::fast_io::u8concat_fast_io(lname, u8".bin")};
	::fast_io::u8string_view key{path.data(), path.size()};

	if (auto it{tls_map.find_key(key)}; it != tls_map.end())
	{
		return it->mapped();
	}

	::fast_io::l10n::lc_locale const *p;
	{
		cache_guard g{global_mtx};
		if (auto it{global_map.find_key(key)}; it != global_map.end())
		{
			p = it->mapped();
		}
		else
		{
			if (locale_dir_file == nullptr)
			{
				locale_dir_file = lc_new<::fast_io::dir_file>(
					::fast_io::mnp::os_c_str(locale_dir().data()));
			}
			// private copy-on-write pages — the default loader mode.
			// the only checks are magic and version; everything else
			// is the file's own struct layout.
			::fast_io::native_file_loader loader{::fast_io::at(*locale_dir_file),
												 path};
			::fast_io::u8string_view bv{reinterpret_cast<char8_t const *>(loader.data()),
										loader.size()};
			if (bv.size() < sizeof(lcblob::lc_locale))
			{
				throw_einval();
			}
			auto const *loc{reinterpret_cast<lcblob::lc_locale const *>(bv.data())};
			if (lcblob::lc_u32(loc->magic) != lcblob::magic ||
				lcblob::lc_u32(loc->version) > lcblob::blob_version)
			{
				throw_einval();
			}
			auto *entry{lc_new<locale_entry>(static_cast<::fast_io::native_file_loader &&>(loader))};
			p = reinterpret_cast<lcblob::lc_locale const *>(entry->loader.data());
			global_map.insert_key(key, p);
		}
	}
	tls_map.insert_key(key, p);
	return p;
}

} // namespace details

FAST_IO_I18N_EXPORT ::fast_io::l10n::lc_locale const *load_l10n(::fast_io::u8string_view name,
																   l10n_load_flags flags)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::u8string lname;
	lcblob::locale_charset cs{};
	bool const ignore_system_settings{
		(flags & l10n_load_flags::ignore_system_settings) != l10n_load_flags::none};
	details::resolve_locale_name(name, lname, cs, nullptr, ignore_system_settings);
	return details::load_blob_impl(::fast_io::u8string_view{lname.data(), lname.size()});
}

FAST_IO_I18N_EXPORT ::fast_io::l10n::lc_locale const *load_l10n(::fast_io::u8string_view name,
																   lcblob::locale_charset enc,
																   l10n_load_flags flags)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::u8string lname;
	lcblob::locale_charset cs{};
	bool const ignore_system_settings{
		(flags & l10n_load_flags::ignore_system_settings) != l10n_load_flags::none};
	details::resolve_locale_name(name, lname, cs, __builtin_addressof(enc), ignore_system_settings);
	return details::load_blob_impl(::fast_io::u8string_view{lname.data(), lname.size()});
}

} // namespace fast_io::l10n

#include <fast_io_dsal/impl/misc/pop_macros.h>
