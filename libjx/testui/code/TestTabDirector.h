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

	~TestTabDirector() override;

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JIndex	itsNextTabIndex;

// begin JXLayout

	JXTabGroup*       itsTabGroup;
	JXTextButton*     itsAddTabButton;
	JXFontNameMenu*   itsFontMenu;
	JXChooseMonoFont* itsMonoFont;
	JXFontSizeMenu*   itsSizeMenu;
	JXRadioGroup*     itsEdgeRG;
	JXInputField*     itsMonoFontSample;

// end JXLayout

private:

	void	BuildWindow();
	void	UpdateFontSample();
};

#endif
