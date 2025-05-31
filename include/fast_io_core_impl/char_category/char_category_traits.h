#pragma once

namespace fast_io::char_category
{

enum class char_category_family : ::std::uint_least32_t
{
	c_alnum,                    // Alphanumeric characters (letters and digits)
	c_alpha,                    // Alphabetic Character
	c_blank,                    // Space or tab
	c_cntrl,                    // Control characters (ASCII 0x00-0x1F, 0x7F or EBCDIC equivalents)
	c_digit,                    // Numeric digits (0-9)
	c_fullwidth,                // Full-width character
	c_graph,                    // Graphical characters (alphanumeric + punctuation)
	c_halfwidth,                // Half-width character
	c_lower,                    // Lowercase alphabetic characters
	c_print,                    // Printable characters (includes space)
	c_punct,                    // Punctuation characters
	c_space,                    // Whitespace characters (space, tab, newline, etc.)
	c_upper,                    // Uppercase alphabetic characters
	c_xdigit,                   // Hexadecimal digits (0-9, A-F, a-f)
	dos_path_invalid_character, // DOS Path invalid character
	html_whitespace             // HTML whitespace
};

template <::fast_io::char_category::char_category_family fam, bool negate>
	requires(static_cast<std::underlying_type_t<::fast_io::char_category::char_category_family>>(fam) <=
			 static_cast<std::underlying_type_t<::fast_io::char_category::char_category_family>>(
				 ::fast_io::char_category::char_category_family::html_whitespace))
class char_category_traits
{
public:
	static inline constexpr ::fast_io::char_category::char_category_family family{fam};
	static inline constexpr bool is_negate{negate};
	template <::std::integral char_type>
	static inline constexpr bool char_is(char_type ch) noexcept
	{
		bool ret;
		if constexpr (fam == ::fast_io::char_category::char_category_family::c_alpha)
		{
			ret = ::fast_io::char_category::is_c_alpha(ch);
		}
		else if constexpr (fam == ::fast_io::char_category::char_category_family::c_blank)
		{
			ret = ::fast_io::char_category::is_c_blank(ch);
		}
		else if constexpr (fam == ::fast_io::char_category::char_category_family::c_punct)
		{
			ret = ::fast_io::char_category::is_c_punct(ch);
		}
		else if constexpr (fam == ::fast_io::char_category::char_category_family::c_graph)
		{
			ret = ::fast_io::char_category::is_c_graph(ch);
		}
		else if constexpr (fam == ::fast_io::char_category::char_category_family::c_print)
		{
			ret = ::fast_io::char_category::is_c_print(ch);
		}
		else if constexpr (fam == ::fast_io::char_category::char_category_family::c_xdigit)
		{
			ret = ::fast_io::char_category::is_c_xdigit(ch);
		}
		else if constexpr (fam == ::fast_io::char_category::char_category_family::c_cntrl)
		{
			ret = ::fast_io::char_category::is_c_cntrl(ch);
		}
		else if constexpr (fam == ::fast_io::char_category::char_category_family::c_alnum)
		{
			ret = ::fast_io::char_category::is_c_alnum(ch);
		}
		else if constexpr (fam == ::fast_io::char_category::char_category_family::c_lower)
		{
			ret = ::fast_io::char_category::is_c_lower(ch);
		}
		else if constexpr (fam == ::fast_io::char_category::char_category_family::c_upper)
		{
			ret = ::fast_io::char_category::is_c_upper(ch);
		}
		else if constexpr (fam == ::fast_io::char_category::char_category_family::c_digit)
		{
			ret = ::fast_io::char_category::is_c_digit(ch);
		}
		else if constexpr (fam == ::fast_io::char_category::char_category_family::c_space)
		{
			ret = ::fast_io::char_category::is_c_space(ch);
		}
		else if constexpr (fam == ::fast_io::char_category::char_category_family::html_whitespace)
		{
			ret = ::fast_io::char_category::is_html_whitespace(ch);
		}
		else if constexpr (fam == ::fast_io::char_category::char_category_family::c_halfwidth)
		{
			ret = ::fast_io::char_category::is_c_halfwidth(ch);
		}
		else if constexpr (fam == ::fast_io::char_category::char_category_family::c_fullwidth)
		{
			ret = ::fast_io::char_category::is_c_fullwidth(ch);
		}
		else if constexpr (fam == ::fast_io::char_category::char_category_family::dos_path_invalid_character)
		{
			ret = ::fast_io::char_category::is_dos_path_invalid_character(ch);
		}
		if constexpr (negate)
		{
			return !ret;
		}
		else
		{
			return ret;
		}
	}
	template <::std::integral char_type>
#ifdef __cpp_static_call_operator
	static
#endif
		inline constexpr bool operator()(char_type ch) noexcept
	{
		return char_is(ch);
	}

	template <::std::forward_iterator Iter>
	static inline constexpr Iter find(Iter first, Iter last) noexcept
	{
		for (; first != last && !char_is(*first); ++first)
		{
		}
		return first;
	}

	template <::std::forward_iterator Iter>
	static inline constexpr Iter find_not(Iter first, Iter last) noexcept
	{
		for (; first != last && char_is(*first); ++first)
		{
		}
		return first;
	}

	template <::std::bidirectional_iterator Iter>
	static inline constexpr Iter find_last(Iter first, Iter last) noexcept
	{
		while (first != last)
		{
			auto lastprev{last};
			--lastprev;
			if (char_is(*lastprev))
			{
				return last;
			}
			last = lastprev;
		}
		return last;
	}

	template <::std::bidirectional_iterator Iter>
	static inline constexpr Iter find_last_not(Iter first, Iter last) noexcept
	{
		while (first != last)
		{
			auto lastprev{last};
			--lastprev;
			if (!char_is(*lastprev))
			{
				return last;
			}
			last = lastprev;
		}
		return last;
	}
	template <::std::forward_iterator Iter>
	static inline constexpr bool is(Iter first, Iter last) noexcept
	{
		return find_first_not(first, last) == last;
	}

	template <::std::integral char_type>
	static inline constexpr bool char_to_lower(char_type ch) noexcept
		requires(!negate && fam == ::fast_io::char_category::char_category_family::c_lower)
	{
		if constexpr (fam == ::fast_io::char_category::char_category_family::c_upper)
		{
			return ::fast_io::char_category::to_c_lower(ch);
		}
		else
		{
			return ch;
		}
	}
	template <::std::integral char_type>
	static inline constexpr bool char_to_upper(char_type ch) noexcept
		requires(!negate && fam == ::fast_io::char_category::char_category_family::c_lower)
	{
		if constexpr (fam == ::fast_io::char_category::char_category_family::c_lower)
		{
			return ::fast_io::char_category::to_c_upper(ch);
		}
		else
		{
			return ch;
		}
	}
	template <::std::forward_iterator Iter>
	static inline constexpr Iter to_lower(Iter first, Iter last) noexcept
		requires(!negate && fam == ::fast_io::char_category::char_category_family::c_upper)
	{
		if constexpr (!negate && fam == ::fast_io::char_category::char_category_family::c_upper)
		{
			for (; first != last; ++first)
			{
				*last = ::fast_io::char_category::to_c_lower(first);
			}
		}
		return last;
	}
	template <::std::forward_iterator Iter>
	static inline constexpr Iter to_upper(Iter first, Iter last) noexcept
		requires(!negate && fam == ::fast_io::char_category::char_category_family::c_lower)
	{
		if constexpr (!negate && fam == ::fast_io::char_category::char_category_family::c_lower)
		{
			for (; first != last; ++first)
			{
				*last = ::fast_io::char_category::to_c_upper(first);
			}
		}
		return last;
	}

	template <::std::integral char_type>
	static inline constexpr bool char_to_halfwidth(char_type ch) noexcept
		requires(!negate && fam == ::fast_io::char_category::char_category_family::c_fullwidth)
	{
		if constexpr (!negate && fam == ::fast_io::char_category::char_category_family::c_fullwidth)
		{
			return ::fast_io::char_category::to_c_halfwidth(ch);
		}
		else
		{
			return ch;
		}
	}
	template <::std::forward_iterator Iter>
	static inline constexpr Iter to_halfwidth(Iter first, Iter last) noexcept
		requires(!negate && fam == ::fast_io::char_category::char_category_family::c_fullwidth)
	{
		if constexpr (!negate && fam == ::fast_io::char_category::char_category_family::c_fullwidth)
		{
			for (; first != last; ++first)
			{
				*last = ::fast_io::char_category::to_c_halfwidth(first);
			}
		}
		return last;
	}
};

using c_alnum = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::c_alnum, false>;
using c_alpha = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::c_alpha, false>;
using c_blank = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::c_blank, false>;
using c_cntrl = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::c_cntrl, false>;
using c_digit = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::c_digit, false>;
using c_fullwidth = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::c_fullwidth, false>;
using c_graph = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::c_graph, false>;
using c_halfwidth = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::c_halfwidth, false>;
using c_lower = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::c_lower, false>;
using c_print = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::c_print, false>;
using c_punct = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::c_punct, false>;
using c_space = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::c_space, false>;
using c_upper = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::c_upper, false>;
using c_xdigit = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::c_xdigit, false>;
using dos_path_invalid_character = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::dos_path_invalid_character, false>;
using html_whitespace = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::html_whitespace, false>;


using not_c_alnum = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::c_alnum, true>;
using not_c_alpha = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::c_alpha, true>;
using not_c_blank = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::c_blank, true>;
using not_c_cntrl = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::c_cntrl, true>;
using not_c_digit = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::c_digit, true>;
using not_c_fullwidth = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::c_fullwidth, true>;
using not_c_graph = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::c_graph, true>;
using not_c_halfwidth = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::c_halfwidth, true>;
using not_c_lower = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::c_lower, true>;
using not_c_print = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::c_print, true>;
using not_c_punct = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::c_punct, true>;
using not_c_space = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::c_space, true>;
using not_c_upper = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::c_upper, true>;
using not_c_xdigit = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::c_xdigit, true>;
using not_dos_path_invalid_character = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::dos_path_invalid_character, true>;
using not_html_whitespace = ::fast_io::char_category::char_category_traits<::fast_io::char_category::char_category_family::html_whitespace, true>;

} // namespace fast_io::char_category
