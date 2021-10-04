/******************************************************************************
 JImagPart.h

	Interface for JImagPart class.

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#ifndef _H_JImagPart
#define _H_JImagPart

#include "jx-af/jexpr/JUnaryFunction.h"

class JImagPart : public JUnaryFunction
{
public:

	JImagPart(JFunction* arg = nullptr);
	JImagPart(const JImagPart& source);

	virtual ~JImagPart();

	bool		Evaluate(JFloat* result) const override;
	bool		Evaluate(JComplex* result) const override;
	JFunction*	Copy() const override;
};

#endif
