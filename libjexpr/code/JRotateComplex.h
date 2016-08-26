/******************************************************************************
 JRotateComplex.h

	Interface for JRotateComplex class.

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JRotateComplex
#define _H_JRotateComplex

#include <JBinaryFunction.h>

class JRotateComplex : public JBinaryFunction
{
public:

	JRotateComplex();
	JRotateComplex(JFunction* arg1, JFunction* arg2);
	JRotateComplex(const JRotateComplex& source);

	virtual ~JRotateComplex();

	virtual JBoolean	Evaluate(JFloat* result) const;
	virtual JBoolean	Evaluate(JComplex* result) const;
	virtual JFunction*	Copy() const;
	virtual JBoolean	SameAs(const JFunction& theFunction) const;

private:

	// not allowed

	const JRotateComplex& operator=(const JRotateComplex& source);
};

#endif
