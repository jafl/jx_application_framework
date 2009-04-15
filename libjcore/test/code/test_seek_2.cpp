// 4567890

#include <iostream>

int
main()
{
	char c;
	ifstream input("test.seek.cc");
	filebuf* buf = input.rdbuf();

	buf->pubseekpos(4);
	c = input.get();
	cout << "(5?) char '" << c << "', value " << (int) c << endl;
	cout << "(5?) position: " << (buf->pubseekoff(0, ios::cur)).offset() << endl;

	buf->pubseekoff(2, ios::cur);
	c = input.get();
	cout << "(8?) char '" << c << "', value " << (int) c << endl;
	cout << "(8?) position: " << (buf->pubseekoff(0, ios::cur)).offset() << endl;

	buf->pubseekoff(-2, ios::end);
	c = input.get();
	cout << "(}?) char '" << c << "', value " << (int) c << endl;

	cout << "file length: " << (buf->pubseekoff(0, ios::end)).offset() << endl;

	return 0;
}
