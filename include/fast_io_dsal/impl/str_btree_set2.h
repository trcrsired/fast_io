#pragma once

namespace fast_io::containers
{

namespace details
{
struct btree_node_common
{
	void *root;
	void *next;
};

template <::std::integral chtype, ::std::size_t keys_number>
struct str_btree_set_node
{
	using char_type = chtype;
	::std::size_t size;                                                      // Number of keys currently stored
	bool leaf;                                                               // Indicates whether the node is a leaf
	::fast_io::details::associative_string<char_type> keys[keys_number];     // Array of keys
	str_btree_set_node<char_type, keys_number> *childrens[keys_number + 1u]; // Array of child pointers
	str_btree_set_node<char_type, keys_number> *parent;                      // Pointer to parent node
};

template <::std::size_t keys_number>
struct
#if __has_cpp_attribute(__gnu__::__may_alias__)
[[__gnu__::__may_alias__]]
#endif
str_btree_set_common
{
	::std::size_t size;
	bool leaf;
	::fast_io::io_scatter_t keys[keys_number];
	void *childrens[keys_number + 1u];
	void *parent;
};

struct find_btree_node_insert_position_result
{
	::std::size_t pos{};
	bool found{};
};

template <typename nodetype>
inline constexpr find_btree_node_insert_position_result find_str_btree_node_insert_position(nodetype *node,
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

template <typename allocator_type, ::std::size_t keys_number, typename nodetype>
inline constexpr void str_btree_split_node(nodetype *node, nodetype** proot) noexcept;

template <typename allocator_type, ::std::size_t keys_number, typename nodetype>
inline constexpr bool str_btree_insert_key(nodetype *node,
										   typename nodetype::char_type const *keystrptr, ::std::size_t keystrn, bool ismoved, nodetype** proot) noexcept
{
	using char_type = typename nodetype::char_type;
	// **Find the correct position for insertion**
	auto [pos, found] = find_str_btree_node_insert_position(node, keystrptr, keystrn);

	// **If the key already exists, return false (no duplicate keys)**
	if (found)
	{
		return false;
	}

	// **If the node is a leaf**
	if (node->leaf)
	{

		// **If there is space, insert the key directly**
		if (node->size < keys_number)
		{
			for (::std::size_t j{node->size}; pos < j; --j)
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
				astr = ::fast_io::details::create_associative_string<allocator_type, char_type>(keystrptr, keystrn);
			}
			node->keys[pos] = astr;
			++node->size;
			return true;
		}
		else
		{
			// **If the node is full, split it**
			str_btree_split_node<allocator_type, keys_number>(node, proot);
			return str_btree_insert_key<allocator_type, keys_number>(node->parent, keystrptr, keystrn, ismoved, proot);
		}
	}
	else
	{
		// **Recursively insert into the correct child**
		return str_btree_insert_key<allocator_type, keys_number>(node->childrens[pos], keystrptr, keystrn, ismoved, proot);
	}
}

template <typename allocator_type, ::std::size_t keys_number, typename nodetype>
inline constexpr void str_btree_split_node(nodetype *node, nodetype** proot) noexcept
{
	using char_type = typename nodetype::char_type;
	using typed_allocator_type = ::fast_io::typed_generic_allocator_adapter<allocator_type, nodetype>;
	constexpr ::std::size_t keys_number_div2{keys_number >> 1u};
	constexpr ::std::size_t keys_number_p1{keys_number + 1u};
	constexpr ::std::size_t mid{keys_number_div2};
	constexpr ::std::size_t midp1{mid + 1u};
	constexpr ::std::size_t keys_number_sub_midp1{keys_number - midp1};

	// **Allocate a new node using the stateless allocator**
	nodetype *new_node{typed_allocator_type::allocate(1)};

	new_node->parent = node->parent;
	new_node->leaf = node->leaf;

	// **Move the right half of the keys to the new node**
	for (::std::size_t j{midp1}, k{}; j != keys_number; ++j, ++k)
	{
		new_node->keys[k] = node->keys[j];
	}

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

	node->size = mid; // **Keep only the left half in the current node**

	// **Promote the middle key to the parent node**
	if (node->parent == nullptr)
	{
		// **Allocate a new root using the stateless allocator**
		nodetype *new_root{typed_allocator_type::allocate(1)};
		*proot = new_root;
		*(new_root->keys) = node->keys[mid];
		new_root->size = 1;
		*(new_root->childrens) = node;
		new_root->childrens[1] = new_node;
		new_root->leaf = false;
		new_node->parent = nullptr;
		node->parent = new_root;
		new_node->parent = new_root;
	}
	else
	{
		// **Insert the middle key into the parent node**
		str_btree_insert_key<allocator_type, keys_number>(node->parent, node->keys[mid].ptr, node->keys[mid].n, true, proot);
		node->parent->childrens[node->parent->size] = new_node;
		new_node->parent = node->parent;
	}
}

template <typename allocator_type, ::std::size_t keys_number, typename nodetype>
inline constexpr bool str_btree_insert_key_with_root(nodetype **proot,
													 typename nodetype::char_type const *keystrptr, ::std::size_t keystrn) noexcept
{
	using typed_allocator_type = ::fast_io::typed_generic_allocator_adapter<allocator_type, nodetype>;

	auto node{*proot};

	// **If the tree is empty, allocate a new root**
	if (node == nullptr)
	{
		node = typed_allocator_type::allocate(1);
		node->size = 1;
		node->leaf = true;
		node->parent = nullptr;
		*(node->keys) = ::fast_io::details::create_associative_string<allocator_type, typename nodetype::char_type>(keystrptr, keystrn);
		*proot = node;
		return true;
	}
	return ::fast_io::containers::details::str_btree_insert_key<allocator_type, keys_number>(node, keystrptr, keystrn, false, proot);
}
#if 0
template <::std::integral chtype, ::std::size_t keys_number>
struct str_btree_set_iterator
{
	::fast_io::containers::str_btree_set_node<char_type>* node{};
	::std::size_t pos{};
	constexpr str_btree_set_iterator& operator++() noexcept
	{
        if (node == nullptr)
            return *this;

        // **If there are more keys in the current node, move right**
        if (pos + 1 < node->size)
        {
            ++pos;
            return *this;
        }

        // **If there is a right child, move to the smallest key in the right subtree**
        if (!node->leaf)
        {
            node = node->childrens[pos + 1];
            while (!node->leaf)
                node = node->childrens[0];
            pos = 0;
            return *this;
        }

        // **Otherwise, move up until we find a larger key**
        while (node->parent && node == node->parent->childrens[node->parent->size])
            node = node->parent;

        if (node->parent)
        {
            pos = 0;
            node = node->parent;
        }
        else
        {
            node = nullptr;  // **End of iteration**
        }
	}
#if 0
	constexpr str_btree_set_iterator& operator--() noexcept
	{

	}
#endif
	constexpr ::fast_io::containers::basic_cstring_view<chtype> operator*() const noexcept
	{
		auto &def{this->node->keys[this->pos]};
		return def.strvw();
	}
};

template <::std::integral chtype, ::std::size_t keys_number>
inline constexpr bool operator==(::fast_io::containers::details::str_btree_set_iterator<chtype,keys_number> a,
				::fast_io::containers::details::str_btree_set_iterator<chtype,keys_number> b)
{
	return a.node == b.node && a.pos == b.pos;
}

template <::std::integral chtype, ::std::size_t keys_number>
inline constexpr bool operator!=(::fast_io::containers::details::str_btree_set_iterator<chtype,keys_number> a,
				::fast_io::containers::details::str_btree_set_iterator<chtype,keys_number> b)
{
	return !(a==b);
}
#endif

template <::std::integral chtype, ::std::size_t keys_number, bool isrev>
struct str_btree_set_iterator
{
	using char_type = chtype;
	using value_type = char_type;
	using cstring_view_type = ::fast_io::basic_cstring_view<char_type>;
	using const_iterator = ::fast_io::containers::details::str_btree_set_iterator<chtype, keys_number, isrev>;
	using iterator = const_iterator;
	::fast_io::containers::details::str_btree_set_node<chtype, keys_number> *ptr{};
	inline constexpr const_iterator operator++() noexcept
	{
		return {};
	}
	inline constexpr const_iterator operator++(int) noexcept
	{
		auto tmp{*this};
		++*this;
		return tmp;
	}
};

template <::std::integral chtype, ::std::size_t keys_number, bool isrev>
inline constexpr bool operator==(::fast_io::containers::details::str_btree_set_iterator<chtype, keys_number, isrev> a,
								 ::fast_io::containers::details::str_btree_set_iterator<chtype, keys_number, isrev> b) noexcept
{
	return a.ptr == b.ptr;
}

template <::std::integral chtype, ::std::size_t keys_number, bool isrev>
inline constexpr bool operator!=(::fast_io::containers::details::str_btree_set_iterator<chtype, keys_number, isrev> a,
								 ::fast_io::containers::details::str_btree_set_iterator<chtype, keys_number, isrev> b) noexcept
{
	return a.ptr != b.ptr;
}

template <::std::integral chtype, ::std::size_t keys_number, bool isrev>
inline constexpr bool operator==(::fast_io::containers::details::str_btree_set_iterator<chtype, keys_number, isrev> a,
								 ::std::default_sentinel_t) noexcept
{
	return a.ptr == nullptr;
}

template <::std::integral chtype, ::std::size_t keys_number, bool isrev>
inline constexpr bool operator==(::std::default_sentinel_t,
								 ::fast_io::containers::details::str_btree_set_iterator<chtype, keys_number, isrev> a) noexcept
{
	return a.ptr == nullptr;
}


template <::std::integral chtype, ::std::size_t keys_number, bool isrev>
inline constexpr bool operator!=(::fast_io::containers::details::str_btree_set_iterator<chtype, keys_number, isrev> a,
								 ::std::default_sentinel_t) noexcept
{
	return a.ptr != nullptr;
}

template <::std::integral chtype, ::std::size_t keys_number, bool isrev>
inline constexpr bool operator!=(::std::default_sentinel_t,
								 ::fast_io::containers::details::str_btree_set_iterator<chtype, keys_number, isrev> a) noexcept
{
	return a.ptr != nullptr;
}

template <::std::integral chtype, ::std::size_t keys_number, bool isrev>
struct str_btree_set_iters_type
{
	using char_type = chtype;
	using value_type = char_type;
	using cstring_view_type = ::fast_io::basic_cstring_view<char_type>;
	using const_iterator = ::fast_io::containers::details::str_btree_set_iterator<chtype, keys_number, isrev>;
	using iterator = const_iterator;
	const_iterator i;
	constexpr const_iterator cbegin() const noexcept
	{
		return i;
	}
	constexpr ::std::default_sentinel_t cend() const noexcept
	{
		return {};
	}
	constexpr const_iterator begin() const noexcept
	{
		return i;
	}
	constexpr ::std::default_sentinel_t end() const noexcept
	{
		return {};
	}
};

} // namespace details

template <::std::integral chtype, typename Allocator, ::std::size_t keys_number = 63>
class basic_str_btree_set
{
	using node_type = ::fast_io::containers::details::str_btree_set_node<chtype, keys_number>;
	using untyped_allocator_type = Allocator;
	using typed_allocator_type = ::fast_io::typed_generic_allocator_adapter<untyped_allocator_type, node_type>;

public:
	using char_type = chtype;
	using string_view_type = ::fast_io::basic_string_view<char_type>;
	using cstring_view_type = ::fast_io::basic_cstring_view<char_type>;
	using allocator_type = Allocator;
	using const_iters_type = ::fast_io::containers::details::str_btree_set_iters_type<chtype, keys_number, false>;
	using const_reverse_iters_type = ::fast_io::containers::details::str_btree_set_iters_type<chtype, keys_number, true>;
#if 0
	using const_iterator = ::fast_io::details::str_btree_set_iterator<char_type, keys_number>;
	using iterator = ::fast_io::details::str_btree_set_iterator<char_type, keys_number>;
	::fast_io::containers::details::btree_node_common root;
#endif
	node_type *root{};

	constexpr basic_str_btree_set() noexcept = default;

	explicit constexpr basic_str_btree_set(::std::initializer_list<string_view_type> ilist) noexcept
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
		return ::fast_io::containers::details::str_btree_insert_key_with_root<allocator_type, keys_number>(__builtin_addressof(this->root), strvw.data(), strvw.size());
	}

private:
	static inline constexpr void clear_node(node_type *node) noexcept
	{
		if (node == nullptr)
		{
			return;
		}
		::std::size_t const n{node->size};
		for (::std::size_t i{}; i != n; ++i)
		{
			if (!node->leaf)
			{
				clear_node(node->childrens[i]);
			}
#if 0
			::fast_io::io::debug_println(::fast_io::mnp::pointervw(node),
										 i, " ", ::fast_io::mnp::pointervw(node->keys + i), " ", node->keys[i].strvw());
#endif
			auto ki{node->keys[i]};
			::fast_io::typed_generic_allocator_adapter<untyped_allocator_type, char_type>::deallocate_n(const_cast<char_type *>(ki.ptr),
																										static_cast<::std::size_t>(ki.n + 1u));
		}
#if 0
		if (!node->leaf)
		{
			clear_node(node->childrens[n]);

			auto ki{node->childrens[node->size]};
			::fast_io::typed_generic_allocator_adapter<untyped_allocator_type, char_type>::deallocate_n(,
					static_cast<::std::size_t>(ki.n+1u));
		}
#endif

		// **Deallocate the node using the allocator**
		typed_allocator_type::deallocate_n(node, 1);
	}

public:
	constexpr void clear() noexcept
	{
		clear_node(this->root);
		this->root = nullptr;
	}

	constexpr void clear_destroy() noexcept
	{
		this->clear();
	}

	constexpr basic_str_btree_set(basic_str_btree_set const &) noexcept = delete;
	constexpr basic_str_btree_set &operator=(basic_str_btree_set const &) noexcept = delete;

	constexpr basic_str_btree_set(basic_str_btree_set &&other) noexcept : root{other.root}
	{
		other.root = nullptr;
	}
	constexpr basic_str_btree_set &operator=(basic_str_btree_set &&other) noexcept
	{
		if (::std::addressof(other) == this)
		{
			return *this;
		}
		this->root = other.root;
		other.root = nullptr;
		return *this;
	}
	constexpr ~basic_str_btree_set()
	{
		clear_node(this->root);
	}
#if 0
	constexpr  citers() const noexcept
	{}
#endif
};

} // namespace fast_io::containers