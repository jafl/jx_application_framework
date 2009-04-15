/******************************************************************************
 JXIntegerInput.cpp

	Maintains a line of editable text for entering an integer value.

	BASE CLASS = JXInputField

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXIntegerInput.h>
#include <jXGlobals.h>
#include <jAssert.h>

// string ID's

static const JCharacter* kNotANumberID   = "NotANumber::JXIntegerInput";
static const JCharacter* kOutsideRangeID = "OutsideRange::JXIntegerInput";
static const JCharacter* kBelowMinID     = "BelowMin::JXIntegerInput";
static const JCharacter* kAboveMaxID     = "AboveMax::JXIntegerInput";

/******************************************************************************
 Constructor

 ******************************************************************************/

JXIntegerInput::JXIntegerInput
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
	SetValue(0);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXIntegerInput::~JXIntegerInput()
{
}

/******************************************************************************
 GetValue

 ******************************************************************************/

JBoolean
JXIntegerInput::GetValue
	(
	JInteger* value
	)
	const
{
	return JConvertToBoolean(
			(GetText()).ConvertToInteger(value) && ValueValid(*value) );
}

/******************************************************************************
 SetValue

 ******************************************************************************/

void
JXIntegerInput::SetValue
	(
	const JInteger value
	)
{
	if (ValueValid(value))
		{
		const JString text(value, 0);
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
JXIntegerInput::SetLimits
	(
	const JInteger minValue,
	const JInteger maxValue
	)
{
	assert( minValue < maxValue );
	SetLowerLimit(minValue);
	SetUpperLimit(maxValue);
}

void
JXIntegerInput::SetLowerLimit
	(
	const JInteger minValue
	)
{
	assert( !itsHasUpperLimitFlag || minValue < itsUpperLimit );

	itsHasLowerLimitFlag = kJTrue;
	itsLowerLimit = minValue;

	JInteger v;
	if (!GetValue(&v))
		{
		SetValue(itsLowerLimit);
		}
}

void
JXIntegerInput::SetUpperLimit
	(
	const JInteger maxValue
	)
{
	assert( !itsHasLowerLimitFlag || itsLowerLimit < maxValue );

	itsHasUpperLimitFlag = kJTrue;
	itsUpperLimit = maxValue;

	JInteger v;
	if (!GetValue(&v))
		{
		SetValue(itsUpperLimit);
		}
}

/******************************************************************************
 InputValid (virtual)

 ******************************************************************************/

JBoolean
JXIntegerInput::InputValid()
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

		JInteger value;
		if (!text.ConvertToInteger(&value))
			{
			(JGetUserNotification())->ReportError(JGetString(kNotANumberID));
			return kJFalse;
			}

		const JBoolean valid = ValueValid(value);
		JString errorStr;
		if (!valid && itsHasLowerLimitFlag && itsHasUpperLimitFlag)
			{
			const JString n(itsLowerLimit, 0), m(itsUpperLimit, 0);
			const JCharacter* map[] =
				{
				"min", n.GetCString(),
				"max", m.GetCString()
				};
			errorStr = JGetString(kOutsideRangeID, map, sizeof(map));
			}
		else if (!valid && itsHasLowerLimitFlag)
			{
			const JString n(itsLowerLimit, 0);
			const JCharacter* map[] =
				{
				"min", n.GetCString()
				};
			errorStr = JGetString(kBelowMinID, map, sizeof(map));
			}
		else if (!valid)
			{
			assert( itsHasUpperLimitFlag );

			const JString n(itsUpperLimit, 0);
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
