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

	~JNegation() override;

	bool		Evaluate(JFloat* result) const override;
	bool		Evaluate(JComplex* result) const override;
	JFunction*	Copy() const override;
	void		Print(std::ostream& output) const override;
	JIndex		Layout(const JExprRenderer& renderer,
							   const JPoint& upperLeft, const JSize fontSize,
							   JExprRectList* rectList) override;
	void		Render(const JExprRenderer& renderer,
							   const JExprRectList& rectList) const override;
};

#endif
