/******************************************************************************
 JSummation.h

	Interface for JSummation class.

	Copyright (C) 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JSummation
#define _H_JSummation

#include <JNaryOperator.h>

class JSummation : public JNaryOperator
{
public:

	JSummation();
	JSummation(const JSummation& source);

	virtual ~JSummation();

	virtual JBoolean	Evaluate(JFloat* result) const;
	virtual JBoolean	Evaluate(JComplex* result) const;
	virtual JFunction*	Copy() const;
	virtual void		Print(std::ostream& output) const;
	virtual JIndex		PrepareToRender(const JExprRenderer& renderer,
										const JPoint& upperLeft, const JSize fontSize,
										JExprRectList* rectList);
	virtual void		Render(const JExprRenderer& renderer,
							   const JExprRectList& rectList) const;

private:

	// not allowed

	const JSummation& operator=(const JSummation& source);
};

#endif
