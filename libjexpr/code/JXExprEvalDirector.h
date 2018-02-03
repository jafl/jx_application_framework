/******************************************************************************
 JXExprEvalDirector.h

	Interface for the JXExprEvalDirector class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXExprEvalDirector
#define _H_JXExprEvalDirector

#include <JXWindowDirector.h>

class JString;
class JVariableList;
class JFunction;
class JXTextButton;
class JXStaticText;

class JXExprEvalDirector : public JXWindowDirector
{
public:

	JXExprEvalDirector(JXWindowDirector* supervisor,
					   const JVariableList* varList, const JFunction& f);

	virtual ~JXExprEvalDirector();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	const JVariableList*	itsVarList;
	JFunction*				itsFunction;
	JXTextButton*			itsCloseButton;
	JXStaticText*			itsTextDisplay;

private:

	void	BuildWindow(JXWindowDirector* supervisor);
	void	UpdateDisplay();

	// not allowed

	JXExprEvalDirector(const JXExprEvalDirector& source);
	const JXExprEvalDirector& operator=(const JXExprEvalDirector& source);
};

#endif
