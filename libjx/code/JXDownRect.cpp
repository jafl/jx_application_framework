/******************************************************************************
 JXDownRect.cpp

	Draws a box with a down frame, useful for decorative purposes.

	BASE CLASS = JXDecorRect

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXDownRect.h>
#include <JXWindowPainter.h>
#include <jXPainterUtil.h>
#include <jXConstants.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXDownRect::JXDownRect
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

JXDownRect::~JXDownRect()
{
}

/******************************************************************************
 DrawBorder (virtual protected)

 ******************************************************************************/

void
JXDownRect::DrawBorder
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
	JXDrawDownFrame(p, frame, GetBorderWidth());
}
