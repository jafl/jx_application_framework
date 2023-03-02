/******************************************************************************
 JMaxFunc.h

	Interface for JMaxFunc class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JMaxFunc
#define _H_JMaxFunc

#include "JNaryFunction.h"

class JMaxFunc : public JNaryFunction
{
public:

	JMaxFunc(JPtrArray<JFunction>* argList = nullptr);
	JMaxFunc(const JMaxFunc& source);

	~JMaxFunc() override;

	bool		Evaluate(JFloat* result) const override;
	bool		Evaluate(JComplex* result) const override;
	JFunction*	Copy() const override;
};

#endif
