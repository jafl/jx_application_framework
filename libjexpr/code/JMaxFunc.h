/******************************************************************************
 JMaxFunc.h

	Interface for JMaxFunc class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JMaxFunc
#define _H_JMaxFunc

#include <JNaryFunction.h>

class JMaxFunc : public JNaryFunction
{
public:

	JMaxFunc(JPtrArray<JFunction>* argList = nullptr);
	JMaxFunc(const JMaxFunc& source);

	virtual ~JMaxFunc();

	virtual bool		Evaluate(JFloat* result) const override;
	virtual bool		Evaluate(JComplex* result) const override;
	virtual JFunction*	Copy() const override;
};

#endif
