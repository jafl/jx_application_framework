/******************************************************************************
 JParallel.h

	Interface for JParallel class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JParallel
#define _H_JParallel

#include "JNaryOperator.h"

class JParallel : public JNaryOperator
{
public:

	JParallel(JPtrArray<JFunction>* argList = nullptr);
	JParallel(const JParallel& source);

	~JParallel() override;

	bool		Evaluate(JFloat* result) const override;
	bool		Evaluate(JComplex* result) const override;
	JFunction*	Copy() const override;
	JIndex		Layout(const JExprRenderer& renderer,
					   const JPoint& upperLeft, const JSize fontSize,
					   JExprRectList* rectList) override;
	void		Render(const JExprRenderer& renderer,
					   const JExprRectList& rectList) const override;
};

#endif
