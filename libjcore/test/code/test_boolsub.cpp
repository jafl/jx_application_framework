#include <jTypes.h>

std::istream&
operator>>(std::istream& input, bool& b);

std::istream&
operator>>(std::istream& input, bool& b)
{
	input >> std::ws;
	char c = input.get();
	if (c == 'T')
		{
		b = true;
		}
	else if (c == 'F')
		{
		b = false;
		}
	else
		{
		input.putback(c);
		input.setstate(std::ios::failbit);
		}

	return input;
}

std::ostream&
operator<<(std::ostream& output, const bool b);

std::ostream&
operator<<(std::ostream& output, const bool b)
{
	output << (b ? 'T' : 'F');
	return output;
}
