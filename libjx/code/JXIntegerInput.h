/******************************************************************************
 JXIntegerInput.h

	Interface for the JXIntegerInput class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXIntegerInput
#define _H_JXIntegerInput

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXInputField.h>

class JString;

class JXIntegerInput : public JXInputField
{
public:

	JXIntegerInput(JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);

	virtual ~JXIntegerInput();

	JBoolean	GetValue(JInteger* value) const;
	void		SetValue(const JInteger value);
	JBoolean	ValueValid(const JInteger value) const;

	void	SetLimits(const JInteger minValue, const JInteger maxValue);

	JInteger	GetLowerLimit() const;
	void		SetLowerLimit(const JInteger minValue);
	void		ClearLowerLimit();

	JInteger	GetUpperLimit() const;
	void		SetUpperLimit(const JInteger maxValue);
	void		ClearUpperLimit();

	virtual JBoolean	InputValid();

private:

	JBoolean	itsHasLowerLimitFlag;
	JInteger	itsLowerLimit;

	JBoolean	itsHasUpperLimitFlag;
	JInteger	itsUpperLimit;

private:

	// not allowed

	JXIntegerInput(const JXIntegerInput& source);
	const JXIntegerInput& operator=(const JXIntegerInput& source);
};

/******************************************************************************
 Limits

 ******************************************************************************/

inline JInteger
JXIntegerInput::GetLowerLimit()
	const
{
	return itsLowerLimit;
}

inline void
JXIntegerInput::ClearLowerLimit()
{
	itsHasLowerLimitFlag = kJFalse;
}

inline JInteger
JXIntegerInput::GetUpperLimit()
	const
{
	return itsUpperLimit;
}

inline void
JXIntegerInput::ClearUpperLimit()
{
	itsHasUpperLimitFlag = kJFalse;
}

/******************************************************************************
 ValueValid

 ******************************************************************************/

inline JBoolean
JXIntegerInput::ValueValid
	(
	const JInteger value
	)
	const
{
	return JNegate((itsHasLowerLimitFlag && value < itsLowerLimit) ||
				   (itsHasUpperLimitFlag && value > itsUpperLimit));
}

#endif
