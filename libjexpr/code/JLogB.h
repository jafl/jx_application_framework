/******************************************************************************
 JLogB.h

	Interface for JLogB class.

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JLogB
#define _H_JLogB

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JBinaryFunction.h>

class JLogB : public JBinaryFunction
{
public:

	JLogB();
	JLogB(JFunction* arg1, JFunction* arg2);
	JLogB(const JLogB& source);

	virtual ~JLogB();

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

	const JLogB& operator=(const JLogB& source);
};

#endif
