/******************************************************************************
 JSliderBase.h

	Interface for the JSliderBase class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JSliderBase
#define _H_JSliderBase

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JBroadcaster.h>
#include <JRect.h>

class JSliderBase : virtual public JBroadcaster
{
public:

	JSliderBase(const JCoordinate thumbHalfSize,
				const JCoordinate w, const JCoordinate h);

	virtual ~JSliderBase();

	JFloat	GetValue() const;
	JFloat	GetMinValue() const;
	JFloat	GetMaxValue() const;
	JFloat	GetStepSize() const;

	void	SetValue(const JFloat value);
	void	SetMinValue(const JFloat minValue);
	void	SetMaxValue(const JFloat maxValue);
	void	SetRange(const JFloat minValue, const JFloat maxValue);
	void	SetStepSize(const JFloat step);

protected:

	enum SliderOrientation
	{
		kVertical,
		kHorizontal
	};

protected:

	SliderOrientation	GetOrientation() const;
	JCoordinate			GetThumbPosition() const;

	JFloat		ClickToValue(const JPoint& pt) const;
	JCoordinate	ValueToPixel(const JFloat value) const;

	virtual void		SliderRedraw() const = 0;
	virtual JCoordinate	SliderGetWidth() const = 0;
	virtual JCoordinate	SliderGetHeight() const = 0;

private:

	JFloat	itsValue;					 
	JFloat	itsMinValue;
	JFloat	itsMaxValue;
	JFloat	itsStepSize;	

	const SliderOrientation	itsOrientation;
	const JCoordinate		itsThumbHalfSize;

	// buffered value so we know when to redraw

	JCoordinate	itsPrevThumbPosition;

private:

	JFloat	ClickToValueHoriz(const JPoint& pt) const;
	JFloat	ClickToValueVert(const JPoint& pt) const;

	JCoordinate	ValueToPixelHoriz(const JFloat value) const;
	JCoordinate	ValueToPixelVert(const JFloat value) const;

	JFloat	Quantize(const JFloat value) const;

	// not allowed

	JSliderBase(const JSliderBase& source);
	const JSliderBase& operator=(const JSliderBase& source);

public:

	// JBroadcaster messages

	static const JCharacter* kMoved;

	class Moved : public JBroadcaster::Message
		{
		public:

			Moved(const JFloat value)
				:
				JBroadcaster::Message(kMoved),
				itsValue(value)
				{ };

			JFloat
			GetValue() const
			{
				return itsValue;
			};

		private:

			JFloat	itsValue;
		};
};


/******************************************************************************
 GetValue

 ******************************************************************************/

inline JFloat
JSliderBase::GetValue()
	const
{
	return itsValue;
}

/******************************************************************************
 GetMinValue

 ******************************************************************************/

inline JFloat
JSliderBase::GetMinValue()
	const
{
	return itsMinValue;
}

/******************************************************************************
 GetMaxValue

 ******************************************************************************/

inline JFloat
JSliderBase::GetMaxValue()
	const
{
	return itsMaxValue;
}

/******************************************************************************
 GetStepSize

 ******************************************************************************/

inline JFloat
JSliderBase::GetStepSize()
	const
{
	return itsStepSize;
}

/******************************************************************************
 GetOrientation (protected)

 ******************************************************************************/

inline JSliderBase::SliderOrientation 
JSliderBase::GetOrientation()
	const
{
	return itsOrientation;
}

/******************************************************************************
 GetThumbPosition (protected)

 ******************************************************************************/

inline JCoordinate
JSliderBase::GetThumbPosition()
	const
{
	return ValueToPixel(itsValue);
}

#endif
