/******************************************************************************
 JSliderBase.cpp

	Abstract base class for all types of sliders.  A slider is an object that
	lets the user graphically set a value within some range.  Each slider is
	either horizontal or vertical and can have a thumb for the user to
	grab.

	BASE CLASS = virtual JBroadcaster

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JSliderBase.h>
#include <jAssert.h>

// JBroadcaster message types

const JCharacter* JSliderBase::kMoved = "Moved::JSliderBase";

/******************************************************************************
 Constructor

 ******************************************************************************/

JSliderBase::JSliderBase
	(
	const JCoordinate thumbHalfSize,
	const JCoordinate w,
	const JCoordinate h
	)
	:
	itsOrientation( w>h ? kHorizontal : kVertical ),
	itsThumbHalfSize( thumbHalfSize )
{
	itsValue    = 1;
	itsMinValue = 1;
	itsMaxValue = 100;
	itsStepSize = 1;

	itsPrevThumbPosition = -1;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JSliderBase::~JSliderBase()
{
}

/******************************************************************************
 Quantize (private)

 ******************************************************************************/

inline JFloat
JSliderBase::Quantize
	(
	const JFloat value
	)
	const
{
	return itsMinValue +
		   itsStepSize * JLFloor((value - itsMinValue) / itsStepSize);
}

/******************************************************************************
 SetValue

 ******************************************************************************/

void
JSliderBase::SetValue
	(
	const JFloat value
	)
{
	const JFloat origValue = itsValue;

	JFloat trueValue = value;
	if (trueValue < itsMinValue) 
		{
		trueValue = itsMinValue;
		}
	else if (trueValue > itsMaxValue) 
		{
		trueValue = itsMaxValue;
		}
	trueValue = Quantize(trueValue);

	itsValue = trueValue;
	const JCoordinate newThumbPosition = GetThumbPosition();
	if (newThumbPosition != itsPrevThumbPosition)
		{
		itsPrevThumbPosition = newThumbPosition;
		SliderRedraw();		// this first because Broadcast() could take a while
		}

	if (itsValue != origValue)
		{
		Broadcast(Moved(itsValue));
		}
}

/******************************************************************************
 SetMinValue

 ******************************************************************************/

void
JSliderBase::SetMinValue
	(
	const JFloat minValue
	)
{
	if (minValue != itsMinValue && minValue <= itsMaxValue)
		{
		itsMinValue = minValue;
		SetValue(itsValue);
		}
}

/******************************************************************************
 SetMaxValue

 ******************************************************************************/

void
JSliderBase::SetMaxValue
	(
	const JFloat maxValue
	)
{
	if (maxValue != itsMaxValue && maxValue >= itsMinValue)
		{
		itsMaxValue = maxValue;
		SetValue(itsValue);
		}
}

/******************************************************************************
 SetRange

 ******************************************************************************/

void
JSliderBase::SetRange
	(
	const JFloat minValue,
	const JFloat maxValue
	)
{
	if (minValue <= maxValue)
		{
		itsMinValue = minValue;
		itsMaxValue = maxValue;
		}
	else
		{
		itsMinValue = maxValue;
		itsMaxValue = minValue;
		}

	SetValue(itsValue);
}

/******************************************************************************
 SetStepSize

 ******************************************************************************/

void
JSliderBase::SetStepSize
	(
	const JFloat step
	)
{
	assert( step > 0 );
	itsStepSize = step;
	SetValue(itsValue);
}

/******************************************************************************
 ClickToValue (protected)

	Convert a pixel value to a slider value.

 ******************************************************************************/

JFloat
JSliderBase::ClickToValue
	(
	const JPoint& pt
	)
	const
{
	JFloat value;
	if (itsOrientation == kHorizontal)
		{
		value = ClickToValueHoriz(pt);
		}
	else
		{
		assert( itsOrientation == kVertical );
		value = ClickToValueVert(pt);
		}

	return Quantize(value);
}

// private

JFloat
JSliderBase::ClickToValueHoriz
	(
	const JPoint& pt
	)
	const
{
	const JFloat totalWidth = SliderGetWidth() - 2*itsThumbHalfSize;
	if (totalWidth > 0)
		{
		JFloat dx = totalWidth / ((itsMaxValue - itsMinValue)/itsStepSize);
		JFloat x  = pt.x - itsThumbHalfSize + dx/2;
		if (x > totalWidth)
			{
			x = totalWidth;
			}
		return itsMinValue + (itsMaxValue - itsMinValue) * (x / totalWidth);
		}
	else
		{
		return itsMinValue;
		}
}

// private

JFloat
JSliderBase::ClickToValueVert
	(
	const JPoint& pt
	)
	const
{
	const JFloat boundsHeight = SliderGetHeight();
	const JFloat totalHeight  = boundsHeight - 2*itsThumbHalfSize;
	if (totalHeight > 0)
		{
		JFloat dy = totalHeight / ((itsMaxValue - itsMinValue)/itsStepSize);
		JFloat y  = boundsHeight - pt.y - itsThumbHalfSize + dy/2;
		if (y > totalHeight)
			{
			y = totalHeight;
			}
		return itsMinValue + (itsMaxValue - itsMinValue) * (y / totalHeight);
		}
	else
		{
		return itsMinValue;
		}
}

/******************************************************************************
 ValueToPixel (protected)

	Convert a slider value to a pixel value in local coordinates.

 ******************************************************************************/

JCoordinate
JSliderBase::ValueToPixel
	(
	const JFloat value
	)
	const
{
	assert( itsMinValue <= value && value <= itsMaxValue );

	if (itsOrientation == kHorizontal)
		{
		return ValueToPixelHoriz(value);
		}
	else
		{
		assert( itsOrientation == kVertical );
		return ValueToPixelVert(value);
		}
}

// private

JCoordinate
JSliderBase::ValueToPixelHoriz
	(
	const JFloat value
	)
	const
{
	const JCoordinate totalWidth = SliderGetWidth() - 2*itsThumbHalfSize;
	if (itsMaxValue > itsMinValue)
		{
		return itsThumbHalfSize +
			JRound(totalWidth * (value - itsMinValue)/(itsMaxValue - itsMinValue));
		}
	else
		{
		return itsThumbHalfSize;
		}
}

// private

JCoordinate
JSliderBase::ValueToPixelVert
	(
	const JFloat value
	)
	const
{
	const JCoordinate boundsHeight = SliderGetHeight();
	const JCoordinate totalHeight  = boundsHeight - 2*itsThumbHalfSize;

	JCoordinate y;
	if (itsMaxValue > itsMinValue)
		{
		y = itsThumbHalfSize +
			JRound(totalHeight * (value - itsMinValue)/(itsMaxValue - itsMinValue));
		}
	else
		{
		y = itsThumbHalfSize;
		}

	return boundsHeight - y;
}
