/******************************************************************************
 JConjugate.h

	Interface for JConjugate class.

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#ifndef _H_JConjugate
#define _H_JConjugate

#include <JUnaryFunction.h>

class JConjugate : public JUnaryFunction
{
public:

	JConjugate();
	JConjugate(JFunction* arg);
	JConjugate(const JConjugate& source);

	virtual ~JConjugate();

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

	const JConjugate& operator=(const JConjugate& source);
};

#endif
