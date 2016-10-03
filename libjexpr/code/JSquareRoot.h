/******************************************************************************
 JSquareRoot.h

	Interface for JSquareRoot class.

	Copyright (C) 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JSquareRoot
#define _H_JSquareRoot

#include <JUnaryFunction.h>

class JSquareRoot : public JUnaryFunction
{
public:

	JSquareRoot();
	JSquareRoot(JFunction* arg);
	JSquareRoot(const JSquareRoot& source);

	virtual ~JSquareRoot();

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

	const JSquareRoot& operator=(const JSquareRoot& source);
};

#endif
