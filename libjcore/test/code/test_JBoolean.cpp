/******************************************************************************
 test_JBoolean.cpp

	Program to test JBoolean enum.

	Written by John Lindal.

 ******************************************************************************/

#include <JTestManager.h>
#include <sstream>
#include <jAssert.h>

int main()
{
	return JTestManager::Execute();
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
	s >> JBoolFromString(b);
	JAssertFalse(b);

	s.str("T");
	s >> JBoolFromString(b);
	JAssertTrue(b);

	s.str("g");
	s >> JBoolFromString(b);
	JAssertFalse(s.good());
}

JTEST(Write)
{
	std::ostringstream s;
	s << JBoolToString(kJTrue) << JBoolToString(kJFalse);
	JAssertStringsEqual("TF", s.str().c_str());
}
