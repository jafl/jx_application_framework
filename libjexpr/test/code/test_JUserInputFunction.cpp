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
	JAssertStringsEqual("f", JUserInputFunction::ConvertToGreek('f').GetBytes());
	JAssertStringsEqual("7", JUserInputFunction::ConvertToGreek('7').GetBytes());
	JAssertStringsEqual("+", JUserInputFunction::ConvertToGreek('+').GetBytes());

	JAssertStringsEqual("\xCF\x80", JUserInputFunction::ConvertToGreek('p').GetBytes());
}

JTEST(ConvertToGreekString)
{
	JAssertStringsEqual("foo", JUserInputFunction::ConvertToGreek(JString("foo", kJFalse)));

	JAssertStringsEqual("\xCF\x80", JUserInputFunction::ConvertToGreek(JString("`p", kJFalse)));
}
