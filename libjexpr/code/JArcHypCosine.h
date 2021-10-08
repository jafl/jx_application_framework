/******************************************************************************
 JArcHypCosine.h

	Interface for JArcHypCosine class.

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_JArcHypCosine
#define _H_JArcHypCosine

#include "jx-af/jexpr/JUnaryFunction.h"

class JArcHypCosine : public JUnaryFunction
{
public:

	JArcHypCosine(JFunction* arg = nullptr);
	JArcHypCosine(const JArcHypCosine& source);

	~JArcHypCosine() override;

	bool		Evaluate(JFloat* result) const override;
	bool		Evaluate(JComplex* result) const override;
	JFunction*	Copy() const override;
};

#endif
