/******************************************************************************
 JHypSine.h

	Interface for JHypSine class.

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#ifndef _H_JHypSine
#define _H_JHypSine

#include "JUnaryFunction.h"

class JHypSine : public JUnaryFunction
{
public:

	JHypSine(JFunction* arg = nullptr);
	JHypSine(const JHypSine& source);

	~JHypSine() override;

	bool		Evaluate(JFloat* result) const override;
	bool		Evaluate(JComplex* result) const override;
	JFunction*	Copy() const override;
};

#endif
