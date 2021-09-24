/******************************************************************************
 JAlgSign.h

	Interface for JAlgSign class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JAlgSign
#define _H_JAlgSign

#include <JUnaryFunction.h>

class JAlgSign : public JUnaryFunction
{
public:

	JAlgSign(JFunction* arg = nullptr);
	JAlgSign(const JAlgSign& source);

	virtual ~JAlgSign();

	virtual bool		Evaluate(JFloat* result) const override;
	virtual bool		Evaluate(JComplex* result) const override;
	virtual JFunction*	Copy() const override;
};

#endif
