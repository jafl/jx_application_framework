/******************************************************************************
 JArcSine.h

	Interface for JArcSine class.

	Copyright (C) 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JArcSine
#define _H_JArcSine

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
