/******************************************************************************
 JListUtil.cpp

	Useful code for JListT and JTableData message classes.

	Copyright (C) 1996-97 by John Lindal.

 ******************************************************************************/

#include "JListUtil.h"

/******************************************************************************
 JCompareIndices

 ******************************************************************************/

std::weak_ordering
JCompareIndices
	(
	const JIndex& i,
	const JIndex& j
	)
{
	if (i < j)
	{
		return std::weak_ordering::less;
	}
	else if (i > j)
	{
		return std::weak_ordering::greater;
	}
	else
	{
		return std::weak_ordering::equivalent;
	}
}

/******************************************************************************
 JCompareSizes

 ******************************************************************************/

std::weak_ordering
JCompareSizes
	(
	const JSize& s1,
	const JSize& s2
	)
{
	if (s1 < s2)
	{
		return std::weak_ordering::less;
	}
	else if (s1 > s2)
	{
		return std::weak_ordering::greater;
	}
	else
	{
		return std::weak_ordering::equivalent;
	}
}

/******************************************************************************
 JCompareCoordinates

 ******************************************************************************/

std::weak_ordering
JCompareCoordinates
	(
	const JCoordinate& x1,
	const JCoordinate& x2
	)
{
	if (x1 < x2)
	{
		return std::weak_ordering::less;
	}
	else if (x1 > x2)
	{
		return std::weak_ordering::greater;
	}
	else
	{
		return std::weak_ordering::equivalent;
	}
}

/******************************************************************************
 JCompareUInt64

 ******************************************************************************/

std::weak_ordering
JCompareUInt64
	(
	const JUInt64& i1,
	const JUInt64& i2
	)
{
	if (i1 < i2)
	{
		return std::weak_ordering::less;
	}
	else if (i1 > i2)
	{
		return std::weak_ordering::greater;
	}
	else
	{
		return std::weak_ordering::equivalent;
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
	Returns false and sets *index=0 if this was one of the elements that were removed.

 ******************************************************************************/

bool
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

		return true;
	}
	else if (*indexToAdjust > firstRemovedIndex + count-1)
	{
		*indexToAdjust -= count;
		return true;
	}
	else
	{
		*indexToAdjust = 0;
		return false;
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
