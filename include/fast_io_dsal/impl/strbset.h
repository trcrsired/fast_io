#pragma once

namespace fast_io::containers
{

namespace details
{

template<::integral chtype, ::std::size_t keys_number>
struct btree_strset_node
{
	using char_type = chtype;
	::std::size_t size;
	::fast_io::details::associative_string<char_type> keys[keys];
	btree_strset_node<char_type> *childrens[keys_number+1u];
	btree_strset_node<char_type> *parent;
};

struct find_btree_node_insert_position_result
{
	::std::size_t pos{};
	bool found{};
};

template<typename nodetype>
inline constexpr find_btree_insert_position_result find_btree_node_insert_position(nodetype* node,
	typename nodetype::char_type const* keystrptr,::std::size_t keystrn)
{
	using char_type = typename nodetype::char_type;
	auto *b{node->keys}, *i{b},*e{b+node->size};
	bool found{};
	::fast_io::basic_string_view<char_type> newkey(keystrptr,keystrn);
	for (;i != e; ++i)
	{
		auto cmpres{newkey<=>i->strvw()};
		if(cmpres<=0)
		{
			found=(cmpres==0);
			break;
		}
	}
	return {static_cast<::std::size_t>(i-b), found};
}

template<typename nodetype>
inline constexpr bool insert_key(nodetype* node, 
	typename nodetype::char_type const* keystrptr,::std::size_t keystrn)
{
	
}

}

template<::std::integral chtype, typename T, bool ranked, typename Allocator, ::std::size_t keys_number = 63>
class basic_btree_strset
{
public:
	using char_type = chtype;
	using string_view_type = ::fast_io::basic_string_view<char_type>;
	using cstring_view_type = ::fast_io::basic_cstring_view<char_type>;
	using value_type = T;
	using allocator_type = Allocator;

	explicit constexpr basic_btree_strset(::std::initializer_list<string_view_type> ilist) noexcept
	{
		
	}

	constexpr bool contains(string_view_type) const noexcept
	{
	}
	constexpr bool is_empty() const noexcept
	{
	}
	constexpr bool erase_key(string_view_type) noexcept
	{
	}
	constexpr bool insert_key(string_view_type) noexcept
	{
	}
	constexpr void clear() noexcept
	{
	}
	constexpr void clear_destroy() noexcept
	{
		this->clear();
	}
};

}