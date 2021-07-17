/******************************************************************************
 JAbsValue.h

	Interface for JAbsValue class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JAbsValue
#define _H_JAbsValue

#include <JUnaryFunction.h>

class JAbsValue : public JUnaryFunction
{
public:

	JAbsValue(JFunction* arg = nullptr);
	JAbsValue(const JAbsValue& source);

	virtual ~JAbsValue();

	virtual bool	Evaluate(JFloat* result) const override;
	virtual bool	Evaluate(JComplex* result) const override;
	virtual JFunction*	Copy() const override;
	virtual JIndex		Layout(const JExprRenderer& renderer,
							   const JPoint& upperLeft, const JSize fontSize,
							   JExprRectList* rectList) override;
	virtual void		Render(const JExprRenderer& renderer,
							   const JExprRectList& rectList) const override;

private:

	// not allowed

	const JAbsValue& operator=(const JAbsValue& source);
};

#endif
