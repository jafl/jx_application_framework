/******************************************************************************
 JArcHypCosine.h

	Interface for JArcHypCosine class.

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_JArcHypCosine
#define _H_JArcHypCosine

#include <JUnaryFunction.h>

class JArcHypCosine : public JUnaryFunction
{
public:

	JArcHypCosine(JFunction* arg = nullptr);
	JArcHypCosine(const JArcHypCosine& source);

	virtual ~JArcHypCosine();

	virtual bool	Evaluate(JFloat* result) const override;
	virtual bool	Evaluate(JComplex* result) const override;
	virtual JFunction*	Copy() const override;

private:

	// not allowed

	const JArcHypCosine& operator=(const JArcHypCosine& source);
};

#endif
