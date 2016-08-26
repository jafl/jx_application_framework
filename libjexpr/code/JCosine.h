/******************************************************************************
 JCosine.h

	Interface for JCosine class.

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JCosine
#define _H_JCosine

#include <JUnaryFunction.h>

class JCosine : public JUnaryFunction
{
public:

	JCosine();
	JCosine(JFunction* arg);
	JCosine(const JCosine& source);

	virtual ~JCosine();

	virtual JBoolean	Evaluate(JFloat* result) const;
	virtual JBoolean	Evaluate(JComplex* result) const;
	virtual JFunction*	Copy() const;

private:

	// not allowed

	const JCosine& operator=(const JCosine& source);
};

#endif
