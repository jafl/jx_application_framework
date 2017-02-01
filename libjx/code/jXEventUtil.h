/******************************************************************************
 jXEventUtil.h

	Interface for jXEventUtil.cc

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_jXEventUtil
#define _H_jXEventUtil

#include <JUtf8Character.h>
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

JUtf8Byte	JXCtrl(const JUtf8Character& c);

#endif
