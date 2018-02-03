/******************************************************************************
 JDiscreteEquality.h

	Interface for JDiscreteEquality class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JDiscreteEquality
#define _H_JDiscreteEquality

#include "JDecisionWithVar.h"

class JDiscreteEquality : public JDecisionWithVar
{
public:

	JDiscreteEquality(const JVariableList* theVariableList,
					  const JIndex variableIndex, JFunction* arrayIndex,
					  const JIndex value);
	JDiscreteEquality(const JDiscreteEquality& source);

	virtual ~JDiscreteEquality();

	virtual JBoolean	Evaluate() const;
	virtual void		Print(std::ostream& output) const;
	virtual JDecision*	Copy() const;
	virtual JBoolean	SameAs(const JDecision& theDecision) const;

	virtual void	SetVariableIndex(const JIndex variableIndex);

	JIndex	GetValue() const;
	void	SetValue(const JIndex value);

protected:

	JBoolean	ValueValid(const JIndex value) const;

private:

	JIndex	itsValue;

private:

	// not allowed

	const JDiscreteEquality& operator=(const JDiscreteEquality& source);
};

/******************************************************************************
 GetValue

 ******************************************************************************/

inline JIndex
JDiscreteEquality::GetValue()
	const
{
	return itsValue;
}

#endif
