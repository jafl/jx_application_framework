/******************************************************************************
 JXSlider.cpp

	Draws a thumb that can be moved up and down to set a value.

	BASE CLASS = JXSliderBase

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXSlider.h>
#include <JXWindowPainter.h>
#include <JXColormap.h>
#include <jXPainterUtil.h>
#include <jAssert.h>

const JSize kThumbHalfSize  = 5;
const JSize kTrackHalfWidth = 3;

/******************************************************************************
 Constructor

 ******************************************************************************/

JXSlider::JXSlider
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
	JXSliderBase(kThumbHalfSize + kJXDefaultBorderWidth,
				 enclosure, hSizing, vSizing, x,y, w,h)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXSlider::~JXSlider()
{
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
JXSlider::Draw
	(
	JXWindowPainter&	p,
	const JRect&		r
	)
{
	const JRect bounds = GetBounds();
	JRect rTrack       = bounds;
	JRect rThumb       = bounds;

	const JCoordinate thumbPosition = GetThumbPosition();
	if (GetOrientation() == kHorizontal)
		{
		rTrack.top    = bounds.ycenter() - kTrackHalfWidth;
		rTrack.bottom = bounds.ycenter() + kTrackHalfWidth;

		rThumb.left   = thumbPosition - kThumbHalfSize;
		rThumb.right  = thumbPosition + kThumbHalfSize;
		}
	else
		{
		rTrack.left   = bounds.xcenter() - kTrackHalfWidth;
		rTrack.right  = bounds.xcenter() + kTrackHalfWidth;

		rThumb.top    = thumbPosition - kThumbHalfSize;
		rThumb.bottom = thumbPosition + kThumbHalfSize;
		}

	JXDrawDownFrame(p, rTrack, kJXDefaultBorderWidth, kJTrue,
					(p.GetColormap())->GetDefaultSliderBackColor());
	JXDrawUpFrame(p, rThumb, kJXDefaultBorderWidth, kJTrue,
				  (p.GetColormap())->GetDefaultBackColor());
}
