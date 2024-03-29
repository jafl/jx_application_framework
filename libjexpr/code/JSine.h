/******************************************************************************
 JSine.h

	Interface for JSine class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JSine
#define _H_JSine

#include "JUnaryFunction.h"

class JSine : public JUnaryFunction
{
public:

	JSine(JFunction* arg = nullptr);
	JSine(const JSine& source);

	~JSine() override;

	bool		Evaluate(JFloat* result) const override;
	bool		Evaluate(JComplex* result) const override;
	JFunction*	Copy() const override;
};

#endif
