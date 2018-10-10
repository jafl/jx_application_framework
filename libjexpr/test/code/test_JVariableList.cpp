/******************************************************************************
 test_JVariableList.cpp

	Tests for JVariableList.

	Written by John Lindal.

 *****************************************************************************/

#include <JTestManager.h>
#include <JVariableList.h>
#include <jAssert.h>

int main()
{
	return JTestManager::Execute();
}

JTEST(NameValid)
{
	JAssertTrue(JVariableList::NameValid(JString("foo", kJFalse)));
	JAssertTrue(JVariableList::NameValid(JString("`l", kJFalse)));
	JAssertTrue(JVariableList::NameValid(JString("a0_5Z", kJFalse)));

	JAssertFalse(JVariableList::NameValid(JString("0ab", kJFalse)));
	JAssertFalse(JVariableList::NameValid(JString("a_0 5_Z", kJFalse)));
	JAssertFalse(JVariableList::NameValid(JString("a+b", kJFalse)));

	// named constants
	JAssertFalse(JVariableList::NameValid(JString("π", kJFalse)));
	JAssertFalse(JVariableList::NameValid(JString("e", kJFalse)));
	JAssertFalse(JVariableList::NameValid(JString("i", kJFalse)));
}
