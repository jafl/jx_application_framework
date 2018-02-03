/******************************************************************************
 JXFloatInput.cpp

	Maintains a line of editable text for entering a floating point value.

	BASE CLASS = JXInputField

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include <JXFloatInput.h>
#include <jXGlobals.h>
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
			(JGetUserNotification())->ReportError(JGetString("NotANumber::JXFloatInput"));
			return kJFalse;
			}

		const JBoolean valid = ValueValid(value);
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
			(JGetUserNotification())->ReportError(errorStr);
			}
		return valid;
		}
}
