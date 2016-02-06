#include <iostream>

int main()
{
	char s[10000];
	while (!std::cin.eof())
		{
		std::cout << "input: ";
		std::cin >> s;
		std::cout << s << std::endl;
		}

	return 0;
}
