/******************************************************************************
 TestFEditDirector.h

	Interface for the TestFEditDirector class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_TestFEditDirector
#define _H_TestFEditDirector

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWindowDirector.h>
#include <jFStreamUtil.h>

class JVariableList;
class TestVarList;
class JXExprEditor;
class JXTextButton;

class TestFEditDirector : public JXWindowDirector
{
public:

	TestFEditDirector(JXDirector* supervisor, const JCharacter* fileName,
					  const JBoolean useComplex);

	virtual ~TestFEditDirector();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	ifstream*		itsInput;
	TestVarList*	itsVarList;
	JXExprEditor*	itsExprWidget;
	JBoolean		itsUseComplexFlag;

// begin JXLayout

    JXTextButton* itsDoneButton;
    JXTextButton* itsNextButton;
    JXTextButton* itsEvalButton;

// end JXLayout

private:

	void	BuildWindow(const JVariableList* varList);

	void	GetNextFunction();
	void	EvaluateFunction();

	// not allowed

	TestFEditDirector(const TestFEditDirector& source);
	const TestFEditDirector& operator=(const TestFEditDirector& source);
};

#endif
