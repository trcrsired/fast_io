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
	::std::size_t parent_pos;                                                // Position in Parent
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
inline constexpr bool str_btree_insert_key(nodetype *node,
										   typename nodetype::char_type const *keystrptr, ::std::size_t keystrn, nodetype **proot) noexcept
{
	__builtin_printf("\n\n%s %d %s\n",__FILE__,__LINE__,keystrptr);
	using char_type = typename nodetype::char_type;
	using typed_allocator_type = ::fast_io::typed_generic_allocator_adapter<allocator_type, nodetype>;
	::std::size_t pos;
	// **Find the correct position for insertion**
	for (;;)
	{
		auto [postemp, found] = find_str_btree_node_insert_position(node, keystrptr, keystrn);
		pos = postemp;
		// **If the key already exists, return false (no duplicate keys)**
		if (found)
		{
			return false;
		}
		// **If the node is a leaf**
		if (node->leaf)
		{
			break;
		}
		node = node->childrens[pos];
	}
	auto tempkey = ::fast_io::details::create_associative_string<allocator_type, char_type>(keystrptr, keystrn);

	auto keys{node->keys};
	auto n{node->size};
	auto keysit{keys + pos};
	auto keysed{keys + keys_number};
	// **If there is space, insert the key directly**
	if (n != keys_number)
	{
		::fast_io::freestanding::overlapped_copy(keysit, keysed, keysit + 1);
		*keysit = tempkey;
		++node->size;
		return true;
	}

	constexpr ::std::size_t keys_number_half{keys_number >> (1u)};
	constexpr ::std::size_t keys_number_half_p1{keys_number_half + 1u};
	// ** Split now
	auto rightchild{typed_allocator_type::allocate(1)};
	node->leaf = rightchild->leaf = true;
	node->size = rightchild->size = keys_number_half;

	auto rightchildkeys{rightchild->keys};
	//	auto rightchildchildrens{rightchild->childrens};

	char_type const *movekeystrptr{};
	::std::size_t movekeystrn{};

	auto midptr{keys + keys_number_half};
	auto poskeys_number_halfcmp{pos <=> keys_number_half};
	if (poskeys_number_halfcmp < 0)
	{
		::fast_io::details::non_overlapped_copy_n(midptr, keys_number_half, rightchildkeys);
		::fast_io::freestanding::overlapped_copy(keys + pos, keys + keys_number_half, keys + pos + 1);
		auto &keystrptrkeysnumber{midptr[-1]};
		movekeystrptr = keystrptrkeysnumber.ptr;
		movekeystrn = keystrptrkeysnumber.n;
		keys[pos] = tempkey;
	}
	else if (poskeys_number_halfcmp == 0)
	{
		::fast_io::details::non_overlapped_copy_n(midptr, keys_number_half, rightchildkeys);
		movekeystrptr = tempkey.ptr;
		movekeystrn = tempkey.n;
	}
	else
	{
		auto &keystrptrkeysnumber{*midptr};
		movekeystrptr = keystrptrkeysnumber.ptr;
		movekeystrn = keystrptrkeysnumber.n;
		auto it{::fast_io::details::non_overlapped_copy(midptr + 1, keysit, rightchildkeys)};
		*it = tempkey;
		++it;
		::fast_io::details::non_overlapped_copy(keysit, keysed, it);
	}

	__builtin_printf("%s %d %s\n",__FILE__,__LINE__,keystrptr);
	::std::size_t child_pos{node->parent_pos};
	for (auto j{node->parent}; j; j = j->parent)
	{
		__builtin_printf("%s %d %s j=%p\n",__FILE__,__LINE__,movekeystrptr, j);
		auto jkeys{j->keys};
		auto jchildrens{j->childrens};
		auto jn{j->size};
		// If parent node has space, insert the promoted key and return
		if (jn != keys_number)
		{
			// Shift keys and children to make room for the promoted key and new child
			::fast_io::freestanding::overlapped_copy(jkeys + child_pos, jkeys + jn, jkeys + child_pos + 1);
			jkeys[child_pos].ptr = movekeystrptr;
			jkeys[child_pos].n = movekeystrn;

			::fast_io::freestanding::overlapped_copy(jchildrens + child_pos + 1, jchildrens + jn + 1, jchildrens + child_pos + 2);
			jchildrens[child_pos + 1] = rightchild;
			rightchild->parent = j;
			rightchild->parent_pos = child_pos + 1;

			for(auto k{jchildrens+(child_pos+2)},ked{jchildrens+(jn+2)};k!=ked;++k)
			{
				(*k)->parent_pos = static_cast<::std::size_t>(k-jchildrens);
			}
			++j->size;
			return true;
		}

	__builtin_printf("%s %d %s j=%p\n",__FILE__,__LINE__,movekeystrptr, j);
		// Parent is full, must split upward
		auto new_right = typed_allocator_type::allocate(1);
		j->leaf = new_right->leaf = false;
		j->size = new_right->size = keys_number_half;

		auto jmidptr = jkeys + keys_number_half;

		auto new_right_keys{new_right->keys};
		auto new_right_childrens{new_right->childrens};
		auto child_poskeys_number_halfcmp{child_pos <=> keys_number_half};
		// CASE 1: promoted child is in the left half (before the mid key)
		if (child_poskeys_number_halfcmp < 0)
		{
	__builtin_printf("%s %d %s j=%p\n",__FILE__,__LINE__,tempkey.ptr, j);
			::fast_io::details::non_overlapped_copy_n(jmidptr, keys_number_half, new_right_keys);
			::fast_io::freestanding::overlapped_copy(jkeys + child_pos, jmidptr, jkeys + child_pos + 1);
			jkeys[child_pos].ptr = movekeystrptr;
			jkeys[child_pos].n = movekeystrn;

			auto &jmidkey = jmidptr[-1];
			movekeystrptr = jmidkey.ptr;
			movekeystrn = jmidkey.n;
//	__builtin_printf("%s %d %s j=%p\n",__FILE__,__LINE__,tempkey.ptr, j);
			::fast_io::details::non_overlapped_copy_n(jchildrens + keys_number_half, keys_number_half_p1, new_right_childrens);
			::fast_io::freestanding::overlapped_copy(jchildrens + child_pos + 1, jchildrens + keys_number_half_p1, jchildrens + child_pos + 2);
			jchildrens[child_pos + 1] = rightchild;
			rightchild->parent = node;
			rightchild->parent_pos = child_pos + 1;

			for (::std::size_t i{child_pos + 2}; i != keys_number_half_p1; ++i)
			{
				jchildrens[i]->parent_pos = i;
			}


		}
		else if (child_poskeys_number_halfcmp == 0)
		{
//	__builtin_printf("%s %d %s j=%p\n",__FILE__,__LINE__,tempkey.ptr, j);
			::fast_io::details::non_overlapped_copy_n(jmidptr, keys_number_half, new_right_keys);
			::fast_io::details::non_overlapped_copy_n(jchildrens + keys_number_half_p1, keys_number_half_p1, new_right_childrens);
			*(new_right->childrens) = rightchild;
			rightchild->parent = node;
			rightchild->parent_pos = 0;
		}
		else
		{
	__builtin_printf("%s %d %s j=%p\n",__FILE__,__LINE__,movekeystrptr, j);
			auto jkeysit{jkeys + child_pos};
			auto jkeysed{jkeys + keys_number};
			auto it{::fast_io::details::non_overlapped_copy(jmidptr + 1, jkeysit, new_right_keys)};
			it->ptr = movekeystrptr;
			it->n = movekeystrn;
			++it;
			::fast_io::details::non_overlapped_copy(jkeysit, jkeysed, it);

			auto &jkeystrptrkeysnumber{*jmidptr};
			movekeystrptr = jkeystrptrkeysnumber.ptr;
			movekeystrn = jkeystrptrkeysnumber.n;

			auto jchildrensit{jchildrens + child_pos + 1};
			auto jchildrensed{jchildrens + keys_number + 1};
			auto kit{::fast_io::details::non_overlapped_copy(jchildrens + keys_number_half_p1, jchildrensit, new_right_childrens)};
			*kit = rightchild;
			++kit;
			::fast_io::details::non_overlapped_copy(jchildrensit, jchildrensed, kit);
	__builtin_printf("%s %d %s j=%p\n",__FILE__,__LINE__,movekeystrptr, j);
		}

		for (auto k{new_right_childrens},ked{new_right_childrens+keys_number_half_p1}; k != ked; ++k)
		{
			auto kref{**k};
			kref.parent = new_right;
			kref.parent_pos = static_cast<::std::size_t>(k-new_right_childrens);
		}
		rightchild = new_right;
		child_pos = j->parent_pos;
		node = j;
	}
	__builtin_printf("%s %d %s\n",__FILE__,__LINE__,movekeystrptr);
	auto new_root = typed_allocator_type::allocate(1);
	new_root->leaf = false;
	new_root->size = 1;
	new_root->keys->ptr = movekeystrptr;
	new_root->keys->n = movekeystrn;
	new_root->parent = nullptr;
	new_root->parent_pos = 0;
	*(new_root->childrens) = node;
	new_root->childrens[1] = rightchild;
	node->parent = new_root;
	node->parent_pos = 0;
	rightchild->parent = new_root;
	rightchild->parent_pos = 1;
	*proot = new_root;

	return true;
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
		node->parent_pos = 0;
		*(node->keys) = ::fast_io::details::create_associative_string<allocator_type, typename nodetype::char_type>(keystrptr, keystrn);
		*proot = node;
		return true;
	}
	return ::fast_io::containers::details::str_btree_insert_key<allocator_type, keys_number>(node, keystrptr, keystrn, proot);
}

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
		if (!node->leaf)
		{
			clear_node(node->childrens[n]);

		}

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
};

} // namespace fast_io::containers