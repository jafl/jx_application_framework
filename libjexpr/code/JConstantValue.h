/******************************************************************************
 JConstantValue.h

	Interface for JConstantValue class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JConstantValue
#define _H_JConstantValue

#include <JFunction.h>

class JConstantValue : public JFunction
{
public:

	JConstantValue(const JFloat value);
	JConstantValue(const JConstantValue& source);

	virtual ~JConstantValue();

	virtual bool	Evaluate(JFloat* result) const override;
	virtual bool	Evaluate(JComplex* result) const override;
	virtual void		Print(std::ostream& output) const override;
	virtual JFunction*	Copy() const override;

	JFloat	GetValue() const;
	void	SetValue(const JFloat value);

private:

	JFloat	itsValue;

private:

	// not allowed

	const JConstantValue& operator=(const JConstantValue& source);
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
