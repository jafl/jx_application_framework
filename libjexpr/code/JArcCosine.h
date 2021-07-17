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

	JArcCosine(JFunction* arg = nullptr);
	JArcCosine(const JArcCosine& source);

	virtual ~JArcCosine();

	virtual bool	Evaluate(JFloat* result) const override;
	virtual bool	Evaluate(JComplex* result) const override;
	virtual JFunction*	Copy() const override;

private:

	// not allowed

	const JArcCosine& operator=(const JArcCosine& source);
};

#endif
