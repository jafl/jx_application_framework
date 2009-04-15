/******************************************************************************
 jXConstants.h

	Useful constants

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_jXConstants
#define _H_jXConstants

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JFontStyle.h>
#include <X11/X.h>

const JCoordinate kJXDefaultBorderWidth = 2;

const JSize kJXDefaultFontSize = kJDefaultFontSize;

const Time kJXDoubleClickTime = 500;	// milliseconds

// Since 24-bit color is the maximum, we should never get indicies
// that are this large.

const JColorIndex kJXTransparentColor = kJIndexMax;		// illegal except for JXImageMask

#endif
