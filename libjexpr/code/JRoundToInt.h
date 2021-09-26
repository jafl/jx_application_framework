/******************************************************************************
 JRoundToInt.h

	Interface for JRoundToInt class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JRoundToInt
#define _H_JRoundToInt

#include "jx-af/jexpr/JUnaryFunction.h"

class JRoundToInt : public JUnaryFunction
{
public:

	JRoundToInt(JFunction* arg = nullptr);
	JRoundToInt(const JRoundToInt& source);

	virtual ~JRoundToInt();

	virtual bool		Evaluate(JFloat* result) const override;
	virtual bool		Evaluate(JComplex* result) const override;
	virtual JFunction*	Copy() const override;
};

#endif
