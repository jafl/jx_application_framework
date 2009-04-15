/******************************************************************************
 JXEmbossedRect.cpp

	Draws a box with an embossed frame, useful for decorative purposes.

	BASE CLASS = JXDecorRect

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXEmbossedRect.h>
#include <JXWindowPainter.h>
#include <jXPainterUtil.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXEmbossedRect::JXEmbossedRect
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
	itsUpWidth      = 1;
	itsBetweenWidth = 0;
	itsDownWidth    = 1;
	UpdateBorderWidth();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXEmbossedRect::~JXEmbossedRect()
{
}

/******************************************************************************
 DrawBorder (virtual protected)

 ******************************************************************************/

void
JXEmbossedRect::DrawBorder
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
	JXDrawEmbossedFrame(p, frame, itsUpWidth, itsBetweenWidth, itsDownWidth);
}
