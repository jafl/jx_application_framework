/******************************************************************************
 test_JVector.cpp

	Program to test JVector class.

	Written by John Lindal.

 ******************************************************************************/

#include "JTestManager.h"
#include "JVector.h"
#include "JMatrix.h"
#include <sstream>
#include "jAssert.h"

int main()
{
	return JTestManager::Execute();
}

JTEST(Exercise)
{
	JFloat v1e[] = { 1.0, 1.0, 0.0 };

	JVector v1(3, v1e);
	JAssertEqual(3, v1.GetDimensionCount());

	v1.SetElement(1, 0.0);
	JAssertEqual(0.0, v1.GetElement(1));
	JAssertEqual(1.0, v1.GetElement(2));
	JAssertEqual(0.0, v1.GetElement(3));

	JVector v2(3, 1.0, 0.0, 1.0);
	JAssertEqual(1.0, v2.GetElement(1));
	JAssertEqual(0.0, v2.GetElement(2));
	JAssertEqual(1.0, v2.GetElement(3));

	JAssertEqual(0.0, JDotProduct(v1,v2));

	JMatrix outerProduct = JOuterProduct(v1, v2);
	JAssertEqual(0.0, outerProduct.GetElement(1, 1));
	JAssertEqual(0.0, outerProduct.GetElement(1, 2));
	JAssertEqual(0.0, outerProduct.GetElement(1, 3));
	JAssertEqual(1.0, outerProduct.GetElement(2, 1));
	JAssertEqual(0.0, outerProduct.GetElement(2, 2));
	JAssertEqual(1.0, outerProduct.GetElement(2, 3));
	JAssertEqual(0.0, outerProduct.GetElement(3, 1));
	JAssertEqual(0.0, outerProduct.GetElement(3, 2));
	JAssertEqual(0.0, outerProduct.GetElement(3, 3));

	v1 += v2;
	JAssertEqual(1.0, v1.GetElement(1));
	JAssertEqual(1.0, v1.GetElement(2));
	JAssertEqual(1.0, v1.GetElement(3));

	JAssertWithin(0.001, 1.732, v1.GetLength());

	v2 -= v1;
	JAssertEqual( 0.0, v2.GetElement(1));
	JAssertEqual(-1.0, v2.GetElement(2));
	JAssertEqual( 0.0, v2.GetElement(3));

	JAssertEqual(-1.0, JDotProduct(v1,v2));

	JVector unit = v1.UnitVector();
	v1.Normalize();
	JAssertEqual(unit, v1);

	JVector v3 = v2;
	JAssertEqual(v2, v3);

	JVector v4 = v2 + v3;
	JAssertEqual( 0.0, v4.GetElement(1));
	JAssertEqual(-2.0, v4.GetElement(2));
	JAssertEqual( 0.0, v4.GetElement(3));

	JAssertTrue(v4 != v2);

	JVector v5(3);
	v5 = v4 - v2;
	JAssertEqual( 0.0, v5.GetElement(1));
	JAssertEqual(-1.0, v5.GetElement(2));
	JAssertEqual( 0.0, v5.GetElement(3));

	v5 *= 3.0;
	JAssertEqual( 0.0, v5.GetElement(1));
	JAssertEqual(-3.0, v5.GetElement(2));
	JAssertEqual( 0.0, v5.GetElement(3));

	JVector v6 = v5 / 2.0;
	JAssertEqual( 0.0, v6.GetElement(1));
	JAssertEqual(-1.5, v6.GetElement(2));
	JAssertEqual( 0.0, v6.GetElement(3));

	v6 = 2.0 * v5;
	JAssertEqual( 0.0, v6.GetElement(1));
	JAssertEqual(-6.0, v6.GetElement(2));
	JAssertEqual( 0.0, v6.GetElement(3));

	v6 = -v5;
	JAssertEqual(0.0, v6.GetElement(1));
	JAssertEqual(3.0, v6.GetElement(2));
	JAssertEqual(0.0, v6.GetElement(3));

	v6.IncrementElement(2, 3.0);
	JAssertEqual(0.0, v6.GetElement(1));
	JAssertEqual(6.0, v6.GetElement(2));
	JAssertEqual(0.0, v6.GetElement(3));

	v6.IncrementElement(2, -3.0);

	JMatrix v6t = v6.Transpose();
	JAssertEqual(0.0, v6t.GetElement(1, 1));
	JAssertEqual(3.0, v6t.GetElement(1, 2));
	JAssertEqual(0.0, v6t.GetElement(1, 3));

	v6.SetAllElements(2.5);
	JAssertEqual(2.5, v6.GetElement(1));
	JAssertEqual(2.5, v6.GetElement(2));
	JAssertEqual(2.5, v6.GetElement(3));

	JFloat v6e[] = { 3.0, 2.0, 1.0 };
	v6.Set(v6e);
	JAssertEqual(3.0, v6.GetElement(1));
	JAssertEqual(2.0, v6.GetElement(2));
	JAssertEqual(1.0, v6.GetElement(3));

	v6.Set(1.0, 2.0, 3.0);
	JAssertEqual(1.0, v6.GetElement(1));
	JAssertEqual(2.0, v6.GetElement(2));
	JAssertEqual(3.0, v6.GetElement(3));

	v6 = v6 * 1.5;

	JIndex i;
	JAssertEqual(1.5, v6.GetMinElement(&i));
	JAssertEqual(1, i);

	JAssertEqual(4.5, v6.GetMaxElement(&i));
	JAssertEqual(3, i);

	JAssertEqual(JVector(3, -3.0,6.0,-3.0), JCrossProduct(JVector(3, 2.0,3.0,4.0), JVector(3, 5.0,6.0,7.0)));

	std::stringstream s;
	s << v6;
	s.seekg(0);
	JVector v7(3);
	s >> v7;
	JAssertEqual(v6, v7);
}
