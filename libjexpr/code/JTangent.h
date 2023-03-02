/******************************************************************************
 JTangent.h

	Interface for JTangent class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JTangent
#define _H_JTangent

#include "JUnaryFunction.h"

class JTangent : public JUnaryFunction
{
public:

	JTangent(JFunction* arg = nullptr);
	JTangent(const JTangent& source);

	~JTangent() override;

	bool		Evaluate(JFloat* result) const override;
	bool		Evaluate(JComplex* result) const override;
	JFunction*	Copy() const override;
};

#endif
