export namespace fast_io
{
namespace freestanding
{

using ::fast_io::freestanding::is_zero_default_constructible_v;
using ::fast_io::freestanding::is_trivially_copyable_or_relocatable_v;

using ::fast_io::freestanding::is_trivially_copyable_or_relocatable;
using ::fast_io::freestanding::is_zero_default_constructible;

using ::fast_io::freestanding::my_memcpy;
using ::fast_io::freestanding::my_memmove;
using ::fast_io::freestanding::my_memset;
using ::fast_io::freestanding::my_memcmp;
using ::fast_io::freestanding::nonoverlapped_bytes_copy_n;
using ::fast_io::freestanding::bytes_clear_n;

} // namespace freestanding
} // namespace fast_io
