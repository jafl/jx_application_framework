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

	std::cout << "We are in " << v1.GetDimensionCount() << " dimensions." << std::endl;

	std::cout << "v1 should be 0 1 0" << std::endl;
	std::cout << "v1 =" << v1 << std::endl;

	std::cout << "v2 should be 1 0 1" << std::endl;
	std::cout << "v2 =" << v2 << std::endl;

	std::cout << "v1 dot v2 should be 0" << std::endl;
	std::cout << "v1 dot v2 =" << JDotProduct(v1,v2) << std::endl;

	std::cout << "v1 outer product v2 should be 0 0 0, 1 0 1, 0 0 0" << std::endl;
	std::cout << "v1 outer product v2 =" << JOuterProduct(v1, v2) << std::endl;

	JWaitForReturn();

	v1 += v2;

	std::cout << "v1 should be 1 1 1" << std::endl;
	std::cout << "v1 =" << v1 << std::endl;

	std::cout << "|v1| should be 1.732" << std::endl;
	std::cout << "|v1| =" << v1.GetLength() << std::endl;

	v2 -= v1;

	std::cout << "v2 should be 0 -1 0" << std::endl;
	std::cout << "v2 =" << v2 << std::endl;

	std::cout << "v1 dot v2 should be -1" << std::endl;
	std::cout << "v1 dot v2 =" << JDotProduct(v1,v2) << std::endl;

	JWaitForReturn();

	std::cout << "Enter jnew v1: ";
	std::cin >> v1;
	JInputFinished();

	std::cout << "New v1 =" << v1 << std::endl;

	std::cout << "Unit vector from v1 =" << v1.UnitVector() << std::endl;
	v1.Normalize();
	std::cout << "Normalized v1       =" << v1 << std::endl;

JVector v3 = v2;												// copy constructor

	std::cout << "v3 should equal v2" << std::endl;
	std::cout << "v3 equals v2? " << (v3 == v2) << std::endl;

JVector v4 = v2 + v3;

	std::cout << "v4 should equal 0 -2 0" << std::endl;
	std::cout << "v4 =" << v4 << std::endl;

	std::cout << "v4 should not equal v2" << std::endl;
	std::cout << "v4 not equal to v2? " << (v4 != v2) << std::endl;

JVector v5(3);

	v5 = v4 - v2;												// assignment operator

	std::cout << "v5 should equal 0 -1 0" << std::endl;
	std::cout << "v5 =" << v5 << std::endl;

	JWaitForReturn();

	v5 *= 3.0;

	std::cout << "v5 should equal 0 -3 0" << std::endl;
	std::cout << "v5 =" << v5 << std::endl;

JVector v6 = v5 / 2.0;

	std::cout << "v6 should equal 0 -1.5 0" << std::endl;
	std::cout << "v6 =" << v6 << std::endl;

	v6 = 2.0 * v5;

	std::cout << "v6 should equal 0 -6 0" << std::endl;
	std::cout << "v6 =" << v6 << std::endl;

	v6 = -v5;

	std::cout << "v6 should equal 0 3 0" << std::endl;
	std::cout << "v6 =" << v6 << std::endl;

	std::cout << "first element should be zero" << std::endl;
	std::cout << "is first element zero? " << (v6.GetElement(1) == 0.0) << std::endl;

	std::cout << "transpose(v6) =" << v6.Transpose() << std::endl;

	return 0;
}
