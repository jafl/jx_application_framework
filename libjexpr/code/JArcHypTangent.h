/******************************************************************************
 JArcHypTangent.h

	Interface for JArcHypTangent class.

	Copyright (C) 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JArcHypTangent
#define _H_JArcHypTangent

#include <JUnaryFunction.h>

class JArcHypTangent : public JUnaryFunction
{
public:

	JArcHypTangent();
	JArcHypTangent(JFunction* arg);
	JArcHypTangent(const JArcHypTangent& source);

	virtual ~JArcHypTangent();

	virtual JBoolean	Evaluate(JFloat* result) const;
	virtual JBoolean	Evaluate(JComplex* result) const;
	virtual JFunction*	Copy() const;

private:

	// not allowed

	const JArcHypTangent& operator=(const JArcHypTangent& source);
};

#endif
