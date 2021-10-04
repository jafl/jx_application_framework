/******************************************************************************
 TestPTPrinter.h

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestPTPrinter
#define _H_TestPTPrinter

#include <jx-af/jcore/JPTPrinter.h>
#include <jx-af/jcore/JString.h>

class TestPTPrinter : public JPTPrinter
{
public:

	TestPTPrinter(const JString& fileName);

	virtual ~TestPTPrinter();

	void	Print(const JString& text) override;

private:

	JString itsFileName;

private:

	// not allowed

	TestPTPrinter(const TestPTPrinter& source);
	const TestPTPrinter& operator=(const TestPTPrinter& source);
};

#endif
