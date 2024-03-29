/******************************************************************************
 JXBorderRect.cpp

	Draws a box with an simple frame, useful for decorative purposes.

	BASE CLASS = JXDecorRect

	Copyright (C) 1999 by Glenn W. Bach.

 ******************************************************************************/

#include "JXBorderRect.h"
#include "JXWindowPainter.h"
#include "jXPainterUtil.h"
#include <jx-af/jcore/JColorManager.h>

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
	JXDecorRect(enclosure, hSizing, vSizing, x,y, w,h),
	itsColor(JColorManager::GetBlackColor())
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
	p.SetPenColor(itsColor);
	p.SetFilling(true);
	p.Rect(frame);
}
