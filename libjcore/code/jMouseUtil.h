/******************************************************************************
 jMouseUtil.h

	Copyright © 2003 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_jMouseUtil
#define _H_jMouseUtil

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JPoint.h>

JBoolean	JMouseMoved(const JPoint& pt1, const JPoint& pt2,
						const JCoordinate debounceWidth = 3);

#endif
