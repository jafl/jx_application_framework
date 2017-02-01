/******************************************************************************
 JXIntegerInput.cpp

	Maintains a line of editable text for entering an integer value.

	BASE CLASS = JXInputField

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXIntegerInput.h>
#include <jXGlobals.h>
#include <jAssert.h>

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
			(JGetUserNotification())->ReportError(JGetString("NotANumber::JXIntegerInput"));
			return kJFalse;
			}

		const JBoolean valid = ValueValid(value);
		JString errorStr;
		if (!valid && itsHasLowerLimitFlag && itsHasUpperLimitFlag)
			{
			const JString n(itsLowerLimit, 0), m(itsUpperLimit, 0);
			const JUtf8Byte* map[] =
				{
				"min", n.GetBytes(),
				"max", m.GetBytes()
				};
			errorStr = JGetString("OutsideRange::JXIntegerInput", map, sizeof(map));
			}
		else if (!valid && itsHasLowerLimitFlag)
			{
			const JString n(itsLowerLimit, 0);
			const JUtf8Byte* map[] =
				{
				"min", n.GetBytes()
				};
			errorStr = JGetString("BelowMin::JXIntegerInput", map, sizeof(map));
			}
		else if (!valid)
			{
			assert( itsHasUpperLimitFlag );

			const JString n(itsUpperLimit, 0);
			const JUtf8Byte* map[] =
				{
				"max", n.GetBytes()
				};
			errorStr = JGetString("AboveMax::JXIntegerInput", map, sizeof(map));
			}

		if (!valid)
			{
			(JGetUserNotification())->ReportError(errorStr);
			}
		return valid;
		}
}
