/******************************************************************************
 JXProgressIndicator.cpp

	Displays the progress of an operation, measured from 0 to itsMaxValue.

	BASE CLASS = JXWidget

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXProgressIndicator.h>
#include <JXWindowPainter.h>
#include <jXPainterUtil.h>
#include <JXColormap.h>
#include <jXConstants.h>
#include <jAssert.h>

// Netscape colors

const JRGB foreColor(0,     0,     46920);
const JRGB backColor(26520, 45900, 44880);

/******************************************************************************
 Constructor

 ******************************************************************************/

JXProgressIndicator::JXProgressIndicator
	(
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h)
	:
	JXWidget(enclosure, hSizing, vSizing, x,y, w,h)
{
	itsValue    = 0;
	itsMaxValue = 1;

	JXColormap* colormap = GetColormap();

	if (!colormap->JColormap::AllocateStaticColor(foreColor, &itsForeColor))
		{
		itsForeColor = colormap->GetBlueColor();
		}
	if (!colormap->JColormap::AllocateStaticColor(backColor, &itsBackColor))
		{
		itsBackColor = colormap->GetDefaultSliderBackColor();
		}
	SetBackColor(itsBackColor);

	SetBorderWidth(2);
}

/******************************************************************************
 Destructor

	We don't deallocate our colors because we always need the same ones
	and we will probably be used again later.

 ******************************************************************************/

JXProgressIndicator::~JXProgressIndicator()
{
}

/******************************************************************************
 SetValue

 ******************************************************************************/

void
JXProgressIndicator::SetValue
	(
	const JSize value
	)
{
	assert( value <= itsMaxValue );

	if (value != itsValue)
		{
		itsValue = value;
		Redraw();
		}
}

/******************************************************************************
 SetMaxValue

 ******************************************************************************/

void
JXProgressIndicator::SetMaxValue
	(
	const JSize maxValue
	)
{
	assert( maxValue > 0 );

	if (maxValue != itsMaxValue)
		{
		itsMaxValue = maxValue;
		if (itsValue > itsMaxValue)
			{
			itsValue = itsMaxValue;
			}
		Redraw();
		}
}

/******************************************************************************
 DrawBorder (virtual protected)

 ******************************************************************************/

void
JXProgressIndicator::DrawBorder
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
JXProgressIndicator::Draw
	(	
	JXWindowPainter&	p,
	const JRect&		r
	)
{
	const JRect bounds = GetBounds();	
	JRect fillRect;

	const JCoordinate w = bounds.width();
	const JCoordinate h = bounds.height();
	const JFloat f      = itsValue / (JFloat)itsMaxValue;
	if (w > h)
		{
		fillRect.top    = 0;
		fillRect.left   = 0;
		fillRect.bottom = h;
		fillRect.right  = JRound(w * f);
		}
	else
		{
		fillRect.top    = JRound(h * (1.0 - f));
		fillRect.left   = 0;
		fillRect.bottom = h;
		fillRect.right  = w;
		}

	p.SetFilling(kJTrue);
	p.SetPenColor(itsForeColor);
	p.JPainter::Rect(fillRect);
}
