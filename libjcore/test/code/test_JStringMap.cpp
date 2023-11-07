/******************************************************************************
 test_JStringMap.cpp

	Test code for the JStringMap class and related code.

	Copyright (C) 1997 by Dustin Laurence.

 *****************************************************************************/

#include "JTestManager.h"
#include "JStringMap.h"
#include "jAssert.h"

int main()
{
	return JTestManager::Execute();
}

JTEST(GetSet)
{
	JString key;
	key = "testkey";

	JStringMap<int> map;

	int v;
	JAssertFalse(map.GetItem(key, &v));

	map.SetItem(key, 42);

	v = 10;
	JAssertTrue(map.GetItem(key, &v));
	JAssertEqual(42, v);

	JAssertTrue(map.RemoveItem(key));

	JAssertFalse(map.GetItem(key, &v));
}

JTEST(InsertRemove)
{
	JIndex i;
	const JSize gNumStrings = 16;
	int v;

	JUtf8Byte keyBytes[8];
	strcpy(keyBytes, "testkey");

	JString key;
	key = keyBytes;

	JStringMap<int> map(0); // Ridiculous table size to really exercise resizer

	JAssertEqual(0, map.GetItemCount());
	JAssertEqualWithMessage(0, map.GetLoadCount(), "Map should not have deleted elements!");

// Search for keys, all should fail
	for (i=0;i<gNumStrings;i++)
	{
		keyBytes[0] = 'A' + i;
		key         = keyBytes;
		JAssertFalse(map.GetItem(key, &v));
	}
	JAssertEqual(0, map.GetItemCount());
	JAssertEqualWithMessage(0, map.GetLoadCount(), "Map should not have deleted elements!");

// Insert keys
	for (i=0;i<gNumStrings;i++)
	{
		keyBytes[0] = 'A' + i;
		key         = keyBytes;
//		cout << "Inserting string \"" << key << "\" with fill/load factor "
//			 << map.GetFillFactor() << "/" << map.GetLoadFactor() << endl;
		JAssertTrue(map.SetNewElement(key, i+42));
	}
	JAssertEqual(gNumStrings, map.GetItemCount());
	JAssertEqualWithMessage(gNumStrings, map.GetLoadCount(), "Map contains extra deleted elements!");

{
// Compare with cursor
	JSize count = 0;
	JStringMapCursor<int> cursor(&map);
	while (cursor.Next())
	{
		const JString& thisKey = cursor.GetKey();
		int value = cursor.GetValue();
		JAssertEqual(value - 42, thisKey.GetBytes()[0] - 'A');

		keyBytes[0] = value - 42 + 'A';
		JAssertStringsEqual(keyBytes, thisKey);
		++count;
	}
	JAssertEqual(gNumStrings, count);
}

// Search for keys, should all succeed
	for (i=0;i<gNumStrings;i++)
	{
		keyBytes[0] = 'A' + i;
		key         = keyBytes;
		JAssertTrue(map.GetItem(key, &v));
		JAssertEqual(i+42, JSize(v));
	}
	JAssertEqual(gNumStrings, map.GetItemCount());
	JAssertEqualWithMessage(gNumStrings, map.GetLoadCount(), "Map contains extra deleted elements!");

// Search for other keys, all should fail
	for (i=0;i<gNumStrings;i++)
	{
		keyBytes[0] = 'a' + i;
		key         = keyBytes;
		JAssertFalse(map.GetItem(key, &v));
	}
	JAssertEqual(gNumStrings, map.GetItemCount());
	JAssertEqualWithMessage(gNumStrings, map.GetLoadCount(), "Map contains extra deleted elements!");

// Modify values
	for (i=0;i<gNumStrings;i++)
	{
		keyBytes[0] = 'A' + i;
		key         = keyBytes;
//		cout << "Changing value of string \"" << key << "\" with fill/load factor "
//		     << map.GetFillFactor() << "/" << map.GetLoadFactor() << endl;
		JAssertTrue(map.SetOldElement(key, 99+i));
	}
	JAssertEqual(gNumStrings, map.GetItemCount());
	JAssertEqualWithMessage(gNumStrings, map.GetLoadCount(), "Map contains extra deleted elements!");

// Search for keys, should all succeed
	for (i=0;i<gNumStrings;i++)
	{
		keyBytes[0] = 'A' + i;
		key         = keyBytes;
		JAssertTrue(map.GetItem(key, &v));
		JAssertEqual(i+99, JSize(v));
	}
	JAssertEqual(gNumStrings, map.GetItemCount());
	JAssertEqualWithMessage(gNumStrings, map.GetLoadCount(), "Map contains extra deleted elements!");

// Search for other keys, all should fail
	for (i=0;i<gNumStrings;i++)
	{
		keyBytes[0] = 'a' + i;
		key         = keyBytes;
		JAssertFalse(map.GetItem(key, &v));
	}
	JAssertEqual(gNumStrings, map.GetItemCount());
	JAssertEqualWithMessage(gNumStrings, map.GetLoadCount(), "Map contains extra deleted elements!");

// Remove other keys, all should fail
	for (i=0;i<gNumStrings;i++)
	{
		keyBytes[0] = 'a' + i;
		key         = keyBytes;
		JAssertFalse(map.RemoveItem(key));
	}
	JAssertEqual(gNumStrings, map.GetItemCount());
	JAssertEqualWithMessage(gNumStrings, map.GetLoadCount(), "Map contains extra deleted elements!");

// Remove keys, all should succeed
	for (i=0;i<gNumStrings;i++)
	{
		keyBytes[0] = 'A' + i;
		key         = keyBytes;
//		cout << "Removing string \"" << key << "\" with fill/load factor "
//			 << map.GetFillFactor() << "/" << map.GetLoadFactor() << endl;
		JAssertTrue(map.RemoveItem(key));
	}
	JAssertEqual(0, map.GetItemCount());
	JAssertEqualWithMessage(0, map.GetLoadCount(), "Map still contains deleted elements!");

// Remove other keys, all should fail
	for (i=0;i<gNumStrings;i++)
	{
		keyBytes[0] = 'a' + i;
		key         = keyBytes;
		JAssertFalse(map.RemoveItem(key));
	}
	JAssertEqual(0, map.GetItemCount());
	JAssertEqualWithMessage(0, map.GetLoadCount(), "Map still contains deleted elements!");

// Re-insert
	for (i=0;i<gNumStrings;i++)
	{
		keyBytes[0] = 'A' + i;
		key         = keyBytes;
//		cout << "Inserting string \"" << key << "\" with fill/load factor "
//			 << map.GetFillFactor() << "/" << map.GetLoadFactor() << endl;
		JAssertTrue(map.SetNewElement(key, -i));
	}
	JAssertEqual(gNumStrings, map.GetItemCount());
	JAssertEqualWithMessage(gNumStrings, map.GetLoadCount(), "Map contains extra deleted elements!");

// Remove them all at once
	map.RemoveAll();
	JAssertEqual(0, map.GetItemCount());
	JAssertEqualWithMessage(0, map.GetLoadCount(), "Map still contains deleted elements!");
	JAssertEqual(0, map.GetFillFactor());
	JAssertEqual(0, map.GetLoadFactor());
}
