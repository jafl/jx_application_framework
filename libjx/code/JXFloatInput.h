/******************************************************************************
 JXFloatInput.h

	Interface for the JXFloatInput class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXFloatInput
#define _H_JXFloatInput

#include "JXInputField.h"

class JXFloatInput : public JXInputField
{
public:

	JXFloatInput(JXContainer* enclosure,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);

	virtual ~JXFloatInput();

	bool	GetValue(JFloat* value) const;
	void	SetValue(const JFloat value);
	bool	ValueValid(const JFloat value) const;

	void	SetLimits(const JFloat minValue, const JFloat maxValue);

	JFloat	GetLowerLimit() const;
	void	SetLowerLimit(const JFloat minValue);
	void	ClearLowerLimit();

	JFloat	GetUpperLimit() const;
	void	SetUpperLimit(const JFloat maxValue);
	void	ClearUpperLimit();

	virtual bool	InputValid();

private:

	bool	itsHasLowerLimitFlag;
	JFloat	itsLowerLimit;

	bool	itsHasUpperLimitFlag;
	JFloat	itsUpperLimit;
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
	itsHasLowerLimitFlag = false;
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
	itsHasUpperLimitFlag = false;
}

/******************************************************************************
 ValueValid

 ******************************************************************************/

inline bool
JXFloatInput::ValueValid
	(
	const JFloat value
	)
	const
{
	return !((itsHasLowerLimitFlag && value < itsLowerLimit) ||
			 (itsHasUpperLimitFlag && value > itsUpperLimit));
}

#endif
