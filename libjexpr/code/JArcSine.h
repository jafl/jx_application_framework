/******************************************************************************
 JArcSine.h

	Interface for JArcSine class.

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JArcSine
#define _H_JArcSine

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JUnaryFunction.h>

class JArcSine : public JUnaryFunction
{
public:

	JArcSine();
	JArcSine(JFunction* arg);
	JArcSine(const JArcSine& source);

	virtual ~JArcSine();

	virtual JBoolean	Evaluate(JFloat* result) const;
	virtual JBoolean	Evaluate(JComplex* result) const;
	virtual JFunction*	Copy() const;

private:

	// not allowed

	const JArcSine& operator=(const JArcSine& source);
};

#endif
