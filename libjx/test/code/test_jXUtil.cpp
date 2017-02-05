/******************************************************************************
 test_jXUtil.cc

	Test X utility functions.

	Written by John Lindal.

 *****************************************************************************/

#include <JUnitTestManager.h>
#include <jXUtil.h>
#include <jAssert.h>

int main()
{
	return JUnitTestManager::Execute();
}

JTEST(PackStrings)
{
	JPtrArray<JString> list1(JPtrArrayT::kDeleteAll),
					   list2(JPtrArrayT::kDeleteAll);

	JString s = JXPackStrings(list1);
	JXUnpackStrings(s.GetBytes(), s.GetByteCount(), &list2);
	JAssertTrue(list2.IsEmpty());

	list1.Append(JString("foo", 0, kJFalse));
	s = JXPackStrings(list1);
	JXUnpackStrings(s.GetBytes(), s.GetByteCount(), &list2);
	JAssertEqual(1, list2.GetElementCount());
	JAssertStringsEqual("foo", *(list2.GetFirstElement()));

	list1.Prepend(JString("bar", 0, kJFalse));
	s = JXPackStrings(list1);
	JXUnpackStrings(s.GetBytes(), s.GetByteCount(), &list2);
	JAssertEqual(2, list2.GetElementCount());
	JAssertStringsEqual("bar", *(list2.GetElement(1)));
	JAssertStringsEqual("foo", *(list2.GetElement()2));
}

JTEST(PackFileNames)
{
	JPtrArray<JString> list1(JPtrArrayT::kDeleteAll),
					   list2(JPtrArrayT::kDeleteAll);

	JString s = JXPackFileNames(list1);
	JXUnpackFileNames(s.GetBytes(), s.GetByteCount(), &list2);
	JAssertTrue(list2.IsEmpty());

	list1.Append(JString("/bin/ls", 0, kJFalse));
	s = JXPackFileNames(list1);
	JXUnpackFileNames(s.GetBytes(), s.GetByteCount(), &list2);
	JAssertEqual(1, list2.GetElementCount());
	JAssertStringsEqual("/bin/ls", *(list2.GetFirstElement()));

	list1.Prepend(JString("/bin/cat", 0, kJFalse));
	s = JXPackFileNames(list1);
	JXUnpackFileNames(s.GetBytes(), s.GetByteCount(), &list2);
	JAssertEqual(2, list2.GetElementCount());
	JAssertStringsEqual("/bin/cat", *(list2.GetElement(1)));
	JAssertStringsEqual("/bin/ls", *(list2.GetElement()2));
}
