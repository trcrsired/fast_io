#pragma once

namespace fast_io::containers
{

namespace details
{

template <::std::integral chtype, ::std::size_t keys_number>
struct str_btreeset_node
{
	using char_type = chtype;
	::std::size_t size;                                                  // Number of keys currently stored
	bool leaf;                                                           // Indicates whether the node is a leaf
	::fast_io::details::associative_string<char_type> keys[keys_number]; // Array of keys
	str_btreeset_node<char_type> *childrens[keys_number + 1u];           // Array of child pointers
	str_btreeset_node<char_type> *parent;                                // Pointer to parent node
};


struct find_btree_node_insert_position_result
{
	::std::size_t pos{};
	bool found{};
};

template <typename nodetype>
inline constexpr find_btree_insert_position_result find_str_btree_node_insert_position(nodetype *node,
																					   typename nodetype::char_type const *keystrptr, ::std::size_t keystrn) noexcept
{
	using char_type = typename nodetype::char_type;
	auto *b{node->keys}, *i{b}, *e{b + node->size};
	bool found{};
	::fast_io::basic_string_view<char_type> newkey(keystrptr, keystrn);
	for (; i != e; ++i)
	{
		auto cmpres{newkey <=> i->strvw()};
		if (cmpres <= 0)
		{
			found = (cmpres == 0);
			break;
		}
	}
	return {static_cast<::std::size_t>(i - b), found};
}

template<typename allocator_type, ::std::size_t keys_number, typename nodetype>
inline constexpr void str_btree_split_node(nodetype* node) noexcept;

template <typename allocator_type, ::std::size_t keys_number, typename nodetype>
inline constexpr bool str_btree_insert_key(nodetype *node,
										   typename nodetype::char_type const *keystrptr, ::std::size_t keystrn, bool ismoved) noexcept
{
	using char_type = typename nodetype::char_type;
    // **Find the correct position for insertion**
    auto [pos, found] = find_str_btree_node_insert_position<::fast_io::containers::details::find_str_btree_node_insert_position>(node, keystrptr, keystrn);

    // **If the key already exists, return false (no duplicate keys)**
    if (found)
        return false;

    // **If the node is a leaf**
    if (node->leaf)
    {
        // **If there is space, insert the key directly**
        if (node->size < keys_number)
        {
            for (::std::size_t j = node->size; pos < j ; --j)
			{
                node->keys[j] = node->keys[j - 1];
			}
			::fast_io::details::associative_string<char_type> astr;
			if (ismoved)
			{
				astr.ptr = keystrptr;
				astr.n = keystrn;
			}
			else
			{
				astr = ::fast_io::details::create_associative_string<allocator_type, char_type>{keystrptr, keystrn};
			}
            node->keys[pos] = astr;
            ++node->size;
            return true;
        }
        else
        {
            // **If the node is full, split it**
            str_btree_split_node<allocator_type, keys_number>(node);
            return str_btree_insert_key<allocator_type, keys_number>(node->parent, keystrptr, keystrn, ismoved);
        }
    }
    else
    {
        // **Recursively insert into the correct child**
        return str_btree_insert_key<allocator_type, keys_number>(node->childrens[pos], keystrptr, keystrn, ismoved);
    }
}

template<typename allocator_type, ::std::size_t keys_number, typename nodetype>
inline constexpr void str_btree_split_node(nodetype* node) noexcept
{
    using char_type = typename nodetype::char_type;
    using typed_allocator_type = ::fast_io::typed_generic_allocator_adapter<allocator_type, nodetype>;
	constexpr
		::std::size_t keys_number_div2{keys_number>>1u};
	constexpr
		::std::size_t keys_number_p1{keys_number+1u};
	constexpr
	    ::std::size_t mid{keys_number_div2};
	constexpr
	    ::std::size_t midp1{mid + 1u};
	constexpr
		::std::size_t keys_number_sub_midp1{keys_number - midp1};

    // **Allocate a new node using the stateless allocator**
    nodetype* new_node{typed_allocator_type::allocate(1)};

    new_node->parent = node->parent;
    new_node->leaf = node->leaf;

    // **Move the right half of the keys to the new node**
    for (::std::size_t j{midp1}, k{}; j != keys_number; ++j, ++k)
        new_node->keys[k] = node->keys[j];

    new_node->size = keys_number_sub_midp1;

    // **Handle child pointers (if not a leaf)**
    if (!node->leaf)
    {
        for (::std::size_t j{midp1}, k{}; j != keys_number_p1; ++j, ++k)
        {
			auto &ck{new_node->childrens[k]};
            ck = node->childrens[j];
			auto ckv{ck};
            if (ckv)
			{
                ckv->parent = new_node;
			}
        }
    }

    node->size = mid;  // **Keep only the left half in the current node**

    // **Promote the middle key to the parent node**
    if (node->parent == nullptr)
    {
        // **Allocate a new root using the stateless allocator**
        nodetype* new_root{typed_allocator_type::allocate(1)};
        new_root->keys[0] = node->keys[mid];
        new_root->size = 1;
        new_root->childrens[0] = node;
        new_root->childrens[1] = new_node;
        new_root->leaf = false;
        node->parent = new_root;
        new_node->parent = new_root;
    }
    else
    {
        // **Insert the middle key into the parent node**
        str_btree_insert_key<allocator_type, keys_number>(node->parent, node->keys[mid].ptr, node->keys[mid].n, true);
        node->parent->childrens[node->parent->size] = new_node;
        new_node->parent = node->parent;
    }
}

template <typename allocator_type, ::std::size_t keys_number, typename nodetype>
inline constexpr bool str_btree_insert_key_with_root(nodetype *&node,
										   typename nodetype::char_type const *keystrptr, ::std::size_t keystrn) noexcept
{
    using typed_allocator_type = ::fast_io::typed_generic_allocator_adapter<allocator_type, nodetype>;

    // **If the tree is empty, allocate a new root**
    if (node == nullptr)
    {
        node = typed_allocator_type::allocate(1);
        node->size = 1;
        node->leaf = true;
        node->parent = nullptr;
        *(node->keys) = ::fast_io::details::create_associative_string<allocator_type, typename nodetype::char_type>(keystrptr, keystrn);
        return true;
    }
	return ::fast_io::containers::details::str_btree_insert_key<allocator_type, keys_number>(node,keystrptr,keystrn, false);
}

} // namespace details

template <::std::integral chtype, typename T, typename Allocator, ::std::size_t keys_number = 63>
class basic_str_btreeset
{
	using node_type = ::fast_io::containers::details::str_btreeset_node<chtype, keys_number>;

public:
	using char_type = chtype;
	using string_view_type = ::fast_io::basic_string_view<char_type>;
	using cstring_view_type = ::fast_io::basic_cstring_view<char_type>;
	using value_type = T;
	using allocator_type = Allocator;
	node_type *root{};

	constexpr basic_str_btreeset() noexcept = default;

	explicit constexpr basic_str_btreeset(::std::initializer_list<string_view_type> ilist) noexcept
	{
		for (auto const &e : ilist)
		{
			this->insert_key(e);
		}
	}

	constexpr bool contains(string_view_type) const noexcept
	{
		return false;
	}
	constexpr bool is_empty() const noexcept
	{
		return false;
	}
	constexpr bool erase_key(string_view_type) noexcept
	{
		return false;
	}
	constexpr bool insert_key(string_view_type strvw) noexcept
	{
		return ::fast_io::containers::details::str_btree_insert_key_with_root<allocator_type, keys_number>(this->root, strvw.data(), strvw.size());
	}
	constexpr void clear() noexcept
	{
	}
	constexpr void clear_destroy() noexcept
	{
		this->clear();
	}
};

} // namespace fast_io::containers