/******************************************************************************
 JListUtil.cpp

	Useful code for JListT and JTableData message classes.

	Copyright (C) 1996-97 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JListUtil.h>

/******************************************************************************
 JCompareIndices

 ******************************************************************************/

JListT::CompareResult
JCompareIndices
	(
	const JIndex& i,
	const JIndex& j
	)
{
	if (i < j)
		{
		return JListT::kFirstLessSecond;
		}
	else if (i == j)
		{
		return JListT::kFirstEqualSecond;
		}
	else
		{
		return JListT::kFirstGreaterSecond;
		}
}

/******************************************************************************
 JCompareSizes

 ******************************************************************************/

JListT::CompareResult
JCompareSizes
	(
	const JSize& s1,
	const JSize& s2
	)
{
	if (s1 < s2)
		{
		return JListT::kFirstLessSecond;
		}
	else if (s1 == s2)
		{
		return JListT::kFirstEqualSecond;
		}
	else
		{
		return JListT::kFirstGreaterSecond;
		}
}

/******************************************************************************
 JCompareCoordinates

 ******************************************************************************/

JListT::CompareResult
JCompareCoordinates
	(
	const JCoordinate& x1,
	const JCoordinate& x2
	)
{
	if (x1 < x2)
		{
		return JListT::kFirstLessSecond;
		}
	else if (x1 == x2)
		{
		return JListT::kFirstEqualSecond;
		}
	else
		{
		return JListT::kFirstGreaterSecond;
		}
}

/******************************************************************************
 JCompareUInt64

 ******************************************************************************/

JListT::CompareResult
JCompareUInt64
	(
	const JUInt64& i1,
	const JUInt64& i2
	)
{
	if (i1 < i2)
		{
		return JListT::kFirstLessSecond;
		}
	else if (i1 == i2)
		{
		return JListT::kFirstEqualSecond;
		}
	else
		{
		return JListT::kFirstGreaterSecond;
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
