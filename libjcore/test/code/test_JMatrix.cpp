/******************************************************************************
 test_JMatrix.cc

	Program to test JMatrix class.

	Written by John Lindal.

 ******************************************************************************/

#include <JMatrix.h>
#include <JVector.h>
#include <jCommandLine.h>
#include <jAssert.h>

int
main()
{
JMatrix mx1(2,2),mx2(2,2);										// constructor

	cout << "mx1 is " << mx1.GetRowCount() << " x " << mx1.GetColCount() << endl;

	cout << "mx1 should be 0 0, 0 0" << endl;
	cout << "mx1 = " << mx1 << endl;

	mx1 = JIdentityMatrix(2);

	mx2.SetElement(1,1, 0.0);
	mx2.SetElement(1,2, 1.0);
	mx2.SetElement(2,1, 1.0);
	mx2.SetElement(2,2, 0.0);

	cout << "mx1 should be 1 0, 0 1" << endl;
	cout << "mx1 = " << mx1 << endl;

	cout << "mx2 should be 0 1, 1 0" << endl;
	cout << "mx2 = " << mx2 << endl;

	JWaitForReturn();

JMatrix mx3 = -mx2;												// copy constructor

	cout << "mx3 should be 0 -1, -1 0" << endl;
	cout << "mx3 = " << mx3 << endl;

	mx3 += mx1;

JMatrix mx4 = mx1 - mx2;

	cout << "mx4 should be 1 -1, -1 1" << endl;
	cout << "mx4 = " << mx4 << endl;

	cout << "mx3 should equal mx4" << endl;
	cout << "mx3 equals mx4? " << (mx3 == mx4) << endl;

	mx4 /= 0.5;

	cout << "mx4 should be 2 -2, -2 2" << endl;
	cout << "mx4 = " << mx4 << endl;

	mx4.SetAllElements(1.0);
	mx4.SetElement(1,2, 0.0);

	cout << "mx4 should be 1 0, 1 1" << endl;
	cout << "mx4 = " << mx4 << endl;

JMatrix mx5 = mx4.Transpose();

	cout << "mx5 should be 1 1, 0 1" << endl;
	cout << "mx5 = " << mx5 << endl;

	cout << "mx4 * mx5 should be 1 1, 1 2" << endl;
	cout << "mx4 * mx5 = " << mx4 * mx5 << endl;

	JWaitForReturn();

JVector v1 = mx4.GetColVector(2);
JVector v2 = mx4.GetRowVector(1);

	cout << "v1 should be 0 1" << endl;
	cout << "v1 = " << v1 << endl;

	cout << "v1 dot v2 should be 0" << endl;
	cout << "v1 dot v2 = " << JDotProduct(v1, v2) << endl;

	cout << "mx4 * v1 should be 0, 1" << endl;
	cout << "mx4 * v1 = " << mx4 * v1 << endl;

	cout << "(v1)t * mx4 should be 1 1" << endl;
	cout << "(v1)t * mx4 = " << v1.Transpose() * mx4 << endl;

	cout << "Enter new mx5: ";
	cin >> mx5;
	JInputFinished();
	cout << "mx5 = " << mx5 << endl;

	JWaitForReturn();

JMatrix mx6(3,3), mx7(3,3);

	mx6.SetElement(1,1, 1.0);
	mx6.SetElement(2,2, 2.0);
	mx6.SetElement(3,3, 3.0);

	cout << "det(mx6) should be 6" << endl;
	cout << "det(mx6) = " << mx6.Determinant() << endl;

	assert( mx6.Invert(&mx7) );
	cout << "mx7 should be 1 0 0, 0 0.5 0, 0 0 0.333333" << endl;
	cout << "mx7 = " << mx7 << endl;

	mx6.SetElement(1,1, 1.0);
	mx6.SetElement(1,2, 2.0);
	mx6.SetElement(1,3, 3.0);
	mx6.SetElement(2,1, 4.0);
	mx6.SetElement(2,2, 5.0);
	mx6.SetElement(2,3, 6.0);
	mx6.SetElement(3,1, 2.0);
	mx6.SetElement(3,2, 3.0);
	mx6.SetElement(3,3, 4.0);

	cout << endl;
	cout << "mx6 should be 1 2 3, 4 5 6, 2 3 4" << endl;
	cout << "mx6 = " << mx6 << endl;

	cout << "det(mx6) should be 0" << endl;
	cout << "det(mx6) = " << mx6.Determinant() << endl;

	assert( !mx6.Invert(&mx7) );

	cout << "mx6 was not invertible (correct)" << endl;

	mx6.SetElement(1,1, 1.0);
	mx6.SetElement(1,2, 0.5);
	mx6.SetElement(1,3, -2.0);
	mx6.SetElement(2,1, 1.0/3.0);
	mx6.SetElement(2,2, 5.0);
	mx6.SetElement(2,3, -7.5);
	mx6.SetElement(3,1, 0.0);
	mx6.SetElement(3,2, 1.0);
	mx6.SetElement(3,3, -3.0);

	cout << endl;
	assert( mx6.Invert(&mx7) );

	cout << "det(mx6) should be 1/det(mx7) should be -7.666667" << endl;
	cout << mx6.Determinant() << " ?= " << 1.0/mx7.Determinant() << endl;

	cout << "A*A-1 should get 1 0 0, 0 1 0, 0 0 1" << endl;
	cout << "A*A-1 = " << mx6 * mx7 << endl;
	cout << "A-1*A = " << mx7 * mx6 << endl;

	mx6.SetElement(1,1, 2.0);
	mx6.SetElement(1,2, -1.0);
	mx6.SetElement(1,3, 1.0);
	mx6.SetElement(2,1, 1.0);
	mx6.SetElement(2,2, 3.0);
	mx6.SetElement(2,3, 2.0);
	mx6.SetElement(3,1, 5.0);
	mx6.SetElement(3,2, 0.0);
	mx6.SetElement(3,3, -2.0);

JVector b(3, 3.0, 13.0, -1.0);
JVector x(3);

	cout << endl;
	cout << "Solution should be: 1 2 3" << endl;
	if (JGaussianElimination(mx6, b, &x))
		{
		cout << "Solution is:        " << x << endl;
		}
	else
		{
		cout << "ERROR: mx6 should be invertible!" << endl;
		}

	mx6.SetElement(3,1, 3.0);
	mx6.SetElement(3,2, 2.0);
	mx6.SetElement(3,3, 3.0);

	cout << endl;
	cout << "Solution should not exist" << endl;
	if (!JGaussianElimination(mx6, b, &x))
		{
		cout << "Solution does not exist" << endl;
		}
	else
		{
		cout << "Solution exists: " << x << endl;
		cout << "(because of round off error)" << endl;
		}

	return 0;
}
