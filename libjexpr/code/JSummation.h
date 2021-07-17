/******************************************************************************
 JSummation.h

	Interface for JSummation class.

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#ifndef _H_JSummation
#define _H_JSummation

#include <JNaryOperator.h>

class JSummation : public JNaryOperator
{
public:

	JSummation(JPtrArray<JFunction>* argList = nullptr);
	JSummation(const JSummation& source);

	virtual ~JSummation();

	virtual bool	Evaluate(JFloat* result) const override;
	virtual bool	Evaluate(JComplex* result) const override;
	virtual JFunction*	Copy() const override;
	virtual void		Print(std::ostream& output) const override;
	virtual JIndex		Layout(const JExprRenderer& renderer,
							   const JPoint& upperLeft, const JSize fontSize,
							   JExprRectList* rectList) override;
	virtual void		Render(const JExprRenderer& renderer,
							   const JExprRectList& rectList) const override;

private:

	// not allowed

	const JSummation& operator=(const JSummation& source);
};

#endif
