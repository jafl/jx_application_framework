/******************************************************************************
 JConstantValue.h

	Interface for JConstantValue class.

	Copyright © 1995 by John Lindal. All rights reserved.

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

	virtual JBoolean	Evaluate(JFloat* result) const;
	virtual JBoolean	Evaluate(JComplex* result) const;
	virtual void		Print(ostream& output) const;
	virtual JFunction*	Copy() const;
	virtual JBoolean	SameAs(const JFunction& theFunction) const;
	virtual void		BuildNodeList(JExprNodeList* nodeList, const JIndex myNode);

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
