/******************************************************************************
 JXDragPainter.cpp

	This class implements the functions required to draw to an X window
	during a drag operation.

	Since dragging should only occur between a mouse down and mouse up
	event pair, it is safe to use XSetFunction.

	BASE CLASS = JXWindowPainter

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXDragPainter.h>
#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXGC.h>
#include <string.h>
#include <jMath.h>
#include <jAssert.h>

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
	JXWindowPainter(new JXGC(display, window->GetColormap(), window->GetXWindow()),
					window->GetXWindow(), defaultClipRect, NULL)
{
	JXGC* gc = GetGC();
	assert( gc != NULL );

	gc->SetDrawingFunction(GXinvert);
	gc->SetSubwindowMode(IncludeInferiors);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXDragPainter::~JXDragPainter()
{
	JXGC* gc = GetGC();
	delete gc;
}
