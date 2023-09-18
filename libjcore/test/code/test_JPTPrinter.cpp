/******************************************************************************
 test_JPTPrinter.cpp

	Program to test JPTPrinter class.

	Written by John Lindal.

 ******************************************************************************/

#include "JTestManager.h"
#include "TestPTPrinter.h"
#include "jFStreamUtil.h"
#include "jFileUtil.h"
#include <sstream>
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

JTEST(LineNumbers)
{
	JString text;
	JReadFile(JString("./data/test_print.in.txt", JString::kNoCopy), &text);

	JString fileName;
	JAssertOK(JCreateTempFile(&fileName));

	TestPTPrinter p(fileName);
	p.ShouldPrintLineNumbers(true);
	p.Print(text);

	JString expected, actual;
	JReadFile(JString("./data/test_print.linenumbers.txt", JString::kNoCopy), &expected);
	JReadFile(fileName, &actual);

	JAssertStringsEqual(expected, actual);

	JRemoveFile(fileName);
}

JTEST(PrefsData)
{
	TestPTPrinter p1(JString::empty);
	p1.ShouldPrintReverseOrder(true);
	p1.ShouldPrintLineNumbers(true);

	std::stringstream s;
	p1.WritePTSetup(s);

	s.seekg(0);

	TestPTPrinter p2(JString::empty);
	JAssertFalse(p2.WillPrintReverseOrder());
	JAssertFalse(p2.WillPrintLineNumbers());

	p2.ReadPTSetup(s);
	JAssertTrue(p2.WillPrintReverseOrder());
	JAssertTrue(p2.WillPrintLineNumbers());
}
