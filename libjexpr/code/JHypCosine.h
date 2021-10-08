/******************************************************************************
 JHypCosine.h

	Interface for JHypCosine class.

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#ifndef _H_JHypCosine
#define _H_JHypCosine

#include "jx-af/jexpr/JUnaryFunction.h"

class JHypCosine : public JUnaryFunction
{
public:

	JHypCosine(JFunction* arg = nullptr);
	JHypCosine(const JHypCosine& source);

	~JHypCosine() override;

	bool		Evaluate(JFloat* result) const override;
	bool		Evaluate(JComplex* result) const override;
	JFunction*	Copy() const override;
};

#endif
