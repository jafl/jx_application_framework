/******************************************************************************
 TestPTPrinter.h

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestPTPrinter
#define _H_TestPTPrinter

#include <JPTPrinter.h>
#include <JString.h>

class TestPTPrinter : public JPTPrinter
{
public:

	TestPTPrinter(const JString& fileName);

	virtual ~TestPTPrinter();

	virtual void	Print(const JString& text) override;

private:

	JString itsFileName;

private:

	// not allowed

	TestPTPrinter(const TestPTPrinter& source);
	const TestPTPrinter& operator=(const TestPTPrinter& source);
};

#endif
