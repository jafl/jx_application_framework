#include <jCommandLine.h>
#include <string.h>

istream&
operator>>(istream& input, bool& b);

ostream&
operator<<(ostream& output, const bool b);

int main()
{
	const bool b1 = true;
	const bool b2 = false;
	const bool b3 = b1 && b2;
	const bool b4 = (strcmp("hello", "hi") == 0);
	const bool b5 = (7 > 5);
	const bool b6 = b1 || b2;

	cout << "TFFFTT" << endl;
	cout << b1 << b2 << b3 << b4 << b5 << b6 << endl;
	cout << endl;

	bool bin;
	cout << "Enter a boolean value: ";
	cin >> bin;
	if (cin.good())
		{
		cout << "You entered " << bin << endl;
		}
	else
		{
		cout << "You entered an invalid value." << endl;
		}
	JInputFinished();

	return 0;
}
