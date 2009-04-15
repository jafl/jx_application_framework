/******************************************************************************
 JOrderedSetT.cpp

	Provides unique point of definition of messages for JOrderedSet<T>.

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JOrderedSet.h>
#include <JOrderedSetUtil.h>

// JBroadcaster message types

// for all objects

const JCharacter* JOrderedSetT::kElementsInserted = "ElementsInserted::JOrderedSetT";
const JCharacter* JOrderedSetT::kElementsRemoved  = "ElementsRemoved::JOrderedSetT";

// for JBroadcasters

const JCharacter* JOrderedSetT::kElementMoved    = "ElementMoved::JOrderedSetT";
const JCharacter* JOrderedSetT::kElementsSwapped = "ElementsSwapped::JOrderedSetT";
const JCharacter* JOrderedSetT::kElementChanged  = "ElementChanged::JOrderedSetT";
const JCharacter* JOrderedSetT::kSorted          = "Sorted::JOrderedSetT";

// for iterators

const JCharacter* JOrderedSetT::kGoingAway = "GoingAway::JOrderedSetT";
const JCharacter* JOrderedSetT::kCopied    = "Copied::JOrderedSetT";


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
