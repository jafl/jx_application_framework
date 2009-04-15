// Demonstrates problem with seekp(0, ios::beg) on Symantec C++ 7.0

// seekp(0) works correctly on Symantec C++ 7.0
// seekp(0, ios::beg) sets ios::badbit with Symantec C++ 7.0.

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
	fstream file(kFileName, kJTextFile | ios::trunc);

	file << "This is a test";

	cout << file.rdstate() << endl;

//	file.seekp(0);					// works
	file.seekp(0, ios::beg);		// fails

	cout << file.rdstate() << endl;

	char c;
	file >> c;
	cout << "First character: " << c << endl;

	cout << file.rdstate() << endl;

//	file.seekp(length);				// works
	file.seekp(0, ios::end);		// fails

	cout << file.rdstate() << endl;

	file << endl << "This is line 2";
}
