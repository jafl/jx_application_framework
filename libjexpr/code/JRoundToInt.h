/******************************************************************************
 JRoundToInt.h

	Interface for JRoundToInt class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JRoundToInt
#define _H_JRoundToInt

#include <JUnaryFunction.h>

class JRoundToInt : public JUnaryFunction
{
public:

	JRoundToInt(JFunction* arg = nullptr);
	JRoundToInt(const JRoundToInt& source);

	virtual ~JRoundToInt();

	virtual JBoolean	Evaluate(JFloat* result) const override;
	virtual JBoolean	Evaluate(JComplex* result) const override;
	virtual JFunction*	Copy() const override;

private:

	// not allowed

	const JRoundToInt& operator=(const JRoundToInt& source);
};

#endif
