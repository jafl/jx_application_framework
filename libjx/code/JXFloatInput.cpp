/******************************************************************************
 JXFloatInput.cpp

	Maintains a line of editable text for entering a floating point value.

	BASE CLASS = JXInputField

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JXFloatInput.h"
#include "jXGlobals.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXFloatInput::JXFloatInput
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
	JXInputField(enclosure, hSizing, vSizing, x,y, w,h)
{
	SetIsRequired();
	itsHasLowerLimitFlag = itsHasUpperLimitFlag = false;
	SetValue(0.0);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXFloatInput::~JXFloatInput()
{
}

/******************************************************************************
 GetValue

 ******************************************************************************/

bool
JXFloatInput::GetValue
	(
	JFloat* value
	)
	const
{
	return GetText().GetText().ConvertToFloat(value) && ValueValid(*value);
}

/******************************************************************************
 SetValue

 ******************************************************************************/

void
JXFloatInput::SetValue
	(
	const JFloat value
	)
{
	if (ValueValid(value))
	{
		const JString text(value);
		if (text != GetText()->GetText())
		{
			GetText()->SetText(text);
		}
	}
}

/******************************************************************************
 Set limits

 ******************************************************************************/

void
JXFloatInput::SetLimits
	(
	const JFloat minValue,
	const JFloat maxValue
	)
{
	assert( minValue < maxValue );
	SetLowerLimit(minValue);
	SetUpperLimit(maxValue);
}

void
JXFloatInput::SetLowerLimit
	(
	const JFloat minValue
	)
{
	assert( !itsHasUpperLimitFlag || minValue < itsUpperLimit );

	itsHasLowerLimitFlag = true;
	itsLowerLimit = minValue;

	JFloat x;
	if (!GetValue(&x))
	{
		SetValue(itsLowerLimit);
	}
}

void
JXFloatInput::SetUpperLimit
	(
	const JFloat maxValue
	)
{
	assert( !itsHasLowerLimitFlag || itsLowerLimit < maxValue );

	itsHasUpperLimitFlag = true;
	itsUpperLimit = maxValue;

	JFloat x;
	if (!GetValue(&x))
	{
		SetValue(itsUpperLimit);
	}
}

/******************************************************************************
 InputValid (virtual)

 ******************************************************************************/

bool
JXFloatInput::InputValid()
{
	if (!JXInputField::InputValid())
	{
		return false;
	}
	else
	{
		const JString& text = GetText()->GetText();

		if (!IsRequired() && text.IsEmpty())
		{
			return true;
		}

		JFloat value;
		if (!text.ConvertToFloat(&value))
		{
			JGetUserNotification()->ReportError(JGetString("NotANumber::JXFloatInput"));
			return false;
		}

		const bool valid = ValueValid(value);
		JString errorStr;
		if (!valid && itsHasLowerLimitFlag && itsHasUpperLimitFlag)
		{
			const JString n(itsLowerLimit), m(itsUpperLimit);
			const JUtf8Byte* map[] =
			{
				"min", n.GetBytes(),
				"max", m.GetBytes()
			};
			errorStr = JGetString("OutsideRange::JXFloatInput", map, sizeof(map));
		}
		else if (!valid && itsHasLowerLimitFlag)
		{
			const JString n(itsLowerLimit);
			const JUtf8Byte* map[] =
			{
				"min", n.GetBytes()
			};
			errorStr = JGetString("BelowMin::JXFloatInput", map, sizeof(map));
		}
		else if (!valid)
		{
			assert( itsHasUpperLimitFlag );

			const JString n(itsUpperLimit);
			const JUtf8Byte* map[] =
			{
				"max", n.GetBytes()
			};
			errorStr = JGetString("AboveMax::JXFloatInput", map, sizeof(map));
		}

		if (!valid)
		{
			JGetUserNotification()->ReportError(errorStr);
		}
		return valid;
	}
}
