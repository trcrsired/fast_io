#pragma once

namespace fast_io::details
{
/*
The string is null terminated
*/
template<::integral chtype>
struct associative_string
{
    using char_type = chtype;
    using size_type = ::std::size_t;
    char_type* ptr;
    size_type n;
    constexpr ::fast_io::basic_string_view<char_type> strvw() const noexcept
    {
        return ::fast_io::basic_string_view<char_type>(ptr,n);
    }
};
}
