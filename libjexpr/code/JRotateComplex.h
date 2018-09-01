/******************************************************************************
 JRotateComplex.h

	Interface for JRotateComplex class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JRotateComplex
#define _H_JRotateComplex

#include <JBinaryFunction.h>

class JRotateComplex : public JBinaryFunction
{
public:

	JRotateComplex(JFunction* arg1, JFunction* arg2);
	JRotateComplex(const JRotateComplex& source);

	virtual ~JRotateComplex();

	virtual JBoolean	Evaluate(JFloat* result) const override;
	virtual JBoolean	Evaluate(JComplex* result) const override;
	virtual JFunction*	Copy() const override;

private:

	// not allowed

	const JRotateComplex& operator=(const JRotateComplex& source);
};

#endif
