/******************************************************************************
 test_JXSpellChecker.cpp

	Test interface to aspell.

	Written by John Lindal.

 *****************************************************************************/

#include <JTestManager.h>
#include <JXSpellChecker.h>
#include <JPtrArray-JString.h>
#include <jAssert.h>

int main()
{
	return JTestManager::Execute();
}

JTEST(Exercise)
{
	JXSpellChecker sp;
	JAssertTrue(sp.IsAvailable());

	JPtrArray<JString> list(JPtrArrayT::kDeleteAll);
	JBoolean good;
	JAssertTrue(sp.CheckWord(JString("hello", kJFalse), &list, &good));
	JAssertTrue(list.IsEmpty());

	JAssertFalse(sp.CheckWord(JString("heelo", kJFalse), &list, &good));
	JAssertFalse(list.IsEmpty());
	JAssertTrue(good);
	JAssertStringsEqual("heel", *list.GetFirstElement());

	// aspell treats this as two words
	JAssertTrue(sp.CheckWord(JString("sﺺd", kJFalse), &list, &good));
	JAssertTrue(list.IsEmpty());

	JAssertFalse(sp.CheckWord(JString("heelo", kJFalse), &list, &good));
	JAssertFalse(list.IsEmpty());
}
