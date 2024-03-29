/******************************************************************************
 JRotateComplex.h

	Interface for JRotateComplex class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JRotateComplex
#define _H_JRotateComplex

#include "JBinaryFunction.h"

class JRotateComplex : public JBinaryFunction
{
public:

	JRotateComplex(JFunction* arg1, JFunction* arg2);
	JRotateComplex(const JRotateComplex& source);

	~JRotateComplex() override;

	bool		Evaluate(JFloat* result) const override;
	bool		Evaluate(JComplex* result) const override;
	JFunction*	Copy() const override;
};

#endif
