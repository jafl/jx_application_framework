/******************************************************************************
 test_JSpellChecker.cpp

	Test interface to aspell.

	Written by John Lindal.

 *****************************************************************************/

#include "JTestManager.h"
#include "JSpellChecker.h"
#include "jAssert.h"

int main()
{
	return JTestManager::Execute();
}

JTEST(Exercise)
{
	JSpellChecker sp;
	JAssertTrue(sp.IsAvailable());

	JPtrArray<JString> list(JPtrArrayT::kDeleteAll);
	bool good;
	JAssertTrue(sp.CheckWord(JString("hello", JString::kNoCopy), &list, &good));
	JAssertTrue(list.IsEmpty());
/*
	JAssertFalse(sp.CheckWord(JString("heelo", JString::kNoCopy), &list, &good));
	JAssertFalse(list.IsEmpty());
	JAssertTrue(good);
	JAssertStringsEqual("heel", *list.GetFirstElement());
*/
	// aspell treats this as two words
	JAssertTrue(sp.CheckWord(JString("sﺺd", JString::kNoCopy), &list, &good));
	JAssertTrue(list.IsEmpty());
}
