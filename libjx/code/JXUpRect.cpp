/******************************************************************************
 JXUpRect.cpp

	Draws a box with an up frame, useful for decorative purposes.

	BASE CLASS = JXDecorRect

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXUpRect.h>
#include <JXWindowPainter.h>
#include <jXPainterUtil.h>
#include <jXConstants.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXUpRect::JXUpRect
	(
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXDecorRect(enclosure, hSizing, vSizing, x,y, w,h)
{
	SetBorderWidth(kJXDefaultBorderWidth);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXUpRect::~JXUpRect()
{
}

/******************************************************************************
 DrawBorder (virtual protected)

 ******************************************************************************/

void
JXUpRect::DrawBorder
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
	JXDrawUpFrame(p, frame, GetBorderWidth());
}
