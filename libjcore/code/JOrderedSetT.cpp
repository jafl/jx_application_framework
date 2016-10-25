/******************************************************************************
 JOrderedSetT.cpp

	Provides unique point of definition of messages for JOrderedSet<T>.

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JOrderedSet.h>
#include <JOrderedSetUtil.h>

// JBroadcaster message types

// for all objects

const JUtf8Byte* JOrderedSetT::kElementsInserted = "ElementsInserted::JOrderedSetT";
const JUtf8Byte* JOrderedSetT::kElementsRemoved  = "ElementsRemoved::JOrderedSetT";

// for JBroadcasters

const JUtf8Byte* JOrderedSetT::kElementMoved    = "ElementMoved::JOrderedSetT";
const JUtf8Byte* JOrderedSetT::kElementsSwapped = "ElementsSwapped::JOrderedSetT";
const JUtf8Byte* JOrderedSetT::kElementChanged  = "ElementChanged::JOrderedSetT";
const JUtf8Byte* JOrderedSetT::kSorted          = "Sorted::JOrderedSetT";

// for iterators

const JUtf8Byte* JOrderedSetT::kGoingAway = "GoingAway::JOrderedSetT";
const JUtf8Byte* JOrderedSetT::kCopied    = "Copied::JOrderedSetT";


/******************************************************************************
 AdjustIndex

 ******************************************************************************/

void
JOrderedSetT::ElementsInserted::AdjustIndex
	(
	JIndex* index
	)
	const
{
	JAdjustIndexAfterInsert(GetFirstIndex(), GetCount(), index);
}

JBoolean
JOrderedSetT::ElementsRemoved::AdjustIndex
	(
	JIndex* index
	)
	const
{
	return JAdjustIndexAfterRemove(GetFirstIndex(), GetCount(), index);
}

void
JOrderedSetT::ElementMoved::AdjustIndex
	(
	JIndex* index
	)
	const
{
	JAdjustIndexAfterMove(itsOrigIndex, itsNewIndex, index);
}

void
JOrderedSetT::ElementsSwapped::AdjustIndex
	(
	JIndex* index
	)
	const
{
	JAdjustIndexAfterSwap(itsIndex1, itsIndex2, index);
}
