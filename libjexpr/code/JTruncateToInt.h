/******************************************************************************
 JTruncateToInt.h

	Interface for JTruncateToInt class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JTruncateToInt
#define _H_JTruncateToInt

#include "jx-af/jexpr/JUnaryFunction.h"

class JTruncateToInt : public JUnaryFunction
{
public:

	JTruncateToInt(JFunction* arg = nullptr);
	JTruncateToInt(const JTruncateToInt& source);

	virtual ~JTruncateToInt();

	bool		Evaluate(JFloat* result) const override;
	bool		Evaluate(JComplex* result) const override;
	JFunction*	Copy() const  override;
};

#endif
