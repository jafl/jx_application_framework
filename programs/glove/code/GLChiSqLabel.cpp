/******************************************************************************
 GLChiSqLabel.cpp

	Draws a box with an up frame, useful for decorative purposes.

	BASE CLASS = JXDecorRect

	Copyright (C) 2000 by Glenn W. Bach.

 ******************************************************************************/

#include <GLChiSqLabel.h>

#include <JXColorManager.h>
#include <JXWindowPainter.h>
#include <JXFontManager.h>
#include <jXPainterUtil.h>
#include <jXConstants.h>

#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GLChiSqLabel::GLChiSqLabel
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

GLChiSqLabel::~GLChiSqLabel()
{
}

/******************************************************************************
 DrawBorder (virtual protected)

 ******************************************************************************/

void
GLChiSqLabel::DrawBorder
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
	JXDrawDownFrame(p, frame, GetBorderWidth());
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
GLChiSqLabel::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	JXDrawUpFrame(p, GetBounds(), GetBorderWidth());
	p.JPainter::String(0, 0, JGetString("Label::GLChiSqLabel"), GetBoundsWidth(), JPainter::kHAlignCenter, GetBoundsHeight(), JPainter::kVAlignCenter);
}
