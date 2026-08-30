#pragma once

namespace fast_io
{

namespace operations::decay::defines
{

template <typename stmtype>
inline constexpr bool has_any_of_read_operations_herbceptions_throws =
#if defined(__HERBCEPTIONS__)
	__is_herbceptions_throws_invocable(::fast_io::operations::decay::read_all_decay, stmtype, nullptr, nullptr) ||
	__is_herbceptions_throws_invocable(::fast_io::operations::decay::read_some_decay, stmtype, nullptr, nullptr) ||
	__is_herbceptions_throws_invocable(::fast_io::operations::decay::read_all_bytes_decay, stmtype, nullptr, nullptr) ||
	__is_herbceptions_throws_invocable(::fast_io::operations::decay::read_some_bytes_decay, stmtype, nullptr, nullptr) ||
	__is_herbceptions_throws_invocable(::fast_io::operations::decay::pread_all_decay, stmtype, nullptr, nullptr, ::std::int_least64_t{}) ||
	__is_herbceptions_throws_invocable(::fast_io::operations::decay::pread_some_decay, stmtype, nullptr, nullptr, ::std::int_least64_t{}) ||
	__is_herbceptions_throws_invocable(::fast_io::operations::decay::scatter_read_all_decay, stmtype, static_cast<::fast_io::basic_io_scatter_t<typename stmtype::input_char_type> const*>(nullptr), ::std::size_t{}) ||
	__is_herbceptions_throws_invocable(::fast_io::operations::decay::scatter_read_some_decay, stmtype, static_cast<::fast_io::basic_io_scatter_t<typename stmtype::input_char_type> const*>(nullptr), ::std::size_t{});
#else
	false;
#endif

} // namespace operations::decay::defines

} // namespace fast_io
