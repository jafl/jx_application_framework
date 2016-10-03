/******************************************************************************
 JRoundToInt.h

	Interface for JRoundToInt class.

	Copyright (C) 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JRoundToInt
#define _H_JRoundToInt

#include <JUnaryFunction.h>

class JRoundToInt : public JUnaryFunction
{
public:

	JRoundToInt();
	JRoundToInt(JFunction* arg);
	JRoundToInt(const JRoundToInt& source);

	virtual ~JRoundToInt();

	virtual JBoolean	Evaluate(JFloat* result) const;
	virtual JBoolean	Evaluate(JComplex* result) const;
	virtual JFunction*	Copy() const;

private:

	// not allowed

	const JRoundToInt& operator=(const JRoundToInt& source);
};

#endif
