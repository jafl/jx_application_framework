/******************************************************************************
 JHypTangent.h

	Interface for JHypTangent class.

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#ifndef _H_JHypTangent
#define _H_JHypTangent

#include <JUnaryFunction.h>

class JHypTangent : public JUnaryFunction
{
public:

	JHypTangent();
	JHypTangent(JFunction* arg);
	JHypTangent(const JHypTangent& source);

	virtual ~JHypTangent();

	virtual JBoolean	Evaluate(JFloat* result) const;
	virtual JBoolean	Evaluate(JComplex* result) const;
	virtual JFunction*	Copy() const;

private:

	// not allowed

	const JHypTangent& operator=(const JHypTangent& source);
};

#endif
