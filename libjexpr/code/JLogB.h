/******************************************************************************
 JLogB.h

	Interface for JLogB class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JLogB
#define _H_JLogB

#include <JBinaryFunction.h>

class JLogB : public JBinaryFunction
{
public:

	JLogB(JFunction* arg1, JFunction* arg2);
	JLogB(const JLogB& source);

	virtual ~JLogB();

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

	const JLogB& operator=(const JLogB& source);
};

#endif
