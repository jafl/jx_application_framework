/******************************************************************************
 JHypTangent.h

	Interface for JHypTangent class.

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#ifndef _H_JHypTangent
#define _H_JHypTangent

#include "jx-af/jexpr/JUnaryFunction.h"

class JHypTangent : public JUnaryFunction
{
public:

	JHypTangent(JFunction* arg = nullptr);
	JHypTangent(const JHypTangent& source);

	virtual ~JHypTangent();

	virtual bool		Evaluate(JFloat* result) const override;
	virtual bool		Evaluate(JComplex* result) const override;
	virtual JFunction*	Copy() const override;
};

#endif
