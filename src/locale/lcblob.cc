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
//   native_file_loader mmap -> read_header validate -> publish
//
// The mapped file itself is shared across processes through the OS page
// cache — the whole point of moving from per-locale .so modules to .bin.
//
// name "" resolves to the system default locale: L10N/LC_ALL/LANG on
// POSIX, GetUserDefaultLocaleName + the International registry key on
// Windows. "C"/"POSIX" are loaded from their blob files like any locale.

#include <fast_io_hosted.h>
#include <fast_io_dsal/string.h>
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

using loader_ptr = ::fast_io::native_file_loader const *;
using cache_map = ::fast_io::u8str_swiss_map<loader_ptr>;

// leaked globals — no destruction-order hazards, cache lives for the
// process lifetime by design
inline cache_map &global_cache() noexcept
{
	static cache_map *m{new cache_map{}};
	return *m;
}

inline ::fast_io::native_mutex &global_cache_lock() noexcept
{
	static ::fast_io::native_mutex *m{new ::fast_io::native_mutex{}};
	return *m;
}

struct cache_guard
{
	inline cache_guard() FAST_IO_HERBCEPTIONS_THROWS
	{
		global_cache_lock().lock();
	}
	inline ~cache_guard() noexcept
	{
		global_cache_lock().unlock();
	}
	cache_guard(cache_guard const &) = delete;
	cache_guard &operator=(cache_guard const &) = delete;
};

// lazily-allocated, intentionally leaked: safe to touch from other TLS
// destructors and never torn down
inline cache_map &thread_cache() noexcept
{
	static thread_local cache_map *m{new cache_map{}};
	return *m;
}

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
	::fast_io::u8string out;
	::fast_io::u8ostring_ref_fast_io ref{__builtin_addressof(out)};
	::fast_io::print(ref,
					 ::fast_io::mnp::code_cvt<::fast_io::encoding_scheme::utf_le,
											  ::fast_io::encoding_scheme::utf_le>(
						 ::fast_io::basic_io_scatter_t<char16_t>{s, n}));
	return out;
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
		::std::int_least32_t res{::fast_io::win32::RegQueryValueExW(
			hkey, u"LocaleName", nullptr, nullptr, buf, __builtin_addressof(bytes))};
		::fast_io::win32::RegCloseKey(hkey);
		if (res == 0 && bytes >= 4)
		{
			return u16_to_u8(buf, (bytes / 2) - 1);
		}
	}
	return {};
}

#endif

// "" -> system default; normalize '-' -> '_' (BCP-47 vs posix spellings)
// and drop a trailing ".codeset" — charset is selected by enc, not the name
inline ::fast_io::u8string resolve_locale_name(::fast_io::u8string_view name) FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::u8string out;
	if (name.empty())
	{
#if defined(_WIN32) && !defined(__WINE__) && !defined(__CYGWIN__)
		out = default_locale_name_win32();
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
			out.append(u8"C", 1);
		}
		else
		{
			auto *e8{reinterpret_cast<char8_t const *>(env)};
			out.append(e8, ::std::char_traits<char>::length(env));
		}
#endif
	}
	else
	{
		out.append(name.data(), name.size());
	}
	for (::std::size_t i{}, n{out.size()}; i < n; ++i)
	{
		char8_t ch{out.data()[i]};
		if (ch == u8'-')
		{
			out.data()[i] = u8'_';
		}
		else if (ch == u8'.')
		{
			out.resize(i); // drop .codeset
			break;
		}
	}
	check_name(::fast_io::u8string_view{out.data(), out.size()});
	return out;
}

inline ::fast_io::u8string_view locale_dir() noexcept
{
#if (!defined(_WIN32) || defined(__WINE__)) && !defined(__CYGWIN__)
	if (char const *v{lc_getenv("FAST_IO_LOCALE_PATH")}; v != nullptr && *v != 0)
	{
		return ::fast_io::u8string_view{reinterpret_cast<char8_t const *>(v),
										::std::char_traits<char>::length(v)};
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

FAST_IO_I18N_EXPORT ::fast_io::native_file_loader const *load_locale_blob(::fast_io::u8string_view name,
																		 locale_charset enc)
	FAST_IO_HERBCEPTIONS_THROWS
{
	::fast_io::u8string lname{details::resolve_locale_name(name)};
	char8_t const *enc_c{lcblob::blob_charset_name(
		static_cast<lcblob::blob_charset>(static_cast<::std::uint_least8_t>(enc)))};
	::fast_io::u8string_view enc_name{enc_c, ::std::char_traits<char8_t>::length(enc_c)};
	::fast_io::u8string_view dir{details::locale_dir()};
	if (dir.empty())
	{
		details::throw_einval();
	}
	// key = the resolved file path
	::fast_io::u8string path{::fast_io::u8concat_fast_io(dir, u8"/", lname, u8".", enc_name, u8".bin")};
	::fast_io::u8string_view key{path.data(), path.size()};

	if (auto it{details::thread_cache().find_key(key)}; it != details::thread_cache().end())
	{
		return it->mapped();
	}

	details::loader_ptr p;
	{
		details::cache_guard g;
		auto &gm{details::global_cache()};
		if (auto it{gm.find_key(key)}; it != gm.end())
		{
			p = it->mapped();
		}
		else
		{
			auto *loader{new ::fast_io::native_file_loader(path)};
			FAST_IO_HERBCEPTIONS_TRY
			{
				// never cache a bad blob: magic/version/bounds checked once here
				::std::ignore = lcblob::read_header(::fast_io::u8string_view{
					reinterpret_cast<char8_t const *>(loader->data()), loader->size()});
			}
			FAST_IO_HERBCEPTIONS_CATCH_ALL
			{
				delete loader;
				throw;
			}
			p = loader;
			gm.insert_key(key, p);
		}
	}
	details::thread_cache().insert_key(key, p);
	return p;
}

} // namespace fast_io::i18n

#include <fast_io_dsal/impl/misc/pop_macros.h>
