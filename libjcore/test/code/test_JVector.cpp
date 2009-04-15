/******************************************************************************
 test_JVector.cc

	Program to test JVector class.

	Written by John Lindal.

 ******************************************************************************/

#include <JVector.h>
#include <JMatrix.h>
#include <jCommandLine.h>
#include <jAssert.h>

int
main()
{
JFloat v1e[] = {1.0, 1.0, 0.0};

JVector v1(3, v1e);												// constructor

	v1.SetElement(1,0.0);

JVector v2(3, 1.0,0.0,1.0);										// constructor

	cout << "We are in " << v1.GetDimensionCount() << " dimensions." << endl;

	cout << "v1 should be 0 1 0" << endl;
	cout << "v1 =" << v1 << endl;

	cout << "v2 should be 1 0 1" << endl;
	cout << "v2 =" << v2 << endl;

	cout << "v1 dot v2 should be 0" << endl;
	cout << "v1 dot v2 =" << JDotProduct(v1,v2) << endl;

	cout << "v1 outer product v2 should be 0 0 0, 1 0 1, 0 0 0" << endl;
	cout << "v1 outer product v2 =" << JOuterProduct(v1, v2) << endl;

	JWaitForReturn();

	v1 += v2;

	cout << "v1 should be 1 1 1" << endl;
	cout << "v1 =" << v1 << endl;

	cout << "|v1| should be 1.732" << endl;
	cout << "|v1| =" << v1.GetLength() << endl;

	v2 -= v1;

	cout << "v2 should be 0 -1 0" << endl;
	cout << "v2 =" << v2 << endl;

	cout << "v1 dot v2 should be -1" << endl;
	cout << "v1 dot v2 =" << JDotProduct(v1,v2) << endl;

	JWaitForReturn();

	cout << "Enter new v1: ";
	cin >> v1;
	JInputFinished();

	cout << "New v1 =" << v1 << endl;

	cout << "Unit vector from v1 =" << v1.UnitVector() << endl;
	v1.Normalize();
	cout << "Normalized v1       =" << v1 << endl;

JVector v3 = v2;												// copy constructor

	cout << "v3 should equal v2" << endl;
	cout << "v3 equals v2? " << (v3 == v2) << endl;

JVector v4 = v2 + v3;

	cout << "v4 should equal 0 -2 0" << endl;
	cout << "v4 =" << v4 << endl;

	cout << "v4 should not equal v2" << endl;
	cout << "v4 not equal to v2? " << (v4 != v2) << endl;

JVector v5(3);

	v5 = v4 - v2;												// assignment operator

	cout << "v5 should equal 0 -1 0" << endl;
	cout << "v5 =" << v5 << endl;

	JWaitForReturn();

	v5 *= 3.0;

	cout << "v5 should equal 0 -3 0" << endl;
	cout << "v5 =" << v5 << endl;

JVector v6 = v5 / 2.0;

	cout << "v6 should equal 0 -1.5 0" << endl;
	cout << "v6 =" << v6 << endl;

	v6 = 2.0 * v5;

	cout << "v6 should equal 0 -6 0" << endl;
	cout << "v6 =" << v6 << endl;

	v6 = -v5;

	cout << "v6 should equal 0 3 0" << endl;
	cout << "v6 =" << v6 << endl;

	cout << "first element should be zero" << endl;
	cout << "is first element zero? " << (v6.GetElement(1) == 0.0) << endl;

	cout << "transpose(v6) =" << v6.Transpose() << endl;

	return 0;
}
