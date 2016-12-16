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

	std::cout << "mx1 is " << mx1.GetRowCount() << " x " << mx1.GetColCount() << std::endl;

	std::cout << "mx1 should be 0 0, 0 0" << std::endl;
	std::cout << "mx1 = " << mx1 << std::endl;

	mx1 = JIdentityMatrix(2);

	mx2.SetElement(1,1, 0.0);
	mx2.SetElement(1,2, 1.0);
	mx2.SetElement(2,1, 1.0);
	mx2.SetElement(2,2, 0.0);

	std::cout << "mx1 should be 1 0, 0 1" << std::endl;
	std::cout << "mx1 = " << mx1 << std::endl;

	std::cout << "mx2 should be 0 1, 1 0" << std::endl;
	std::cout << "mx2 = " << mx2 << std::endl;

	JWaitForReturn();

JMatrix mx3 = -mx2;												// copy constructor

	std::cout << "mx3 should be 0 -1, -1 0" << std::endl;
	std::cout << "mx3 = " << mx3 << std::endl;

	mx3 += mx1;

JMatrix mx4 = mx1 - mx2;

	std::cout << "mx4 should be 1 -1, -1 1" << std::endl;
	std::cout << "mx4 = " << mx4 << std::endl;

	std::cout << "mx3 should equal mx4" << std::endl;
	std::cout << "mx3 equals mx4? " << (mx3 == mx4) << std::endl;

	mx4 /= 0.5;

	std::cout << "mx4 should be 2 -2, -2 2" << std::endl;
	std::cout << "mx4 = " << mx4 << std::endl;

	mx4.SetAllElements(1.0);
	mx4.SetElement(1,2, 0.0);

	std::cout << "mx4 should be 1 0, 1 1" << std::endl;
	std::cout << "mx4 = " << mx4 << std::endl;

JMatrix mx5 = mx4.Transpose();

	std::cout << "mx5 should be 1 1, 0 1" << std::endl;
	std::cout << "mx5 = " << mx5 << std::endl;

	std::cout << "mx4 * mx5 should be 1 1, 1 2" << std::endl;
	std::cout << "mx4 * mx5 = " << mx4 * mx5 << std::endl;

	JWaitForReturn();

JVector v1 = mx4.GetColVector(2);
JVector v2 = mx4.GetRowVector(1);

	std::cout << "v1 should be 0 1" << std::endl;
	std::cout << "v1 = " << v1 << std::endl;

	std::cout << "v1 dot v2 should be 0" << std::endl;
	std::cout << "v1 dot v2 = " << JDotProduct(v1, v2) << std::endl;

	std::cout << "mx4 * v1 should be 0, 1" << std::endl;
	std::cout << "mx4 * v1 = " << mx4 * v1 << std::endl;

	std::cout << "(v1)t * mx4 should be 1 1" << std::endl;
	std::cout << "(v1)t * mx4 = " << v1.Transpose() * mx4 << std::endl;

	std::cout << "Enter new mx5: ";
	std::cin >> mx5;
	JInputFinished();
	std::cout << "mx5 = " << mx5 << std::endl;

	JWaitForReturn();

JMatrix mx6(3,3), mx7(3,3);

	mx6.SetElement(1,1, 1.0);
	mx6.SetElement(2,2, 2.0);
	mx6.SetElement(3,3, 3.0);

	std::cout << "det(mx6) should be 6" << std::endl;
	std::cout << "det(mx6) = " << mx6.Determinant() << std::endl;

	assert( mx6.Invert(&mx7) );
	std::cout << "mx7 should be 1 0 0, 0 0.5 0, 0 0 0.333333" << std::endl;
	std::cout << "mx7 = " << mx7 << std::endl;

	mx6.SetElement(1,1, 1.0);
	mx6.SetElement(1,2, 2.0);
	mx6.SetElement(1,3, 3.0);
	mx6.SetElement(2,1, 4.0);
	mx6.SetElement(2,2, 5.0);
	mx6.SetElement(2,3, 6.0);
	mx6.SetElement(3,1, 2.0);
	mx6.SetElement(3,2, 3.0);
	mx6.SetElement(3,3, 4.0);

	std::cout << std::endl;
	std::cout << "mx6 should be 1 2 3, 4 5 6, 2 3 4" << std::endl;
	std::cout << "mx6 = " << mx6 << std::endl;

	std::cout << "det(mx6) should be 0" << std::endl;
	std::cout << "det(mx6) = " << mx6.Determinant() << std::endl;

	assert( !mx6.Invert(&mx7) );

	std::cout << "mx6 was not invertible (correct)" << std::endl;

	mx6.SetElement(1,1, 1.0);
	mx6.SetElement(1,2, 0.5);
	mx6.SetElement(1,3, -2.0);
	mx6.SetElement(2,1, 1.0/3.0);
	mx6.SetElement(2,2, 5.0);
	mx6.SetElement(2,3, -7.5);
	mx6.SetElement(3,1, 0.0);
	mx6.SetElement(3,2, 1.0);
	mx6.SetElement(3,3, -3.0);

	std::cout << std::endl;
	assert( mx6.Invert(&mx7) );

	std::cout << "det(mx6) should be 1/det(mx7) should be -7.666667" << std::endl;
	std::cout << mx6.Determinant() << " ?= " << 1.0/mx7.Determinant() << std::endl;

	std::cout << "A*A-1 should get 1 0 0, 0 1 0, 0 0 1" << std::endl;
	std::cout << "A*A-1 = " << mx6 * mx7 << std::endl;
	std::cout << "A-1*A = " << mx7 * mx6 << std::endl;

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

	std::cout << std::endl;
	std::cout << "Solution should be: 1 2 3" << std::endl;
	if (JGaussianElimination(mx6, b, &x))
		{
		std::cout << "Solution is:        " << x << std::endl;
		}
	else
		{
		std::cout << "ERROR: mx6 should be invertible!" << std::endl;
		}

	mx6.SetElement(3,1, 3.0);
	mx6.SetElement(3,2, 2.0);
	mx6.SetElement(3,3, 3.0);

	std::cout << std::endl;
	std::cout << "Solution should not exist" << std::endl;
	if (!JGaussianElimination(mx6, b, &x))
		{
		std::cout << "Solution does not exist" << std::endl;
		}
	else
		{
		std::cout << "Solution exists: " << x << std::endl;
		std::cout << "(because of round off error)" << std::endl;
		}

	return 0;
}
