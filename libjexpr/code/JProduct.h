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

	JProduct();
	JProduct(const JProduct& source);

	virtual ~JProduct();

	virtual JBoolean	Evaluate(JFloat* result) const override;
	virtual JBoolean	Evaluate(JComplex* result) const override;
	virtual JFunction*	Copy() const override;
	virtual void		Print(std::ostream& output) const override;
	virtual JIndex		Layout(const JExprRenderer& renderer,
							   const JPoint& upperLeft, const JSize fontSize,
							   JExprRectList* rectList) override;
	virtual void		Render(const JExprRenderer& renderer,
							   const JExprRectList& rectList) const override;

private:

	// not allowed

	const JProduct& operator=(const JProduct& source);
};

#endif
