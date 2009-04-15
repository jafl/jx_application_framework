/******************************************************************************
 JXDecorRect.cpp

	Draws a box, useful for decorative purposes.  Derived classes must
	decide how to draw the border.

	BASE CLASS = JXWidget

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXDecorRect.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXDecorRect::JXDecorRect
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
	JXWidget(enclosure, hSizing, vSizing, x,y, w,h)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXDecorRect::~JXDecorRect()
{
}

/******************************************************************************
 Draw (virtual protected)

	There is nothing to do since JXWidget draws our color.

 ******************************************************************************/

void
JXDecorRect::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
}
