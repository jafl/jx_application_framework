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

	JRealPart();
	JRealPart(JFunction* arg);
	JRealPart(const JRealPart& source);

	virtual ~JRealPart();

	virtual JBoolean	Evaluate(JFloat* result) const override;
	virtual JBoolean	Evaluate(JComplex* result) const override;
	virtual JFunction*	Copy() const override;

private:

	// not allowed

	const JRealPart& operator=(const JRealPart& source);
};

#endif
