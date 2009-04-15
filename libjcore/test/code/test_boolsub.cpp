#include <jTypes.h>

istream&
operator>>(istream& input, bool& b);

istream&
operator>>(istream& input, bool& b)
{
	input >> ws;
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
		input.setstate(ios::failbit);
		}

	return input;
}

ostream&
operator<<(ostream& output, const bool b);

ostream&
operator<<(ostream& output, const bool b)
{
	output << (b ? 'T' : 'F');
	return output;
}
