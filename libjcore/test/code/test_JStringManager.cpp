/******************************************************************************
 test_JStringManager.cpp

	Program to test JStringManager class.

	Written by John Lindal.

 ******************************************************************************/

#include "JTestManager.h"
#include "jGlobals.h"
#include "jAssert.h"

int main()
{
	return JTestManager::Execute();
}

JTEST(Basic)
{
	JStringManager* m = JGetStringManager();

	JAssertStringsEqual("No errors occurred.", m->Get("JNoError"));
	JAssertStringsEqual("String not found: foobar", m->Get("foobar"));

	m->EnablePseudotranslation();

	const JUtf8Byte* data[] = { "1\n", "testkey\t\"No errors occurred.\"", nullptr };
	m->Register("test", data);

	JAssertStringsEqual("[\xE6\x96\x87\xE5\xAD\x97N\xC3\x91o\xC3\xB8 e\xC3\xA9" "r\xD1\x8F" "r\xD1\x8F" "o\xC3\xB8" "r\xD1\x8F" "s\xC5\xA1 o\xC3\xB8" "c\xC3\xA7" "c\xC3\xA7" "u\xC3\xBC" "r\xD1\x8F" "r\xD1\x8F" "e\xC3\xA9" "d.\xEF\xBD\xA1" "??]", m->Get("testkey"));
}
