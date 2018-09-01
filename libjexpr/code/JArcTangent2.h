/******************************************************************************
 JArcTangent2.h

	Interface for JArcTangent2 class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JArcTangent2
#define _H_JArcTangent2

#include <JBinaryFunction.h>

class JArcTangent2 : public JBinaryFunction
{
public:

	JArcTangent2(JFunction* arg1, JFunction* arg2);
	JArcTangent2(const JArcTangent2& source);

	virtual ~JArcTangent2();

	virtual JBoolean	Evaluate(JFloat* result) const override;
	virtual JBoolean	Evaluate(JComplex* result) const override;
	virtual JFunction*	Copy() const override;

private:

	// not allowed

	const JArcTangent2& operator=(const JArcTangent2& source);
};

#endif
