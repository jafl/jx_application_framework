/******************************************************************************
 test_JPoint.cpp

	Program to test JPoint class.

	Written by John Lindal.

 ******************************************************************************/

#include <JTestManager.h>
#include <JRect.h>
#include <sstream>
#include <jAssert.h>

int main()
{
	return JTestManager::Execute();
}

JTEST(Exercise)
{
	JPoint pt;
	JAssertEqual(JPoint(0,0), pt);

	pt.Set(100,-100);
	JAssertEqual(100, pt.x);
	JAssertEqual(-100, pt.y);

	pt += JPoint(50,50);
	JAssertEqual(JPoint(150,-50), pt);
	pt -= JPoint(50,50);
	JAssertEqual(JPoint(100,-100), pt);
	pt *= 2;
	JAssertEqual(JPoint(200,-200), pt);
	pt /= 2;
	JAssertEqual(JPoint(100,-100), pt);
	pt += - JPoint(50,50);
	JAssertEqual(JPoint(50,-150), pt);

	std::ostringstream output;
	output << pt;

	std::istringstream input(output.str());
	JPoint pt2;
	input >> pt2;
	JAssertEqual(pt, pt2);
	JAssertFalse(pt != pt2);

	pt2 = pt + JPoint(50,50);
	JAssertEqual(JPoint(100,-100), pt2);
	pt2 = pt - JPoint(50,50);
	JAssertEqual(JPoint(0,-200), pt2);

	pt = pt * 2;
	JAssertEqual(JPoint(100,-300), pt);
	pt = 2 * pt;
	JAssertEqual(JPoint(200,-600), pt);
	pt = pt / 2;
	JAssertEqual(JPoint(100,-300), pt);
}

JTEST(PinInRect)
{
	JAssertEqual(JPoint(0,-20), JPinInRect(JPoint(0,-150), JRect(-20,-20,20,20)));
	JAssertEqual(JPoint(19,-20), JPinInRect(JPoint(50,-150), JRect(-20,-20,20,20)));
	JAssertEqual(JPoint(19,0), JPinInRect(JPoint(50,0), JRect(-20,-20,20,20)));
	JAssertEqual(JPoint(19,19), JPinInRect(JPoint(50,150), JRect(-20,-20,20,20)));
	JAssertEqual(JPoint(0,19), JPinInRect(JPoint(0,150), JRect(-20,-20,20,20)));
	JAssertEqual(JPoint(-20,19), JPinInRect(JPoint(-50,150), JRect(-20,-20,20,20)));
	JAssertEqual(JPoint(-20,0), JPinInRect(JPoint(-50,0), JRect(-20,-20,20,20)));
	JAssertEqual(JPoint(-20,-20), JPinInRect(JPoint(-50,-150), JRect(-20,-20,20,20)));
}
