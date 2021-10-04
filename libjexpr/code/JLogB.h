/******************************************************************************
 JLogB.h

	Interface for JLogB class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JLogB
#define _H_JLogB

#include "jx-af/jexpr/JBinaryFunction.h"

class JLogB : public JBinaryFunction
{
public:

	JLogB(JFunction* arg1, JFunction* arg2);
	JLogB(const JLogB& source);

	virtual ~JLogB();

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
