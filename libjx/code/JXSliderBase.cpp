/******************************************************************************
 JXSliderBase.cpp

	Instantiates JSliderBase for JX library.

	BASE CLASS = JXWidget, JSliderBase

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXSliderBase.h>
#include <JXWindowPainter.h>
#include <jXPainterUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXSliderBase::JXSliderBase
	(
	const JCoordinate	thumbHalfSize,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXWidget(enclosure, hSizing, vSizing, x,y, w,h),
	JSliderBase(thumbHalfSize, w,h)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXSliderBase::~JXSliderBase()
{
}

/******************************************************************************
 DrawBorder (virtual protected)

 ******************************************************************************/

void
JXSliderBase::DrawBorder
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
JXSliderBase::HandleMouseDown	
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (button == kJXLeftButton)
		{
		SetValue(ClickToValue(pt));
		}
	else if (button == kJXButton4)
		{
		SetValue(GetValue() + GetStepSize());
		}
	else if (button == kJXButton5)
		{
		SetValue(GetValue() - GetStepSize());
		}
}

/******************************************************************************
 HandleMouseDrag (virtual protected)

 ******************************************************************************/

void
JXSliderBase::HandleMouseDrag
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (buttonStates.left())
		{
		SetValue(ClickToValue(pt));
		}
}

/******************************************************************************
 SliderRedraw (virtual protected)

 ******************************************************************************/

void
JXSliderBase::SliderRedraw()
	const
{
	Redraw();
}

/******************************************************************************
 SliderGetWidth (virtual protected)

 ******************************************************************************/

JCoordinate
JXSliderBase::SliderGetWidth()
	const
{
	return GetBoundsWidth();
}

/******************************************************************************
 SliderGetHeight (virtual protected)

 ******************************************************************************/

JCoordinate
JXSliderBase::SliderGetHeight()
	const
{
	return GetBoundsHeight();
}

/******************************************************************************
 Receive (virtual protected)

	Because of multiple inheritance, we have to route the messages manually.

 ******************************************************************************/

void
JXSliderBase::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	JXWidget::Receive(sender, message);
	JSliderBase::Receive(sender, message);
}
