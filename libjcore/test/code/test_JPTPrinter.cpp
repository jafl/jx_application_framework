/******************************************************************************
 test_JPTPrinter.cpp

	Program to test JPTPrinter class.

	Written by John Lindal.

 ******************************************************************************/

#include "JTestManager.h"
#include "TestPTPrinter.h"
#include "jFStreamUtil.h"
#include "jFileUtil.h"
#include "jAssert.h"

int main()
{
	return JTestManager::Execute();
}

JTEST(Print)
{
	JString text;
	JReadFile(JString("./data/test_print.in.txt", JString::kNoCopy), &text);

	JString fileName;
	JAssertOK(JCreateTempFile(&fileName));

	TestPTPrinter p(fileName);
	p.Print(text);

	JString expected, actual;
	JReadFile(JString("./data/test_print.out.txt", JString::kNoCopy), &expected);
	JReadFile(fileName, &actual);

	JAssertStringsEqual(expected, actual);

	JRemoveFile(fileName);
}

JTEST(PrintReverseOrder)
{
	JString text;
	JReadFile(JString("./data/test_print.in.txt", JString::kNoCopy), &text);

	JString fileName;
	JAssertOK(JCreateTempFile(&fileName));

	TestPTPrinter p(fileName);
	p.ShouldPrintReverseOrder(true);
	p.Print(text);

	JString expected, actual;
	JReadFile(JString("./data/test_print.reverse.txt", JString::kNoCopy), &expected);
	JReadFile(fileName, &actual);

	JAssertStringsEqual(expected, actual);

	JRemoveFile(fileName);
}
