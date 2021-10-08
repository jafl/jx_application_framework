/******************************************************************************
 JXDragPainter.h

	Interface for the JXDragPainter class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXDragPainter
#define _H_JXDragPainter

#include "jx-af/jx/JXWindowPainter.h"

class JXWindow;

class JXDragPainter : public JXWindowPainter
{
public:

	JXDragPainter(JXDisplay* display, JXWindow* window,
				  const JRect& defaultClipRect);

	~JXDragPainter();
};

#endif
