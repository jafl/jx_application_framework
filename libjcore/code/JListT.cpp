/******************************************************************************
 JListT.cpp

	Provides unique point of definition of messages for JList<T>.

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JList.h"
#include "JListUtil.h"

// JBroadcaster message types

// for all objects

const JUtf8Byte* JListT::kItemsInserted = "ItemsInserted::JListT";
const JUtf8Byte* JListT::kItemsRemoved  = "ItemsRemoved::JListT";

// for JBroadcasters

const JUtf8Byte* JListT::kItemMoved    = "ItemMoved::JListT";
const JUtf8Byte* JListT::kItemsSwapped = "ItemsSwapped::JListT";
const JUtf8Byte* JListT::kItemsChanged = "ItemsChanged::JListT";
const JUtf8Byte* JListT::kSorted          = "Sorted::JListT";

// for iterators

const JUtf8Byte* JListT::kGoingAway = "GoingAway::JListT";
const JUtf8Byte* JListT::kCopied    = "Copied::JListT";


/******************************************************************************
 AdjustIndex

 ******************************************************************************/

void
JListT::ItemsInserted::AdjustIndex
	(
	JIndex* index
	)
	const
{
	JAdjustIndexAfterInsert(GetFirstIndex(), GetCount(), index);
}

bool
JListT::ItemsRemoved::AdjustIndex
	(
	JIndex* index
	)
	const
{
	return JAdjustIndexAfterRemove(GetFirstIndex(), GetCount(), index);
}

void
JListT::ItemMoved::AdjustIndex
	(
	JIndex* index
	)
	const
{
	JAdjustIndexAfterMove(itsOrigIndex, itsNewIndex, index);
}

void
JListT::ItemsSwapped::AdjustIndex
	(
	JIndex* index
	)
	const
{
	JAdjustIndexAfterSwap(itsIndex1, itsIndex2, index);
}
