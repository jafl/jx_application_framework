/******************************************************************************
 JHypSine.h

	Interface for JHypSine class.

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#ifndef _H_JHypSine
#define _H_JHypSine

#include "jx-af/jexpr/JUnaryFunction.h"

class JHypSine : public JUnaryFunction
{
public:

	JHypSine(JFunction* arg = nullptr);
	JHypSine(const JHypSine& source);

	virtual ~JHypSine();

	virtual bool		Evaluate(JFloat* result) const override;
	virtual bool		Evaluate(JComplex* result) const override;
	virtual JFunction*	Copy() const override;
};

#endif
