// Iterator accessors are [[unsafe]] in every container family, so a
// source-written begin() is rejected from safe source.
#include <fast_io_hosted.h>
#include <fast_io_dsal/deque.h>
#include <fast_io_dsal/list.h>
#include <fast_io_dsal/array.h>

int main()
{
    fast_io::containers::deque<int, fast_io::native_global_allocator> d;
    (void)d.begin();
    fast_io::containers::list<int, fast_io::native_global_allocator> l;
    (void)l.begin();
}
