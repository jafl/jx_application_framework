/******************************************************************************
 test_JBoolean.cc

	Program to test JBoolean enum.

	Written by John Lindal.

 ******************************************************************************/

#include <JUnitTestManager.h>
#include <sstream>
#include <jassert_simple.h>

void TestValues()
{
	JAssertTrue(kJTrue);
	JAssertFalse(kJFalse);
}

void TestConversion()
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

void TestRead()
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

void TestWrite()
{
	std::ostringstream s;
	s << kJTrue << kJFalse;
	JAssertStringsEqual("TF", s.str().c_str());
}

static const JUnitTest tests[] =
{
	TestValues,
	TestConversion,
	TestRead,
	TestWrite,
	NULL
};

int main()
{
	JUnitTestManager::Execute(tests);
}
