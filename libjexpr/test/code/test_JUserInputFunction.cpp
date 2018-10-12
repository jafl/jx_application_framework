/******************************************************************************
 test_JUserInputFunction.cpp

	Tests for JUserInputFunction.

	Written by John Lindal.

 *****************************************************************************/

#include <JTestManager.h>
#include <JUserInputFunction.h>
#include <jAssert.h>

int main()
{
	return JTestManager::Execute();
}

JTEST(ConvertToGreekCharacter)
{
	JAssertStringsEqual("v", JUserInputFunction::ConvertToGreek(JUtf8Character('v')).GetBytes());
	JAssertStringsEqual("7", JUserInputFunction::ConvertToGreek(JUtf8Character('7')).GetBytes());
	JAssertStringsEqual("+", JUserInputFunction::ConvertToGreek(JUtf8Character('+')).GetBytes());

	JAssertStringsEqual("\xCF\x80", JUserInputFunction::ConvertToGreek(JUtf8Character('p')).GetBytes());
}

JTEST(ConvertToGreekString)
{
	JAssertStringsEqual("foo", JUserInputFunction::ConvertToGreek(JString("foo", kJFalse)));

	JAssertStringsEqual("\xCF\x80", JUserInputFunction::ConvertToGreek(JString("`p", kJFalse)));
}
