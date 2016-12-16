/******************************************************************************
 testtable.cc

	Test code for the JHashTable class and related code.

	Copyright (C) 1997 by Dustin Laurence.  All rights reserved.

 *****************************************************************************/

#include <JUnitTestManager.h>
#include <JHashTable.h>
#include <JKLRand.h>
#include <JStrValue.h>
#include <jassert_simple.h>

	struct TableState {
		JHashTable< JStrValue<int> >* table;
		JSize           listLength;
		JHashValue*     hashList;
		JStrValue<int>* valueList;
		int*            countList;
	};

	// Check consistency with IsOK plus looping through entire table
	// Num elements match?
	// element counts match?
	// any counts negative?

	const JIndex kElementNum = 100;

	JKLRand gRand(192837);

int main()
{
	return JUnitTestManager::Execute();
}

/******************************************************************************
 StoreElement

 *****************************************************************************/

void
StoreElement
	(
	const JSize el,
	JHashTable< JStrValue<int> >* const table,
	const JSize                 listLength,
	const JHashValue*     const hashList,
	const JStrValue<int>* const valueList,
		  int*            const countList
	)
{
	JHashCursor< JStrValue<int> > cursor(table, hashList[el]);
	cursor.ForceNextOpen();
	JAssertFalse(cursor.IsFull());
	cursor.Set(hashList[el], valueList[el]);
	++countList[el];
}

/******************************************************************************
 StoreRand

 *****************************************************************************/

void
StoreRand
	(
	JHashTable< JStrValue<int> >* const table,
	const JSize                 listLength,
	const JHashValue*     const hashList,
	const JStrValue<int>* const valueList,
		  int*            const countList
	)
{
	const JIndex el = gRand.UniformLong(0, listLength-1);
	StoreElement(el, table, listLength, hashList, valueList, countList);
}

/******************************************************************************
 SearchElement

 *****************************************************************************/

void
SearchElement
	(
	const JSize el,
	const JHashTable< JStrValue<int> >* const table,
	const JSize                 listLength,
	const JHashValue*     const hashList,
	const JStrValue<int>* const valueList,
	const int*            const countList
	)
{
	JConstHashCursor< JStrValue<int> > cursor(table, hashList[el]);
	int count = 0;
	while ( cursor.NextHash() )
		{
		++count;
		JAssertTrue(cursor.IsFull());

		const JStrValue<int> value = cursor.GetValue();
		JAssertStringsEqual(*(valueList[el].key), *(value.key));
		JAssertEqual(valueList[el].value, value.value);
		}

	JAssertEqual(countList[el], count);
}

/******************************************************************************
 SearchRand

 *****************************************************************************/

void
SearchRand
	(
	const JHashTable< JStrValue<int> >* const table,
	const JSize                 listLength,
	const JHashValue*     const hashList,
	const JStrValue<int>* const valueList,
	const int*            const countList
	)
{
	const JIndex el = gRand.UniformLong(0, listLength-1);
	SearchElement(el, table, listLength, hashList, valueList, countList);
}

/******************************************************************************
 RemoveElement

 *****************************************************************************/

void
RemoveElement
	(
	const JSize el,
	JHashTable< JStrValue<int> >* const table,
	const JSize                 listLength,
	const JHashValue*     const hashList,
	const JStrValue<int>* const valueList,
		  int*            const countList
	)
{
	JHashCursor< JStrValue<int> > cursor(table, hashList[el]);
	JSize count = 0;
	while ( cursor.NextHash() )
		{
		++count;
		JAssertTrue(cursor.IsFull());
		JAssertEqual(hashList[el], cursor.GetHashValue());
		}

	if (count == 0)
		{
		return;
		}

	const JSize deleteNum = gRand.UniformLong(1, count);

	cursor.Reset();
	for (JIndex i=1;i<=deleteNum;i++)
		{
		JAssertTrue(cursor.NextHash());
		}

	cursor.Remove();
	--countList[el];
}

/******************************************************************************
 RemoveRand

 *****************************************************************************/

void
RemoveRand
	(
	JHashTable< JStrValue<int> >* const table,
	const JSize                 listLength,
	const JHashValue*     const hashList,
	const JStrValue<int>* const valueList,
		  int*            const countList
	)
{
	const JIndex firstEl = gRand.UniformLong(0, listLength-1);
	JIndex el = firstEl;
	while (countList[el] == 0)
		{
		++el;
		if (el == firstEl)
			{
			JAssertEqual(0, table->GetElementCount());
			return;
			}
		}

	RemoveElement(el, table, kElementNum, hashList, valueList, countList);
}

JTEST(Exercise)
{
//	TableState experiment = CreateTableState;

	JHashTable< JStrValue<int> > table;

	// Generate hash list
	JHashValue hashList[kElementNum];
	JIndex i;
	for (i=0;i<kElementNum;i++)
		{
		hashList[i] = gRand.UniformInt32();
		}

	// Generate key list
	const JUtf8Byte* keyList[kElementNum];
	for (i=0;i<kElementNum;i++)
		{
		keyList[i] = "Lazy key";
		}

	// Generate value list
	JStrValue<int> valueList[kElementNum];
	for (i=0;i<kElementNum;i++)
		{
		JString* s = jnew JString("Lazy key");
		assert( s != NULL );
		valueList[i].key = s;
		valueList[i].value = gRand.UniformInt32();
		}

	// Generate storage count list
	int countList[kElementNum];
	for (i=0;i<kElementNum;i++)
		{
		countList[i] = 0;
		}

//	JSize goal = 100;
	for (i=0;i<10000;i++)
		{
		JFloat prob = gRand.UniformClosedProb();
		if (prob < 0.1)
			{
			StoreRand(&table, kElementNum, hashList, valueList, countList);
			}
		else if (prob < 0.2)
			{
			RemoveRand(&table, kElementNum, hashList, valueList, countList);
			}
		else
			{
			SearchRand(&table, kElementNum, hashList, valueList, countList);
			}
		}

//cout << "\nSecond half\n" << endl;

	for (i=0;i<10000;i++)
		{
		JFloat prob = gRand.UniformClosedProb();
		if (prob < 0.05)
			{
			StoreRand(&table, kElementNum, hashList, valueList, countList);
			}
		else if (prob < 0.15)
			{
			RemoveRand(&table, kElementNum, hashList, valueList, countList);
			}
		else
			{
			SearchRand(&table, kElementNum, hashList, valueList, countList);
			}
		}

	std::cout << "         Size: " << table.GetTableSize() << std::endl;

	std::cout << "Element count: " << table.GetElementCount() << std::endl;
	std::cout << "   Load count: " << table.GetLoadCount() << std::endl;

	std::cout << "  Fill factor: " << table.GetFillFactor() << std::endl;
	std::cout << "  Load factor: " << table.GetLoadFactor() << std::endl;
}
