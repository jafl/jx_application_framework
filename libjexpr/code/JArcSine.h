/******************************************************************************
 JArcSine.h

	Interface for JArcSine class.

	Copyright (C) 1995 by John Lindal.

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

	virtual JBoolean	Evaluate(JFloat* result) const override;
	virtual JBoolean	Evaluate(JComplex* result) const override;
	virtual JFunction*	Copy() const override;

private:

	// not allowed

	const JArcSine& operator=(const JArcSine& source);
};

#endif
