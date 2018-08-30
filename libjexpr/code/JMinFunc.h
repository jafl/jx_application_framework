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

	JMinFunc(JPtrArray<JFunction>* argList);
	JMinFunc(const JMinFunc& source);

	virtual ~JMinFunc();

	virtual JBoolean	Evaluate(JFloat* result) const;
	virtual JBoolean	Evaluate(JComplex* result) const;
	virtual JFunction*	Copy() const;
	virtual JBoolean	SameAs(const JFunction& theFunction) const;

private:

	// not allowed

	const JMinFunc& operator=(const JMinFunc& source);
};

#endif
