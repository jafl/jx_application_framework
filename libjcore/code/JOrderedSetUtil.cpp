/******************************************************************************
 JOrderedSetUtil.cpp

	Useful code for JOrderedSetT and JTableData message classes.

	Copyright © 1996-97 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JOrderedSetUtil.h>

/******************************************************************************
 JCompareIndices

 ******************************************************************************/

JOrderedSetT::CompareResult
JCompareIndices
	(
	const JIndex& i,
	const JIndex& j
	)
{
	if (i < j)
		{
		return JOrderedSetT::kFirstLessSecond;
		}
	else if (i == j)
		{
		return JOrderedSetT::kFirstEqualSecond;
		}
	else
		{
		return JOrderedSetT::kFirstGreaterSecond;
		}
}

/******************************************************************************
 JCompareSizes

 ******************************************************************************/

JOrderedSetT::CompareResult
JCompareSizes
	(
	const JSize& s1,
	const JSize& s2
	)
{
	if (s1 < s2)
		{
		return JOrderedSetT::kFirstLessSecond;
		}
	else if (s1 == s2)
		{
		return JOrderedSetT::kFirstEqualSecond;
		}
	else
		{
		return JOrderedSetT::kFirstGreaterSecond;
		}
}

/******************************************************************************
 JAdjustIndexAfterInsert

	The default way to adjust all indices after a block of elements have been inserted.

 ******************************************************************************/

void
JAdjustIndexAfterInsert
	(
	const JIndex	firstInsertedIndex,
	const JSize		count,
	JIndex*			indexToAdjust
	)
{
	if (*indexToAdjust >= firstInsertedIndex)
		{
		*indexToAdjust += count;
		}
}

/******************************************************************************
 JAdjustIndexAfterRemove

	The default way to adjust all indices after a block of elements have been removed.
	Returns kJFalse and sets *index=0 if this was one of the elements that were removed.

 ******************************************************************************/

JBoolean
JAdjustIndexAfterRemove
	(
	const JIndex	firstRemovedIndex,
	const JSize		count,
	JIndex*			indexToAdjust
	)
{
	if (*indexToAdjust < firstRemovedIndex)
		{
		// putting this first reduces the number of comparisons

		return kJTrue;
		}
	else if (*indexToAdjust > firstRemovedIndex + count-1)
		{
		*indexToAdjust -= count;
		return kJTrue;
		}
	else
		{
		*indexToAdjust = 0;
		return kJFalse;
		}
}

/******************************************************************************
 JAdjustIndexAfterMove

	The default way to adjust all indices after an element has moved.

 ******************************************************************************/

void
JAdjustIndexAfterMove
	(
	const JIndex	origIndex,
	const JIndex	newIndex,
	JIndex*			indexToAdjust
	)
{
	if (*indexToAdjust == origIndex)
		{
		*indexToAdjust = newIndex;
		}
	else if (origIndex < *indexToAdjust && *indexToAdjust <= newIndex)
		{
		(*indexToAdjust)--;
		}
	else if (newIndex <= *indexToAdjust && *indexToAdjust < origIndex)
		{
		(*indexToAdjust)++;
		}
}

/******************************************************************************
 JAdjustIndexAfterSwap

	The default way to adjust all indices after two elements have been swapped.

 ******************************************************************************/

void
JAdjustIndexAfterSwap
	(
	const JIndex	swappedIndex1,
	const JIndex	swappedIndex2,
	JIndex*			indexToAdjust
	)
{
	if (*indexToAdjust == swappedIndex1)
		{
		*indexToAdjust = swappedIndex2;
		}
	else if (*indexToAdjust == swappedIndex2)
		{
		*indexToAdjust = swappedIndex1;
		}
}
