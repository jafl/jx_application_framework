/******************************************************************************
 JArcHypTangent.h

	Interface for JArcHypTangent class.

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_JArcHypTangent
#define _H_JArcHypTangent

#include "jx-af/jexpr/JUnaryFunction.h"

class JArcHypTangent : public JUnaryFunction
{
public:

	JArcHypTangent(JFunction* arg = nullptr);
	JArcHypTangent(const JArcHypTangent& source);

	virtual ~JArcHypTangent();

	virtual bool		Evaluate(JFloat* result) const override;
	virtual bool		Evaluate(JComplex* result) const override;
	virtual JFunction*	Copy() const override;
};

#endif
