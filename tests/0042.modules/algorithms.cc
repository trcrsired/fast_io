// module consumer: the algorithm library through `import fast_io`.
//
//   clang++ --config=... -std=c++26 -fherbceptions -fno-exceptions -fno-rtti \
//       --precompile -x c++-module share/fast_io/fast_io.cppm -I include \
//       -o <dir>/fast_io.pcm
//   clang++ --config=... -std=c++26 -fherbceptions -lherbceptions \
//       -fprebuilt-module-path=<dir> tests/0042.modules/algorithms.cc -o algorithms
//
// Every name used below has to come from the module, so this fails to compile
// the moment fast_io_inc/algorithms.inc stops exporting something.
#include <cstddef>
import fast_io;

struct point
{
	int key;
	int payload;
};

struct descending
{
	template <typename T>
	inline constexpr auto compare(T const &a, T const &b) const noexcept
	{
		return b <=> a;
	}
};

struct even_pred
{
	inline constexpr bool operator()(int i) const noexcept
	{
		return (i & 1) == 0;
	}
};

int main()
{
	try
	{
		::fast_io::vector<int> v{5, 3, 1, 4, 2, 9, 8, 7, 6, 0};

		// sort / is_sorted / stable order helpers
		::fast_io::algorithms::sort(v);
		if (!::fast_io::algorithms::is_sorted(v))
		{
			return 1;
		}
		::fast_io::algorithms::sort(v, descending{});
		if (!::fast_io::algorithms::is_sorted(v, descending{}))
		{
			return 2;
		}
		::fast_io::algorithms::reverse(v);
		::fast_io::algorithms::stable_partition(v, even_pred{});
		if (!::fast_io::algorithms::is_partitioned(v, even_pred{}))
		{
			return 3;
		}

		// find family
		auto it{::fast_io::algorithms::find(v, 4)};
		if (it == v.end())
		{
			return 4;
		}
		auto idx{::fast_io::algorithms::find_index_of(v, 4)};
		if (v[idx] != 4)
		{
			return 5;
		}
		if (::fast_io::algorithms::count_if(v, even_pred{}) != 5)
		{
			return 6;
		}

		// numeric families
		::fast_io::vector<int> iota_buf(8);
		::fast_io::algorithms::iota(iota_buf, 0);
		if (::fast_io::algorithms::accumulate(iota_buf, 0) != 28)
		{
			return 7;
		}
		if (::fast_io::algorithms::reduce(iota_buf, 0) != 28)
		{
			return 8;
		}
		::fast_io::vector<int> diff(8);
		::fast_io::algorithms::adjacent_difference(iota_buf, diff.begin());
		::fast_io::vector<int> sums(8);
		::fast_io::algorithms::partial_sum(iota_buf, sums.begin());
		if (::fast_io::algorithms::inner_product(iota_buf, iota_buf, 0) != 140)
		{
			return 9;
		}
		if (::fast_io::algorithms::gcd(12, 18) != 6 ||
			::fast_io::algorithms::lcm(4, 6) != 12)
		{
			return 10;
		}

		// copy / fill / move / transform / remove / replace
		::fast_io::vector<int> out(v.size());
		::fast_io::algorithms::copy(v, out.begin());
		if (!::fast_io::algorithms::equal(v, out))
		{
			return 11;
		}
		::fast_io::algorithms::fill(out, 7);
		if (::fast_io::algorithms::find_if_not(out, [](int x) { return x == 7; }) != out.end())
		{
			return 12;
		}
		::fast_io::algorithms::transform(v, out.begin(), [](int x) { return x + 1; });
		::fast_io::algorithms::replace(out, 1, 99);
		auto removed{::fast_io::algorithms::remove(out, 99)};
		out.resize(static_cast<::std::size_t>(removed - out.begin()));

		// heap family
		::fast_io::algorithms::make_heap(v);
		if (!::fast_io::algorithms::is_heap(v))
		{
			return 13;
		}
		::fast_io::algorithms::push_heap(v);
		::fast_io::algorithms::pop_heap(v);
		::fast_io::algorithms::sort_heap(v);

		// binary search family
		::fast_io::algorithms::sort(v);
		if (!::fast_io::algorithms::binary_search(v, 4))
		{
			return 14;
		}
		if (::fast_io::algorithms::lower_bound_index_of(v, 4) !=
			::fast_io::algorithms::upper_bound_index_of(v, 4) - 1)
		{
			return 15;
		}
		auto [lo, hi]{::fast_io::algorithms::equal_range(v, 4)};

		// set operations, merge, permutation
		::fast_io::vector<int> a{1, 3, 5, 7};
		::fast_io::vector<int> b{2, 3, 6, 7};
		::fast_io::vector<int> u(8);
		auto ue{::fast_io::algorithms::set_union(a, b, u.begin())};
		u.resize(static_cast<::std::size_t>(ue - u.begin()));
		if (!::fast_io::algorithms::includes(u, a))
		{
			return 16;
		}
		::fast_io::vector<int> m(8);
		auto me{::fast_io::algorithms::merge(a, b, m.begin())};
		m.resize(static_cast<::std::size_t>(me - m.begin()));
		::fast_io::algorithms::next_permutation(m);

		// swap / rotate / shift / unique / search
		::fast_io::algorithms::swap(a[0], a[1]);
		::fast_io::algorithms::rotate(a, a.begin() + 1);
		::fast_io::algorithms::shift_left(a, 1);
		::fast_io::algorithms::unique(a);
		::fast_io::algorithms::search(v, a);
		::fast_io::algorithms::mismatch(v, v);
		::fast_io::algorithms::lexicographical_compare(v, v);

		// comparators and numeric operation objects
		if (!(::fast_io::algorithms::compare_three_way{}.compare(1, 2) < 0))
		{
			return 17;
		}
		point p1{2, 0}, p2{1, 0};
		::fast_io::algorithms::compare_by by_key{&point::key};
		if (!(by_key.compare(p1, p2) > 0))
		{
			return 22;
		}
		::fast_io::algorithms::descend_compare_by by_key_desc{&point::key};
		if (!(by_key_desc.compare(p1, p2) < 0))
		{
			return 23;
		}
		if (::fast_io::algorithms::compare_by_apply(&point::key, p1) != 2)
		{
			return 24;
		}
		if (!::fast_io::algorithms::equal_to{}.equal(3, 3))
		{
			return 18;
		}
		if (::fast_io::algorithms::field_ops::add{}.compute(2, 3) != 5)
		{
			return 19;
		}


		// sort a deque through the container customization point
		::fast_io::deque<int> d;
		for (int i{9}; i != -1; --i)
		{
			d.push_back(i);
		}
		::fast_io::algorithms::sort(d);
		if (!::fast_io::algorithms::is_sorted(d))
		{
			return 21;
		}

		// index_range validates the two indices
		::fast_io::algorithms::sort_index_range(v, 0, v.size());
		::fast_io::algorithms::reverse_index_range(v, 0, 4);

		(void)lo;
		(void)hi;
		::fast_io::io::print("algorithms through the module: ok\n");
	}
	catch throws(::std::error e)
	{
		::fast_io::io::perrln("unexpected error: ", e);
		return 1;
	}
	return 0;
}
