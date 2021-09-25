/******************************************************************************
 test_JMatrix.cpp

	Program to test JMatrix class.

	Written by John Lindal.

 ******************************************************************************/

#include <JTestManager.h>
#include <JMatrix.h>
#include <JVector.h>
#include <jAssert.h>

int main()
{
	return JTestManager::Execute();
}

JTEST(Exercise)
{
	JMatrix mx1(2,2);
	JAssertEqual(2, mx1.GetRowCount());
	JAssertEqual(2, mx1.GetColCount());

	JAssertEqual(0.0, mx1.GetElement(1, 1));
	JAssertEqual(0.0, mx1.GetElement(1, 2));
	JAssertEqual(0.0, mx1.GetElement(2, 1));
	JAssertEqual(0.0, mx1.GetElement(2, 2));

	mx1 = JIdentityMatrix(2);
	JAssertEqual(1.0, mx1.GetElement(1, 1));
	JAssertEqual(0.0, mx1.GetElement(1, 2));
	JAssertEqual(0.0, mx1.GetElement(2, 1));
	JAssertEqual(1.0, mx1.GetElement(2, 2));

	JMatrix mx2(2,2);
	mx2.SetElement(1,1, 0.0);
	mx2.SetElement(1,2, 1.0);
	mx2.SetElement(2,1, 1.0);
	mx2.SetElement(2,2, 0.0);
	JAssertEqual(0.0, mx2.GetElement(1, 1));
	JAssertEqual(1.0, mx2.GetElement(1, 2));
	JAssertEqual(1.0, mx2.GetElement(2, 1));
	JAssertEqual(0.0, mx2.GetElement(2, 2));

	JMatrix mx3 = -mx2;
	JAssertEqual( 0.0, mx3.GetElement(1, 1));
	JAssertEqual(-1.0, mx3.GetElement(1, 2));
	JAssertEqual(-1.0, mx3.GetElement(2, 1));
	JAssertEqual( 0.0, mx3.GetElement(2, 2));

	mx3 += mx1;
	JAssertEqual( 1.0, mx3.GetElement(1, 1));
	JAssertEqual(-1.0, mx3.GetElement(1, 2));
	JAssertEqual(-1.0, mx3.GetElement(2, 1));
	JAssertEqual( 1.0, mx3.GetElement(2, 2));

	JMatrix mx4 = mx1 - mx2;
	JAssertEqual( 1.0, mx4.GetElement(1, 1));
	JAssertEqual(-1.0, mx4.GetElement(1, 2));
	JAssertEqual(-1.0, mx4.GetElement(2, 1));
	JAssertEqual( 1.0, mx4.GetElement(2, 2));

	JAssertEqual(mx3, mx4);

	mx4 /= 0.5;
	JAssertEqual( 2.0, mx4.GetElement(1, 1));
	JAssertEqual(-2.0, mx4.GetElement(1, 2));
	JAssertEqual(-2.0, mx4.GetElement(2, 1));
	JAssertEqual( 2.0, mx4.GetElement(2, 2));

	mx4.SetAllElements(1.0);
	mx4.SetElement(1,2, 0.0);
	JAssertEqual(1.0, mx4.GetElement(1, 1));
	JAssertEqual(0.0, mx4.GetElement(1, 2));
	JAssertEqual(1.0, mx4.GetElement(2, 1));
	JAssertEqual(1.0, mx4.GetElement(2, 2));

	JMatrix mx5 = mx4.Transpose();
	JAssertEqual(1.0, mx5.GetElement(1, 1));
	JAssertEqual(1.0, mx5.GetElement(1, 2));
	JAssertEqual(0.0, mx5.GetElement(2, 1));
	JAssertEqual(1.0, mx5.GetElement(2, 2));

	JMatrix prod = mx4 * mx5;
	JAssertEqual(1.0, prod.GetElement(1, 1));
	JAssertEqual(1.0, prod.GetElement(1, 2));
	JAssertEqual(1.0, prod.GetElement(2, 1));
	JAssertEqual(2.0, prod.GetElement(2, 2));

	JVector v1 = mx4.GetColVector(2);
	JVector v2 = mx4.GetRowVector(1);

	JAssertEqual(0.0, v1.GetElement(1));
	JAssertEqual(1.0, v1.GetElement(2));

	JAssertEqual(0.0, JDotProduct(v1, v2));

	JMatrix mx4v1 = mx4 * v1;
	JAssertEqual(0.0, mx4v1.GetElement(1, 1));
	JAssertEqual(1.0, mx4v1.GetElement(2, 1));

	JMatrix v1tmx4 = v1.Transpose() * mx4;
	JAssertEqual(1.0, v1tmx4.GetElement(1, 1));
	JAssertEqual(1.0, v1tmx4.GetElement(1, 2));

	JMatrix mx6(3,3), mx7(3,3);

	mx6.SetElement(1,1, 1.0);
	mx6.SetElement(2,2, 2.0);
	mx6.SetElement(3,3, 3.0);
	JAssertEqual(6, mx6.Determinant());

	JAssertTrue(mx6.Invert(&mx7));
	JAssertEqual(1.0, mx7.GetElement(1, 1));
	JAssertEqual(0.0, mx7.GetElement(1, 2));
	JAssertEqual(0.0, mx7.GetElement(1, 3));
	JAssertEqual(0.0, mx7.GetElement(2, 1));
	JAssertEqual(0.5, mx7.GetElement(2, 2));
	JAssertEqual(0.0, mx7.GetElement(2, 3));
	JAssertEqual(0.0, mx7.GetElement(3, 1));
	JAssertEqual(0.0, mx7.GetElement(3, 2));
	JAssertWithin(4e-6, 0.33333, mx7.GetElement(3, 3));

	mx6.SetElement(1,1, 1.0);
	mx6.SetElement(1,2, 2.0);
	mx6.SetElement(1,3, 3.0);
	mx6.SetElement(2,1, 4.0);
	mx6.SetElement(2,2, 5.0);
	mx6.SetElement(2,3, 6.0);
	mx6.SetElement(3,1, 2.0);
	mx6.SetElement(3,2, 3.0);
	mx6.SetElement(3,3, 4.0);

	JAssertEqual(1.0, mx6.GetElement(1, 1));
	JAssertEqual(2.0, mx6.GetElement(1, 2));
	JAssertEqual(3.0, mx6.GetElement(1, 3));
	JAssertEqual(4.0, mx6.GetElement(2, 1));
	JAssertEqual(5.0, mx6.GetElement(2, 2));
	JAssertEqual(6.0, mx6.GetElement(2, 3));
	JAssertEqual(2.0, mx6.GetElement(3, 1));
	JAssertEqual(3.0, mx6.GetElement(3, 2));
	JAssertEqual(4.0, mx6.GetElement(3, 3));

	JAssertEqual(0.0, mx6.Determinant());

	JAssertFalse(mx6.Invert(&mx7));

	mx6.SetElement(1,1, 1.0);
	mx6.SetElement(1,2, 0.5);
	mx6.SetElement(1,3, -2.0);
	mx6.SetElement(2,1, 1.0/3.0);
	mx6.SetElement(2,2, 5.0);
	mx6.SetElement(2,3, -7.5);
	mx6.SetElement(3,1, 0.0);
	mx6.SetElement(3,2, 1.0);
	mx6.SetElement(3,3, -3.0);

	JAssertTrue(mx6.Invert(&mx7));
	JAssertWithin(1e-6, -7.666667, mx6.Determinant());
	JAssertWithin(1e-6, -7.666667, 1.0/mx7.Determinant());

	JMatrix identity = mx6 * mx7;
	JAssertWithin(1e-9, 1.0, identity.GetElement(1, 1));
	JAssertWithin(1e-9, 0.0, identity.GetElement(1, 2));
	JAssertWithin(1e-9, 0.0, identity.GetElement(1, 3));
	JAssertWithin(1e-9, 0.0, identity.GetElement(2, 1));
	JAssertWithin(1e-9, 1.0, identity.GetElement(2, 2));
	JAssertWithin(1e-9, 0.0, identity.GetElement(2, 3));
	JAssertWithin(1e-9, 0.0, identity.GetElement(3, 1));
	JAssertWithin(1e-9, 0.0, identity.GetElement(3, 2));
	JAssertWithin(1e-9, 1.0, identity.GetElement(3, 3));

	identity = mx7 * mx6;
	JAssertWithin(1e-9, 1.0, identity.GetElement(1, 1));
	JAssertWithin(1e-9, 0.0, identity.GetElement(1, 2));
	JAssertWithin(1e-9, 0.0, identity.GetElement(1, 3));
	JAssertWithin(1e-9, 0.0, identity.GetElement(2, 1));
	JAssertWithin(1e-9, 1.0, identity.GetElement(2, 2));
	JAssertWithin(1e-9, 0.0, identity.GetElement(2, 3));
	JAssertWithin(1e-9, 0.0, identity.GetElement(3, 1));
	JAssertWithin(1e-9, 0.0, identity.GetElement(3, 2));
	JAssertWithin(1e-9, 1.0, identity.GetElement(3, 3));

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
	JAssertTrue(JGaussianElimination(mx6, b, &x));
	JAssertWithin(1e-9, 1.0, x.GetElement(1));
	JAssertWithin(1e-9, 2.0, x.GetElement(2));
	JAssertWithin(1e-9, 3.0, x.GetElement(3));

	mx6.SetElement(3,1, 3.0);
	mx6.SetElement(3,2, 2.0);
	mx6.SetElement(3,3, 3.0);

	if (JGaussianElimination(mx6, b, &x))
	{
		JAssertTrue(fabs(x.GetElement(1)) > 1e15);
		JAssertTrue(fabs(x.GetElement(2)) > 1e15);
		JAssertTrue(fabs(x.GetElement(3)) > 1e15);
	}
}
