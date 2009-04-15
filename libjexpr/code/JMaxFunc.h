/******************************************************************************
 JMaxFunc.h

	Interface for JMaxFunc class.

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JMaxFunc
#define _H_JMaxFunc

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JNaryFunction.h>

class JMaxFunc : public JNaryFunction
{
public:

	JMaxFunc();
	JMaxFunc(const JMaxFunc& source);

	virtual ~JMaxFunc();

	virtual JBoolean	Evaluate(JFloat* result) const;
	virtual JBoolean	Evaluate(JComplex* result) const;
	virtual JFunction*	Copy() const;
	virtual JBoolean	SameAs(const JFunction& theFunction) const;

private:

	// not allowed

	const JMaxFunc& operator=(const JMaxFunc& source);
};

#endif
