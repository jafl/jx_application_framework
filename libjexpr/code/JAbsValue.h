/******************************************************************************
 JAbsValue.h

	Interface for JAbsValue class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JAbsValue
#define _H_JAbsValue

#include "jx-af/jexpr/JUnaryFunction.h"

class JAbsValue : public JUnaryFunction
{
public:

	JAbsValue(JFunction* arg = nullptr);
	JAbsValue(const JAbsValue& source);

	virtual ~JAbsValue();

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
