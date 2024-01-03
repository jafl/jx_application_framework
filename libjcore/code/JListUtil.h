/******************************************************************************
 JListUtil.h

	Copyright (C) 1996-97 by John Lindal.

 ******************************************************************************/

#ifndef _H_JListUtil
#define _H_JListUtil

#include "JList.h"

void	JAdjustIndexAfterInsert(const JIndex firstInsertedIndex, const JSize count,
								JIndex* indexToAdjust);
bool	JAdjustIndexAfterRemove(const JIndex firstRemovedIndex, const JSize count,
								JIndex* indexToAdjust);
void	JAdjustIndexAfterMove(const JIndex origIndex, const JIndex newIndex,
							  JIndex* indexToAdjust);
void	JAdjustIndexAfterSwap(const JIndex swappedIndex1, const JIndex swappedIndex2,
							  JIndex* indexToAdjust);

#endif
