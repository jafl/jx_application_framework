/******************************************************************************
 JSine.h

	Interface for JSine class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JSine
#define _H_JSine

#include <JUnaryFunction.h>

class JSine : public JUnaryFunction
{
public:

	JSine(JFunction* arg = nullptr);
	JSine(const JSine& source);

	virtual ~JSine();

	virtual bool	Evaluate(JFloat* result) const override;
	virtual bool	Evaluate(JComplex* result) const override;
	virtual JFunction*	Copy() const override;

private:

	// not allowed

	const JSine& operator=(const JSine& source);
};

#endif
