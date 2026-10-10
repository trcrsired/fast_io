/*
 * A node.js `http-server` look-alike on fast_io's async socket machinery:
 * a static HTTP/1.1 file server.
 *
 *   http-server [root] [-p port] [-c seconds] [--no-index] [--no-dir]
 *
 * Feature parity with the node tool's defaults: percent-decoded paths
 * with ".." clamped at the root (path.normalize semantics), directory
 * redirect to a trailing slash, index.html, an HTML directory listing,
 * Content-Type by extension, ETag + If-None-Match, If-Modified-Since,
 * single byte ranges, HEAD, and keep-alive. Skipped: TLS, precompressed
 * .gz/.br variants, -o browser launch.
 *
 * One detached coroutine per connection plus the accept coroutine —
 * the same two-coroutine shape as tcp_echo_server.cc.
 */
#include <fast_io.h>
#include <fast_io_device.h>
#include <fast_io_dsal/string.h>
#include <fast_io_dsal/string_view.h>

namespace fi = ::fast_io;
namespace fop = ::fast_io::operations;

/* ------------------------------ mime ------------------------------ */

static bool ascii_ieq(fi::u8string_view a, fi::u8string_view b) noexcept
{
	if (a.size() != b.size())
	{
		return false;
	}
	for (::std::size_t i{}; i != a.size(); ++i)
	{
		auto x{a[i]};
		auto y{b[i]};
		if (u8'A' <= x && x <= u8'Z')
		{
			x = static_cast<char8_t>(x + 32);
		}
		if (u8'A' <= y && y <= u8'Z')
		{
			y = static_cast<char8_t>(y + 32);
		}
		if (x != y)
		{
			return false;
		}
	}
	return true;
}

struct mime_entry
{
	fi::u8string_view ext;
	fi::u8string_view type;
};

/* the common web formats; anything else is application/octet-stream */
static constexpr mime_entry mime_table[]{
	{u8"html", u8"text/html"}, {u8"htm", u8"text/html"}, {u8"css", u8"text/css"}, {u8"js", u8"text/javascript"}, {u8"mjs", u8"text/javascript"}, {u8"json", u8"application/json"}, {u8"txt", u8"text/plain"}, {u8"md", u8"text/markdown"}, {u8"xml", u8"text/xml"}, {u8"csv", u8"text/csv"}, {u8"pdf", u8"application/pdf"}, {u8"wasm", u8"application/wasm"}, {u8"png", u8"image/png"}, {u8"jpg", u8"image/jpeg"}, {u8"jpeg", u8"image/jpeg"}, {u8"gif", u8"image/gif"}, {u8"webp", u8"image/webp"}, {u8"svg", u8"image/svg+xml"}, {u8"ico", u8"image/x-icon"}, {u8"bmp", u8"image/bmp"}, {u8"avif", u8"image/avif"}, {u8"mp3", u8"audio/mpeg"}, {u8"wav", u8"audio/wav"}, {u8"ogg", u8"audio/ogg"}, {u8"m4a", u8"audio/mp4"}, {u8"aac", u8"audio/aac"}, {u8"flac", u8"audio/flac"}, {u8"opus", u8"audio/ogg"}, {u8"mp4", u8"video/mp4"}, {u8"m4v", u8"video/mp4"}, {u8"webm", u8"video/webm"}, {u8"mov", u8"video/quicktime"}, {u8"mkv", u8"video/x-matroska"}, {u8"avi", u8"video/x-msvideo"}, {u8"ts", u8"video/mp2t"}, {u8"m2ts", u8"video/mp2t"}, {u8"flv", u8"video/x-flv"}, {u8"vtt", u8"text/vtt"}, {u8"srt", u8"application/x-subrip"}, {u8"ass", u8"text/x-ssa"}, {u8"woff", u8"font/woff"}, {u8"woff2", u8"font/woff2"}, {u8"ttf", u8"font/ttf"}, {u8"otf", u8"font/otf"}, {u8"zip", u8"application/zip"}, {u8"gz", u8"application/gzip"}, {u8"tar", u8"application/x-tar"}, {u8"7z", u8"application/x-7z-compressed"}, {u8"rar", u8"application/vnd.rar"}, {u8"map", u8"application/json"}};

static fi::u8string_view mime_type_of(fi::u8string_view path) noexcept
{
	auto const first{path.data()};
	auto const last{first + path.size()};
	auto const *dot{last};
	for (auto i{last}; i != first;)
	{
		--i;
		if (*i == u8'.')
		{
			dot = i;
			break;
		}
		if (*i == u8'/')
		{
			break;
		}
	}
	if (dot == last)
	{
		return u8"application/octet-stream";
	}
	fi::u8string_view const ext{dot + 1, static_cast<::std::size_t>(last - dot - 1)};
	for (auto const &e : mime_table)
	{
		if (ascii_ieq(ext, e.ext))
		{
			return e.type;
		}
	}
	return u8"application/octet-stream";
}

/* node appends charset=utf-8 to textual types (sniffed) and js/json */
static bool mime_is_text(fi::u8string_view type) noexcept
{
	constexpr fi::u8string_view t{u8"text/"};
	return (type.size() >= t.size() && fi::u8string_view{type.data(), t.size()} == t) ||
		   type == fi::u8string_view{u8"application/json"} ||
		   type == fi::u8string_view{u8"image/svg+xml"};
}

/* --------------------------- http dates --------------------------- */

static constexpr char8_t wkday_names[][4]{{u8"Sun"}, {u8"Mon"}, {u8"Tue"}, {u8"Wed"}, {u8"Thu"}, {u8"Fri"}, {u8"Sat"}};
static constexpr char8_t month_names[][4]{{u8"Jan"}, {u8"Feb"}, {u8"Mar"}, {u8"Apr"}, {u8"May"}, {u8"Jun"}, {u8"Jul"}, {u8"Aug"}, {u8"Sep"}, {u8"Oct"}, {u8"Nov"}, {u8"Dec"}};

/* days since 1970-01-01 — Howard Hinnant's days_from_civil */
static constexpr ::std::int_least64_t days_from_civil(::std::int_least64_t y, unsigned m,
													  unsigned d) noexcept
{
	y -= m <= 2;
	auto const era{(y >= 0 ? y : y - 399) / 400};
	auto const yoe{static_cast<::std::uint_least64_t>(y - era * 400)};
	auto const doy{(153u * (m + (m > 2 ? static_cast<unsigned>(-3) : 9u)) + 2u) / 5u + d - 1u};
	auto const doe{yoe * 365u + yoe / 4u - yoe / 100u + doy};
	return era * 146097 + static_cast<::std::int_least64_t>(doe) - 719468;
}

/* "Sun, 06 Nov 1994 08:49:37 GMT" — a fixed 29 bytes */
static fi::u8string imf_date(::std::int_least64_t unix_seconds) noexcept
{
	auto days{unix_seconds / 86400};
	auto secs{unix_seconds % 86400};
	if (secs < 0)
	{
		secs += 86400;
		--days;
	}
	auto const iso{fi::utc(::fast_io::posix_statx_timestamp64{days * 86400, 0})};
	auto const wday{static_cast<::std::size_t>(((days % 7) + 11) % 7)};
	/* days==0 is Thursday → index 4; +11 keeps the modulo positive */
	fi::u8string out(29, u8' ');
	auto *p{out.data()};
	__builtin_memcpy(p, wkday_names[wday], 3);
	p += 3;
	*p++ = u8',';
	*p++ = u8' ';
	*p++ = static_cast<char8_t>(u8'0' + iso.day / 10);
	*p++ = static_cast<char8_t>(u8'0' + iso.day % 10);
	*p++ = u8' ';
	__builtin_memcpy(p, month_names[iso.month - 1], 3);
	p += 3;
	*p++ = u8' ';
	auto const y{iso.year};
	*p++ = static_cast<char8_t>(u8'0' + (y / 1000) % 10);
	*p++ = static_cast<char8_t>(u8'0' + (y / 100) % 10);
	*p++ = static_cast<char8_t>(u8'0' + (y / 10) % 10);
	*p++ = static_cast<char8_t>(u8'0' + y % 10);
	*p++ = u8' ';
	auto const hh{secs / 3600};
	auto const mm{secs / 60 % 60};
	auto const ss{secs % 60};
	*p++ = static_cast<char8_t>(u8'0' + hh / 10);
	*p++ = static_cast<char8_t>(u8'0' + hh % 10);
	*p++ = u8':';
	*p++ = static_cast<char8_t>(u8'0' + mm / 10);
	*p++ = static_cast<char8_t>(u8'0' + mm % 10);
	*p++ = u8':';
	*p++ = static_cast<char8_t>(u8'0' + ss / 10);
	*p++ = static_cast<char8_t>(u8'0' + ss % 10);
	__builtin_memcpy(p, u8" GMT", 4);
	return out;
}

static constexpr int hex_val(char8_t c) noexcept
{
	if (u8'0' <= c && c <= u8'9')
	{
		return c - u8'0';
	}
	if (u8'a' <= c && c <= u8'f')
	{
		return c - u8'a' + 10;
	}
	if (u8'A' <= c && c <= u8'F')
	{
		return c - u8'A' + 10;
	}
	return -1;
}

/* IMF-fixdate parser; -1 on anything unrecognized — node's Date.parse
 * fails the same way and it simply skips the 304 check */
static ::std::int_least64_t parse_imf_date(fi::u8string_view v) noexcept
{
	if (v.size() != 29 || v.data()[3] != u8',' || v.data()[28] != u8'T' ||
		v.data()[27] != u8'M')
	{
		return -1;
	}
	auto const *p{v.data() + 5}; /* past "<wkday>, " */
	auto dig{[](char8_t c) noexcept -> int {
		return (u8'0' <= c && c <= u8'9') ? c - u8'0' : -1;
	}};
	if (dig(p[0]) < 0 || dig(p[1]) < 0 || p[2] != u8' ')
	{
		return -1;
	}
	unsigned const day{static_cast<unsigned>(dig(p[0]) * 10 + dig(p[1]))};
	p += 3;
	unsigned month{};
	for (; month != 12; ++month)
	{
		if (p[0] == month_names[month][0] && p[1] == month_names[month][1] &&
			p[2] == month_names[month][2])
		{
			break;
		}
	}
	if (month == 12 || p[3] != u8' ')
	{
		return -1;
	}
	++month;
	p += 4;
	if (dig(p[0]) < 0 || dig(p[1]) < 0 || dig(p[2]) < 0 || dig(p[3]) < 0 || p[4] != u8' ')
	{
		return -1;
	}
	auto const year{dig(p[0]) * 1000 + dig(p[1]) * 100 + dig(p[2]) * 10 + dig(p[3])};
	p += 5;
	if (dig(p[0]) < 0 || dig(p[1]) < 0 || p[2] != u8':' || dig(p[3]) < 0 ||
		dig(p[4]) < 0 || p[5] != u8':' || dig(p[6]) < 0 || dig(p[7]) < 0)
	{
		return -1;
	}
	return days_from_civil(year, month, day) * 86400 +
		   (dig(p[0]) * 10 + dig(p[1])) * 3600 + (dig(p[3]) * 10 + dig(p[4])) * 60 +
		   dig(p[6]) * 10 + dig(p[7]);
}

/* --------------------- request-target handling -------------------- */

/*
 * decode_target: percent-decode the request-target, drop the query,
 * fold '\' to '/', then normalize like node's path.normalize: empty and
 * "." segments vanish, ".." pops the previous segment and clamps at the
 * root — a traversal can never produce a path outside root. Returns
 * false on malformed %-escapes or control characters (the analog of
 * node's decodeURIComponent throw → 400). `trailing_slash` reports
 * whether the raw target ended in '/', for the directory redirect.
 */
static bool decode_target(fi::u8string_view target, fi::u8string &out,
						  bool &trailing_slash)
{
	auto const *first{target.data()};
	auto const *last{first + target.size()};
	for (auto i{first}; i != last; ++i)
	{
		if (*i == u8'?' || *i == u8'#')
		{
			last = i;
			break;
		}
	}
	trailing_slash = last != first && last[-1] == u8'/';
	fi::u8string decoded;
	for (auto i{first}; i != last; ++i)
	{
		char8_t c{*i};
		if (c == u8'%')
		{
			if (last - i < 3)
			{
				return false;
			}
			int const hi{hex_val(i[1])}, lo{hex_val(i[2])};
			if (hi < 0 || lo < 0)
			{
				return false;
			}
			c = static_cast<char8_t>((hi << 4) | lo);
			i += 2;
		}
		if (c == u8'\\')
		{
			c = u8'/';
		}
		if (c < 0x20 || c == 0x7f)
		{
			return false;
		}
		decoded.push_back(c);
	}
	/* normalize: out accumulates "seg/seg" with no leading slash; ".."
	 * pops back to the previous separator, clamped when already empty */
	out.clear();
	auto const *dfirst{decoded.data()};
	auto const *dlast{dfirst + decoded.size()};
	for (auto seg{dfirst};;)
	{
		auto e{seg};
		while (e != dlast && *e != u8'/')
		{
			++e;
		}
		fi::u8string_view const piece{seg, static_cast<::std::size_t>(e - seg)};
		if (piece == fi::u8string_view{u8".."})
		{
			if (!out.empty())
			{
				auto n{out.size()};
				while (n != 0 && out.data()[n - 1] != u8'/')
				{
					--n;
				}
				out.assign(fi::u8string_view{out.data(), n == 0 ? 0 : n - 1});
			}
		}
		else if (!piece.empty() && piece != fi::u8string_view{u8"."})
		{
			if (!out.empty())
			{
				out.push_back(u8'/');
			}
			out.append(piece.data(), piece.size());
		}
		if (e == dlast)
		{
			break;
		}
		seg = e + 1;
	}
	return true;
}

/* ------------------------------ model ----------------------------- */

/*
 * The root is an open dir handle, not a path string: every lookup goes
 * through openat/fstatat relative to it. Combined with the normalized
 * rel (no "..", no leading slash) a request physically cannot name a
 * file outside the tree.
 */
struct server_cfg
{
	fi::native_at_entry root{};
	::std::size_t cache_seconds{3600};
	bool autoindex{true};
	bool showdir{true};
};

/* views point into the request's http_header_buffer — alive until the
 * session's next scan */
struct http_request
{
	fi::u8string_view method{};
	fi::u8string_view target{};
	fi::u8string_view version{};
	fi::u8string_view connection{};
	fi::u8string_view range{};
	fi::u8string_view inm{};      /* If-None-Match */
	fi::u8string_view ims{};      /* If-Modified-Since */
	fi::u8string_view if_range{}; /* If-Range — mismatch degrades 206 to 200 */
	bool head_only{};
	bool keep_alive{};
};

struct range_result
{
	::std::uint_least64_t start{};
	::std::uint_least64_t length{};
	bool valid{};
	bool present{};
};

/*
 * Range: bytes=a-b | a- | -n — a single range, like node's parse.
 * Empty or non-"bytes=" input is "not present" (serve the whole file);
 * a malformed range is !valid → 416.
 */
static range_result parse_range(fi::u8string_view rv, ::std::uint_least64_t size) noexcept
{
	constexpr fi::u8string_view prefix{u8"bytes="};
	if (rv.size() <= prefix.size() ||
		fi::u8string_view{rv.data(), prefix.size()} != prefix)
	{
		return {.valid = true};
	}
	rv = fi::u8string_view{rv.data() + prefix.size(), rv.size() - prefix.size()};
	auto const *p{rv.data()};
	auto const *e{p + rv.size()};
	auto const *dash{p};
	while (dash != e && *dash != u8'-')
	{
		++dash;
	}
	if (dash == e || (e - dash - 1 == 0 && dash == p))
	{
		return {.valid = true}; /* "bytes=-" — nothing usable, ignore */
	}
	if (size == 0)
	{
		return {}; /* any range on an empty file is unsatisfiable */
	}
	auto digits{[](char8_t const *f, char8_t const *l, ::std::uint_least64_t &v) noexcept {
		if (f == l)
		{
			return false;
		}
		v = 0;
		for (; f != l; ++f)
		{
			if (*f < u8'0' || u8'9' < *f)
			{
				return false;
			}
			v = v * 10 + static_cast<::std::uint_least64_t>(*f - u8'0');
		}
		return true;
	}};
	::std::uint_least64_t s{}, n{};
	if (dash == p)
	{
		/* suffix form "-n": the last n bytes */
		if (!digits(dash + 1, e, n) || n == 0)
		{
			return {};
		}
		if (n > size)
		{
			n = size;
		}
		return {.start = size - n, .length = n, .valid = true, .present = true};
	}
	if (!digits(p, dash, s) || s >= size)
	{
		return {};
	}
	::std::uint_least64_t endv{size - 1};
	if (dash + 1 != e && !digits(dash + 1, e, endv))
	{
		return {};
	}
	if (endv >= size)
	{
		endv = size - 1;
	}
	if (s > endv)
	{
		return {};
	}
	return {.start = s, .length = endv - s + 1, .valid = true, .present = true};
}

/* --------------------------- responders --------------------------- */

/*
 * Every responder ends with an explicit flush: on the keep-alive path
 * the next scan's tie would flush anyway, but a "Connection: close"
 * response must be on the wire before the socket dies.
 */

static fi::io_async_task<> respond_status(fi::io_async_observer sched,
										  fi::u8iobuf_socket_file &sock, unsigned code,
										  fi::u8string_view phrase, http_request const &req) throws
{
	fi::u8string const body{fi::u8concat_fast_io(
		u8"<!doctype html><title>", code, u8" ", phrase, u8"</title><h1>", code, u8" ",
		phrase, u8"</h1><hr><i>http-server (fast_io)</i>")};
	co_await fi::io::async_print(
		sched, {}, sock, u8"HTTP/1.1 ", code, u8" ", phrase,
		code == 405 ? fi::u8string_view{u8"\r\nAllow: GET, HEAD"} : fi::u8string_view{},
		u8"\r\nContent-Type: text/html\r\nContent-Length: ", body.size(),
		u8"\r\nConnection: ",
		req.keep_alive ? fi::u8string_view{u8"keep-alive"} : fi::u8string_view{u8"close"},
		u8"\r\n\r\n");
	if (!req.head_only)
	{
		co_await fi::io::async_print(sched, {}, sock,
									 fi::u8string_view{body.data(), body.size()});
	}
	co_await fop::async_output_stream_flush(sched, {}, sock);
}

/* showDir: plain href-per-line listing — no icons, no perms columns.
 * dirat is the at-entry of the directory being listed. */
static fi::io_async_task<> respond_listing(fi::io_async_observer sched,
										   fi::u8iobuf_socket_file &sock,
										   fi::u8string_view url_path,
										   fi::native_at_entry dirat, server_cfg const &cfg,
										   http_request const &req) throws
{
	fi::u8string page;
	{
		fi::u8ostring_ref_fast_io w{__builtin_addressof(page)};
		fi::io::print(w, u8"<!doctype html><meta charset=\"utf-8\"><title>Index of ",
					  fi::u8string_view{url_path},
					  u8"</title><h1>Index of ", fi::u8string_view{url_path},
					  u8"</h1><hr><pre>\n");
		try
		{
			for (auto const &ent : fi::current(dirat))
			{
				if (fi::is_dot(ent))
				{
					continue;
				}
				auto const name{fi::u8filename(ent)};
				bool const isdir{fi::type(ent) == fi::file_type::directory};
				fi::io::print(w, u8"<a href=\"");
				for (auto c : fi::u8string_view{name.data(), name.size()})
				{
					if ((u8'a' <= c && c <= u8'z') || (u8'A' <= c && c <= u8'Z') ||
						(u8'0' <= c && c <= u8'9') || c == u8'-' || c == u8'_' ||
						c == u8'.' || c == u8'~')
					{
						fi::io::print(w, ::fast_io::mnp::chvw(c));
					}
					else
					{
						fi::io::print(w, ::fast_io::mnp::chvw(u8'%'),
									  ::fast_io::mnp::hexupper<false, true>(c));
					}
				}
				fi::io::print(w, isdir ? fi::u8string_view{u8"/\">"}
									   : fi::u8string_view{u8"\">"});
				for (auto c : fi::u8string_view{name.data(), name.size()})
				{
					switch (c)
					{
					case u8'&':
						fi::io::print(w, u8"&amp;");
						break;
					case u8'<':
						fi::io::print(w, u8"&lt;");
						break;
					case u8'>':
						fi::io::print(w, u8"&gt;");
						break;
					case u8'"':
						fi::io::print(w, u8"&quot;");
						break;
					default:
						fi::io::print(w, ::fast_io::mnp::chvw(c));
					}
				}
				fi::io::print(w, isdir ? fi::u8string_view{u8"/</a>\n"}
									   : fi::u8string_view{u8"</a>\n"});
			}
		}
		catch throws(::std::error)
		{
		}
		fi::io::print(w, u8"</pre><hr><i>http-server (fast_io)</i>");
	}
	co_await fi::io::async_print(
		sched, {}, sock, u8"HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\n",
		u8"Content-Length: ", page.size(), u8"\r\nCache-Control: max-age=",
		cfg.cache_seconds, u8"\r\nConnection: ",
		req.keep_alive ? fi::u8string_view{u8"keep-alive"} : fi::u8string_view{u8"close"},
		u8"\r\n\r\n");
	if (!req.head_only)
	{
		co_await fi::io::async_print(sched, {}, sock,
									 fi::u8string_view{page.data(), page.size()});
	}
	co_await fop::async_output_stream_flush(sched, {}, sock);
}

/*
 * regular file — the serve() half of node's middleware: conditional
 * headers, a single byte range, HEAD, then the body rides
 * async_transmit_all straight from the file fd to the socket. The
 * stream is templated because the file handle arrives as either a
 * u8native_file or (on win32) a dir-mode handle that turned out to be
 * a regular file; only the transmit call site needs the concrete type.
 */
template <typename stmtype>
static fi::io_async_task<> respond_file(fi::io_async_observer sched,
										fi::u8iobuf_socket_file &sock, stmtype &file,
										fi::posix_file_status const &st,
										http_request const &req, fi::u8string_view reqname,
										server_cfg const &cfg) throws
{
	fi::u8string_view const type{mime_type_of(reqname)};
	/* node etag: "ino-size-mtime"; same shape, unix time for the mtime */
	fi::u8string const etag{fi::u8concat_fast_io(u8"\"", st.ino, u8"-", st.size, u8"-",
												 st.mtim.tv_sec, u8".", st.mtim.tv_nsec, u8"\"")};
	fi::u8string const lastmod{imf_date(st.mtim.tv_sec)};
	fi::u8string_view const connv{
		req.keep_alive ? fi::u8string_view{u8"keep-alive"} : fi::u8string_view{u8"close"}};

	/* If-None-Match: any list member may match, weak prefix allowed */
	bool not_modified{};
	fi::u8string_view const ev{etag.data(), etag.size()};
	{
		auto const *i{req.inm.data()};
		auto const *e{i + req.inm.size()};
		for (; i != e && !not_modified;)
		{
			while (i != e && (*i == u8' ' || *i == u8','))
			{
				++i;
			}
			auto const *j{i};
			while (j != e && *j != u8',')
			{
				++j;
			}
			auto const *k{j};
			while (k != i && k[-1] == u8' ')
			{
				--k;
			}
			fi::u8string_view cand{i, static_cast<::std::size_t>(k - i)};
			if (cand.size() >= 2 && cand.data()[0] == u8'W' && cand.data()[1] == u8'/')
			{
				cand = fi::u8string_view{cand.data() + 2, cand.size() - 2};
			}
			not_modified = cand == ev || cand == fi::u8string_view{u8"*"};
			i = j;
		}
	}
	if (!not_modified && !req.ims.empty())
	{
		auto const since{parse_imf_date(req.ims)};
		not_modified = since >= 0 && since >= st.mtim.tv_sec;
	}
	if (not_modified)
	{
		co_await fi::io::async_print(sched, {}, sock,
									 u8"HTTP/1.1 304 Not Modified\r\nETag: ", ev,
									 u8"\r\nConnection: ", connv, u8"\r\n\r\n");
		co_await fop::async_output_stream_flush(sched, {}, sock);
		co_return;
	}

	auto rng{parse_range(req.range, st.size)};
	/*
	 * If-Range: a validator that doesn't match downgrades the 206 to a
	 * full 200 — video clients send it so a changed file isn't resumed
	 * mid-byte. ETag form wins; otherwise it's an HTTP date.
	 */
	if (rng.present && !req.if_range.empty())
	{
		fi::u8string_view iv{req.if_range};
		bool match;
		if (!iv.empty() && iv.data()[0] == u8'"')
		{
			match = iv == ev;
		}
		else if (iv.size() >= 2 && iv.data()[0] == u8'W' && iv.data()[1] == u8'/')
		{
			match = fi::u8string_view{iv.data() + 2, iv.size() - 2} == ev;
		}
		else
		{
			auto const since{parse_imf_date(iv)};
			match = since >= 0 && since >= st.mtim.tv_sec;
		}
		if (!match)
		{
			rng.present = false;
		}
	}
	if (!rng.valid)
	{
		co_await fi::io::async_print(
			sched, {}, sock,
			u8"HTTP/1.1 416 Range Not Satisfiable\r\nContent-Range: bytes */", st.size,
			u8"\r\nContent-Length: 0\r\nConnection: ", connv, u8"\r\n\r\n");
		co_await fop::async_output_stream_flush(sched, {}, sock);
		co_return;
	}
	auto const length{rng.present ? rng.length : st.size};

	co_await fi::io::async_print(
		sched, {}, sock,
		rng.present ? fi::u8string_view{u8"HTTP/1.1 206 Partial Content\r\n"}
					: fi::u8string_view{u8"HTTP/1.1 200 OK\r\n"},
		u8"Content-Type: ", type,
		mime_is_text(type) ? fi::u8string_view{u8"; charset=utf-8"} : fi::u8string_view{},
		u8"\r\nAccept-Ranges: bytes\r\nCache-Control: max-age=", cfg.cache_seconds,
		u8"\r\nLast-Modified: ", fi::u8string_view{lastmod.data(), lastmod.size()},
		u8"\r\nETag: ", ev,
		rng.present ? fi::u8concat_fast_io(u8"\r\nContent-Range: bytes ", rng.start, u8"-",
										   rng.start + length - 1, u8"/", st.size)
					: fi::u8string{},
		u8"\r\nContent-Length: ", length, u8"\r\nConnection: ", connv, u8"\r\n\r\n");
	if (!req.head_only && length != 0)
	{
		::fast_io::intfpos_t const off{static_cast<::fast_io::intfpos_t>(rng.start)};
		co_await fop::async_transmit_all_bytes(sched, {}, sock, {}, file,
											   ::fast_io::intfpos_opt{off, true},
											   fi::size_t_opt{length});
	}
	co_await fop::async_output_stream_flush(sched, {}, sock);
}

/* directory: trailing-slash redirect, index.html, then the listing.
 * dirat is the directory's own at-entry — index.html is opened relative
 * to it, and the listing iterates it. */
static fi::io_async_task<> respond_directory(fi::io_async_observer sched,
											 fi::u8iobuf_socket_file &sock,
											 http_request const &req,
											 fi::u8string_view url_path,
											 fi::native_at_entry dirat, server_cfg const &cfg,
											 bool trailing_slash) throws
{
	if (!trailing_slash)
	{
		/* node: Location = pathname + '/' (+ '?query') — url_path is the
		 * raw target, which still carries the query in the right spot */
		co_await fi::io::async_print(sched, {}, sock,
									 u8"HTTP/1.1 302 Found\r\nLocation: ", url_path,
									 u8"/\r\nContent-Length: 0\r\nConnection: ",
									 req.keep_alive ? fi::u8string_view{u8"keep-alive"}
													: fi::u8string_view{u8"close"},
									 u8"\r\n\r\n");
		co_await fop::async_output_stream_flush(sched, {}, sock);
		co_return;
	}
	if (cfg.autoindex)
	{
		try
		{
			fi::u8native_file idxf{dirat, fi::u8cstring_view{u8"index.html"},
								   fi::open_mode::in};
			auto const st{fi::status(idxf)};
			if (st.type == fi::file_type::regular)
			{
				co_await respond_file(sched, sock, idxf, st, req,
									  fi::u8string_view{u8"index.html"}, cfg);
				co_return;
			}
		}
		catch throws(::std::error)
		{
		}
	}
	if (cfg.showdir)
	{
		co_await respond_listing(sched, sock, url_path, dirat, cfg, req);
		co_return;
	}
	co_await respond_status(sched, sock, 404, u8"Not Found", req);
}

/* ------------------------------- core ----------------------------- */

/*
 * One detached coroutine per connection: scan the request head, resolve
 * the path under root, respond, loop for keep-alive. Errors — client
 * disconnect, malformed headers, write failures — all throw into the
 * detached frame and destroy it; there is nobody to report to.
 */
static fi::io_async_task<> session(fi::io_async_observer sched,
								   fi::u8iobuf_socket_file sock, server_cfg cfg) throws
{
	for (;;)
	{
		fi::u8http_header_buffer hdr{};
		co_await fi::io::async_scan(sched, {}, sock, hdr);

		http_request req;
		auto const m{hdr.request()};
		auto const c{hdr.code()};
		auto const r{hdr.reason()};
		req.method = fi::u8string_view{m.data(), m.size()};
		req.target = fi::u8string_view{c.data(), c.size()};
		req.version = fi::u8string_view{r.data(), r.size()};
		req.head_only = req.method == fi::u8string_view{u8"HEAD"};
		bool const get{req.method == fi::u8string_view{u8"GET"}};
		for (auto [key, value] : line_generator(hdr))
		{
			fi::u8string_view const k{key.data(), key.size()};
			fi::u8string_view const v{value.data(), value.size()};
			if (ascii_ieq(k, u8"connection"))
			{
				req.connection = v;
			}
			else if (ascii_ieq(k, u8"range"))
			{
				req.range = v;
			}
			else if (ascii_ieq(k, u8"if-none-match"))
			{
				req.inm = v;
			}
			else if (ascii_ieq(k, u8"if-modified-since"))
			{
				req.ims = v;
			}
			else if (ascii_ieq(k, u8"if-range"))
			{
				req.if_range = v;
			}
		}
		req.keep_alive = req.version == fi::u8string_view{u8"HTTP/1.0"}
							 ? ascii_ieq(req.connection, u8"keep-alive")
							 : !ascii_ieq(req.connection, u8"close");

		if (!get && !req.head_only)
		{
			co_await respond_status(sched, sock, 405, u8"Method Not Allowed", req);
		}
		else
		{
			fi::u8string rel;
			bool trailing_slash{};
			if (!decode_target(req.target, rel, trailing_slash))
			{
				co_await respond_status(sched, sock, 400, u8"Bad Request", req);
			}
			else if (rel.empty())
			{
				/* "/" — the root handle itself is the directory */
				co_await respond_directory(sched, sock, req, req.target, cfg.root, cfg,
										   trailing_slash);
			}
			else
			{
				bool failed{};
				try
				{
					auto const relc{fi::mnp::os_c_str(rel.c_str())};
					/* one fstatat decides the branch; symlink_nofollow
					 * would diverge from node's fs.stat, so flags stay 0 */
					auto const st{fi::native_fstatat(cfg.root, relc, fi::posix_at_flags{})};
					if (st.type == fi::file_type::directory)
					{
						fi::u8native_file df{cfg.root, relc,
											 fi::open_mode::in | fi::open_mode::directory};
						co_await respond_directory(sched, sock, req, req.target,
												   fi::at(df), cfg, trailing_slash);
					}
					else if (st.type == fi::file_type::regular)
					{
						fi::u8native_file f{cfg.root, relc, fi::open_mode::in};
						co_await respond_file(
							sched, sock, f, st, req,
							fi::u8string_view{rel.data(), rel.size()}, cfg);
					}
					else
					{
						co_await respond_status(sched, sock, 403, u8"Forbidden", req);
					}
				}
				catch throws(::std::error)
				{
					failed = true;
				}
				if (failed)
				{
					/* ENOENT/ENOTDIR/EACCES all collapse to 404 — node's
					 * effectively404 counts unreadable too. (co_await
					 * can't live inside the catch itself) */
					co_await respond_status(sched, sock, 404, u8"Not Found", req);
				}
			}
		}
		if (!req.keep_alive)
		{
			co_return;
		}
	}
}

static fi::io_async_task<> accept_loop(fi::io_async_observer sched,
									   fi::u8native_io_observer listener,
									   server_cfg cfg) throws
{
	for (;;)
	{
		session(sched,
				fi::u8iobuf_socket_file{co_await fop::async_accept(
					sched, {}, listener, fi::open_mode::no_block)},
				cfg)
			.detach();
	}
}

int main(int argc, char const **argv)
{
	using namespace fi::io;
	if (argc == 0)
	{
		return 1;
	}
	server_cfg cfg;
	::std::uint_least16_t port{8080};
	fi::u8cstring_view rootarg;
	fi::u8string rootpath;
	for (int i{1}; i != argc; ++i)
	{
		fi::u8cstring_view const arg{fi::mnp::os_c_str(
			reinterpret_cast<char8_t const *>(argv[i]))};
		if (arg == fi::u8cstring_view{u8"-p"} || arg == fi::u8cstring_view{u8"--port"})
		{
			if (++i == argc)
			{
				break;
			}
			port = static_cast<::std::uint_least16_t>(fi::u8to<unsigned>(
				fi::u8cstring_view{fi::mnp::os_c_str(
					reinterpret_cast<char8_t const *>(argv[i]))}));
		}
		else if (arg == fi::u8cstring_view{u8"-c"} || arg == fi::u8cstring_view{u8"--cache"})
		{
			if (++i == argc)
			{
				break;
			}
			cfg.cache_seconds = fi::u8to<::std::size_t>(
				fi::u8cstring_view{fi::mnp::os_c_str(
					reinterpret_cast<char8_t const *>(argv[i]))});
		}
		else if (arg == fi::u8cstring_view{u8"--no-index"})
		{
			cfg.autoindex = false;
		}
		else if (arg == fi::u8cstring_view{u8"--no-dir"})
		{
			cfg.showdir = false;
		}
		else if (rootarg.is_empty())
		{
			rootarg = arg;
		}
	}
	try
	{
		if (rootarg.is_empty())
		{
			/* node defaults to ./public when it exists, else . */
			try
			{
				fi::u8native_file pub{u8"./public", fi::open_mode::in |
														fi::open_mode::directory};
				rootpath = u8"./public";
			}
			catch throws(::std::error)
			{
				rootpath = u8".";
			}
		}
		else
		{
			rootpath.assign(rootarg);
		}
		/* the root is held as an open dir handle for the process's
		 * lifetime — every request resolves relative to it */
		fi::u8native_file rootdir{fi::mnp::os_c_str(rootpath.c_str()),
								  fi::open_mode::in | fi::open_mode::directory};
		cfg.root = fi::at(rootdir);

		fi::net_service service;
		fi::io_async_scheduler scheduler{fi::io_async};

		/* the bind happens once, outside the supervisor — a listen()
		 * failure (EADDRINUSE etc.) is fatal, not retryable: rebuilding
		 * the listener in a loop would spin on a permanent error */
		fi::u8native_socket_file listener{fi::tcp_listen(port, fi::open_mode::no_block)};
		println("Serving ", ::fast_io::mnp::code_cvt(fi::u8string_view{rootpath.data(), rootpath.size()}),
				" on http://0.0.0.0:", port);

		/* supervisor: if the accept coroutine dies to a mid-run error
		 * (EMFILE & co.), log and rebuild it on the same listener */
		for (;;)
		{
			try
			{
				auto acceptor{accept_loop(scheduler, listener, cfg)};
				acceptor.resume();
				while (!acceptor.done())
				{
					fi::io_async_wait(scheduler);
				}
				acceptor.rethrow_if_error();
			}
			catch throws(::std::error e)
			{
				perrln(e);
			}
		}
	}
	catch throws(::std::error e)
	{
		perrln(e);
		return 1;
	}
	perr("Usage: ", ::fast_io::mnp::os_c_str(*argv),
		 " [root] [-p port] [-c seconds] [--no-index] [--no-dir]\n");
	return 1;
}
