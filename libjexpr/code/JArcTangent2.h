/******************************************************************************
 JArcTangent2.h

	Interface for JArcTangent2 class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JArcTangent2
#define _H_JArcTangent2

#include "jx-af/jexpr/JBinaryFunction.h"

class JArcTangent2 : public JBinaryFunction
{
public:

	JArcTangent2(JFunction* arg1, JFunction* arg2);
	JArcTangent2(const JArcTangent2& source);

	~JArcTangent2() override;

	bool		Evaluate(JFloat* result) const override;
	bool		Evaluate(JComplex* result) const override;
	JFunction*	Copy() const override;
};

#endif
