/******************************************************************************
 jMouseUtil.cpp

	Useful functions for dealing with the mouse cursor.

	Copyright © 2003 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <jMouseUtil.h>
#include <jAssert.h>

/******************************************************************************
 JMouseMoved

	Returns kJTrue if the points are further apart than the debounce width.

 ******************************************************************************/

JBoolean
JMouseMoved
	(
	const JPoint&		pt1,
	const JPoint&		pt2,
	const JCoordinate	debounceWidth
	)
{
	return JI2B(JLAbs(pt2.x - pt1.x) > debounceWidth ||
				JLAbs(pt2.y - pt1.y) > debounceWidth);
}
