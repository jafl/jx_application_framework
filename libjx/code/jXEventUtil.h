/******************************************************************************
 jXEventUtil.h

	Interface for jXEventUtil.cc

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_jXEventUtil
#define _H_jXEventUtil

// for convenience

#include <JPoint.h>
#include <JXButtonStates.h>
#include <JXKeyModifiers.h>

class JXDisplay;

// useful routines

JBoolean JXGetEventTime(const XEvent& xEvent, Time* time);
JBoolean JXGetMouseLocation(const XEvent& xEvent, JXDisplay* display,
							JPoint* pt);
JBoolean JXGetButtonAndModifierStates(const XEvent& xEvent, JXDisplay* display,
									  unsigned int* state);

JBoolean JXIsPrint(const int keysym);

inline JCharacter
JXCtrl
	(
	const JCharacter c		// must be character @ (0x40) through _ (0x5F)
	)
{
	return ( ('@' <= c && c <= '_') ? (c - '@') : c );
}

#endif
