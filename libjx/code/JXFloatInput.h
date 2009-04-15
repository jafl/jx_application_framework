/******************************************************************************
 JXFloatInput.h

	Interface for the JXFloatInput class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXFloatInput
#define _H_JXFloatInput

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXInputField.h>

class JXFloatInput : public JXInputField
{
public:

	JXFloatInput(JXContainer* enclosure,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);

	virtual ~JXFloatInput();

	JBoolean	GetValue(JFloat* value) const;
	void		SetValue(const JFloat value);
	JBoolean	ValueValid(const JFloat value) const;

	void	SetLimits(const JFloat minValue, const JFloat maxValue);

	JFloat	GetLowerLimit() const;
	void	SetLowerLimit(const JFloat minValue);
	void	ClearLowerLimit();

	JFloat	GetUpperLimit() const;
	void	SetUpperLimit(const JFloat maxValue);
	void	ClearUpperLimit();

	virtual JBoolean	InputValid();

private:

	JBoolean	itsHasLowerLimitFlag;
	JFloat		itsLowerLimit;

	JBoolean	itsHasUpperLimitFlag;
	JFloat		itsUpperLimit;

private:

	// not allowed

	JXFloatInput(const JXFloatInput& source);
	const JXFloatInput& operator=(const JXFloatInput& source);
};

/******************************************************************************
 Limits

 ******************************************************************************/

inline JFloat
JXFloatInput::GetLowerLimit()
	const
{
	return itsLowerLimit;
}

inline void
JXFloatInput::ClearLowerLimit()
{
	itsHasLowerLimitFlag = kJFalse;
}

inline JFloat
JXFloatInput::GetUpperLimit()
	const
{
	return itsUpperLimit;
}

inline void
JXFloatInput::ClearUpperLimit()
{
	itsHasUpperLimitFlag = kJFalse;
}

/******************************************************************************
 ValueValid

 ******************************************************************************/

inline JBoolean
JXFloatInput::ValueValid
	(
	const JFloat value
	)
	const
{
	return JNegate((itsHasLowerLimitFlag && value < itsLowerLimit) ||
				   (itsHasUpperLimitFlag && value > itsUpperLimit));
}

#endif
