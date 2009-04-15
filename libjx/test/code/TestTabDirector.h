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
class JXFontNameMenu;
class JXFontSizeMenu;
class JXRadioGroup;

class TestTabDirector : public JXWindowDirector
{
public:

	TestTabDirector(JXDirector* supervisor);

	virtual ~TestTabDirector();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JXTabGroup*		itsTabGroup;
	JXTextButton*	itsAddTabButton;
	JIndex			itsNextTabIndex;
	JXFontNameMenu*	itsFontMenu;
	JXFontSizeMenu*	itsSizeMenu;
	JXRadioGroup*	itsEdgeRG;

private:

	void	BuildWindow();

	// not allowed

	TestTabDirector(const TestTabDirector& source);
	const TestTabDirector& operator=(const TestTabDirector& source);
};

#endif
