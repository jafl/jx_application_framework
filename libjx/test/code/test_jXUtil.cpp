/******************************************************************************
 test_jXUtil.cpp

	Test X utility functions.

	Written by John Lindal.

 *****************************************************************************/

#include <JTestManager.h>
#include <jXUtil.h>
#include <jAssert.h>

int main()
{
	return JTestManager::Execute();
}

JTEST(PackStrings)
{
	JPtrArray<JString> list1(JPtrArrayT::kDeleteAll),
					   list2(JPtrArrayT::kDeleteAll);

	JString s = JXPackStrings(list1);
	JXUnpackStrings(s.GetRawBytes(), s.GetByteCount(), &list2);
	JAssertTrue(list2.IsEmpty());

	list2.CleanOut();
	list1.Append(JString("foo", kJFalse));
	s = JXPackStrings(list1);
	JXUnpackStrings(s.GetRawBytes(), s.GetByteCount(), &list2);
	JAssertEqual(1, list2.GetElementCount());
	JAssertStringsEqual("foo", *(list2.GetFirstElement()));

	list2.CleanOut();
	list1.Prepend(JString("bar", kJFalse));
	s = JXPackStrings(list1);
	JXUnpackStrings(s.GetRawBytes(), s.GetByteCount(), &list2);
	JAssertEqual(2, list2.GetElementCount());
	JAssertStringsEqual("bar", *(list2.GetElement(1)));
	JAssertStringsEqual("foo", *(list2.GetElement(2)));
}

JTEST(PackFileNames)
{
	JPtrArray<JString> list1(JPtrArrayT::kDeleteAll),
					   list2(JPtrArrayT::kDeleteAll),
					   list3(JPtrArrayT::kDeleteAll);

	JString s = JXPackFileNames(list1);
	JXUnpackFileNames(s.GetRawBytes(), s.GetByteCount(), &list2, &list3);
	JAssertTrue(list2.IsEmpty());

	list2.CleanOut();
	list3.CleanOut();
	list1.Append(JString("/bin/ls", kJFalse));
	s = JXPackFileNames(list1);
	JXUnpackFileNames(s.GetRawBytes(), s.GetByteCount(), &list2, &list3);
	JAssertEqual(1, list2.GetElementCount());
	JAssertStringsEqual("/bin/ls", *(list2.GetFirstElement()));

	list2.CleanOut();
	list3.CleanOut();
	list1.Prepend(JString("/bin/cat", kJFalse));
	s = JXPackFileNames(list1);
	JXUnpackFileNames(s.GetRawBytes(), s.GetByteCount(), &list2, &list3);
	JAssertEqual(2, list2.GetElementCount());
	JAssertStringsEqual("/bin/cat", *(list2.GetElement(1)));
	JAssertStringsEqual("/bin/ls", *(list2.GetElement(2)));

	list2.CleanOut();
	list3.CleanOut();
	s = JXPackFileNames(list1);
	s.Append("\r\nhttp://test.com/baz");	// hack to force invalid element
	JXUnpackFileNames(s.GetRawBytes(), s.GetByteCount(), &list2, &list3);
	JAssertEqual(2, list2.GetElementCount());
	JAssertStringsEqual("/bin/cat", *(list2.GetElement(1)));
	JAssertStringsEqual("/bin/ls", *(list2.GetElement(2)));
	JAssertEqual(1, list3.GetElementCount());
	JAssertStringsEqual("http://test.com/baz", *(list3.GetFirstElement()));
}
