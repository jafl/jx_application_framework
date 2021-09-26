/******************************************************************************
 JXDragPainter.cpp

	This class implements the functions required to draw to an X window
	during a drag operation.

	Since dragging should only occur between a mouse down and mouse up
	event pair, it is safe to use XSetFunction.

	BASE CLASS = JXWindowPainter

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "jx-af/jx/JXDragPainter.h"
#include "jx-af/jx/JXDisplay.h"
#include "jx-af/jx/JXWindow.h"
#include "jx-af/jx/JXGC.h"
#include <string.h>
#include <jx-af/jcore/jMath.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXDragPainter::JXDragPainter
	(
	JXDisplay*		display,
	JXWindow*		window,
	const JRect&	defaultClipRect
	)
	:
	JXWindowPainter(jnew JXGC(display, window->GetXWindow()),
					window->GetXWindow(), defaultClipRect, nullptr)
{
	JXGC* gc = GetGC();
	assert( gc != nullptr );

	gc->SetDrawingFunction(GXinvert);
	gc->SetSubwindowMode(IncludeInferiors);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXDragPainter::~JXDragPainter()
{
	JXGC* gc = GetGC();
	jdelete gc;
}
