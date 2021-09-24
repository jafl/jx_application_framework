/******************************************************************************
 JExponent.h

	Interface for JExponent class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JExponent
#define _H_JExponent

#include <JBinaryOperator.h>

class JExponent : public JBinaryOperator
{
public:

	JExponent(JFunction* arg1, JFunction* arg2);
	JExponent(const JExponent& source);

	virtual ~JExponent();

	virtual bool		Evaluate(JFloat* result) const override;
	virtual bool		Evaluate(JComplex* result) const override;
	virtual JFunction*	Copy() const override;
	virtual JIndex		Layout(const JExprRenderer& renderer,
							   const JPoint& upperLeft, const JSize fontSize,
							   JExprRectList* rectList) override;
	virtual void		Render(const JExprRenderer& renderer,
							   const JExprRectList& rectList) const override;
};

#endif
