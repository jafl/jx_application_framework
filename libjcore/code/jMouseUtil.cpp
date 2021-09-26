/******************************************************************************
 jMouseUtil.cpp

	Useful functions for dealing with the mouse cursor.

	Copyright (C) 2003 by John Lindal.

 ******************************************************************************/

#include "jx-af/jcore/jMouseUtil.h"
#include "jx-af/jcore/jAssert.h"

/******************************************************************************
 JMouseMoved

	Returns true if the points are further apart than the debounce width.

 ******************************************************************************/

bool
JMouseMoved
	(
	const JPoint&		pt1,
	const JPoint&		pt2,
	const JCoordinate	debounceWidth
	)
{
	return JLAbs(pt2.x - pt1.x) > debounceWidth ||
				JLAbs(pt2.y - pt1.y) > debounceWidth;
}
