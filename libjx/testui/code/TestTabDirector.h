/******************************************************************************
 TestTabDirector.h

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestTabDirector
#define _H_TestTabDirector

#include <jx-af/jx/JXWindowDirector.h>

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

	~TestTabDirector();

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;

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
};

#endif
