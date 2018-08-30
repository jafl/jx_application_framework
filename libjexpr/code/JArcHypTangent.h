/******************************************************************************
 JArcHypTangent.h

	Interface for JArcHypTangent class.

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_JArcHypTangent
#define _H_JArcHypTangent

#include <JUnaryFunction.h>

class JArcHypTangent : public JUnaryFunction
{
public:

	JArcHypTangent(JFunction* arg = nullptr);
	JArcHypTangent(const JArcHypTangent& source);

	virtual ~JArcHypTangent();

	virtual JBoolean	Evaluate(JFloat* result) const override;
	virtual JBoolean	Evaluate(JComplex* result) const override;
	virtual JFunction*	Copy() const override;

private:

	// not allowed

	const JArcHypTangent& operator=(const JArcHypTangent& source);
};

#endif
