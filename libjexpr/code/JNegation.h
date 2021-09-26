/******************************************************************************
 JNegation.h

	Interface for JNegation class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JNegation
#define _H_JNegation

#include "jx-af/jexpr/JUnaryFunction.h"

class JNegation : public JUnaryFunction
{
public:

	JNegation(JFunction* arg = nullptr);
	JNegation(const JNegation& source);

	virtual ~JNegation();

	virtual bool		Evaluate(JFloat* result) const override;
	virtual bool		Evaluate(JComplex* result) const override;
	virtual JFunction*	Copy() const override;
	virtual void		Print(std::ostream& output) const override;
	virtual JIndex		Layout(const JExprRenderer& renderer,
							   const JPoint& upperLeft, const JSize fontSize,
							   JExprRectList* rectList) override;
	virtual void		Render(const JExprRenderer& renderer,
							   const JExprRectList& rectList) const override;
};

#endif
