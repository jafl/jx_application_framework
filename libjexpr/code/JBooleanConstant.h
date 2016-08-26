/******************************************************************************
 JBooleanConstant.h

	Interface for JBooleanConstant class.

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JBooleanConstant
#define _H_JBooleanConstant

#include <JDecision.h>

class JBooleanConstant : public JDecision
{
public:

	JBooleanConstant(const JBoolean value);
	JBooleanConstant(const JBooleanConstant& source);

	virtual ~JBooleanConstant();

	virtual JBoolean	Evaluate() const;
	virtual void		Print(ostream& output) const;
	virtual JDecision*	Copy() const;
	virtual JBoolean	SameAs(const JDecision& theDecision) const;
	virtual void		BuildNodeList(JExprNodeList* nodeList, const JIndex myNode);

	JBoolean	GetValue() const;
	void		SetValue(const JBoolean value);

private:

	JBoolean	itsValue;

private:

	// not allowed

	const JBooleanConstant& operator=(const JBooleanConstant& source);
};

/******************************************************************************
 GetValue

 ******************************************************************************/

inline JBoolean
JBooleanConstant::GetValue()
	const
{
	return itsValue;
}

/******************************************************************************
 SetValue

 ******************************************************************************/

inline void
JBooleanConstant::SetValue
	(
	const JBoolean value
	)
{
	itsValue = value;
}

#endif
