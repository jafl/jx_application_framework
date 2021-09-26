/******************************************************************************
 JXIntegerInput.h

	Interface for the JXIntegerInput class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXIntegerInput
#define _H_JXIntegerInput

#include "jx-af/jx/JXInputField.h"

class JString;

class JXIntegerInput : public JXInputField
{
public:

	JXIntegerInput(JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);

	virtual ~JXIntegerInput();

	bool	GetValue(JInteger* value) const;
	void		SetValue(const JInteger value);
	bool	ValueValid(const JInteger value) const;

	void	SetLimits(const JInteger minValue, const JInteger maxValue);

	JInteger	GetLowerLimit() const;
	void		SetLowerLimit(const JInteger minValue);
	void		ClearLowerLimit();

	JInteger	GetUpperLimit() const;
	void		SetUpperLimit(const JInteger maxValue);
	void		ClearUpperLimit();

	virtual bool	InputValid();

private:

	bool		itsHasLowerLimitFlag;
	JInteger	itsLowerLimit;

	bool		itsHasUpperLimitFlag;
	JInteger	itsUpperLimit;
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
	itsHasLowerLimitFlag = false;
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
	itsHasUpperLimitFlag = false;
}

/******************************************************************************
 ValueValid

 ******************************************************************************/

inline bool
JXIntegerInput::ValueValid
	(
	const JInteger value
	)
	const
{
	return !((itsHasLowerLimitFlag && value < itsLowerLimit) ||
			 (itsHasUpperLimitFlag && value > itsUpperLimit));
}

#endif
