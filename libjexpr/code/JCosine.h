/******************************************************************************
 JCosine.h

	Interface for JCosine class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JCosine
#define _H_JCosine

#include <JUnaryFunction.h>

class JCosine : public JUnaryFunction
{
public:

	JCosine();
	JCosine(JFunction* arg);
	JCosine(const JCosine& source);

	virtual ~JCosine();

	virtual JBoolean	Evaluate(JFloat* result) const override;
	virtual JBoolean	Evaluate(JComplex* result) const override;
	virtual JFunction*	Copy() const override;

private:

	// not allowed

	const JCosine& operator=(const JCosine& source);
};

#endif
