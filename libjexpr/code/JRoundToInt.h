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

	bool		Evaluate(JFloat* result) const override;
	bool		Evaluate(JComplex* result) const override;
	JFunction*	Copy() const override;
};

#endif
