#include <fast_io.h>
#include <fast_io_dsal/string.h>
#include <fast_io_dsal/string_view.h>
#include <fast_io_dsal/impl/associative_string.h>
#include <fast_io_dsal/impl/str_btree_set.h>
#include <fast_io_device.h>

template <::std::integral chtype, ::std::size_t keys_number>
inline constexpr void str_btree_set_inorder_traverse(
	::fast_io::containers::details::str_btree_set_node<chtype, keys_number> *node,
	auto &&callback) noexcept
{
	if (node == nullptr)
	{
		return;
	}

	// **Traverse left subtree**
	for (::std::size_t i{}; i != node->size; ++i)
	{
		if (!node->leaf)
		{
			str_btree_set_inorder_traverse<chtype, keys_number>(node->childrens[i], callback);
		}

		// **Visit current key**
		callback(node->keys[i].strvw());
	}

	// **Traverse rightmost subtree**
	if (!node->leaf)
	{
		str_btree_set_inorder_traverse<chtype, keys_number>(node->childrens[node->size], callback);
	}
}

int main()
{
	::fast_io::containers::basic_str_btree_set<char, ::fast_io::native_global_allocator, 4> bset;
	::fast_io::ibuf_file ibf("name.txt");
	for(::fast_io::string str; scan<true>(ibf, str);)
	{
		bset.insert_key(::fast_io::mnp::os_c_str(str));
	};

	str_btree_set_inorder_traverse(bset.root, [](::fast_io::string_view v) {
		::fast_io::io::println(v);
	});
}
