/******************************************************************************
 JXBorderRect.cpp

	Draws a box with an simple frame, useful for decorative purposes.

	BASE CLASS = JXDecorRect

	Copyright © 1999 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXBorderRect.h>
#include <JXWindowPainter.h>
#include <jXPainterUtil.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXBorderRect::JXBorderRect
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
	SetBorderWidth(1);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXBorderRect::~JXBorderRect()
{
}

/******************************************************************************
 DrawBorder (virtual protected)

 ******************************************************************************/

void
JXBorderRect::DrawBorder
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
	p.JPainter::Rect(frame);
}
