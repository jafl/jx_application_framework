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

	JAbsValue();
	JAbsValue(JFunction* arg);
	JAbsValue(const JAbsValue& source);

	virtual ~JAbsValue();

	virtual JBoolean	Evaluate(JFloat* result) const;
	virtual JBoolean	Evaluate(JComplex* result) const;
	virtual JFunction*	Copy() const;
	virtual JIndex		PrepareToRender(const JExprRenderer& renderer,
										const JPoint& upperLeft, const JSize fontSize,
										JExprRectList* rectList);
	virtual void		Render(const JExprRenderer& renderer,
							   const JExprRectList& rectList) const;

private:

	// not allowed

	const JAbsValue& operator=(const JAbsValue& source);
};

#endif
