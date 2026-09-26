#pragma once

namespace fast_io::curve25519
{

struct field_number
{
	using value_type = std::uint_least64_t;
	using size_type = ::std::size_t;
	static inline constexpr size_type array_size{4};
	static inline constexpr size_type array_size_bytes{array_size * sizeof(value_type)};
	value_type content[array_size];
	static inline constexpr bool is_empty() noexcept
	{
		return false;
	}
	static inline constexpr size_type size() noexcept
	{
		return array_size;
	}
	static inline constexpr size_type size_bytes() noexcept
	{
		return array_size_bytes;
	}
	static inline constexpr size_type max_size() noexcept
	{
		return array_size;
	}
	static inline constexpr size_type max_size_bytes() noexcept
	{
		return array_size_bytes;
	}
	inline constexpr value_type const &front() const noexcept
	{
		return *content;
	}
	inline constexpr value_type &front() noexcept
	{
		return *content;
	}
	inline constexpr value_type const &back() const noexcept
	{
		constexpr size_type nm1{array_size - 1u};
		return content[nm1];
	}
	inline constexpr value_type &back() noexcept
	{
		constexpr size_type nm1{array_size - 1u};
		return content[nm1];
	}
	inline constexpr value_type const &front_unchecked() const noexcept
	{
		return *content;
	}
	inline constexpr value_type &front_unchecked() noexcept
	{
		return *content;
	}
	inline constexpr value_type const &back_unchecked() const noexcept
	{
		constexpr size_type nm1{array_size - 1u};
		return content[nm1];
	}
	inline constexpr value_type &back_unchecked() noexcept
	{
		constexpr size_type nm1{array_size - 1u};
		return content[nm1];
	}
	inline constexpr value_type const &operator[](size_type pos) const noexcept
	{
		if (array_size <= pos) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		return content[pos];
	}
	inline constexpr value_type &operator[](size_type pos) noexcept
	{
		if (array_size <= pos) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		return content[pos];
	}
	inline constexpr value_type const &index_unchecked(size_type pos) const noexcept
	{
		return content[pos];
	}
	inline constexpr value_type &index_unchecked(size_type pos) noexcept
	{
		return content[pos];
	}
	inline constexpr value_type const *data() const noexcept
	{
		return content;
	}
	inline constexpr value_type *data() noexcept
	{
		return content;
	}
};

static_assert(::std::is_trivially_copyable_v<field_number>);

} // namespace fast_io::curve25519
