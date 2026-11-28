// All container families are classified: iterator accessors, unchecked
// operations, iterator-taking operations, move and swap are [[unsafe]].
// Index/checked access, push/pop, size queries and range-for stay safe.
#include <fast_io_hosted.h>
#include <fast_io_dsal/deque.h>
#include <fast_io_dsal/list.h>
#include <fast_io_dsal/array.h>

int main()
{
    fast_io::containers::deque<int, fast_io::native_global_allocator> d;
    d.push_back(1);
    int sum = static_cast<int>(d.size()) + d[0];
    fast_io::containers::list<int, fast_io::native_global_allocator> l;
    l.push_back(2);
    for (in auto x : d) sum += x;
    for (in auto x : l) sum += x;
    return sum - 4;
}
