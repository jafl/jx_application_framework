/******************************************************************************
 JImagPart.h

	Interface for JImagPart class.

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#ifndef _H_JImagPart
#define _H_JImagPart

#include "JUnaryFunction.h"

class JImagPart : public JUnaryFunction
{
public:

	JImagPart(JFunction* arg = nullptr);
	JImagPart(const JImagPart& source);

	~JImagPart() override;

	bool		Evaluate(JFloat* result) const override;
	bool		Evaluate(JComplex* result) const override;
	JFunction*	Copy() const override;
};

#endif
