/******************************************************************************
 JArcCosine.h

	Interface for JArcCosine class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JArcCosine
#define _H_JArcCosine

#include <JUnaryFunction.h>

class JArcCosine : public JUnaryFunction
{
public:

	JArcCosine();
	JArcCosine(JFunction* arg);
	JArcCosine(const JArcCosine& source);

	virtual ~JArcCosine();

	virtual JBoolean	Evaluate(JFloat* result) const;
	virtual JBoolean	Evaluate(JComplex* result) const;
	virtual JFunction*	Copy() const;

private:

	// not allowed

	const JArcCosine& operator=(const JArcCosine& source);
};

#endif
