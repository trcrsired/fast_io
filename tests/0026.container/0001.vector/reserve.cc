#include <cstddef>
#include <new>
#include <fast_io.h>
#include <fast_io_dsal/vector.h>
using namespace fast_io::io;
using namespace fast_io::mnp;

// minimal allocator with debug output
struct NAlloc
{
	static void *allocate(std::size_t n) noexcept
	{
		void *p = ::operator new(n);
		return p;
	}
	static void deallocate_n(void *p, std::size_t) noexcept
	{
		::operator delete(p);
	}
	static void deallocate(void *p) noexcept
	{
		::operator delete(p);
	}
	constexpr bool operator==(::NAlloc const &) const noexcept = default;
};

using NAllocAdapter = fast_io::generic_allocator_adapter<NAlloc>;

int main()
{
	constexpr int max_elements = 32;

	// using reserve
	{
		fast_io::vector<int, ::NAllocAdapter> v1;
		v1.reserve(max_elements); // reserves at least max_elements * sizeof(int) bytes

		for (int n = 0; n < max_elements; ++n)
		{
			v1.push_back(n);
		}
		for (int n = 0; n < max_elements; ++n)
		{
			int v{v1[n]};
			if (v != n) {
				::fast_io::fast_terminate();
			}
		}
	}

	// not using reserve
	{
		fast_io::vector<int, ::NAllocAdapter> v1;

		for (int n = 0; n < max_elements; ++n)
		{
			v1.push_back(n);
		}
		for (int n = 0; n < max_elements; ++n)
		{
			int v{v1[n]};
			if (v != n) {
				::fast_io::fast_terminate();
			}
		}
	}
}
