#include <iostream.h>
int main()
{
	if (int test = (5 > 3))
		{
		cout << test << endl;
		}
	else
		{
		cout << test << endl;
		}
	cout << test << endl;

	int u = 3;
	int x;
	for (int s=0; s<=1; s++)
		{
		if (int t = u || s)
			{
			x = t+1;
			}
		if (s || int t = u)
			{
			x = t+1;
			}
		}

	int y = u ? (int t=u) : 0;
	int z = t;
}
