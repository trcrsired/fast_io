// Manipulator results that borrow from their operands are expression-only:
// they may be created as temporaries inside a print/scan expression, but not
// stored in a variable or field (see manipulator_expression_reject.cc).
#include <fast_io.h>

int main()
{
	fast_io::print(fast_io::mnp::os_c_str_carr("hello world"), "\n");
}
