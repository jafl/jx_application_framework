/******************************************************************************
 JNegation.h

	Interface for JNegation class.

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JNegation
#define _H_JNegation

#include <JUnaryFunction.h>

class JNegation : public JUnaryFunction
{
public:

	JNegation();
	JNegation(JFunction* arg);
	JNegation(const JNegation& source);

	virtual ~JNegation();

	virtual JBoolean	Evaluate(JFloat* result) const;
	virtual JBoolean	Evaluate(JComplex* result) const;
	virtual JFunction*	Copy() const;
	virtual void		Print(ostream& output) const;
	virtual JIndex		PrepareToRender(const JExprRenderer& renderer,
										const JPoint& upperLeft, const JSize fontSize,
										JExprRectList* rectList);
	virtual void		Render(const JExprRenderer& renderer,
							   const JExprRectList& rectList) const;

private:

	// not allowed

	const JNegation& operator=(const JNegation& source);
};

#endif
