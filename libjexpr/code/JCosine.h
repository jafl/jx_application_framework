/******************************************************************************
 JCosine.h

	Interface for JCosine class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JCosine
#define _H_JCosine

#include "JUnaryFunction.h"

class JCosine : public JUnaryFunction
{
public:

	JCosine(JFunction* arg = nullptr);
	JCosine(const JCosine& source);

	~JCosine() override;

	bool		Evaluate(JFloat* result) const override;
	bool		Evaluate(JComplex* result) const override;
	JFunction*	Copy() const override;
};

#endif
