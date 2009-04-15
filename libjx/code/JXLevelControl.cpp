/******************************************************************************
 JXLevelControl.cpp

	Fills from the bottom of the aperture up to the current level.

	BASE CLASS = JXSliderBase

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXLevelControl.h>
#include <JXWindowPainter.h>
#include <JXColormap.h>
#include <jXPainterUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXLevelControl::JXLevelControl
	(
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h)
	:
	JXSliderBase(0, enclosure, hSizing, vSizing, x,y, w,h)
{
	SetBorderWidth(kJXDefaultBorderWidth);
	SetBackColor((GetColormap())->GetDefaultSliderBackColor());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXLevelControl::~JXLevelControl()
{
}

/******************************************************************************
 DrawBorder (virtual protected)

 ******************************************************************************/

void
JXLevelControl::DrawBorder
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
JXLevelControl::Draw
	(	
	JXWindowPainter&	p,
	const JRect&		r
	)
{
	const JRect bounds = GetBounds();	
	JRect rThumb;

	if (GetOrientation() == kHorizontal)
		{
		rThumb.top    = 0;
		rThumb.left   = 0;
		rThumb.bottom = bounds.height();
		rThumb.right  = GetThumbPosition();
		}
	else
		{
		rThumb.top    = GetThumbPosition();
		rThumb.left   = 0;
		rThumb.bottom = bounds.height();
		rThumb.right  = bounds.width();
		}

	JXDrawUpFrame(p, rThumb, GetBorderWidth(), kJTrue, (GetColormap())->GetDefaultBackColor());
}
