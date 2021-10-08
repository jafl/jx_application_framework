/******************************************************************************
 JArcTangent.h

	Interface for JArcTangent class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JArcTangent
#define _H_JArcTangent

#include "jx-af/jexpr/JUnaryFunction.h"

class JArcTangent : public JUnaryFunction
{
public:

	JArcTangent(JFunction* arg = nullptr);
	JArcTangent(const JArcTangent& source);

	~JArcTangent() override;

	bool		Evaluate(JFloat* result) const override;
	bool		Evaluate(JComplex* result) const override;
	JFunction*	Copy() const override;
};

#endif
