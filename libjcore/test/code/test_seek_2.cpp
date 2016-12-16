// 4567890

#include <iostream>

int
main()
{
	char c;
	std::ifstream input("test.seek.cc");
	filebuf* buf = input.rdbuf();

	buf->pubseekpos(4);
	c = input.get();
	std::cout << "(5?) char '" << c << "', value " << (int) c << std::endl;
	std::cout << "(5?) position: " << (buf->pubseekoff(0, std::ios::cur)).offset() << std::endl;

	buf->pubseekoff(2, std::ios::cur);
	c = input.get();
	std::cout << "(8?) char '" << c << "', value " << (int) c << std::endl;
	std::cout << "(8?) position: " << (buf->pubseekoff(0, std::ios::cur)).offset() << std::endl;

	buf->pubseekoff(-2, std::ios::end);
	c = input.get();
	std::cout << "(}?) char '" << c << "', value " << (int) c << std::endl;

	std::cout << "file length: " << (buf->pubseekoff(0, std::ios::end)).offset() << std::endl;

	return 0;
}
