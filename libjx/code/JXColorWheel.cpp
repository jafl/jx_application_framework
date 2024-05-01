/******************************************************************************
 JXColorWheel.cpp

	Draws a color wheel + slider for selecting a color.

	Hue = angle, Saturation = radius, Brightness

	BASE CLASS = JXDecorRect

	Copyright (C) 2010 by John Lindal.

 ******************************************************************************/

#include "JXColorWheel.h"
#include "JXWindow.h"
#include "JXSlider.h"
#include "JXImage.h"
#include "JXWindowPainter.h"
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/jAssert.h>

const JCoordinate kSliderWidth  = 20;
const JCoordinate kSliderMargin = 5;
const JCoordinate kWheelMargin  = 2;

// JBroadcaster message types

const JUtf8Byte* JXColorWheel::kColorChanged = "ColorChanged::JXColorWheel";

/******************************************************************************
 Constructor

 ******************************************************************************/

JXColorWheel::JXColorWheel
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
	JXWidget(enclosure, hSizing, vSizing, x,y, w,h),
	itsImage(nullptr)
{
	itsBrightnessSlider =
		jnew JXSlider(this, kFixedRight, kVElastic, w-kSliderWidth,0, kSliderWidth,h);
	itsBrightnessSlider->SetMinValue(0);
	itsBrightnessSlider->SetMaxValue(kJMaxHSBValue);
	ListenTo(itsBrightnessSlider);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXColorWheel::~JXColorWheel()
{
}

/******************************************************************************
 SetColor

 ******************************************************************************/

void
JXColorWheel::SetColor
	(
	const JHSB& hsb
	)
{
	if (itsColor != hsb)
	{
		itsColor = hsb;

		StopListening(itsBrightnessSlider);
		itsBrightnessSlider->SetValue(itsColor.brightness);
		ListenTo(itsBrightnessSlider);

		Broadcast(ColorChanged());
		Refresh();
	}
}

/******************************************************************************
 DrawBorder (virtual protected)

 ******************************************************************************/

void
JXColorWheel::DrawBorder
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
JXColorWheel::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	const JColorID black = JColorManager::GetBlackColor();

	const JRect bounds       = GetBoundsGlobal();
	const JCoordinate max    = JMin(bounds.height(), bounds.width() - kSliderWidth - kSliderMargin);
	const JCoordinate size   = max - 2*kWheelMargin - 1;
	const JCoordinate center = size/2 + kWheelMargin;
	if (itsImage == nullptr || itsImage->GetWidth() != max || itsColor.brightness != itsLastDrawBrightness)
	{
		p.SetFilling(true);
		p.SetPenColor(black);
		p.Ellipse(kWheelMargin, kWheelMargin, size, size);
		p.SetFilling(false);

		JRect r  = bounds;
		r.bottom = r.top  + max;
		r.right  = r.left + max;

		jdelete itsImage;
		itsImage = jnew JXImage(GetDisplay(), p.GetDrawable(), r);

		itsLastDrawBrightness = itsColor.brightness;
		for (JCoordinate x=0; x<max; x++)
		{
			const JCoordinate dx = - x + center;

			for (JCoordinate y=0; y<max; y++)
			{
				if (itsImage->GetColor(x,y) == black)
				{
					const JCoordinate dy = y - center;
					const JFloat r = sqrt(dx*dx + dy*dy) / center;
					const JFloat a = 0.5 + atan2(dy, dx) / (2.0 * std::numbers::pi);

					JHSB color(JRound(a * kJMaxHSBValue), JRound(r * kJMaxHSBValue), itsLastDrawBrightness);
					itsImage->SetColor(x,y, JColorManager::GetColorID(color));
				}
			}
		}

		itsImage->ConvertToRemoteStorage();
	}

	p.Image(*itsImage, itsImage->GetBounds(), 0,0);

	const JFloat r = (itsColor.saturation / kJMaxHSBValueF) * size/2;
	const JFloat a = ((itsColor.hue / kJMaxHSBValueF) - 0.5) * 2.0 * std::numbers::pi;

	const JCoordinate x = center - JRound(r * cos(a));
	const JCoordinate y = center + JRound(r * sin(a));

	JRect mark(y-kWheelMargin, x-kWheelMargin, y+kWheelMargin+1, x+kWheelMargin+1);

	p.SetPenColor(JColorManager::GetWhiteColor());
	p.SetFilling(true);
	p.Rect(mark);
	p.SetFilling(false);
	p.SetPenColor(black);
	p.Rect(mark);
}

/******************************************************************************
 SetColor (private)

 ******************************************************************************/

void
JXColorWheel::SetColor
	(
	const JPoint& pt
	)
{
	if (itsImage == nullptr)
	{
		Redraw();
	}

	const JRect bounds       = GetBoundsGlobal();
	const JCoordinate max    = JMin(bounds.height(), bounds.width() - kSliderWidth - kSliderMargin);
	const JCoordinate size   = max - 2*kWheelMargin - 1;
	const JCoordinate center = size/2 + kWheelMargin;

	const JCoordinate dx = - pt.x + center;
	const JCoordinate dy = pt.y - center;

	const JFloat r = JMin(sqrt(dx*dx + dy*dy) / center, 1.0);
	const JFloat a = 0.5 + atan2(dy, dx) / (2.0 * std::numbers::pi);

	const JSize b = JRound(itsBrightnessSlider->GetValue());
	SetColor(JHSB(JRound(a * kJMaxHSBValue), JRound(r * kJMaxHSBValue), b));
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
JXColorWheel::HandleMouseDown	
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	SetColor(pt);
}

/******************************************************************************
 HandleMouseDrag (virtual protected)

 ******************************************************************************/

void
JXColorWheel::HandleMouseDrag
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	SetColor(pt);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JXColorWheel::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsBrightnessSlider && message.Is(JSliderBase::kMoved))
	{
		itsColor.brightness = JRound(itsBrightnessSlider->GetValue());
		Broadcast(ColorChanged());
		Refresh();
	}
	else
	{
		JXWidget::Receive(sender, message);
	}
}
