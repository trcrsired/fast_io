
#include <fast_io.h>
#include <fast_io_dsal/str_btree_set.h>
#if 0
template <::std::integral chtype, ::std::size_t keys_number>
inline constexpr void str_btree_set_inorder_traverse(
	::fast_io::containers::details::str_btree_set_node<chtype, keys_number> *node,
	auto &&callback) noexcept
{
//	::fast_io::io::debug_println(::std::source_location::current()," node->size=",node->size, " node->leaf=",node->leaf);
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
//	::fast_io::io::debug_println(::std::source_location::current()," node->size=",node->size, " node->leaf=",node->leaf, " i=",i);
		// **Visit current key**
		callback(node->keys[i].strvw());
	}
//	::fast_io::io::debug_println(::std::source_location::current()," node->size=",node->size, " node->leaf=",node->leaf);

	// **Traverse rightmost subtree**
	if (!node->leaf)
	{
		str_btree_set_inorder_traverse<chtype, keys_number>(node->childrens[node->size], callback);
	}
}
#endif
int main()
{
	::fast_io::str_btree_set bset;
	constexpr ::fast_io::string_view fruits[]{
		::fast_io::string_view("kiwi"),
		::fast_io::string_view("ugli"),
		::fast_io::string_view("lemon"),
		::fast_io::string_view("fig"),
		::fast_io::string_view("zucchini"),
		::fast_io::string_view("honeydew"),
		::fast_io::string_view("raspberry"),
		::fast_io::string_view("orange"),
		::fast_io::string_view("vanilla"),
		::fast_io::string_view("yellowfruit"),
		::fast_io::string_view("date"),
		::fast_io::string_view("grape"),
		::fast_io::string_view("strawberry"),
		::fast_io::string_view("cherry"),
		::fast_io::string_view("quince"),
		::fast_io::string_view("apple"),
		::fast_io::string_view("elderberry"),
		::fast_io::string_view("banana"),
		::fast_io::string_view("nectarine"),
		::fast_io::string_view("watermelon"),
		::fast_io::string_view("xigua"),
		::fast_io::string_view("papaya"),
		::fast_io::string_view("mango")};

	for (auto const &e : fruits)
	{
		bset.insert_key(e);
	}

	for (auto const &e : bset)
	{
		::fast_io::io::println(e);
	}

	::fast_io::io::print("\nreverse:\n");
	for (auto const &e : ::std::ranges::reverse_view(bset))
	{
		::fast_io::io::println(e);
	}

	::fast_io::io::println("\nfront()=", bset.front(), "\tback()=", bset.back());
}
