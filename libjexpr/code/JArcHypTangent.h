/******************************************************************************
 JArcHypTangent.h

	Interface for JArcHypTangent class.

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_JArcHypTangent
#define _H_JArcHypTangent

#include "JUnaryFunction.h"

class JArcHypTangent : public JUnaryFunction
{
public:

	JArcHypTangent(JFunction* arg = nullptr);
	JArcHypTangent(const JArcHypTangent& source);

	~JArcHypTangent() override;

	bool		Evaluate(JFloat* result) const override;
	bool		Evaluate(JComplex* result) const override;
	JFunction*	Copy() const override;
};

#endif
