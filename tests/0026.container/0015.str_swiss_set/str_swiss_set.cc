#include <fast_io.h>
#include <fast_io_dsal/str_swiss_set.h>

int main()
{
	::fast_io::str_swiss_set sset;

	sset.insert_key("hello");
	sset.insert_key("world");
	sset.insert_key("foo");
	sset.insert_key("bar");
	sset.insert_key("baz");
	sset.insert_key("quux");
	sset.insert_key("corge");
	sset.insert_key("grault");
	sset.insert_key("garply");
	sset.insert_key("waldo");
	sset.insert_key("fred");
	sset.insert_key("plugh");
	sset.insert_key("xyzzy");
	sset.insert_key("thud");
	sset.insert_key("abcdefghijklmnop");
	sset.insert_key("the quick brown fox jumps over the lazy dog");
	sset.insert_key("lorem ipsum dolor sit amet consectetur adipiscing elit");
	sset.insert_key("a");
	sset.insert_key("ab");
	sset.insert_key("abc");
	sset.insert_key("abcd");
	sset.insert_key("abcde");
	sset.insert_key("abcdef");
	sset.insert_key("abcdefg");
	sset.insert_key("abcdefgh");
	sset.insert_key("abcdefghi");
	sset.insert_key("abcdefghij");
	sset.insert_key("abcdefghijk");
	sset.insert_key("abcdefghijkl");
	sset.insert_key("abcdefghijklm");
	sset.insert_key("abcdefghijklmn");
	sset.insert_key("abcdefghijklmno");
	sset.insert_key("1234567890");
	sset.insert_key("0987654321");
	sset.insert_key("test");
	sset.insert_key("testing");
	sset.insert_key("tester");
	sset.insert_key("tested");
	sset.insert_key("testes");

	::fast_io::io::println("size: ", sset.size());

	::fast_io::io::println(sset.contains("hello"));
	::fast_io::io::println(sset.contains("world"));
	::fast_io::io::println(sset.contains("foo"));
	::fast_io::io::println(sset.contains("bar"));
	::fast_io::io::println(sset.contains("baz"));
	::fast_io::io::println(sset.contains("quux"));
	::fast_io::io::println(sset.contains("corge"));
	::fast_io::io::println(sset.contains("grault"));
	::fast_io::io::println(sset.contains("garply"));
	::fast_io::io::println(sset.contains("waldo"));
	::fast_io::io::println(sset.contains("fred"));
	::fast_io::io::println(sset.contains("plugh"));
	::fast_io::io::println(sset.contains("xyzzy"));
	::fast_io::io::println(sset.contains("thud"));
	::fast_io::io::println(sset.contains("abcdefghijklmnop"));
	::fast_io::io::println(sset.contains("the quick brown fox jumps over the lazy dog"));
	::fast_io::io::println(sset.contains("lorem ipsum dolor sit amet consectetur adipiscing elit"));
	::fast_io::io::println(sset.contains("a"));
	::fast_io::io::println(sset.contains("ab"));
	::fast_io::io::println(sset.contains("abc"));
	::fast_io::io::println(sset.contains("abcd"));
	::fast_io::io::println(sset.contains("abcde"));
	::fast_io::io::println(sset.contains("abcdef"));
	::fast_io::io::println(sset.contains("abcdefg"));
	::fast_io::io::println(sset.contains("abcdefgh"));
	::fast_io::io::println(sset.contains("abcdefghi"));
	::fast_io::io::println(sset.contains("abcdefghij"));
	::fast_io::io::println(sset.contains("abcdefghijk"));
	::fast_io::io::println(sset.contains("abcdefghijkl"));
	::fast_io::io::println(sset.contains("abcdefghijklm"));
	::fast_io::io::println(sset.contains("abcdefghijklmn"));
	::fast_io::io::println(sset.contains("abcdefghijklmno"));
	::fast_io::io::println(sset.contains("1234567890"));
	::fast_io::io::println(sset.contains("0987654321"));
	::fast_io::io::println(sset.contains("test"));
	::fast_io::io::println(sset.contains("testing"));
	::fast_io::io::println(sset.contains("tester"));
	::fast_io::io::println(sset.contains("tested"));
	::fast_io::io::println(sset.contains("testes"));

	// These should NOT be found
	::fast_io::io::println(sset.contains("notinserted"));
	::fast_io::io::println(sset.contains(""));
	::fast_io::io::println(sset.contains("hell"));
	::fast_io::io::println(sset.contains("helloo"));
	::fast_io::io::println(sset.contains("hello world"));
	::fast_io::io::println(sset.contains("foobar"));
	::fast_io::io::println(sset.contains("tes"));
	::fast_io::io::println(sset.contains("testin"));
	::fast_io::io::println(sset.contains("xyz"));
	::fast_io::io::println(sset.contains("THUD"));
}
