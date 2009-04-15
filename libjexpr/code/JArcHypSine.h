/******************************************************************************
 JArcHypSine.h

	Interface for JArcHypSine class.

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JArcHypSine
#define _H_JArcHypSine

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JUnaryFunction.h>

class JArcHypSine : public JUnaryFunction
{
public:

	JArcHypSine();
	JArcHypSine(JFunction* arg);
	JArcHypSine(const JArcHypSine& source);

	virtual ~JArcHypSine();

	virtual JBoolean	Evaluate(JFloat* result) const;
	virtual JBoolean	Evaluate(JComplex* result) const;
	virtual JFunction*	Copy() const;

private:

	// not allowed

	const JArcHypSine& operator=(const JArcHypSine& source);
};

#endif
