/******************************************************************************
 JListUtil.h

	Interface for JListUtil.cc

	Copyright (C) 1996-97 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JOrderedSetUtil
#define _H_JOrderedSetUtil

#include <JList.h>

JListT::CompareResult JCompareIndices(const JIndex& i, const JIndex& j);
JListT::CompareResult JCompareSizes(const JSize& s1, const JSize& s2);
JListT::CompareResult JCompareUInt64(const JUInt64& i1, const JUInt64& i2);

void		JAdjustIndexAfterInsert(const JIndex firstInsertedIndex, const JSize count,
									JIndex* indexToAdjust);
JBoolean	JAdjustIndexAfterRemove(const JIndex firstRemovedIndex, const JSize count,
									JIndex* indexToAdjust);
void		JAdjustIndexAfterMove(const JIndex origIndex, const JIndex newIndex,
								  JIndex* indexToAdjust);
void		JAdjustIndexAfterSwap(const JIndex swappedIndex1, const JIndex swappedIndex2,
								  JIndex* indexToAdjust);

#endif
