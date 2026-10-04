// locale_emit — compile every glibc localedata file into flat binary
// locale blobs (name.utf8.bin / name.utf16.bin / name.utf32.bin).
//
// usage: locale_emit <localedata dir> <output dir>

#include <herbceptions/error>
#include <fast_io.h>
#include <fast_io_device.h>
#include <fast_io_unit/gb18030.h>
#include <fast_io_dsal/vector.h>
#include <fast_io_dsal/string.h>
#include <fast_io_dsal/string_view.h>
#include "localedef.h"

// one codeset the emitter may produce: names normalized like the
// consumer (UTF-8, gb18030, UTF_EBCDIC)
inline bool parse_cset(::std::string_view sv, ::fast_io_i18n::lcblob::blob_charset &enc,
					   ::std::u8string_view &canon) noexcept
{
	char buf[16];
	::std::size_t n{};
	for (char ch : sv)
	{
		if (ch == '-' || ch == '_' || ch == ' ')
		{
			continue;
		}
		if (n >= sizeof(buf))
		{
			return false;
		}
		buf[n++] = static_cast<char>(ch >= 'A' && ch <= 'Z' ? ch + 0x20 : ch);
	}
	::std::string_view nsv{buf, n};
	if (nsv == "utf8")
	{
		enc = ::fast_io_i18n::lcblob::blob_charset::utf8;
		canon = u8"UTF-8";
	}
	else if (nsv == "gb18030")
	{
		enc = ::fast_io_i18n::lcblob::blob_charset::gb18030;
		canon = u8"GB18030";
	}
	else if (nsv == "utfebcdic")
	{
		enc = ::fast_io_i18n::lcblob::blob_charset::utf_ebcdic;
		canon = u8"UTF-EBCDIC";
	}
	else
	{
		return false;
	}
	return true;
}

int main(int argc, char **argv) try
{
	using namespace ::fast_io_i18n;
	if (argc < 3)
	{
		::fast_io::perrln(
			"usage: locale_emit <localedata dir> <output dir> [codeset ...]\n"
			"  codesets: UTF-8, GB18030, UTF-EBCDIC (default: UTF-8 GB18030)");
		return 1;
	}
	// codesets to emit — default UTF-8 + GB18030 like the old system
	::fast_io_i18n::lcblob::blob_charset encs[8];
	::std::u8string_view encn[8];
	::std::size_t nenc{};
	if (argc > 3)
	{
		for (int i{3}; i < argc && nenc < 8; ++i)
		{
			if (!parse_cset({argv[i], ::fast_io::cstr_len(argv[i])}, encs[nenc], encn[nenc]))
			{
				::fast_io::perrln("unsupported codeset: ", ::fast_io::mnp::os_c_str(argv[i]));
				return 1;
			}
			++nenc;
		}
	}
	else
	{
		encs[0] = ::fast_io_i18n::lcblob::blob_charset::utf8;
		encn[0] = u8"UTF-8";
		encs[1] = ::fast_io_i18n::lcblob::blob_charset::gb18030;
		encn[1] = u8"GB18030";
		nenc = 2;
	}
	::fast_io::dir_file df(::fast_io::mnp::os_c_str(argv[1]));
	::fast_io::dir_file outdir(::fast_io::mnp::os_c_str(argv[2]));
	localedef::file_cache cache;
	for (auto ent : ::fast_io::current(::fast_io::at(df)))
	{
		auto fn{::fast_io::u8filename(ent)};
		::std::u8string_view uname{fn.c_str(), fn.c_str() + fn.n};
		// file name as char string for parsing/writing
		::fast_io::string name;
		name.append(reinterpret_cast<char const *>(fn.c_str()), fn.n);
		::std::string_view nsv{name.data(), name.size()};
		if (nsv == "." || nsv == ".." || nsv == "cns11643_stroke" ||
		    nsv == "i18n_ctype" || nsv == "POSIX" || nsv.substr(0, 8) == "iso14651" ||
		    nsv.substr(0, 8) == "translit")
		{
			continue;
		}
		// C and POSIX are the same UTF-8 locale — one file, POSIX.UTF-8
		bool const is_c{nsv == "C"};
		try
		{
			::fast_io::println("parse ", nsv);
			localedef::lc_file_data d;
			localedef::parse_file(df, name, d, cache, 0);
			::fast_io_i18n::lcblob::cat_src cats[::fast_io_i18n::lcblob::cat_count]{};
			localedef::to_cats(d, cats, {{name.data(), name.size()}, 0});
			::std::string_view ctx{name.data(), name.size()};
			for (::std::size_t e{}; e < (is_c ? 1 : nenc); ++e)
			{
				// canonical locale name = lang.codeset
				::fast_io::u8string lname;
				if (is_c)
				{
					lname.append(u8"POSIX.UTF-8", 11);
				}
				else
				{
					lname.append(uname.data(), uname.size());
					lname.push_back(u8'.');
					lname.append(encn[e].data(), encn[e].size());
				}
				::std::u8string_view const lv{lname.data(), lname.size()};
				// section slots [charset, utf8, utf16, utf32]; the charset
				// one is only built for non-utf8 codesets — otherwise
				// slot 0 aliases the utf8 section
				::fast_io_i18n::lcblob::sec_build secs[4];
				if (encs[e] != ::fast_io_i18n::lcblob::blob_charset::utf8)
				{
					secs[0] = ::fast_io_i18n::lcblob::build_section(
						cats, lv, encn[e], encs[e], ctx);
				}
				secs[1] = ::fast_io_i18n::lcblob::build_section(
					cats, lv, u8"UTF-8", ::fast_io_i18n::lcblob::blob_charset::utf8,
					ctx);
				secs[2] = ::fast_io_i18n::lcblob::build_section(
					cats, lv, u8"UTF-16", ::fast_io_i18n::lcblob::blob_charset::utf16,
					ctx);
				secs[3] = ::fast_io_i18n::lcblob::build_section(
					cats, lv, u8"UTF-32", ::fast_io_i18n::lcblob::blob_charset::utf32,
					ctx);
				// blob_charset payload tag -> locale_charset codeset id
				namespace ilc = ::fast_io::l10n;
				::std::uint_least32_t const codeset{static_cast<::std::uint_least32_t>(
					encs[e] == ::fast_io_i18n::lcblob::blob_charset::gb18030
						? ilc::locale_charset::gb18030
					: encs[e] == ::fast_io_i18n::lcblob::blob_charset::utf_ebcdic
						? ilc::locale_charset::utf_ebcdic
						: ilc::locale_charset::utf8)};
				auto blob{::fast_io_i18n::lcblob::build_container(
					secs, encs[e] == ::fast_io_i18n::lcblob::blob_charset::utf8, lv,
					codeset)};
				::fast_io::u8string outname{lname};
				outname.append(u8".bin", 4);
				::fast_io::obuf_file of{
					::fast_io::at(outdir),
					::fast_io::mnp::os_c_str(reinterpret_cast<char const *>(outname.c_str()))};
				::fast_io::print(of,
						 ::std::string_view{blob.data(), blob.size()});
			}
		}
		catch throws(::std::error e)
		{
			::fast_io::perrln("  ^^ on ", nsv, ": ", e);
		}
	}
	return 0;
}
catch throws(::std::error e)
{
	::fast_io::perrln("err: ", e);
	return 1;
}
