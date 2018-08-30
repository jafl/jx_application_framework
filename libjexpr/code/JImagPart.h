/******************************************************************************
 JImagPart.h

	Interface for JImagPart class.

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#ifndef _H_JImagPart
#define _H_JImagPart

#include <JUnaryFunction.h>

class JImagPart : public JUnaryFunction
{
public:

	JImagPart();
	JImagPart(JFunction* arg);
	JImagPart(const JImagPart& source);

	virtual ~JImagPart();

	virtual JBoolean	Evaluate(JFloat* result) const override;
	virtual JBoolean	Evaluate(JComplex* result) const override;
	virtual JFunction*	Copy() const override;

private:

	// not allowed

	const JImagPart& operator=(const JImagPart& source);
};

#endif
