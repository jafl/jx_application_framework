#include <jTypes.h>
#include <jCommandLine.h>
#include <stdio.h>
#include <jAssert.h>

int main()
{
	cout << "True:  " << kJTrue  << endl;
	cout << "False: " << kJFalse << endl;

	cout << "should see:" << endl << "TT" << endl;

	JBoolean b;
/*	b = 3;
	if (b)				cout << 'T';
	if (b == kJTrue)		cout << 'T';
	if (!b)				cout << 'F';
	if (b == kJFalse)	cout << 'F';

	b = 0;
	if (!b)				cout << 'T';
	if (b == kJFalse)	cout << 'T';
	if (b)				cout << 'F';
	if (b == kJTrue)		cout << 'F';

	b = (5 > 3);
	if (b)				cout << 'T';
	if (b == kJTrue)		cout << 'T';
	if (!b)				cout << 'F';
	if (b == kJFalse)	cout << 'F';
*/
	b = JF2B(0.1);
	if (b)				cout << 'T';
	if (b == kJTrue)		cout << 'T';
	if (!b)				cout << 'F';
	if (b == kJFalse)	cout << 'F';

	cout << endl;

	b = kJTrue;
	assert( b );
	b = kJFalse;
	assert( !b );

	cout << "Enter a boolean value: ";
	cin >> b;
	if (cin.good())
		{
		cout << "You entered " << b << endl;
		}
	else
		{
		cout << "You entered an invalid value." << endl;
		}
	JInputFinished();

	return 0;
}

//	operator void*() const
//		{
//		return (void*)(itsValue ? this : NULL);
//		}
