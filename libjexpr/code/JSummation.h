/******************************************************************************
 JSummation.h

	Interface for JSummation class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JSummation
#define _H_JSummation

#include "jx-af/jexpr/JNaryOperator.h"

class JSummation : public JNaryOperator
{
public:

	JSummation(JPtrArray<JFunction>* argList = nullptr);
	JSummation(const JSummation& source);

	~JSummation() override;

	bool		Evaluate(JFloat* result) const override;
	bool		Evaluate(JComplex* result) const override;
	JFunction*	Copy() const override;
	void		Print(std::ostream& output) const override;
	JIndex		Layout(const JExprRenderer& renderer,
					   const JPoint& upperLeft, const JSize fontSize,
					   JExprRectList* rectList) override;
	void		Render(const JExprRenderer& renderer,
					   const JExprRectList& rectList) const override;
};

#endif
