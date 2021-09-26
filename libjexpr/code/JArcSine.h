/******************************************************************************
 JArcSine.h

	Interface for JArcSine class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JArcSine
#define _H_JArcSine

#include "jx-af/jexpr/JUnaryFunction.h"

class JArcSine : public JUnaryFunction
{
public:

	JArcSine(JFunction* arg = nullptr);
	JArcSine(const JArcSine& source);

	virtual ~JArcSine();

	virtual bool		Evaluate(JFloat* result) const override;
	virtual bool		Evaluate(JComplex* result) const override;
	virtual JFunction*	Copy() const override;
};

#endif
