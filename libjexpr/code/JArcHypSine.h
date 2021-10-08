/******************************************************************************
 JArcHypSine.h

	Interface for JArcHypSine class.

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_JArcHypSine
#define _H_JArcHypSine

#include "jx-af/jexpr/JUnaryFunction.h"

class JArcHypSine : public JUnaryFunction
{
public:

	JArcHypSine(JFunction* arg = nullptr);
	JArcHypSine(const JArcHypSine& source);

	~JArcHypSine() override;

	bool		Evaluate(JFloat* result) const override;
	bool		Evaluate(JComplex* result) const override;
	JFunction*	Copy() const override;
};

#endif
