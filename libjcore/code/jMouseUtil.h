/******************************************************************************
 jMouseUtil.h

	Copyright (C) 2003 by John Lindal.

 ******************************************************************************/

#ifndef _H_jMouseUtil
#define _H_jMouseUtil

#include <JPoint.h>

JBoolean	JMouseMoved(const JPoint& pt1, const JPoint& pt2,
						const JCoordinate debounceWidth = 3);

#endif
