/******************************************************************************
 JXDecorRect.cpp

	Draws a box, useful for decorative purposes.  Derived classes must
	decide how to draw the border.

	BASE CLASS = JXWidget

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

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

/******************************************************************************
 NeedsInternalFTC (virtual protected)

	Return kJTrue if the contents are a set of widgets that need to expand.

 ******************************************************************************/

JBoolean
JXDecorRect::NeedsInternalFTC()
	const
{
	return kJTrue;
}
