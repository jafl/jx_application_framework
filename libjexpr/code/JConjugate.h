/******************************************************************************
 JConjugate.h

	Interface for JConjugate class.

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#ifndef _H_JConjugate
#define _H_JConjugate

#include "jx-af/jexpr/JUnaryFunction.h"

class JConjugate : public JUnaryFunction
{
public:

	JConjugate(JFunction* arg = nullptr);
	JConjugate(const JConjugate& source);

	virtual ~JConjugate();

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
