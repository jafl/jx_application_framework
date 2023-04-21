/******************************************************************************
 JListUtil.h

	Copyright (C) 1996-97 by John Lindal.

 ******************************************************************************/

#ifndef _H_JListUtil
#define _H_JListUtil

#include "JList.h"

std::weak_ordering JCompareIndices(const JIndex& i, const JIndex& j);
std::weak_ordering JCompareSizes(const JSize& s1, const JSize& s2);
std::weak_ordering JCompareCoordinates(const JCoordinate& x1, const JCoordinate& x2);
std::weak_ordering JCompareUInt64(const JUInt64& i1, const JUInt64& i2);

void	JAdjustIndexAfterInsert(const JIndex firstInsertedIndex, const JSize count,
								JIndex* indexToAdjust);
bool	JAdjustIndexAfterRemove(const JIndex firstRemovedIndex, const JSize count,
								JIndex* indexToAdjust);
void	JAdjustIndexAfterMove(const JIndex origIndex, const JIndex newIndex,
							  JIndex* indexToAdjust);
void	JAdjustIndexAfterSwap(const JIndex swappedIndex1, const JIndex swappedIndex2,
							  JIndex* indexToAdjust);

#endif
