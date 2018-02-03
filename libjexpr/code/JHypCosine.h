/******************************************************************************
 JHypCosine.h

	Interface for JHypCosine class.

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#ifndef _H_JHypCosine
#define _H_JHypCosine

#include <JUnaryFunction.h>

class JHypCosine : public JUnaryFunction
{
public:

	JHypCosine();
	JHypCosine(JFunction* arg);
	JHypCosine(const JHypCosine& source);

	virtual ~JHypCosine();

	virtual JBoolean	Evaluate(JFloat* result) const;
	virtual JBoolean	Evaluate(JComplex* result) const;
	virtual JFunction*	Copy() const;

private:

	// not allowed

	const JHypCosine& operator=(const JHypCosine& source);
};

#endif
