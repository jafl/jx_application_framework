/******************************************************************************
 JArcHypCosine.h

	Interface for JArcHypCosine class.

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JArcHypCosine
#define _H_JArcHypCosine

#include <JUnaryFunction.h>

class JArcHypCosine : public JUnaryFunction
{
public:

	JArcHypCosine();
	JArcHypCosine(JFunction* arg);
	JArcHypCosine(const JArcHypCosine& source);

	virtual ~JArcHypCosine();

	virtual JBoolean	Evaluate(JFloat* result) const;
	virtual JBoolean	Evaluate(JComplex* result) const;
	virtual JFunction*	Copy() const;

private:

	// not allowed

	const JArcHypCosine& operator=(const JArcHypCosine& source);
};

#endif
