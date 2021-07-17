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
	JAssertTrue(JVariableList::NameValid(JString("foo", JString::kNoCopy)));
	JAssertTrue(JVariableList::NameValid(JString("`l", JString::kNoCopy)));
	JAssertTrue(JVariableList::NameValid(JString("a0_5Z", JString::kNoCopy)));

	JAssertFalse(JVariableList::NameValid(JString("0ab", JString::kNoCopy)));
	JAssertFalse(JVariableList::NameValid(JString("a_0 5_Z", JString::kNoCopy)));
	JAssertFalse(JVariableList::NameValid(JString("a+b", JString::kNoCopy)));

	// named constants
	JAssertFalse(JVariableList::NameValid(JString("π", JString::kNoCopy)));
	JAssertFalse(JVariableList::NameValid(JString("e", JString::kNoCopy)));
	JAssertFalse(JVariableList::NameValid(JString("i", JString::kNoCopy)));
}
