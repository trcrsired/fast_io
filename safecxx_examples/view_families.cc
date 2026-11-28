// Safe C++ experiment (fast_io safecxx branch): string_view / cstring_view /
// index_span. Like span.h, these kernels live in an [[unsafe]] fence and state
// their borrow relationships with the public [[lifetime("this")]] mark, so a
// view cannot outlive the owner it was built from.
//
// Build with the safe compiler:
//   clang++ --config=$HOME/cfgs/x86_64-linux-gnu-libcxx.cfg \
//     -fsafecxx -fherbceptions -lherbceptions -O3 -s -flto=thin \
//     -Iinclude safecxx_examples/view_families.cc -o /tmp/view_families

[[unsafe]] {
#include <fast_io_dsal/impl/misc/push_macros.h>
#include <fast_io_dsal/impl/misc/push_warnings.h>
#include <fast_io_core.h>
#include <fast_io_dsal/impl/freestanding.h>
#include <fast_io_dsal/impl/common.h>
}

#include <fast_io_dsal/vector.h>
#include <fast_io_dsal/string_view.h>
#include <fast_io_dsal/index_span.h>

static fast_io::string_view make_string(in fast_io::vector<char> v) noexcept
{
	return fast_io::string_view(fast_io::freestanding::from_range, v);
}

static fast_io::cstring_view make_cstring(in fast_io::vector<char> v) noexcept
{
	return fast_io::cstring_view(fast_io::null_terminated,
		fast_io::freestanding::from_range, v);
}

// index_span needs mutable elements, so it takes an inout owner; iv has no
// outstanding view, so the freeze rule does not apply to it.
static fast_io::index_span<int, 1> make_index(inout fast_io::vector<int> v) noexcept
{
	return fast_io::index_span<int, 1>(fast_io::freestanding::from_range, v);
}

int main()
{
	fast_io::vector<char> v;
	v.push_back('a');

	auto s = make_string(v);
	auto sub = s.subview(0, 1);
	auto csv = make_cstring(v);

	fast_io::vector<int> iv;
	iv.push_back(7);
	auto is = make_index(iv);
	auto sp = is.subspan(0, 1);

	return (sub.size() == 1 && csv.size() == 1 && sp[0] == 7) ? 0 : 1;
}
