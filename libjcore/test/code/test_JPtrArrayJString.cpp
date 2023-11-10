/******************************************************************************
 test_JPtrArrayJString.cpp

	Program to test JPtrArray<JString>.

	Written by John Lindal.

 *****************************************************************************/

#include "JTestManager.h"
#include "JPtrArray-JString.h"
#include <sstream>
#include "jAssert.h"

int main()
{
	return JTestManager::Execute();
}

JTEST(ReadWritePtrArray)
{
	JPtrArray<JString> list1(JPtrArrayT::kDeleteAll),
					   list2(JPtrArrayT::kDeleteAll);

	list2.Append(JString("foobar", JString::kNoCopy));

	std::stringstream io1;
	io1 << list1;
	io1.seekg(0);
	io1 >> list2;
	JAssertTrue(list2.IsEmpty());

	list1.Append(JString("foobar", JString::kNoCopy));
	std::stringstream io2;
	io2 << list1;
	io2.seekg(0);
	io2 >> list2;
	JAssertEqual(1, list2.GetItemCount());
	JAssertStringsEqual("foobar", *(list2.GetItem(1)));

	list1.Prepend(JString("barbaz", JString::kNoCopy));

	std::stringstream io3;
	io3 << list1;
	io3.seekg(0);
	io3 >> list2;
	JAssertEqual(2, list2.GetItemCount());
	JAssertStringsEqual("barbaz", *(list2.GetItem(1)));
	JAssertStringsEqual("foobar", *(list2.GetItem(2)));
}

JTEST(ReadWritePtrMap)
{
	JStringPtrMap<JString> map1(JPtrArrayT::kDeleteAll),
						   map2(JPtrArrayT::kDeleteAll);

	const JString key1("a", JString::kNoCopy),
				  key2("z", JString::kNoCopy);

	JString* s;

	map2.SetNewItem(key1, JString("foobar", JString::kNoCopy));

	std::stringstream io1;
	io1 << map1;
	io1.seekg(0);
	io1 >> map2;
	JAssertTrue(map2.IsEmpty());

	map1.SetNewItem(key1, JString("foobar", JString::kNoCopy));
	std::stringstream io2;
	io2 << map1;
	io2.seekg(0);
	io2 >> map2;
	JAssertEqual(1, map2.GetItemCount());
	JAssertTrue(map2.GetItem(key1, &s));
	JAssertStringsEqual("foobar", *s);

	map1.SetNewItem(key2, JString("barbaz", JString::kNoCopy));

	std::stringstream io3;
	io3 << map1;
	io3.seekg(0);
	io3 >> map2;
	JAssertEqual(2, map2.GetItemCount());
	JAssertTrue(map2.GetItem(key1, &s));
	JAssertStringsEqual("foobar", *s);
	JAssertTrue(map2.GetItem(key2, &s));
	JAssertStringsEqual("barbaz", *s);
}

JTEST(JSameStrings)
{
	JPtrArray<JString> list1(JPtrArrayT::kDeleteAll),
					   list2(JPtrArrayT::kDeleteAll);

	JAssertTrue(JSameStrings(list1, list2, JString::kIgnoreCase));
	JAssertTrue(JSameStrings(list1, list2, JString::kCompareCase));

	list1.Append(JString("foobar", JString::kNoCopy));
	JAssertFalse(JSameStrings(list1, list2, JString::kIgnoreCase));
	JAssertFalse(JSameStrings(list1, list2, JString::kCompareCase));

	list2.Append(JString("barbaz", JString::kNoCopy));
	JAssertFalse(JSameStrings(list1, list2, JString::kIgnoreCase));
	JAssertFalse(JSameStrings(list1, list2, JString::kCompareCase));

	list2.SetItem(1, JString("foobar", JString::kNoCopy), JPtrArrayT::kDelete);
	JAssertTrue(JSameStrings(list1, list2, JString::kIgnoreCase));
	JAssertTrue(JSameStrings(list1, list2, JString::kCompareCase));

	list1.Append(JString("barbaz", JString::kNoCopy));
	JAssertFalse(JSameStrings(list1, list2, JString::kIgnoreCase));
	JAssertFalse(JSameStrings(list1, list2, JString::kCompareCase));

	list2.Append(JString("barbaz", JString::kNoCopy));
	JAssertTrue(JSameStrings(list1, list2, JString::kIgnoreCase));
	JAssertTrue(JSameStrings(list1, list2, JString::kCompareCase));

	list2.SetItem(1, JString("FOOBAR", JString::kNoCopy), JPtrArrayT::kDelete);
	JAssertTrue(JSameStrings(list1, list2, JString::kIgnoreCase));
	JAssertFalse(JSameStrings(list1, list2, JString::kCompareCase));
}

JTEST(Join)
{
	JPtrArray<JString> list(JPtrArrayT::kDeleteAll);

	JAssertStringsEqual("", JStringJoin(",", list));

	list.Append(JString("foobar", JString::kNoCopy));
	JAssertStringsEqual("foobar", JStringJoin(",", list));

	list.Append(JString("barbaz", JString::kNoCopy));
	JAssertStringsEqual("foobar,barbaz", JStringJoin(",", list));

	JAssertStringsEqual("foobarbarbaz", JStringJoin("", list));
}
