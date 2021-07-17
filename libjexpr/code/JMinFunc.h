/******************************************************************************
 JMinFunc.h

	Interface for JMinFunc class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JMinFunc
#define _H_JMinFunc

#include <JNaryFunction.h>

class JMinFunc : public JNaryFunction
{
public:

	JMinFunc(JPtrArray<JFunction>* argList = nullptr);
	JMinFunc(const JMinFunc& source);

	virtual ~JMinFunc();

	virtual bool	Evaluate(JFloat* result) const override;
	virtual bool	Evaluate(JComplex* result) const override;
	virtual JFunction*	Copy() const override;

private:

	// not allowed

	const JMinFunc& operator=(const JMinFunc& source);
};

#endif
