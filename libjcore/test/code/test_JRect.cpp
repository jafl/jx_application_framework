/******************************************************************************
 test_JRect.cpp

	Program to test JRect class.

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
	JRect r(20,20,40,60);
	JAssertEqual(40, r.width());
	JAssertEqual(20, r.height());
	JAssertEqual(800, r.area());
	JAssertEqual(40, r.xcenter());
	JAssertEqual(30, r.ycenter());
	JAssertEqual(JPoint(20,20), r.topLeft());
	JAssertEqual(JPoint(20,40), r.bottomLeft());
	JAssertEqual(JPoint(60,20), r.topRight());
	JAssertEqual(JPoint(60,40), r.bottomRight());

	r.SetSize(20,40);
	JAssertEqual(JRect(20,20,60,40), r);

	r.Place(30,50);
	JAssertEqual(JRect(30,50,70,70), r);

	r.Place(JPoint(20,20));
	JAssertEqual(JRect(20,20,60,40), r);

	JAssertFalse(r.IsEmpty());

	JRect r2;
	JAssertTrue(r2.IsEmpty());

	JAssertTrue(r.Contains(JPoint(30,30)));
	JAssertTrue(r.Contains(JPoint(20,20)));
	JAssertTrue(r.Contains(JPoint(20,59)));
	JAssertFalse(r.Contains(JPoint(20,60)));
	JAssertTrue(r.Contains(JPoint(39,59)));
	JAssertFalse(r.Contains(JPoint(40,59)));
	JAssertTrue(r.Contains(JPoint(39,20)));
	JAssertFalse(r.Contains(JPoint(40,20)));

	JAssertTrue(r.Contains(JRect(25,25,55,35)));
	JAssertFalse(r.Contains(JRect(15,25,55,35)));
	JAssertFalse(r.Contains(JRect(25,15,55,35)));
	JAssertFalse(r.Contains(JRect(25,25,65,35)));
	JAssertFalse(r.Contains(JRect(25,25,55,45)));

	r.Shift(JPoint(20,20));
	JAssertEqual(JRect(40,40,80,60), r);

	r = r - JPoint(20,20);
	JAssertEqual(JRect(20,20,60,40), r);

	r = r + JPoint(20,20);
	JAssertEqual(JRect(40,40,80,60), r);

	r.Shrink(5,5);
	JAssertEqual(JRect(45,45,75,55), r);

	r.Expand(5,5);
	JAssertEqual(JRect(40,40,80,60), r);

	std::ostringstream output;
	output << r;

	std::istringstream input(output.str());
	input >> r2;
	JAssertEqual(r, r2);
	JAssertFalse(r != r2);
}

JTEST(Intersection)
{
	JRect r1(-20,-20,20,20), r2, r3;

	r2.Set(0,0,30,30);
	JAssertTrue(JIntersection(r1, r2, &r3));
	JAssertEqual(JRect(0,0,20,20), r3);

	r2.Set(30,30,50,50);
	JAssertFalse(JIntersection(r1, r2, &r3));

	r2.Set(-30,-30,0,0);
	JAssertTrue(JIntersection(r1, r2, &r3));
	JAssertEqual(JRect(-20,-20,0,0), r3);
}

JTEST(Covering)
{
	JRect r1(-20,-20,20,20), r2;

	r2.Set(0,0,30,30);
	JAssertEqual(JRect(-20,-20,30,30), JCovering(r1, r2));

	r2.Set(30,30,50,50);
	JAssertEqual(JRect(-20,-20,50,50), JCovering(r1, r2));

	r2.Set(-30,-30,0,0);
	JAssertEqual(JRect(-30,-30,20,20), JCovering(r1, r2));
}
