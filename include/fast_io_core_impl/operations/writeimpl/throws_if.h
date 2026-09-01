#pragma once

namespace fast_io::operations::decay::defines
{

template <typename T>
concept output_stream_operations_nothrow = requires() {
	{ T::output_char_type } -> ::std::integral;
	{ T::output_nothrow_tag } -> ::std::same_as<::fast_io::io_nothrow_tag>;
};

}

namespace fast_io::operations::defines
{

template <typename T>
concept output_stream_operations_nothrow = ::fast_io::operations::decay::defines::output_stream_operations_nothrow<decltype(::fast_io::operations::output_stream_ref(::std::declval<T>()))>;

}
