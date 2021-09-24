/******************************************************************************
 JArcTangent.h

	Interface for JArcTangent class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JArcTangent
#define _H_JArcTangent

#include <JUnaryFunction.h>

class JArcTangent : public JUnaryFunction
{
public:

	JArcTangent(JFunction* arg = nullptr);
	JArcTangent(const JArcTangent& source);

	virtual ~JArcTangent();

	virtual bool		Evaluate(JFloat* result) const override;
	virtual bool		Evaluate(JComplex* result) const override;
	virtual JFunction*	Copy() const override;
};

#endif
