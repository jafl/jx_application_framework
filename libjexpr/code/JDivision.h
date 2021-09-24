/******************************************************************************
 JDivision.h

	Interface for JDivision class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_Division
#define _H_Division

#include <JBinaryOperator.h>

class JDivision : public JBinaryOperator
{
public:

	JDivision(JFunction* arg1, JFunction* arg2);
	JDivision(const JDivision& source);

	virtual ~JDivision();

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
