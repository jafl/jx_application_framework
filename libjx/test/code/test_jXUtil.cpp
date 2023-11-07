/******************************************************************************
 test_jXUtil.cpp

	Test X utility functions.

	Written by John Lindal.

 *****************************************************************************/

#include <jx-af/jcore/JTestManager.h>
#include <jx-af/jx/jXUtil.h>
#include <jx-af/jcore/jAssert.h>

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
	list1.Append(JString("foo", JString::kNoCopy));
	s = JXPackStrings(list1);
	JXUnpackStrings(s.GetRawBytes(), s.GetByteCount(), &list2);
	JAssertEqual(1, list2.GetItemCount());
	JAssertStringsEqual("foo", *list2.GetFirstItem());

	list2.CleanOut();
	list1.Prepend(JString("bar", JString::kNoCopy));
	s = JXPackStrings(list1);
	JXUnpackStrings(s.GetRawBytes(), s.GetByteCount(), &list2);
	JAssertEqual(2, list2.GetItemCount());
	JAssertStringsEqual("bar", *list2.GetItem(1));
	JAssertStringsEqual("foo", *list2.GetItem(2));
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
	list1.Append(JString("/bin/ls", JString::kNoCopy));
	s = JXPackFileNames(list1);
	JXUnpackFileNames(s.GetRawBytes(), s.GetByteCount(), &list2, &list3);
	JAssertEqual(1, list2.GetItemCount());
	JAssertStringsEqual("/bin/ls", *list2.GetFirstItem());

	list2.CleanOut();
	list3.CleanOut();
	list1.Prepend(JString("/bin/cat", JString::kNoCopy));
	s = JXPackFileNames(list1);
	JXUnpackFileNames(s.GetRawBytes(), s.GetByteCount(), &list2, &list3);
	JAssertEqual(2, list2.GetItemCount());
	JAssertStringsEqual("/bin/cat", *list2.GetItem(1));
	JAssertStringsEqual("/bin/ls", *list2.GetItem(2));

	list2.CleanOut();
	list3.CleanOut();
	s = JXPackFileNames(list1);
	s.Append("\r\nhttp://test.com/baz");	// hack to force invalid item
	JXUnpackFileNames(s.GetRawBytes(), s.GetByteCount(), &list2, &list3);
	JAssertEqual(2, list2.GetItemCount());
	JAssertStringsEqual("/bin/cat", *list2.GetItem(1));
	JAssertStringsEqual("/bin/ls", *list2.GetItem(2));
	JAssertEqual(1, list3.GetItemCount());
	JAssertStringsEqual("http://test.com/baz", *list3.GetFirstItem());
}
