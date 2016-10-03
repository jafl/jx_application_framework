/******************************************************************************
 JParallel.h

	Interface for JParallel class.

	Copyright (C) 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JParallel
#define _H_JParallel

#include <JNaryOperator.h>

class JParallel : public JNaryOperator
{
public:

	JParallel();
	JParallel(const JParallel& source);

	virtual ~JParallel();

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

	const JParallel& operator=(const JParallel& source);
};

#endif
