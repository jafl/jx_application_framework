/******************************************************************************
 JRealPart.h

	Interface for JRealPart class.

	Copyright © 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JRealPart
#define _H_JRealPart

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JUnaryFunction.h>

class JRealPart : public JUnaryFunction
{
public:

	JRealPart();
	JRealPart(JFunction* arg);
	JRealPart(const JRealPart& source);

	virtual ~JRealPart();

	virtual JBoolean	Evaluate(JFloat* result) const;
	virtual JBoolean	Evaluate(JComplex* result) const;
	virtual JFunction*	Copy() const;

private:

	// not allowed

	const JRealPart& operator=(const JRealPart& source);
};

#endif
