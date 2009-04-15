/******************************************************************************
 JXFloatInput.cpp

	Maintains a line of editable text for entering a floating point value.

	BASE CLASS = JXInputField

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXFloatInput.h>
#include <jXGlobals.h>
#include <jAssert.h>

// string ID's

static const JCharacter* kNotANumberID   = "NotANumber::JXFloatInput";
static const JCharacter* kOutsideRangeID = "OutsideRange::JXFloatInput";
static const JCharacter* kBelowMinID     = "BelowMin::JXFloatInput";
static const JCharacter* kAboveMaxID     = "AboveMax::JXFloatInput";

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
	itsHasLowerLimitFlag = itsHasUpperLimitFlag = kJFalse;
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

JBoolean
JXFloatInput::GetValue
	(
	JFloat* value
	)
	const
{
	return JI2B((GetText()).ConvertToFloat(value) && ValueValid(*value));
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
		const JString text = value;
		if (text != GetText())
			{
			SetText(text);
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

	itsHasLowerLimitFlag = kJTrue;
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

	itsHasUpperLimitFlag = kJTrue;
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

JBoolean
JXFloatInput::InputValid()
{
	if (!JXInputField::InputValid())
		{
		return kJFalse;
		}
	else
		{
		const JString& text = GetText();

		if (!IsRequired() && text.IsEmpty())
			{
			return kJTrue;
			}

		JFloat value;
		if (!text.ConvertToFloat(&value))
			{
			(JGetUserNotification())->ReportError(JGetString(kNotANumberID));
			return kJFalse;
			}

		const JBoolean valid = ValueValid(value);
		JString errorStr;
		if (!valid && itsHasLowerLimitFlag && itsHasUpperLimitFlag)
			{
			const JString n(itsLowerLimit), m(itsUpperLimit);
			const JCharacter* map[] =
				{
				"min", n.GetCString(),
				"max", m.GetCString()
				};
			errorStr = JGetString(kOutsideRangeID, map, sizeof(map));
			}
		else if (!valid && itsHasLowerLimitFlag)
			{
			const JString n(itsLowerLimit);
			const JCharacter* map[] =
				{
				"min", n.GetCString()
				};
			errorStr = JGetString(kBelowMinID, map, sizeof(map));
			}
		else if (!valid)
			{
			assert( itsHasUpperLimitFlag );

			const JString n(itsUpperLimit);
			const JCharacter* map[] =
				{
				"max", n.GetCString()
				};
			errorStr = JGetString(kAboveMaxID, map, sizeof(map));
			}

		if (!valid)
			{
			(JGetUserNotification())->ReportError(errorStr);
			}
		return valid;
		}
}
