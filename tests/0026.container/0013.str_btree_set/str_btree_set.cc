
#include <fast_io.h>
#include <fast_io_dsal/str_btree_set.h>

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

	::fast_io::io::print("\nfront()=", bset.front(), "\tback()=", bset.back(), "\nbset.find(\"xigua\"):\n");
	{
		auto it{bset.find("xigua")};
		if (it == bset.cend())
		{
			::fast_io::io::print("not found\n");
		}
		else
		{
			::fast_io::io::println("found: ", *it);
		}
	}

	::fast_io::io::print("bset.find(\"buffalo\"):\n");
	{
		auto it{bset.find("buffalo")};
		if (it == bset.cend())
		{
			::fast_io::io::print("not found\n");
		}
		else
		{
			::fast_io::io::println("found: ", *it);
		}
	}
}
