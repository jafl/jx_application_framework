/******************************************************************************
 JExponent.h

	Interface for JExponent class.

	Copyright (C) 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JExponent
#define _H_JExponent

#include <JBinaryOperator.h>

class JExponent : public JBinaryOperator
{
public:

	JExponent();
	JExponent(JFunction* arg1, JFunction* arg2);
	JExponent(const JExponent& source);

	virtual ~JExponent();

	virtual JBoolean	Evaluate(JFloat* result) const;
	virtual JBoolean	Evaluate(JComplex* result) const;
	virtual JFunction*	Copy() const;
	virtual JBoolean	SameAs(const JFunction& theFunction) const;
	virtual JIndex		PrepareToRender(const JExprRenderer& renderer,
										const JPoint& upperLeft, const JSize fontSize,
										JExprRectList* rectList);
	virtual void		Render(const JExprRenderer& renderer,
							   const JExprRectList& rectList) const;

private:

	// not allowed

	const JExponent& operator=(const JExponent& source);
};

#endif
