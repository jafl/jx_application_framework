/******************************************************************************
 JListT.cpp

	Provides unique point of definition of messages for JList<T>.

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JList.h"
#include "JListUtil.h"

// JBroadcaster message types

// for all objects

const JUtf8Byte* JListT::kElementsInserted = "ElementsInserted::JListT";
const JUtf8Byte* JListT::kElementsRemoved  = "ElementsRemoved::JListT";

// for JBroadcasters

const JUtf8Byte* JListT::kElementMoved    = "ElementMoved::JListT";
const JUtf8Byte* JListT::kElementsSwapped = "ElementsSwapped::JListT";
const JUtf8Byte* JListT::kElementsChanged = "ElementsChanged::JListT";
const JUtf8Byte* JListT::kSorted          = "Sorted::JListT";

// for iterators

const JUtf8Byte* JListT::kGoingAway = "GoingAway::JListT";
const JUtf8Byte* JListT::kCopied    = "Copied::JListT";


/******************************************************************************
 AdjustIndex

 ******************************************************************************/

void
JListT::ElementsInserted::AdjustIndex
	(
	JIndex* index
	)
	const
{
	JAdjustIndexAfterInsert(GetFirstIndex(), GetCount(), index);
}

JBoolean
JListT::ElementsRemoved::AdjustIndex
	(
	JIndex* index
	)
	const
{
	return JAdjustIndexAfterRemove(GetFirstIndex(), GetCount(), index);
}

void
JListT::ElementMoved::AdjustIndex
	(
	JIndex* index
	)
	const
{
	JAdjustIndexAfterMove(itsOrigIndex, itsNewIndex, index);
}

void
JListT::ElementsSwapped::AdjustIndex
	(
	JIndex* index
	)
	const
{
	JAdjustIndexAfterSwap(itsIndex1, itsIndex2, index);
}
