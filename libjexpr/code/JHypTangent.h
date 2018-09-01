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

	JHypTangent(JFunction* arg = nullptr);
	JHypTangent(const JHypTangent& source);

	virtual ~JHypTangent();

	virtual JBoolean	Evaluate(JFloat* result) const override;
	virtual JBoolean	Evaluate(JComplex* result) const override;
	virtual JFunction*	Copy() const override;

private:

	// not allowed

	const JHypTangent& operator=(const JHypTangent& source);
};

#endif
