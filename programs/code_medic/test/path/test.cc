#include <JString.h>
#include "code/JString.cc"
#include "code2/JString.cc"
#include <jAssert.h>

static JString kS = "hello";

int main(int argc, char** argv) {
	int x[10];
	JString s("hi");
	foo(x);
	bar();
	std::cout << kS << std::endl;
	assert( 0 );
	return 0;
}
