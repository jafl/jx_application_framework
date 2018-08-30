/******************************************************************************
 JSquareRoot.h

	Interface for JSquareRoot class.

	Copyright (C) 1995 by John Lindal.

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

	virtual JBoolean	Evaluate(JFloat* result) const override;
	virtual JBoolean	Evaluate(JComplex* result) const override;
	virtual JFunction*	Copy() const override;
	virtual JIndex		Layout(const JExprRenderer& renderer,
							   const JPoint& upperLeft, const JSize fontSize,
							   JExprRectList* rectList) override;
	virtual void		Render(const JExprRenderer& renderer,
							   const JExprRectList& rectList) const override;

private:

	// not allowed

	const JSquareRoot& operator=(const JSquareRoot& source);
};

#endif
