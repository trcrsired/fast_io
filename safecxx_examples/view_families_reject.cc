// Safe C++ experiment (fast_io safecxx branch): view families — REJECT cases.
//
// Expected to be rejected by the safe compiler. Each function builds a view
// from a local owner and lets it escape; [[lifetime("this")]] on the borrowing
// constructor/subview makes the provenance visible to the analysis.
//
//   clang++ --config=$HOME/cfgs/x86_64-linux-gnu-libcxx.cfg \
//     -std=c++26 -fsafecxx -fherbceptions -fsyntax-only -Iinclude \
//     safecxx_examples/view_families_reject.cc
//
// Expected diagnostics: -Werror,-Wlifetime-safety-return-stack-addr on each
// return of a view over the local `v`.

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

static auto bad_string() noexcept
{
	fast_io::vector<char> v;
	v.push_back('a');
	return fast_io::string_view(fast_io::freestanding::from_range, v);
}

static auto bad_cstring() noexcept
{
	fast_io::vector<char> v;
	v.push_back('a');
	return fast_io::cstring_view(fast_io::null_terminated,
		fast_io::freestanding::from_range, v);
}

static auto bad_index() noexcept
{
	fast_io::vector<int> v;
	v.push_back(1);
	return fast_io::index_span<int, 1>(fast_io::freestanding::from_range, v);
}

static auto bad_subview() noexcept
{
	fast_io::vector<char> v;
	v.push_back('a');
	fast_io::string_view s(fast_io::freestanding::from_range, v);
	return s.subview(0, 1);
}

int main() { return 0; }
