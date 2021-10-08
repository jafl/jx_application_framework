/******************************************************************************
 JAlgSign.h

	Interface for JAlgSign class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JAlgSign
#define _H_JAlgSign

#include "jx-af/jexpr/JUnaryFunction.h"

class JAlgSign : public JUnaryFunction
{
public:

	JAlgSign(JFunction* arg = nullptr);
	JAlgSign(const JAlgSign& source);

	~JAlgSign() override;

	bool		Evaluate(JFloat* result) const override;
	bool		Evaluate(JComplex* result) const override;
	JFunction*	Copy() const override;
};

#endif
