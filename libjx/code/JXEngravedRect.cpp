/******************************************************************************
 JXEngravedRect.cpp

	Draws a box with an engraved frame, useful for decorative purposes.

	BASE CLASS = JXDecorRect

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXEngravedRect.h>
#include <JXWindowPainter.h>
#include <jXPainterUtil.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXEngravedRect::JXEngravedRect
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
	itsDownWidth    = 1;
	itsBetweenWidth = 0;
	itsUpWidth      = 1;
	UpdateBorderWidth();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXEngravedRect::~JXEngravedRect()
{
}

/******************************************************************************
 DrawBorder (virtual protected)

 ******************************************************************************/

void
JXEngravedRect::DrawBorder
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
	JXDrawEngravedFrame(p, frame, itsDownWidth, itsBetweenWidth, itsUpWidth);
}
