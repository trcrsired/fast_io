#pragma once

namespace fast_io::containers
{

namespace details
{
struct btree_imp
{
	void *root;
	void *leftmost;
	void *rightmost;
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
	::std::size_t parent_pos;
};

struct find_btree_node_insert_position_result
{
	::std::size_t pos{};
	bool found{};
};

template <typename nodetype>
inline constexpr ::fast_io::containers::details::find_btree_node_insert_position_result find_str_btree_node_insert_position(nodetype *node,
																															typename nodetype::char_type const *keystrptr, ::std::size_t keystrn) noexcept
{
	using char_type = typename nodetype::char_type;
	auto *b{node->keys}, *i{b}, *e{b + node->size};

	::fast_io::containers::basic_string_view<char_type> newkey(keystrptr, keystrn);

	bool found{};
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


struct find_btree_insert_result
{
	void *node{};
	::std::size_t pos{};
};

template <typename nodetype>
inline constexpr ::fast_io::containers::details::find_btree_insert_result str_btree_find(nodetype *node, typename nodetype::char_type const *keystrptr, ::std::size_t keystrn) noexcept
{
	if (node == nullptr)
	{
		return {};
	}
	// **Find the correct position for insertion**
	for (;;)
	{
		auto [postemp, found] = ::fast_io::containers::details::find_str_btree_node_insert_position(node, keystrptr, keystrn);
		// **If the key already exists, return false (no duplicate keys)**
		if (found)
		{
			return {node, postemp};
		}
		// **If the node is a leaf**
		if (node->leaf)
		{
			break;
		}
		node = node->childrens[postemp];
	}
	return {};
}

template <typename nodetype>
inline constexpr bool str_btree_contains(nodetype *node, typename nodetype::char_type const *keystrptr, ::std::size_t keystrn) noexcept
{
	if (node == nullptr)
	{
		return false;
	}
	// **Find the correct position for insertion**
	for (;;)
	{
		auto [postemp, found] = ::fast_io::containers::details::find_str_btree_node_insert_position(node, keystrptr, keystrn);
		// **If the key already exists, return false (no duplicate keys)**
		if (found)
		{
			return true;
		}
		// **If the node is a leaf**
		if (node->leaf)
		{
			break;
		}
		node = node->childrens[postemp];
	}
	return false;
}

template <typename allocator_type, ::std::size_t keys_number, typename nodetype>
#if __has_cpp_attribute(__gnu__::__cold__)
[[__gnu__::__cold__]]
#endif
inline constexpr bool str_btree_insert_key_cold(nodetype *node, ::std::size_t pos,
												typename nodetype::char_type const *tempkeystrptr, ::std::size_t tempkeystrn,
												::fast_io::containers::details::btree_imp &imp) noexcept
{
	using char_type = typename nodetype::char_type;
	using typed_allocator_type = ::fast_io::typed_generic_allocator_adapter<allocator_type, nodetype>;


	auto keys{node->keys};
	auto n{node->size};
	auto keysit{keys + pos};
	auto keysed{keys + keys_number};

	constexpr ::std::size_t keys_number_half{keys_number >> (1u)};
	constexpr ::std::size_t keys_number_half_p1{keys_number_half + 1u};
	// ** Split now
	auto rightchild{typed_allocator_type::allocate(1)};
	if (node == imp.rightmost)
	{
		imp.rightmost = rightchild;
	}
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
		auto &keystrptrkeysnumber{midptr[-1]};
		movekeystrptr = keystrptrkeysnumber.ptr;
		movekeystrn = keystrptrkeysnumber.n;

		::fast_io::details::non_overlapped_copy_n(midptr, keys_number_half, rightchildkeys);
		::fast_io::freestanding::overlapped_copy(keys + pos, keys + keys_number_half, keys + pos + 1);

		keys[pos] = {tempkeystrptr, tempkeystrn};
	}
	else if (poskeys_number_halfcmp == 0)
	{
		::fast_io::details::non_overlapped_copy_n(midptr, keys_number_half, rightchildkeys);
		movekeystrptr = tempkeystrptr;
		movekeystrn = tempkeystrn;
	}
	else
	{
		auto &keystrptrkeysnumber{*midptr};
		movekeystrptr = keystrptrkeysnumber.ptr;
		movekeystrn = keystrptrkeysnumber.n;
		auto it{::fast_io::details::non_overlapped_copy(midptr + 1, keysit, rightchildkeys)};
		*it = {tempkeystrptr, tempkeystrn};
		++it;
		::fast_io::details::non_overlapped_copy(keysit, keysed, it);
	}
	::std::size_t child_pos{node->parent_pos};
	for (auto j{node->parent}; j; j = j->parent)
	{
		auto jkeys{j->keys};
		auto jchildrens{j->childrens};
		auto jn{j->size};

		// If parent node has space, insert the promoted key and return
		if (jn != keys_number)
		{
			// Shift keys and children to make room for the promoted key and new child
			::fast_io::freestanding::overlapped_copy(jkeys + child_pos, jkeys + jn, jkeys + child_pos + 1);
			auto &jkeyschildpos{jkeys[child_pos]};
			jkeyschildpos.ptr = movekeystrptr;
			jkeyschildpos.n = movekeystrn;
			::fast_io::freestanding::overlapped_copy(jchildrens + child_pos + 1, jchildrens + jn + 1, jchildrens + child_pos + 2);
			jchildrens[child_pos + 1] = rightchild;
			rightchild->parent = j;
			rightchild->parent_pos = child_pos + 1;

			for (auto k{jchildrens + (child_pos + 2)}, ked{jchildrens + (jn + 2)}; k != ked; ++k)
			{
				(*k)->parent_pos = static_cast<::std::size_t>(k - jchildrens);
			}
			++j->size;

			return true;
		}
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

			auto &jmidkey = jmidptr[-1];
			auto tmpptr{jmidkey.ptr};
			auto tmpn{jmidkey.n};

			::fast_io::details::non_overlapped_copy_n(jmidptr, keys_number_half, new_right_keys);
			::fast_io::freestanding::overlapped_copy(jkeys + child_pos, jmidptr, jkeys + child_pos + 1);
			auto &jkeyschildpos{jkeys[child_pos]};
			jkeyschildpos.ptr = movekeystrptr;
			jkeyschildpos.n = movekeystrn;
			movekeystrptr = tmpptr;
			movekeystrn = tmpn;

			::fast_io::details::non_overlapped_copy_n(jchildrens + keys_number_half, keys_number_half_p1, new_right_childrens);
			::fast_io::freestanding::overlapped_copy(jchildrens + child_pos + 1, jchildrens + keys_number_half_p1, jchildrens + child_pos + 2);
			jchildrens[child_pos + 1] = rightchild;
			rightchild->parent = j;
			rightchild->parent_pos = child_pos + 1;

			for (::std::size_t i{child_pos + 2}; i != keys_number_half_p1; ++i)
			{
				jchildrens[i]->parent_pos = i;
			}
		}
		else if (child_poskeys_number_halfcmp == 0)
		{
			::fast_io::details::non_overlapped_copy_n(jmidptr, keys_number_half, new_right_keys);
			*new_right_childrens = rightchild;
			::fast_io::details::non_overlapped_copy_n(jchildrens + keys_number_half_p1, keys_number_half, new_right_childrens + 1);
		}
		else
		{
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
		}

		for (auto k{new_right_childrens}, ked{new_right_childrens + keys_number_half_p1}; k != ked; ++k)
		{
			auto &kref{**k};
			kref.parent = new_right;
			kref.parent_pos = static_cast<::std::size_t>(k - new_right_childrens);
		}
		rightchild = new_right;
		child_pos = j->parent_pos;
		node = j;
	}
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
	imp.root = new_root;

	return true;
}

template <typename allocator_type, ::std::size_t keys_number, typename nodetype>
inline constexpr bool str_btree_insert_key_with_root(::fast_io::containers::details::btree_imp &imp,
													 typename nodetype::char_type const *keystrptr, ::std::size_t keystrn) noexcept
{
	using char_type = typename nodetype::char_type;
	using typed_allocator_type = ::fast_io::typed_generic_allocator_adapter<allocator_type, nodetype>;

	auto node{static_cast<::fast_io::containers::details::str_btree_set_node<char_type, keys_number> *>(imp.root)};

	// **If the tree is empty, allocate a new root**
	if (node == nullptr)
	{
		node = typed_allocator_type::allocate(1);
		node->size = 1;
		node->leaf = true;
		node->parent = nullptr;
		node->parent_pos = 0;
		*(node->keys) = ::fast_io::details::create_associative_string<allocator_type, char_type>(keystrptr, keystrn);
		imp.rightmost = imp.leftmost = imp.root = node;
		return true;
	}

	::std::size_t pos;
	// **Find the correct position for insertion**
	for (;;)
	{
		auto [postemp, found] = ::fast_io::containers::details::find_str_btree_node_insert_position(node, keystrptr, keystrn);
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
	auto n{node->size};
	// **If there is space, insert the key directly**
	if (n != keys_number)
	{
		auto keys{node->keys};
		auto keysit{keys + pos};
		::fast_io::freestanding::overlapped_copy(keysit, keys + n, keysit + 1);
		*keysit = tempkey;
		++node->size;
		return true;
	}
	return ::fast_io::containers::details::str_btree_insert_key_cold<allocator_type, keys_number>(node, pos, tempkey.ptr, tempkey.n, imp);
}

template <typename allocator_type, ::std::size_t keys_number, typename nodetype>
inline constexpr bool str_btree_erase(::fast_io::containers::details::btree_imp &imp,
									  typename nodetype::char_type const *keystrptr, ::std::size_t keystrn) noexcept
{
	// unfinished
	using char_type = typename nodetype::char_type;
	using typed_allocator_type = ::fast_io::typed_generic_allocator_adapter<allocator_type,
																			::fast_io::containers::details::str_btree_set_common<keys_number>>;
	using nodeptr
#if __has_cpp_attribute(__gnu__::__may_alias__)
		[[__gnu__::__may_alias__]]
#endif
		= ::fast_io::containers::details::str_btree_set_common<keys_number> *;
	auto node{static_cast<::fast_io::containers::details::str_btree_set_node<char_type, keys_number> *>(imp.root)};

	auto [it, pos] = ::fast_io::containers::details::str_btree_find(node, keystrptr, keystrn);
	if (it == nullptr)
	{
		return false;
	}
	constexpr ::std::size_t keys_number_half{keys_number >> 1u};
	auto itv{static_cast<nodeptr>(it)};
	auto keys{itv->keys};
	auto keyspos{keys + pos};
	::fast_io::details::deallocate_associative_string<allocator_type, char_type>(keyspos->base, keyspos->len);
	::std::size_t n{itv->size};

	auto parent{static_cast<nodeptr>(itv->parent)};
	if (itv->leaf)
	{
		__builtin_printf("hello world: %s %d\n",__FILE__,__LINE__);
		if (keys_number_half < n)
		{
			::fast_io::freestanding::overlapped_copy(keyspos + 1, keys + n, keyspos);
			--itv->size;

			return true;
		}
		else if (parent == nullptr)
		{
			::fast_io::freestanding::overlapped_copy(keyspos + 1, keys + n, keyspos);
			if (!(--itv->size))
			{
				// **Deallocate the node using the allocator**
				typed_allocator_type::deallocate_n(itv, 1);
				imp = {};
			}
			return true;
		}
		::std::size_t parpos{itv->parent_pos};
		::std::size_t parent_size{parent->size};
		__builtin_printf("hello world: %s %d\n",__FILE__,__LINE__);
		nodeptr rightbrother{};
		if (parpos != parent_size)
		{
			rightbrother = static_cast<nodeptr>(parent->childrens[parpos + 1]);
			::std::size_t rightbrothern{rightbrother->size};
			if (rightbrothern < keys_number_half)
			{
				// Borrow from right brother's leftmost key
				auto rightbrotherkeys{rightbrother->keys};
				auto &borrowed_key = *rightbrotherkeys;
				auto &parent_key = parent->keys[parpos];

				// Move borrowed key to current node
				keys[n] = parent_key;
				++itv->size;

				// Update parent key
				parent_key = borrowed_key;

				// Shift right brother's keys left
				::fast_io::freestanding::overlapped_copy(
					rightbrotherkeys + 1,
					rightbrotherkeys + rightbrothern,
					rightbrotherkeys);
				--rightbrother->size;

				return true;
			}
		}
		__builtin_printf("hello world: %s %d\n",__FILE__,__LINE__);
		// Try left brother if right failed
		nodeptr leftbrother{};
		if (parpos != 0)
		{
			leftbrother = static_cast<nodeptr>(parent->childrens[parpos - 1]);
			::std::size_t leftbrothern{leftbrother->size};
			if (keys_number_half < leftbrothern)
			{
				// Borrow from left brother's rightmost key
				auto &borrowed_key = leftbrother->keys[leftbrothern - 1];
				auto &parent_key = parent->keys[parpos - 1];

				// Shift current keys right
				::fast_io::freestanding::overlapped_copy(
					keyspos,
					keys + n,
					keyspos + 1);

				// Insert parent key
				*keys = parent_key;
				++itv->size;

				// Update parent key
				parent_key = borrowed_key;
				--leftbrother->size;
		__builtin_printf("hello world: %s %d\n",__FILE__,__LINE__);
				return true;
			}
		}
		// Merge with sibling (prefer right if possible)
		if (parpos != parent_size)
		{
			// Merge with right brother
			keys[n] = parent->keys[parpos];
			::fast_io::freestanding::non_overlapped_copy(
				rightbrother->keys,
				rightbrother->keys + rightbrother->size,
				keys + n + 1);
			itv->size += rightbrother->size + 1;

			// Deallocate right brother
			typed_allocator_type::deallocate_n(rightbrother, 1);

			// Shift parent's keys and children
			::fast_io::freestanding::overlapped_copy(
				parent->keys + parpos + 1,
				parent->keys + parent_size,
				parent->keys + parpos);
			::fast_io::freestanding::overlapped_copy(
				parent->childrens + parpos + 2,
				parent->childrens + parent_size + 1,
				parent->childrens + parpos + 1);
			--parent->size;
		__builtin_printf("hello world: %s %d\n",__FILE__,__LINE__);
			return true;
		}
		else
		{
			// Merge with left brother
			auto &merge_key = parent->keys[parpos - 1];
			::fast_io::freestanding::non_overlapped_copy(
				keys,
				keys + n,
				leftbrother->keys + leftbrother->size + 1);
			leftbrother->keys[leftbrother->size] = merge_key;
			leftbrother->size += n + 1;

			// Deallocate current node
			typed_allocator_type::deallocate_n(itv, 1);

			// Shift parent's keys and children
			::fast_io::freestanding::overlapped_copy(
				parent->keys + parpos,
				parent->keys + parent_size,
				parent->keys + parpos - 1);
			::fast_io::freestanding::overlapped_copy(
				parent->childrens + parpos + 1,
				parent->childrens + parent_size + 1,
				parent->childrens + parpos);
			--parent->size;

			return true;
		}
	}
		__builtin_printf("hello world: %s %d\n",__FILE__,__LINE__);
#if 0

	::fast_io::io::debug_println(::std::source_location::current(),"\t", ::fast_io::mnp::os_c_str_with_known_size(keystrptr, keystrn));

	
	for (auto j{parent}; j; j = static_cast<::fast_io::containers::details::str_btree_set_common<keys_number> *>(j->parent))
	{
		bool isroot{itv==imp.root};

		::std::size_t parent_size{parent->size},
				parentsizep1{static_cast<::std::size_t>(parent_size+1u)};;
		::std::size_t parent_pos{itv->parent_pos};
		::std::size_t parent_pos_p1{static_cast<::std::size_t>(parent_pos+1u)};
		auto parentchildrens{parent->childrens};
		if(parent_pos!=parentsizep1)
		{
			auto nextnode{static_cast<nodeptr>(parentchildrens[parent_pos_p1])};
			::std::size_t const nextsize{nextnode->size};
			if(keys_number_half <= nextsize)
			{

			}
			else
			{

			}
		}
		else
		{
		}
	}

	else
	{
	}
#endif
	return true;
}

/* Common structure used for str_btree_set iterator.
Holds the current node pointer, position within node, and optional 'last' for reversed iteration fallback. */
struct str_btree_set_iterator_common
{
	void const *ptr{};  // Pointer to current node
	std::size_t pos{};  // Position within current node
	void const *last{}; // Pointer to last node (used for -- from end)
};

/* Advances the iterator to the next key in str_btree_set.
Traverses leaf nodes linearly, and climbs up the parent chain when leaf ends.
For internal nodes, descends leftmost into the right child. */
template <::std::size_t keys_number>
inline constexpr void str_btree_set_next_node(str_btree_set_iterator_common &c) noexcept
{
	auto ptrv{static_cast<::fast_io::containers::details::str_btree_set_common<keys_number> const *>(c.ptr)};

	if (ptrv->leaf)
	{
		// Move to next key inside leaf
		if (++c.pos == ptrv->size)
		{
			// Reached end of leaf, climb parent chain to find next sibling
			auto parent{ptrv->parent};
			auto parent_pos{static_cast<::fast_io::containers::details::str_btree_set_common<keys_number> const *>(ptrv)->parent_pos};
			while (parent)
			{
				auto parentn{static_cast<::fast_io::containers::details::str_btree_set_common<keys_number> const *>(parent)->size};
				if (parent_pos != parentn)
				{
					break; // Found next available key in parent
				}
				parent_pos = static_cast<::fast_io::containers::details::str_btree_set_common<keys_number> const *>(parent)->parent_pos;
				parent = static_cast<::fast_io::containers::details::str_btree_set_common<keys_number> const *>(parent)->parent;
			}
			c.ptr = parent;
			c.pos = parent_pos;
		}
	}
	else
	{
		// Internal node: descend into leftmost key of right child
		auto nextptr{ptrv->childrens[c.pos + 1]};
		while (!static_cast<::fast_io::containers::details::str_btree_set_common<keys_number> const *>(nextptr)->leaf)
		{
			nextptr = *(static_cast<::fast_io::containers::details::str_btree_set_common<keys_number> const *>(nextptr)->childrens);
		}
		c.ptr = nextptr;
		c.pos = 0;
	}
}

/* Moves the iterator to the previous key in str_btree_set.
From null (end), restores last node. In leaf, moves left or climbs parent.
For internal node, descends rightmost into the left child. */
template <::std::size_t keys_number>
inline constexpr void str_btree_set_prev_node(str_btree_set_iterator_common &c) noexcept
{
	if (c.ptr == nullptr)
	{
		// Iterator was at end(); fallback to last node if known
		c.ptr = c.last;
		// c.ptr must not be nullptr or it is on empty container which is undefined behavior
		c.pos = static_cast<::std::size_t>(static_cast<::fast_io::containers::details::str_btree_set_common<keys_number> const *>(c.ptr)->size - 1u);
		return;
	}

	auto ptrv{static_cast<::fast_io::containers::details::str_btree_set_common<keys_number> const *>(c.ptr)};

	if (ptrv->leaf)
	{
		if (!c.pos)
		{
			// Start of leaf, need to go up to parent and find left sibling
			auto parent{ptrv->parent};
			auto parent_pos{static_cast<::fast_io::containers::details::str_btree_set_common<keys_number> const *>(ptrv)->parent_pos};
			while (parent)
			{
				if (parent_pos)
				{
					break; // Found non-zero position in parent
				}
				parent_pos = static_cast<::std::size_t>(static_cast<::fast_io::containers::details::str_btree_set_common<keys_number> const *>(parent)->parent_pos);
				parent = static_cast<::fast_io::containers::details::str_btree_set_common<keys_number> const *>(parent)->parent;
			}
			c.ptr = parent;
			c.pos = 0;
			if (parent)
			{
				c.pos = static_cast<::std::size_t>(parent_pos - 1u);
			}
			return;
		}
		--c.pos; // Move left inside leaf
	}
	else
	{
		// Internal node: descend into rightmost key of left child
		auto prevptr{ptrv->childrens[c.pos]};
		while (!static_cast<::fast_io::containers::details::str_btree_set_common<keys_number> const *>(prevptr)->leaf)
		{
			auto e{static_cast<::fast_io::containers::details::str_btree_set_common<keys_number> const *>(prevptr)};
			prevptr = e->childrens[e->size];
		}
		c.ptr = prevptr;
		c.pos = static_cast<::std::size_t>(static_cast<::fast_io::containers::details::str_btree_set_common<keys_number> const *>(c.ptr)->size - 1u);
	}
}

template <::std::integral chtype, ::std::size_t keys_number>
class str_btree_set_iterator
{
public:
	using value_type = ::fast_io::containers::basic_cstring_view<chtype>;
	using iterator_tag = ::std::bidirectional_iterator_tag;
	using difference_type = ::std::ptrdiff_t;
	::fast_io::containers::details::str_btree_set_iterator_common node;

	constexpr str_btree_set_iterator &operator++() noexcept
	{
		::fast_io::containers::details::str_btree_set_next_node<keys_number>(this->node);
		return *this;
	}
	constexpr str_btree_set_iterator operator++(int) noexcept
	{
		auto tmp{*this};
		++*this;
		return tmp;
	}
	constexpr str_btree_set_iterator &operator--() noexcept
	{
		::fast_io::containers::details::str_btree_set_prev_node<keys_number>(this->node);
		return *this;
	}
	constexpr str_btree_set_iterator operator--(int) noexcept
	{
		auto tmp{*this};
		--*this;
		return tmp;
	}
	constexpr value_type operator*() const noexcept
	{
		return static_cast<::fast_io::containers::details::str_btree_set_node<char, keys_number> const *>(node.ptr)->keys[node.pos].strvw();
	}
};

template <::std::integral chtype, ::std::size_t keys_number>
inline constexpr bool operator==(::fast_io::containers::details::str_btree_set_iterator<chtype, keys_number> const &a,
								 ::fast_io::containers::details::str_btree_set_iterator<chtype, keys_number> const &b) noexcept
{
	return a.node.ptr == b.node.ptr && a.node.pos == b.node.pos;
}

template <::std::integral chtype, ::std::size_t keys_number>
inline constexpr bool operator!=(::fast_io::containers::details::str_btree_set_iterator<chtype, keys_number> const &a,
								 ::fast_io::containers::details::str_btree_set_iterator<chtype, keys_number> const &b) noexcept
{
	return !operator==(a, b);
}

} // namespace details

template <::std::integral chtype, typename Allocator, ::std::size_t keys_number = 16>
	requires((keys_number != 0) && ((keys_number & 1) == 0))
class basic_str_btree_set
{
	using node_type = ::fast_io::containers::details::str_btree_set_node<chtype, keys_number>;
	using untyped_allocator_type = Allocator;
	using typed_allocator_type = ::fast_io::typed_generic_allocator_adapter<untyped_allocator_type, node_type>;

public:
	using char_type = chtype;
	using string_view_type = ::fast_io::containers::basic_string_view<char_type>;
	using cstring_view_type = ::fast_io::containers::basic_cstring_view<char_type>;
	using allocator_type = Allocator;
	using const_iterator = ::fast_io::containers::details::str_btree_set_iterator<char_type, keys_number>;
	using iterator = const_iterator;
	using const_reverse_iterator = ::std::reverse_iterator<const_iterator>;
	using reverse_iterator = const_reverse_iterator;
	using size_type = ::std::size_t;
	using difference_type = ::std::ptrdiff_t;

	::fast_io::containers::details::btree_imp imp{nullptr, nullptr, nullptr};

	constexpr basic_str_btree_set() noexcept = default;

	explicit constexpr basic_str_btree_set(::std::initializer_list<string_view_type> ilist) noexcept
	{
		for (auto const &e : ilist)
		{
			this->insert_key(e);
		}
	}

	constexpr bool contains(string_view_type key) const noexcept
	{
		return ::fast_io::containers::details::str_btree_contains(static_cast<node_type *>(this->imp.root), key.ptr, key.n);
	}
	constexpr size_type count(string_view_type key) const noexcept
	{
		return static_cast<size_type>(this->contains(key));
	}
	constexpr const_iterator find(string_view_type key) const noexcept
	{
		auto [ptr, pos] = ::fast_io::containers::details::str_btree_find(static_cast<node_type *>(this->imp.root), key.ptr, key.n);
		return {ptr, pos, this->imp.rightmost};
	}
	constexpr bool is_empty() const noexcept
	{
		return this->imp.root == nullptr;
	}
	constexpr size_type erase_key(string_view_type key) noexcept
	{
		return ::fast_io::containers::details::str_btree_erase<allocator_type, keys_number, node_type>(this->imp, key.ptr, key.n);
	}
	constexpr bool insert_key(string_view_type key) noexcept
	{
		return ::fast_io::containers::details::str_btree_insert_key_with_root<allocator_type, keys_number, node_type>(this->imp, key.ptr, key.n);
	}

private:
	static inline constexpr void clear_node(void *nodev) noexcept
	{
		auto node{static_cast<node_type *>(nodev)};
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
		clear_node(this->imp.root);
		this->imp = {nullptr, nullptr, nullptr};
	}

	constexpr void clear_destroy() noexcept
	{
		this->clear();
	}

	constexpr basic_str_btree_set(basic_str_btree_set const &) noexcept = delete;
	constexpr basic_str_btree_set &operator=(basic_str_btree_set const &) noexcept = delete;

	constexpr basic_str_btree_set(basic_str_btree_set &&other) noexcept : imp(other.imp)
	{
		other.imp = {nullptr, nullptr, nullptr};
	}
	constexpr basic_str_btree_set &operator=(basic_str_btree_set &&other) noexcept
	{
		if (__builtin_addressof(other) == this)
		{
			return *this;
		}
		this->imp = other.imp;
		other.imp = {nullptr, nullptr, nullptr};
		return *this;
	}
	constexpr const_iterator cbegin() const noexcept
	{
		return {this->imp.leftmost, 0, this->imp.rightmost};
	}

	constexpr const_iterator cend() const noexcept
	{
		return {nullptr, 0, this->imp.rightmost};
	}
	constexpr const_iterator begin() const noexcept
	{
		return this->cbegin();
	}
	constexpr const_iterator end() const noexcept
	{
		return this->cend();
	}

	constexpr const_reverse_iterator crbegin() const noexcept
	{
		return reverse_iterator(cend());
	}

	static inline constexpr size_type max_size() noexcept
	{
		constexpr size_type mx{::std::numeric_limits<size_type>::max() / sizeof(node_type)};
		return mx;
	}

	static inline constexpr size_type max_size_bytes() noexcept
	{
		constexpr size_type mx{::std::numeric_limits<size_type>::max() / sizeof(node_type) * sizeof(node_type)};
		return mx;
	}

	constexpr const_reverse_iterator crend() const noexcept
	{
		return reverse_iterator(cbegin());
	}
	constexpr const_reverse_iterator rbegin() const noexcept
	{
		return this->crbegin();
	}
	constexpr const_reverse_iterator rend() const noexcept
	{
		return this->crend();
	}
	constexpr cstring_view_type front() const noexcept
	{
		if (this->imp.leftmost == nullptr) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		return this->front_unchecked();
	}
	constexpr cstring_view_type back() const noexcept
	{
		if (this->imp.rightmost == nullptr) [[unlikely]]
		{
			::fast_io::fast_terminate();
		}
		return this->back_unchecked();
	}
	constexpr cstring_view_type front_unchecked() const noexcept
	{
		return static_cast<::fast_io::containers::details::str_btree_set_node<char, keys_number> const *>(this->imp.leftmost)->keys->strvw();
	}
	constexpr cstring_view_type back_unchecked() const noexcept
	{
		auto &e{*static_cast<::fast_io::containers::details::str_btree_set_node<char, keys_number> const *>(this->imp.rightmost)};
		return e.keys[e.size - 1u].strvw();
	}
	constexpr ~basic_str_btree_set()
	{
		clear_node(this->imp.root);
	}
};

} // namespace fast_io::containers
