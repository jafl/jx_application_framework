/******************************************************************************
 JLogE.h

	Interface for JLogE class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JLogE
#define _H_JLogE

#include "JUnaryFunction.h"

class JLogE : public JUnaryFunction
{
public:

	JLogE(JFunction* arg = nullptr);
	JLogE(const JLogE& source);

	~JLogE() override;

	bool		Evaluate(JFloat* result) const override;
	bool		Evaluate(JComplex* result) const override;
	JFunction*	Copy() const override;
};

#endif
