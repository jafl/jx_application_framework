/******************************************************************************
 JXSearchTextButton.cpp

	Maintains a pushable button with an arrow pointing to the right if
	forward, and to the left if backward.

	BASE CLASS = JXButton

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#include "JXSearchTextButton.h"
#include "JXWindowPainter.h"
#include "jXPainterUtil.h"
#include "JXColorManager.h"
#include <jx-af/jcore/jAssert.h>

const JCoordinate kArrowHalfWidth  = 2;
const JCoordinate kArrowHalfHeight = 4;

/******************************************************************************
 Constructor

 ******************************************************************************/

JXSearchTextButton::JXSearchTextButton
	(
	const bool			forward,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXButton(enclosure, hSizing, vSizing, x,y, w,h),
	itsFwdFlag( forward )
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXSearchTextButton::~JXSearchTextButton()
{
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
JXSearchTextButton::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	const JRect bounds = GetBounds();

	JRect r;
	r.top    = bounds.ycenter() - kArrowHalfHeight;
	r.bottom = r.top + 2*kArrowHalfHeight + 1;
	r.left   = bounds.xcenter() - kArrowHalfWidth;
	r.right  = r.left + 2*kArrowHalfWidth;

	const JColorID colorIndex =
		IsActive() ? JColorManager::GetGrayColor(40) :
					 JColorManager::GetInactiveLabelColor();
	if (itsFwdFlag)
	{
		r.right++;
		JXFillArrowRight(p, r, colorIndex);
	}
	else
	{
		r.left--;
		JXFillArrowLeft(p, r, colorIndex);
	}
}
