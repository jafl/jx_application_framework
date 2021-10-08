/******************************************************************************
 JProduct.h

	Interface for JProduct class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JProduct
#define _H_JProduct

#include "jx-af/jexpr/JNaryOperator.h"

class JProduct : public JNaryOperator
{
public:

	JProduct(JPtrArray<JFunction>* argList = nullptr);
	JProduct(const JProduct& source);

	~JProduct();

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
