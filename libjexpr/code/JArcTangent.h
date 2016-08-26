/******************************************************************************
 JArcTangent.h

	Interface for JArcTangent class.

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JArcTangent
#define _H_JArcTangent

#include <JUnaryFunction.h>

class JArcTangent : public JUnaryFunction
{
public:

	JArcTangent();
	JArcTangent(JFunction* arg);
	JArcTangent(const JArcTangent& source);

	virtual ~JArcTangent();

	virtual JBoolean	Evaluate(JFloat* result) const;
	virtual JBoolean	Evaluate(JComplex* result) const;
	virtual JFunction*	Copy() const;

private:

	// not allowed

	const JArcTangent& operator=(const JArcTangent& source);
};

#endif
