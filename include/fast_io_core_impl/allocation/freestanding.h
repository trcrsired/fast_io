#pragma once

namespace fast_io
{

class freestanding_global_allocator
{
public:
static ::fast_io::allocation_least_result allocate_aligned_at_least_conditional_zero(::std::size_t alignment, ::std::size_t n, bool zero) noexcept;
static void deallocate_aligned_n(void* p, ::std::size_t alignment, ::std::size_t n) noexcept;
};

#if defined(FAST_IO_DISABLE_FREESTANDING_THREAD_LOCAL_ALLOCATOR)
using freestanding_thread_local_allocator = freestanding_global_allocator;
#else
class freestanding_thread_local_allocator
{
public:
static ::fast_io::allocation_least_result allocate_aligned_at_least_conditional_zero(::std::size_t alignment, ::std::size_t n, bool zero) noexcept;
static void deallocate_aligned_n(void* p, ::std::size_t alignment, ::std::size_t n) noexcept;
};
#endif

} // namespace fast_io
