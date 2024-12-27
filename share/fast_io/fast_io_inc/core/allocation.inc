export namespace fast_io
{
using ::fast_io::generic_allocator_adapter;
using ::fast_io::typed_generic_allocator_adapter;
using ::fast_io::native_global_allocator;
using ::fast_io::native_typed_global_allocator;
using ::fast_io::native_thread_local_allocator;
using ::fast_io::native_typed_thread_local_allocator;

#if (defined(_WIN32) || defined(__CYGWIN__)) && !defined(__WINE__)
using ::fast_io::nt_rtlallocateheap_allocator;
using ::fast_io::win32_heapalloc_allocator;
#endif

#if ((__STDC_HOSTED__ == 1 && (!defined(_GLIBCXX_HOSTED) || _GLIBCXX_HOSTED == 1) && \
	  !defined(_LIBCPP_FREESTANDING)) ||                                             \
	 defined(FAST_IO_ENABLE_HOSTED_FEATURES))
using ::fast_io::c_malloc_allocator;
#if defined(_DEBUG) && defined(_MSC_VER)
using ::fast_io::wincrt_malloc_dbg_allocator;
#endif
#endif

#if (defined(__linux__) && defined(__KERNEL__)) || defined(FAST_IO_USE_LINUX_KERNEL_ALLOCATOR)
using ::fast_io::linux_kmalloc_allocator;
#endif

#if (defined(FAST_IO_ENABLE_MIMALLOC) || defined(FAST_IO_USE_MIMALLOC)) && (!defined(_MSC_VER) || defined(__clang__))
using ::fast_io::mimalloc_allocator;
#endif
} // namespace fast_io