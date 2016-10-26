/******************************************************************************
 test_JBoolean.cc

	Program to test JBoolean enum.

	Written by John Lindal.

 ******************************************************************************/

#include <JUnitTestManager.h>
#include <sstream>
#include <jassert_simple.h>

int main()
{
	JUnitTestManager::Execute();
}

JTEST(Values)
{
	JAssertTrue(kJTrue);
	JAssertFalse(kJFalse);
}

JTEST(Conversion)
{
	JBoolean b = JI2B(false);
	JAssertFalse(b);

	b = JI2B(true);
	JAssertTrue(b);

	b = JI2B(0);
	JAssertFalse(b);

	b = JI2B(3);
	JAssertTrue(b);

	b = JF2B(0.0);
	JAssertFalse(b);

	b = JF2B(0.1);
	JAssertTrue(b);
}

JTEST(Read)
{
	std::istringstream s("F");
	JBoolean b;
	s >> b;
	JAssertFalse(b);

	s.str("T");
	s >> b;
	JAssertTrue(b);

	s.str("g");
	s >> b;
	JAssertFalse(s.good());
}

JTEST(Write)
{
	std::ostringstream s;
	s << kJTrue << kJFalse;
	JAssertStringsEqual("TF", s.str().c_str());
}
