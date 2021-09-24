/******************************************************************************
 JArcHypSine.h

	Interface for JArcHypSine class.

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_JArcHypSine
#define _H_JArcHypSine

#include <JUnaryFunction.h>

class JArcHypSine : public JUnaryFunction
{
public:

	JArcHypSine(JFunction* arg = nullptr);
	JArcHypSine(const JArcHypSine& source);

	virtual ~JArcHypSine();

	virtual bool		Evaluate(JFloat* result) const override;
	virtual bool		Evaluate(JComplex* result) const override;
	virtual JFunction*	Copy() const override;
};

#endif
