/******************************************************************************
 JMinFunc.h

	Interface for JMinFunc class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JMinFunc
#define _H_JMinFunc

#include "JNaryFunction.h"

class JMinFunc : public JNaryFunction
{
public:

	JMinFunc(JPtrArray<JFunction>* argList = nullptr);
	JMinFunc(const JMinFunc& source);

	~JMinFunc() override;

	bool		Evaluate(JFloat* result) const override;
	bool		Evaluate(JComplex* result) const override;
	JFunction*	Copy() const override;
};

#endif
