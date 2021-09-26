/******************************************************************************
 jXConstants.h

	Useful constants

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_jXConstants
#define _H_jXConstants

#include <jx-af/jcore/JFontStyle.h>
#include <X11/X.h>

const JCoordinate kJXDefaultBorderWidth = 2;

const Time kJXDoubleClickTime = 500;	// milliseconds

// Since 24-bit color is the maximum, we should never get indicies
// that are this large.

const JColorID kJXTransparentColor = kJIndexMax;		// illegal except for JXImageMask

#endif
