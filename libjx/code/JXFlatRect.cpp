/******************************************************************************
 JXFlatRect.cpp

	Draws a flat rectangle, useful for decorative purposes.

	BASE CLASS = JXDecorRect

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXFlatRect.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXFlatRect::JXFlatRect
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
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXFlatRect::~JXFlatRect()
{
}

/******************************************************************************
 DrawBorder (virtual protected)

 ******************************************************************************/

void
JXFlatRect::DrawBorder
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
}
