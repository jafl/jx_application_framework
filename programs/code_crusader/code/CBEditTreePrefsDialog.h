/******************************************************************************
 CBEditTreePrefsDialog.h

	Interface for the CBEditTreePrefsDialog class

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBEditTreePrefsDialog
#define _H_CBEditTreePrefsDialog

#include <JXDialogDirector.h>

class JXTextButton;
class JXTextCheckbox;
class JXRadioGroup;
class JXFontSizeMenu;

class CBEditTreePrefsDialog : public JXDialogDirector
{
public:

	CBEditTreePrefsDialog(const JSize fontSize, const bool showInheritedFns,
						  const bool autoMinMILinks, const bool drawMILinksOnTop,
						  const bool raiseWhenSingleMatch);

	virtual ~CBEditTreePrefsDialog();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	enum
	{
		kDrawMILinksAbove = 1,
		kDrawMILinksBelow
	};

private:

// begin JXLayout

	JXTextCheckbox* itsShowInheritedFnsCB;
	JXFontSizeMenu* itsFontSizeMenu;
	JXTextCheckbox* itsAutoMinMILinkCB;
	JXRadioGroup*   itsMILinkStyleRG;
	JXTextButton*   itsHelpButton;
	JXTextCheckbox* itsRaiseSingleMatchCB;

// end JXLayout

private:

	void	BuildWindow(const JSize fontSize, const bool showInheritedFns,
						const bool autoMinMILinks, const bool drawMILinksOnTop,
						const bool raiseWhenSingleMatch);
	void	UpdateSettings();

	// not allowed

	CBEditTreePrefsDialog(const CBEditTreePrefsDialog& source);
	const CBEditTreePrefsDialog& operator=(const CBEditTreePrefsDialog& source);
};

#endif
