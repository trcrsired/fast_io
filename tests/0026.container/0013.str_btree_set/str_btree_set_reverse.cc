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
	constexpr
		::fast_io::string_view fruits[]{
    ::fast_io::string_view("zucchini"),
    ::fast_io::string_view("yellowfruit"),
    ::fast_io::string_view("xigua"),
    ::fast_io::string_view("watermelon"),
    ::fast_io::string_view("vanilla"),
    ::fast_io::string_view("ugli"),
    ::fast_io::string_view("tangerine"),
    ::fast_io::string_view("strawberry"),
    ::fast_io::string_view("raspberry"),
    ::fast_io::string_view("quince"),
    ::fast_io::string_view("papaya"),
    ::fast_io::string_view("orange"),
    ::fast_io::string_view("nectarine"),
    ::fast_io::string_view("mango"),
    ::fast_io::string_view("lemon"),
    ::fast_io::string_view("kiwi"),
    ::fast_io::string_view("grape"),
    ::fast_io::string_view("fig"),
    ::fast_io::string_view("elderberry"),
    ::fast_io::string_view("date"),
    ::fast_io::string_view("cherry"),
    ::fast_io::string_view("banana"),
    ::fast_io::string_view("apple"),
    ::fast_io::string_view("honeydew")
};

	for(auto const & e : fruits)
	{
		bset.insert_key(e);
	}

	str_btree_set_inorder_traverse(bset.root, [](::fast_io::string_view v) {
		::fast_io::io::println(v);
	});
}
