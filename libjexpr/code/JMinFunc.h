/******************************************************************************
 JMinFunc.h

	Interface for JMinFunc class.

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JMinFunc
#define _H_JMinFunc

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JNaryFunction.h>

class JMinFunc : public JNaryFunction
{
public:

	JMinFunc();
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
