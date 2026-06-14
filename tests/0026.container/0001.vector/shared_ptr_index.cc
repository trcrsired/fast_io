#include <cassert>
#include <fast_io_dsal/vector.h>
#include <fast_io.h>
#include <memory>

namespace fast_io::freestanding
{

template <typename T>
struct is_trivially_copyable_or_relocatable<::std::shared_ptr<T>>
{
	static inline constexpr bool value = true;
};

} // namespace fast_io::freestanding

int main()
{
	::fast_io::vector<::std::shared_ptr<int>> vec{std::make_shared<int>(1), std::make_shared<int>(2), std::make_shared<int>(3)};
	assert(vec.size() == 3);
	vec.insert_index(1, std::make_shared<int>(4));
	assert(vec.size() == 4);
	assert(*vec[0] == 1);
	assert(*vec[1] == 4);
	assert(*vec[2] == 2);
	assert(*vec[3] == 3);
	assert(vec[1].use_count() == 1);
	using namespace ::fast_io::io;
	print("After vec.insert_index(1)\n");
	for (auto const &e : vec)
	{
		println(::fast_io::mnp::pointervw(e.get()), " use_count: ", e.use_count());
	}
	vec.erase_index(0, 2);
	assert(vec.size() == 2);
	assert(*vec[0] == 2);
	assert(*vec[1] == 3);
	print("After vec.erase_index(0,2)\n");
	for (auto const &e : vec)
	{
		println(::fast_io::mnp::pointervw(e.get()), " use_count: ", e.use_count());
	}
}
