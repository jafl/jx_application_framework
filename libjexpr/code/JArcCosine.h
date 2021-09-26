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

	virtual ~JArcCosine();

	virtual bool		Evaluate(JFloat* result) const override;
	virtual bool		Evaluate(JComplex* result) const override;
	virtual JFunction*	Copy() const override;
};

#endif
