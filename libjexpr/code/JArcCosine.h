/******************************************************************************
 JArcCosine.h

	Interface for JArcCosine class.

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JArcCosine
#define _H_JArcCosine

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JUnaryFunction.h>

class JArcCosine : public JUnaryFunction
{
public:

	JArcCosine();
	JArcCosine(JFunction* arg);
	JArcCosine(const JArcCosine& source);

	virtual ~JArcCosine();

	virtual JBoolean	Evaluate(JFloat* result) const;
	virtual JBoolean	Evaluate(JComplex* result) const;
	virtual JFunction*	Copy() const;

private:

	// not allowed

	const JArcCosine& operator=(const JArcCosine& source);
};

#endif
