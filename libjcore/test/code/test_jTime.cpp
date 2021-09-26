/******************************************************************************
 test_jTime.cpp

	Program to test jTime.

	Written by John Lindal.

 *****************************************************************************/

#include <jx-af/jcore/JTestManager.h>
#include <jx-af/jcore/jTime.h>
#include <jx-af/jcore/jAssert.h>

int main()
{
	return JTestManager::Execute();
}

JTEST(PrintTimeInterval)
{
	JString s = JPrintTimeInterval(23);
	JAssertStringsEqual("23 sec", s);

	s = JPrintTimeInterval(5 * 60);
	JAssertStringsEqual("5 min", s);

	s = JPrintTimeInterval(60 * 60);
	JAssertStringsEqual("1.0 hours", s);

	s = JPrintTimeInterval(5 * 60 * 60);
	JAssertStringsEqual("5.0 hours", s);

	s = JPrintTimeInterval(5.3 * 60 * 60);
	JAssertStringsEqual("5.3 hours", s);

	s = JPrintTimeInterval(24 * 60 * 60);
	JAssertStringsEqual("1.0 days", s);

	s = JPrintTimeInterval(5 * 24 * 60 * 60);
	JAssertStringsEqual("5.0 days", s);

	s = JPrintTimeInterval(3 * 7 * 24 * 60 * 60);
	JAssertStringsEqual("3.0 weeks", s);

	s = JPrintTimeInterval(60 * 24 * 60 * 60);
	JAssertStringsEqual("2.0 months", s);

	s = JPrintTimeInterval(365 * 24 * 60 * 60);
	JAssertStringsEqual("1.0 years", s);

	s = JPrintTimeInterval(1.5 * 365 * 24 * 60 * 60);
	JAssertStringsEqual("1.5 years", s);
}

JTEST(GetTimezoneOffset)
{
	// test that the function doesn't crash
	std::cout << "local timezone offset: " << JGetTimezoneOffset()/3600 << std::endl;
}
