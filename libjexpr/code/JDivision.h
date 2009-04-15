/******************************************************************************
 JDivision.h

	Interface for JDivision class.

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_Division
#define _H_Division

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JBinaryOperator.h>

class JDivision : public JBinaryOperator
{
public:

	JDivision();
	JDivision(JFunction* arg1, JFunction* arg2);
	JDivision(const JDivision& source);

	virtual ~JDivision();

	virtual JBoolean	Evaluate(JFloat* result) const;
	virtual JBoolean	Evaluate(JComplex* result) const;
	virtual JFunction*	Copy() const;
	virtual JBoolean	SameAs(const JFunction& theFunction) const;
	virtual JIndex		PrepareToRender(const JExprRenderer& renderer,
										const JPoint& upperLeft, const JSize fontSize,
										JExprRectList* rectList);
	virtual void		Render(const JExprRenderer& renderer,
							   const JExprRectList& rectList) const;

private:

	// not allowed

	const JDivision& operator=(const JDivision& source);
};

#endif
