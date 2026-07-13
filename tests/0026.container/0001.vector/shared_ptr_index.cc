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
	if (!(vec.size() == 3)) ::fast_io::fast_terminate();
	vec.insert_index(1, std::make_shared<int>(4));
	if (!(vec.size() == 4)) ::fast_io::fast_terminate();
	if (!(*vec[0] == 1)) ::fast_io::fast_terminate();
	if (!(*vec[1] == 4)) ::fast_io::fast_terminate();
	if (!(*vec[2] == 2)) ::fast_io::fast_terminate();
	if (!(*vec[3] == 3)) ::fast_io::fast_terminate();
	if (!(vec[1].use_count() == 1)) ::fast_io::fast_terminate();
	using namespace ::fast_io::io;
	print("After vec.insert_index(1)\n");
	for (auto const &e : vec)
	{
		println(::fast_io::mnp::pointervw(e.get()), " use_count: ", e.use_count());
	}
	vec.erase_index(0, 2);
	if (!(vec.size() == 2)) ::fast_io::fast_terminate();
	if (!(*vec[0] == 2)) ::fast_io::fast_terminate();
	if (!(*vec[1] == 3)) ::fast_io::fast_terminate();
	print("After vec.erase_index(0,2)\n");
	for (auto const &e : vec)
	{
		println(::fast_io::mnp::pointervw(e.get()), " use_count: ", e.use_count());
	}
}
