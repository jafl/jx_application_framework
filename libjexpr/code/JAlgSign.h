/******************************************************************************
 JAlgSign.h

	Interface for JAlgSign class.

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JAlgSign
#define _H_JAlgSign

#include <JUnaryFunction.h>

class JAlgSign : public JUnaryFunction
{
public:

	JAlgSign();
	JAlgSign(JFunction* arg);
	JAlgSign(const JAlgSign& source);

	virtual ~JAlgSign();

	virtual JBoolean	Evaluate(JFloat* result) const;
	virtual JBoolean	Evaluate(JComplex* result) const;
	virtual JFunction*	Copy() const;

private:

	// not allowed

	const JAlgSign& operator=(const JAlgSign& source);
};

#endif
