#pragma once

namespace fast_io::operations::decay::defines
{

template <typename T>
concept input_stream_operations_nothrow = requires() {
	{ T::input_char_type } -> ::std::integral;
	{ T::input_nothrow_tag } -> ::std::same_as<::fast_io::io_nothrow_tag>;
};

}

namespace fast_io::operations::defines
{

template <typename T>
concept input_stream_operations_nothrow = ::fast_io::operations::decay::defines::input_stream_operations_nothrow<decltype(::fast_io::operations::input_stream_ref(::std::declval<T>()))>;

}
