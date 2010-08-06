/******************************************************************************
 TestTabDirector.h

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestTabDirector
#define _H_TestTabDirector

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWindowDirector.h>

class JXTabGroup;
class JXTextButton;
class JXRadioGroup;
class JXInputField;
class JXFontNameMenu;
class JXFontSizeMenu;
class JXChooseMonoFont;

class TestTabDirector : public JXWindowDirector
{
public:

	TestTabDirector(JXDirector* supervisor);

	virtual ~TestTabDirector();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JXTabGroup*			itsTabGroup;
	JXTextButton*		itsAddTabButton;
	JIndex				itsNextTabIndex;
	JXFontNameMenu*		itsFontMenu;
	JXFontSizeMenu*		itsSizeMenu;
	JXRadioGroup*		itsEdgeRG;
	JXChooseMonoFont*	itsMonoFont;
	JXInputField*		itsMonoFontSample;

private:

	void	BuildWindow();
	void	UpdateFontSample();

	// not allowed

	TestTabDirector(const TestTabDirector& source);
	const TestTabDirector& operator=(const TestTabDirector& source);
};

#endif
