/******************************************************************************
 TestExprDirector.h

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_TestExprDirector
#define _H_TestExprDirector

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWindowDirector.h>

class JXTextButton;

class TestExprDirector : public JXWindowDirector
{
public:

	TestExprDirector(JXDirector* supervisor);

	virtual ~TestExprDirector();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

// begin JXLayout

	JXTextButton* itsTestFEditButton;
	JXTextButton* itsTestDParseButton;
	JXTextButton* itsTestFEqButton;
	JXTextButton* itsTestDEqButton;
	JXTextButton* itsTestCFEditButton;

// end JXLayout

private:

	void	BuildWindow();

	void	TestFunctionEditor(const JBoolean useComplex);
	void	TestDecisionParser();

	// not allowed

	TestExprDirector(const TestExprDirector& source);
	const TestExprDirector& operator=(const TestExprDirector& source);
};

#endif
