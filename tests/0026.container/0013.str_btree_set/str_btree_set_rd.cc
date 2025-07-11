#include <fast_io.h>
#include <fast_io_dsal/string_view.h>
#include <fast_io_dsal/impl/associative_string.h>
#include <fast_io_dsal/impl/str_btree_set.h>

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
	bset.insert_key("kiwi");
	bset.insert_key("ugli");
	bset.insert_key("lemon");
	bset.insert_key("fig");
	bset.insert_key("zucchini");
	bset.insert_key("honeydew");
	bset.insert_key("raspberry");
	bset.insert_key("orange");
	bset.insert_key("vanilla");
	bset.insert_key("yellowfruit");
	bset.insert_key("date");
	bset.insert_key("grape");
	bset.insert_key("strawberry");

	::fast_io::io::println(::std::source_location::current());
	str_btree_set_inorder_traverse(bset.root, [](::fast_io::string_view v) {
		::fast_io::io::println(v);
	});
	::fast_io::io::print("\n\n");
	bset.insert_key("cherry");
	bset.insert_key("quince");
	bset.insert_key("apple");
	bset.insert_key("elderberry");
	bset.insert_key("banana");

	::fast_io::io::println(::std::source_location::current());
	str_btree_set_inorder_traverse(bset.root, [](::fast_io::string_view v) {
		::fast_io::io::println(v);
	});
	::fast_io::io::print("\n\n");

	bset.insert_key("nectarine");
	::fast_io::io::println(::std::source_location::current());
	str_btree_set_inorder_traverse(bset.root, [](::fast_io::string_view v) {
		::fast_io::io::println(v);
	});
	::fast_io::io::print("\n\n");
	bset.insert_key("watermelon");
	bset.insert_key("xigua");
	bset.insert_key("papaya");
	bset.insert_key("mango");

	::fast_io::io::println(::std::source_location::current());
	str_btree_set_inorder_traverse(bset.root, [](::fast_io::string_view v) {
		::fast_io::io::println(v);
	});
	::fast_io::io::print("\n\n");
}
