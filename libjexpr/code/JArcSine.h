/******************************************************************************
 JArcSine.h

	Interface for JArcSine class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JArcSine
#define _H_JArcSine

#include "JUnaryFunction.h"

class JArcSine : public JUnaryFunction
{
public:

	JArcSine(JFunction* arg = nullptr);
	JArcSine(const JArcSine& source);

	~JArcSine() override;

	bool		Evaluate(JFloat* result) const override;
	bool		Evaluate(JComplex* result) const override;
	JFunction*	Copy() const override;
};

#endif
