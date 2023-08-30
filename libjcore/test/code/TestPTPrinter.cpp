/******************************************************************************
 TestPTPrinter.cpp

	BASE CLASS = JPTPrinter

	Written by John Lindal.

 ******************************************************************************/

#include "TestPTPrinter.h"
#include "JTestManager.h"
#include <fstream>
#include "jAssert.h"

/******************************************************************************
 Constructor

 ******************************************************************************/

TestPTPrinter::TestPTPrinter
	(
	const JString& fileName
	)
	:
	JPTPrinter(),
	itsFileName(fileName)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TestPTPrinter::~TestPTPrinter()
{
}

/******************************************************************************
 Print (virtual)

 ******************************************************************************/

void
TestPTPrinter::Print
	(
	const JString& text
	)
{
	std::ofstream output(itsFileName.GetBytes());
	JAssertTrue(output.good());

	JAssertTrue(JPTPrinter::Print(text, output));
	JAssertTrue(output.good());

	output.close();
}
