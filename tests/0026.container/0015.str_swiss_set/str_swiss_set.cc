#include <fast_io.h>
#include <fast_io_dsal/str_swiss_set.h>

int main()
{
	using set_type = ::fast_io::containers::basic_str_swiss_set<char, ::fast_io::native_global_allocator>;
	using string_view_type = ::fast_io::containers::basic_string_view<char>;

	// Test 1: Insert into empty set
	{
		set_type s;
		auto r = s.insert_key(string_view_type("hello"));
		if (!r)
		{
			::fast_io::io::println("FAIL: insert into empty set returned false");
			return 1;
		}
		if (!s.contains(string_view_type("hello")))
		{
			::fast_io::io::println("FAIL: contains after insert returned false");
			return 2;
		}
		::fast_io::io::println("PASS: insert into empty set");
	}

	// Test 2: Duplicate detection
	{
		set_type s;
		s.insert_key(string_view_type("hello"));
		auto r = s.insert_key(string_view_type("hello"));
		if (r)
		{
			::fast_io::io::println("FAIL: duplicate insert returned true");
			return 3;
		}
		if (!s.contains(string_view_type("hello")))
		{
			::fast_io::io::println("FAIL: contains after duplicate insert returned false");
			return 4;
		}
		::fast_io::io::println("PASS: duplicate detection");
	}

	// Test 3: Contains negative
	{
		set_type s;
		s.insert_key(string_view_type("hello"));
		if (s.contains(string_view_type("world")))
		{
			::fast_io::io::println("FAIL: contains returned true for non-existent key");
			return 5;
		}
		::fast_io::io::println("PASS: contains negative");
	}

	// Test 4: Contains on empty set
	{
		set_type s;
		if (s.contains(string_view_type("hello")))
		{
			::fast_io::io::println("FAIL: contains on empty set returned true");
			return 6;
		}
		::fast_io::io::println("PASS: contains on empty set");
	}

	// Test 5: Multiple inserts (triggering table growth)
	{
		set_type s;
		constexpr ::std::size_t num_keys = 100;
		for (::std::size_t i = 0; i != num_keys; ++i)
		{
			char buf[32];
			auto len = ::std::snprintf(buf, sizeof(buf), "key%zu", i);
			s.insert_key(string_view_type(buf, static_cast<::std::size_t>(len)));
		}
		// Verify some keys exist
		if (!s.contains(string_view_type("key0")))
		{
			::fast_io::io::println("FAIL: key0 not found");
			return 7;
		}
		if (!s.contains(string_view_type("key42")))
		{
			::fast_io::io::println("FAIL: key42 not found");
			return 8;
		}
		if (!s.contains(string_view_type("key99")))
		{
			::fast_io::io::println("FAIL: key99 not found");
			return 9;
		}
		if (s.contains(string_view_type("key100")))
		{
			::fast_io::io::println("FAIL: key100 found but should not exist");
			return 10;
		}
		::fast_io::io::println("PASS: multiple inserts with table growth");
	}

	// Test 6: Move constructor
	{
		set_type s1;
		s1.insert_key(string_view_type("hello"));
		s1.insert_key(string_view_type("world"));

		set_type s2(::std::move(s1));

		if (!s2.contains(string_view_type("hello")))
		{
			::fast_io::io::println("FAIL: moved-to set missing key");
			return 11;
		}
		if (s1.contains(string_view_type("hello")))
		{
			::fast_io::io::println("FAIL: moved-from set still has key");
			return 12;
		}
		::fast_io::io::println("PASS: move constructor");
	}

	// Test 7: Move assignment
	{
		set_type s1;
		s1.insert_key(string_view_type("hello"));

		set_type s2;
		s2.insert_key(string_view_type("world"));
		s2 = ::std::move(s1);

		if (!s2.contains(string_view_type("hello")))
		{
			::fast_io::io::println("FAIL: move-assigned set missing key");
			return 13;
		}
		if (s1.contains(string_view_type("hello")))
		{
			::fast_io::io::println("FAIL: move-assigned-from set still has key");
			return 14;
		}
		::fast_io::io::println("PASS: move assignment");
	}

	// Test 8: Self move assignment
	{
		set_type s;
		s.insert_key(string_view_type("hello"));
		s = ::std::move(s);
		if (!s.contains(string_view_type("hello")))
		{
			::fast_io::io::println("FAIL: self move assignment lost key");
			return 15;
		}
		::fast_io::io::println("PASS: self move assignment");
	}

	// Test 9: Empty set destructor (no allocations)
	{
		set_type s;
	}
	::fast_io::io::println("PASS: empty set destructor");

	::fast_io::io::println("\nAll tests passed!");
	return 0;
}
