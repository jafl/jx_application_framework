/******************************************************************************
 jMouseUtil.h

	Copyright (C) 2003 by John Lindal.

 ******************************************************************************/

#ifndef _H_jMouseUtil
#define _H_jMouseUtil

#include "JPoint.h"

/******************************************************************************
 JMouseMoved

	Returns true if the points are further apart than the debounce width.

 ******************************************************************************/

inline bool
JMouseMoved
	(
	const JPoint&		pt1,
	const JPoint&		pt2,
	const JCoordinate	debounceWidth = 3
	)
{
	return (labs(pt2.x - pt1.x) > debounceWidth ||
			labs(pt2.y - pt1.y) > debounceWidth);
}

#endif
