/******************************************************************************
 test_jMath.cpp

	Program to test jMath utilities.

	Written by John Lindal.

 ******************************************************************************/

#include "JTestManager.h"
#include "jMath.h"
#include <sstream>
#include "jAssert.h"

int main()
{
	return JTestManager::Execute();
}

JTEST(Round)
{
	JAssertEqual(5, JRound(4.5));
	JAssertEqual(5, JRound(4.8));
	JAssertEqual(5, JRound(5.2));
	JAssertEqual(5, JRound(5.4));
}

JTEST(Sign)
{
	JAssertEqual(+1, JSign(5.0));
	JAssertEqual( 0, JSign(0.0));
	JAssertEqual(-1, JSign(-5.0));
}

JTEST(InverseHyperbolic)
{
	JAssertWithin(1e-12, 2.3124383412728, JASinh(5.0));
	JAssertWithin(1e-12, 2.2924316695612, JACosh(5.0));
	JAssertWithin(1e-12, 0.97295507452766, JATanh(0.75));
}

JTEST(Truncate)
{
	JAssertEqual(-5, JLFloor(-4.2));
	JAssertEqual(4, JLFloor(4.0));
	JAssertEqual(4, JLFloor(4.3));
	JAssertEqual(4, JLFloor(4.9));

	JAssertEqual(-4, JLCeil(-4.2));
	JAssertEqual(4, JLCeil(4.0));
	JAssertEqual(5, JLCeil(4.3));
	JAssertEqual(5, JLCeil(4.9));

	JAssertEqual(-4, JTruncate(-4.2));
	JAssertEqual(4, JTruncate(4.0));
	JAssertEqual(4, JTruncate(4.3));
	JAssertEqual(4, JTruncate(4.9));
}

JTEST(Abs)
{
	JAssertEqual(5, JLAbs(5));
	JAssertEqual(5, JLAbs(-5));
}
