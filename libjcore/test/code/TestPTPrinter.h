/******************************************************************************
 TestPTPrinter.h

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestPTPrinter
#define _H_TestPTPrinter

#include "JPTPrinter.h"
#include "JString.h"

class TestPTPrinter : public JPTPrinter
{
public:

	TestPTPrinter(const JString& fileName);

	~TestPTPrinter() override;

	void	Print(const JString& text) override;

private:

	JString itsFileName;
};

#endif
