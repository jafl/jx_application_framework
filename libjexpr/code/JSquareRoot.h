/******************************************************************************
 JSquareRoot.h

	Interface for JSquareRoot class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JSquareRoot
#define _H_JSquareRoot

#include "jx-af/jexpr/JUnaryFunction.h"

class JSquareRoot : public JUnaryFunction
{
public:

	JSquareRoot(JFunction* arg = nullptr);
	JSquareRoot(const JSquareRoot& source);

	~JSquareRoot() override;

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
