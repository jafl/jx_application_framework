/******************************************************************************
 JProduct.h

	Interface for JProduct class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JProduct
#define _H_JProduct

#include <JNaryOperator.h>

class JProduct : public JNaryOperator
{
public:

	JProduct(JPtrArray<JFunction>* argList = nullptr);
	JProduct(const JProduct& source);

	virtual ~JProduct();

	virtual bool		Evaluate(JFloat* result) const override;
	virtual bool		Evaluate(JComplex* result) const override;
	virtual JFunction*	Copy() const override;
	virtual void		Print(std::ostream& output) const override;
	virtual JIndex		Layout(const JExprRenderer& renderer,
							   const JPoint& upperLeft, const JSize fontSize,
							   JExprRectList* rectList) override;
	virtual void		Render(const JExprRenderer& renderer,
							   const JExprRectList& rectList) const override;
};

#endif
