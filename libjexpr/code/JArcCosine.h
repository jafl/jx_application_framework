/******************************************************************************
 JArcCosine.h

	Interface for JArcCosine class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JArcCosine
#define _H_JArcCosine

#include "jx-af/jexpr/JUnaryFunction.h"

class JArcCosine : public JUnaryFunction
{
public:

	JArcCosine(JFunction* arg = nullptr);
	JArcCosine(const JArcCosine& source);

	~JArcCosine() override;

	bool		Evaluate(JFloat* result) const override;
	bool		Evaluate(JComplex* result) const override;
	JFunction*	Copy() const override;
};

#endif
