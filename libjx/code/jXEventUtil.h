/******************************************************************************
 jXEventUtil.h

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_jXEventUtil
#define _H_jXEventUtil

#include <jx-af/jcore/JUtf8Character.h>
#include <jx-af/jcore/JPoint.h>
#include "jx-af/jx/JXButtonStates.h"
#include "jx-af/jx/JXKeyModifiers.h"

class JXDisplay;

// useful routines

bool JXGetEventTime(const XEvent& xEvent, Time* time);
bool JXGetMouseLocation(const XEvent& xEvent, JXDisplay* display,
							JPoint* pt);
bool JXGetButtonAndModifierStates(const XEvent& xEvent, JXDisplay* display,
									  unsigned int* state);

JUtf8Byte	JXCtrl(const JUtf8Character& c);


/******************************************************************************
 JXCtrl

	Input must be character @ (0x40) through _ (0x5F).

 ******************************************************************************/

inline JUtf8Byte
JXCtrl
	(
	const JUtf8Byte c		// must be character @ (0x40) through _ (0x5F)
	)
{
	return ( ('@' <= c && c <= '_') ? (c - '@') : -1 );
}

inline JUtf8Byte
JXCtrl
	(
	const JUtf8Character& c
	)
{
	return JXCtrl(c.GetBytes()[0]);
}

#endif
