#pragma once

namespace fast_io
{

//black_hole is a helper class which helps you remove device requirement for iobuf or iotransform.
//sha256 for example. You do not need a real device. You want to send all your bits to black hole

template<::std::integral ch_type>
struct basic_black_hole
{
	using char_type = ch_type;
	using input_char_type = char_type;
	using output_char_type = char_type;
};

template<::std::integral ch_type>
inline void read_all_underflow_define(basic_black_hole<ch_type> iob, ch_type* first, ch_type* last) {}

template<::std::integral ch_type>
inline void write_all_overflow_define(basic_black_hole<ch_type> iob, ch_type const* first, ch_type const* last) {}

template<::std::integral ch_type>
inline void read_all_bytes_underflow_define(basic_black_hole<ch_type> iob, ::std::byte* first, ::std::byte* last) {}

template<::std::integral ch_type>
inline void write_all_bytes_overflow_define(basic_black_hole<ch_type> iob, ::std::byte const* first, ::std::byte const* last) {}

template<::std::integral ch_type>
inline constexpr basic_black_hole<ch_type> io_stream_ref_define(basic_black_hole<ch_type> other) noexcept
{
	return other;
}

template<std::integral ch_type>
inline constexpr basic_black_hole<ch_type> io_bytes_stream_ref_define(basic_black_hole<ch_type> other) noexcept
{
	return other;
}

}