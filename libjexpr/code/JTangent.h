/******************************************************************************
 JTangent.h

	Interface for JTangent class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JTangent
#define _H_JTangent

#include <JUnaryFunction.h>

class JTangent : public JUnaryFunction
{
public:

	JTangent(JFunction* arg = nullptr);
	JTangent(const JTangent& source);

	virtual ~JTangent();

	virtual bool	Evaluate(JFloat* result) const override;
	virtual bool	Evaluate(JComplex* result) const override;
	virtual JFunction*	Copy() const override;

private:

	// not allowed

	const JTangent& operator=(const JTangent& source);
};

#endif
