#include <jCommandLine.h>
#include <string.h>

std::istream&
operator>>(std::istream& input, bool& b);

std::ostream&
operator<<(std::ostream& output, const bool b);

int main()
{
	const bool b1 = true;
	const bool b2 = false;
	const bool b3 = b1 && b2;
	const bool b4 = (strcmp("hello", "hi") == 0);
	const bool b5 = (7 > 5);
	const bool b6 = b1 || b2;

	std::cout << "TFFFTT" << std::endl;
	std::cout << b1 << b2 << b3 << b4 << b5 << b6 << std::endl;
	std::cout << std::endl;

	bool bin;
	std::cout << "Enter a boolean value: ";
	std::cin >> bin;
	if (std::cin.good())
		{
		std::cout << "You entered " << bin << std::endl;
		}
	else
		{
		std::cout << "You entered an invalid value." << std::endl;
		}
	JInputFinished();

	return 0;
}
