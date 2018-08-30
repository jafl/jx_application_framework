/******************************************************************************
 JParallel.h

	Interface for JParallel class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JParallel
#define _H_JParallel

#include <JNaryOperator.h>

class JParallel : public JNaryOperator
{
public:

	JParallel(JPtrArray<JFunction>* argList);
	JParallel(const JParallel& source);

	virtual ~JParallel();

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

	const JParallel& operator=(const JParallel& source);
};

#endif
