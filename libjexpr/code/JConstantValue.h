/******************************************************************************
 JConstantValue.h

	Interface for JConstantValue class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JConstantValue
#define _H_JConstantValue

#include "jx-af/jexpr/JFunction.h"

class JConstantValue : public JFunction
{
public:

	JConstantValue(const JFloat value);
	JConstantValue(const JConstantValue& source);

	~JConstantValue();

	bool		Evaluate(JFloat* result) const override;
	bool		Evaluate(JComplex* result) const override;
	void		Print(std::ostream& output) const override;
	JFunction*	Copy() const override;

	JFloat	GetValue() const;
	void	SetValue(const JFloat value);

private:

	JFloat	itsValue;
};

/******************************************************************************
 GetValue

 ******************************************************************************/

inline JFloat
JConstantValue::GetValue()
	const
{
	return itsValue;
}

/******************************************************************************
 SetValue

 ******************************************************************************/

inline void
JConstantValue::SetValue
	(
	const JFloat value
	)
{
	itsValue = value;
}

#endif
