/******************************************************************************
 JTruncateToInt.h

	Interface for JTruncateToInt class.

	Copyright (C) 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JTruncateToInt
#define _H_JTruncateToInt

#include <JUnaryFunction.h>

class JTruncateToInt : public JUnaryFunction
{
public:

	JTruncateToInt();
	JTruncateToInt(JFunction* arg);
	JTruncateToInt(const JTruncateToInt& source);

	virtual ~JTruncateToInt();

	virtual JBoolean	Evaluate(JFloat* result) const;
	virtual JBoolean	Evaluate(JComplex* result) const;
	virtual JFunction*	Copy() const;

private:

	// not allowed

	const JTruncateToInt& operator=(const JTruncateToInt& source);
};

#endif
