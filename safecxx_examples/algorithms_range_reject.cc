// A range algorithm that returns an iterator is unsafe: the iterator carries
// no owner its borrow could be tied to. Use the *_index_of overload instead.
#include <fast_io_core.h>
#include <fast_io_hosted.h>
#include <fast_io_dsal/vector.h>

int main()
{
	::fast_io::vector<int> v;
	v.push_back(1);
	v.push_back(2);
	auto it = ::fast_io::algorithms::find(v, 2); // error: returns iterator
	(void)it;
}
