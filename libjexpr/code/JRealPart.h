/******************************************************************************
 JRealPart.h

	Interface for JRealPart class.

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#ifndef _H_JRealPart
#define _H_JRealPart

#include <JUnaryFunction.h>

class JRealPart : public JUnaryFunction
{
public:

	JRealPart(JFunction* arg = nullptr);
	JRealPart(const JRealPart& source);

	virtual ~JRealPart();

	virtual bool		Evaluate(JFloat* result) const override;
	virtual bool		Evaluate(JComplex* result) const override;
	virtual JFunction*	Copy() const override;
};

#endif
