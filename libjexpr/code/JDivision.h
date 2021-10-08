/******************************************************************************
 JDivision.h

	Interface for JDivision class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_Division
#define _H_Division

#include "jx-af/jexpr/JBinaryOperator.h"

class JDivision : public JBinaryOperator
{
public:

	JDivision(JFunction* arg1, JFunction* arg2);
	JDivision(const JDivision& source);

	~JDivision();

	bool		Evaluate(JFloat* result) const override;
	bool		Evaluate(JComplex* result) const override;
	JFunction*	Copy() const override;
	JIndex		Layout(const JExprRenderer& renderer,
					   const JPoint& upperLeft, const JSize fontSize,
					   JExprRectList* rectList) override;
	void		Render(const JExprRenderer& renderer,
					   const JExprRectList& rectList) const override;
};

#endif
