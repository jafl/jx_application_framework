/******************************************************************************
 JRoundToInt.h

	Interface for JRoundToInt class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JRoundToInt
#define _H_JRoundToInt

#include "JUnaryFunction.h"

class JRoundToInt : public JUnaryFunction
{
public:

	JRoundToInt(JFunction* arg = nullptr);
	JRoundToInt(const JRoundToInt& source);

	~JRoundToInt() override;

	bool		Evaluate(JFloat* result) const override;
	bool		Evaluate(JComplex* result) const override;
	JFunction*	Copy() const override;
};

#endif
