/******************************************************************************
 JLogE.h

	Interface for JLogE class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JLogE
#define _H_JLogE

#include <JUnaryFunction.h>

class JLogE : public JUnaryFunction
{
public:

	JLogE(JFunction* arg = nullptr);
	JLogE(const JLogE& source);

	virtual ~JLogE();

	virtual bool		Evaluate(JFloat* result) const override;
	virtual bool		Evaluate(JComplex* result) const override;
	virtual JFunction*	Copy() const override;
};

#endif
