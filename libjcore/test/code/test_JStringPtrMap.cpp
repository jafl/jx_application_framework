/******************************************************************************
 tesptrtmap.cc

	Test code for the JStringPtrMap class and related code.

	Copyright (C) 1997 by Dustin Laurence.

 *****************************************************************************/

#include <JTestManager.h>
#include <JStringPtrMap.h>
#include <jAssert.h>

int main()
{
	return JTestManager::Execute();
}

JTEST(Exercise)
{
	const JSize gNumStrings = 16;

	JUtf8Byte keyBytes[8];
	strcpy(keyBytes, "testkey");

	JString key;
	key = keyBytes;

// Mostly just test PtrMap specific things like DeleteAll

// Ridiculous table size to really exercise resizer
	JStringPtrMap<JUtf8Byte> ptrMap(JPtrArrayT::kDeleteAllAsArrays, 0);

// Insert keys
	for (JSize i=0;i<gNumStrings;i++)
		{
		keyBytes[0] = 'A' + i;
		key         = keyBytes;
		JUtf8Byte* newValue = jnew JUtf8Byte[key.GetByteCount()+1];
		strcpy(newValue, keyBytes);
		JAssertTrue(ptrMap.SetNewElement(key, newValue));
		}
	JAssertEqual(gNumStrings, ptrMap.GetElementCount());
	JAssertEqualWithMessage(gNumStrings, ptrMap.GetLoadCount(), "Map contains extra deleted elements!");

// Compare with cursor
	JSize count = 0;
	JStringPtrMapCursor<JUtf8Byte> cursor(&ptrMap);
	while (cursor.Next())
		{
		const JString& thisKey = cursor.GetKey();
		JUtf8Byte* cursorValue = cursor.GetValue();
		JAssertStringsEqual(cursorValue, thisKey);

		JUtf8Byte* mapValue;
		JAssertTrue(ptrMap.GetElement(thisKey, &mapValue));
		JAssertStringsEqual(mapValue, cursorValue);
		++count;
		}
	JAssertEqual(gNumStrings, count);
}
