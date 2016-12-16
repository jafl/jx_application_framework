// Demonstrates problem with seekp(0, std::ios::beg) on Symantec C++ 7.0

// seekp(0) works correctly on Symantec C++ 7.0
// seekp(0, std::ios::beg) sets std::ios::badbit with Symantec C++ 7.0.

// all of this code works with gcc 2.5.8, gcc 2.6.3, Metrowerks 1.3

#include <jFStreamUtil.h>
#include <stdio.h>

const char* kFileName = "test.seekg.dat";

void mainsub();

int main()
{
	#ifdef __SC__
		_ftype = 'TEXT';	// request TEXT file -- Symantec C++ specific
	#endif

	mainsub();

	return 0;
}


void mainsub()
{
	std::fstream file(kFileName, kJTextFile | std::ios::trunc);

	file << "This is a test";

	std::cout << file.rdstate() << std::endl;

//	file.seekp(0);					// works
	file.seekp(0, std::ios::beg);		// fails

	std::cout << file.rdstate() << std::endl;

	char c;
	file >> c;
	std::cout << "First character: " << c << std::endl;

	std::cout << file.rdstate() << std::endl;

//	file.seekp(length);				// works
	file.seekp(0, std::ios::end);		// fails

	std::cout << file.rdstate() << std::endl;

	file << std::endl << "This is line 2";
}
