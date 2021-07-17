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

	JImagPart(JFunction* arg = nullptr);
	JImagPart(const JImagPart& source);

	virtual ~JImagPart();

	virtual bool	Evaluate(JFloat* result) const override;
	virtual bool	Evaluate(JComplex* result) const override;
	virtual JFunction*	Copy() const override;

private:

	// not allowed

	const JImagPart& operator=(const JImagPart& source);
};

#endif
