/******************************************************************************
 JSine.h

	Interface for JSine class.

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JSine
#define _H_JSine

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JUnaryFunction.h>

class JSine : public JUnaryFunction
{
public:

	JSine();
	JSine(JFunction* arg);
	JSine(const JSine& source);

	virtual ~JSine();

	virtual JBoolean	Evaluate(JFloat* result) const;
	virtual JBoolean	Evaluate(JComplex* result) const;
	virtual JFunction*	Copy() const;

private:

	// not allowed

	const JSine& operator=(const JSine& source);
};

#endif
