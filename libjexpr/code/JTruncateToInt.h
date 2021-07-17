/******************************************************************************
 JTruncateToInt.h

	Interface for JTruncateToInt class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JTruncateToInt
#define _H_JTruncateToInt

#include <JUnaryFunction.h>

class JTruncateToInt : public JUnaryFunction
{
public:

	JTruncateToInt(JFunction* arg = nullptr);
	JTruncateToInt(const JTruncateToInt& source);

	virtual ~JTruncateToInt();

	virtual bool	Evaluate(JFloat* result) const override;
	virtual bool	Evaluate(JComplex* result) const override;
	virtual JFunction*	Copy() const  override;

private:

	// not allowed

	const JTruncateToInt& operator=(const JTruncateToInt& source);
};

#endif
